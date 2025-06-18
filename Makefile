CCFLAGS = -Wall

DUMPMEMORY_FILE = dumpmemory.c

SHARED_FILES = lib/iomem.c lib/kcore.c
SHARED_HEADERS = lib/lmat.h lib/iomem.h lib/kcore.h

all: dumpmemory

dumpmemory: $(DUMPMEMORY_FILE) $(SHARED_FILES) $(SHARED_HEADERS)
	gcc $(CCFLAGS) -o dumpmemory ${DUMPMEMORY_FILE} $(SHARED_FILES)

clean: 
	rm -f dumpmemory

.PHONY: clean