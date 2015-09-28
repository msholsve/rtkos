
#include <native/timer.h>

#include "io.h"
#include "util.h"

void testPin(int channel)
{
	io_write(channel, HIGH);

	if (!io_read(channel))
	{
		io_write(channel, LOW);
		
		rt_timer_spin(5000);

		io_write(channel, HIGH);
	}
}