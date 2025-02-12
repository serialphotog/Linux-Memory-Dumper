CCFLAGS = -Wall

DUMPMEMORY_FILE = dumpmemory.c
SCANMEMORY_FILE = scanmemory.c

SHARED_FILES = lib/iomem.c lib/kcore.c lib/hex.c
SHARED_HEADERS = lib/lmat.h lib/iomem.h lib/kcore.h lib/hex.h

all: dumpmemory scanmemory

dumpmemory: $(DUMPMEMORY_FILE) $(SHARED_FILE) $(SHARED_HEADERS)
	gcc $(CCFLAGS) -o dumpmemory ${DUMPMEMORY_FILE} $(SHARED_FILES)

scanmemory: $(SCANMEMORY_FILE) $(SHARED_FILES) $(SHARED_HEADERS)
	gcc $(CCFLAGS) -o scanmemory $(SCANMEMORY_FILE) $(SHARED_FILES)

clean: 
	rm -f dumpmemory scanmemory

.PHONY: clean