# Linux Memory Dumper

This repository contains a proof-of-concept tool for dumping the system memory of a Linux system. This works by locating the physical RAM address ranges by processing `/proc/iomem` and associating with regions in `/proc/kcore`. There is currently a single command-line tool provided here:

1. `dumpmemory` - Dumps the physical RAM of the system to a file on disk:

    ```
    dumpmemory <output_file>
    ```

## Disclaimer

Note that this tool is nothing more than an experimental proof-of-concept. It has not been extensively tested and I make no guarantee about its accuracy or completeness. 

## Building 

```bash
make
```
