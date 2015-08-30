# Variables
MAKE= make
RM= rm

ASSIGNMENTS= AssignmentA AssignmentB AssignmentC AssignmentD AssignmentE

all:
	@for i in ${ASSIGNMENTS} ; do \
		echo "(cd $$i && $(MAKE) $@)"; \
		(cd $$i && $(MAKE) $@); \
		echo "done"; \
	done

clean:
	@for i in ${ASSIGNMENTS} ; do \
		echo "(cd $$i && $(MAKE) $@)"; \
		(cd $$i && $(MAKE) $@); \
		echo "done"; \
	done