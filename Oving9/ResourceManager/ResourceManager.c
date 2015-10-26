#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

#include "fifo.h"

#define LOCK(X) pthread_mutex_lock(&X)

#define RETURN_UNLOCK(X,Y) do { \
	pthread_mutex_unlock(&Y); \
	return (X); \
	} while (0)

void error(char *s)
{
	perror(s);
	exit(EXIT_FAILURE);
}

static fifoHandler_t fifoHandler;
pthread_mutex_t resource_mutex;

int io_read(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb)
{
	LOCK(resource_mutex);
	printf("IO read activated\n");

	int nonblock, status;

	if ((status = iofunc_read_verify (ctp, msg, ocb, &nonblock)) != EOK)
	{
		RETURN_UNLOCK(status, resource_mutex);
	}

	// set data to return
	if (ocb->offset)
	{
		SETIOV(ctp->iov, NULL, 0);
		_IO_SET_READ_NBYTES(ctp, 0);
		RETURN_UNLOCK(_RESMGR_NPARTS(0), resource_mutex);
	}
	ocb->offset = 1;

	if (nonblock && !fifo_status(fifoHandler))
	{
		SETIOV(ctp->iov, NULL, 0);
		_IO_SET_READ_NBYTES(ctp, 0);
		RETURN_UNLOCK(_RESMGR_NPARTS(0), resource_mutex);
	}
	else if(!fifo_status(fifoHandler))
	{
		fifo_add_blocked_id(fifoHandler, ctp->rcvid);
		RETURN_UNLOCK(_RESMGR_NOREPLY, resource_mutex);
	}

	char buffer[WIDTH];
	fifo_rem_string(fifoHandler, buffer);
	SETIOV(ctp->iov, buffer, strlen(buffer));
	_IO_SET_READ_NBYTES(ctp, strlen(buffer));
	RETURN_UNLOCK(_RESMGR_NPARTS(1), resource_mutex);
}

int io_write(resmgr_context_t *ctp, io_write_t *msg, iofunc_ocb_t *ocb)
{
	LOCK(resource_mutex);
	printf("IO write activated\n");

	// set data to return
	_IO_SET_WRITE_NBYTES(ctp, msg->i.nbytes);

	int nBytes = msg->i.nbytes + 1;

	char *buf = (char *)malloc(nBytes * sizeof(char));
	if (!buf)
		RETURN_UNLOCK(ENOMEM, resource_mutex);

	resmgr_msgread(ctp, buf, nBytes, sizeof(msg->i));
	buf[msg->i.nbytes] = '\0';
	printf("Received %d bytes = \"%s\"\n", msg->i.nbytes, buf);

	int rcvid = fifo_rem_blocked_id(fifoHandler);
	if(rcvid != -1)
	{
		printf("Found blocked id, sending string '%s' with %i bytes\n", buf, nBytes);
		MsgReply(rcvid, nBytes, buf, nBytes);
		free(buf);
		RETURN_UNLOCK(_RESMGR_NPARTS(0), resource_mutex);
	}

	fifo_add_string(fifoHandler, buf);
	free(buf);

	RETURN_UNLOCK(_RESMGR_NPARTS(0), resource_mutex);
}

int main(int argc, char *argv[])
{
	dispatch_t              *dpp;
	resmgr_attr_t           resmgr_attr;
	dispatch_context_t      *ctp;
	resmgr_connect_funcs_t  connect_funcs;
	resmgr_io_funcs_t       io_funcs;
	iofunc_attr_t           io_attr;

	printf("Start resource manager\n");

	fifoHandler = init_fifo();
	pthread_mutex_init(&resource_mutex, NULL);

	// create dispatch.
	if (!(dpp = dispatch_create()))
		error("dispatch_create()");

	// initialize resource manager attributes.
	memset(&resmgr_attr, 0, sizeof(resmgr_attr));
	resmgr_attr.nparts_max = 1;
	resmgr_attr.msg_max_size = 2048;

	// set standard connect and io functions.
	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
	_RESMGR_IO_NFUNCS, &io_funcs);
	iofunc_attr_init(&io_attr, S_IFNAM | 0666, 0, 0);

	io_funcs.read = io_read;
	io_funcs.write = io_write;

	// establish resource manager
	if (resmgr_attach(dpp, &resmgr_attr, "/dev/myresource", _FTYPE_ANY, 0, &connect_funcs, &io_funcs, &io_attr) < 0)
		error("resmgr_attach()");

	// wait forever, handling messages.
	ctp = dispatch_context_alloc(dpp);
	while(1)
	{
		if (!(ctp = dispatch_block(ctp)))
			error("dispatch_block()");
		dispatch_handler(ctp);
	}
	exit(EXIT_SUCCESS);
}
