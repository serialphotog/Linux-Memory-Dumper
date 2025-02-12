# Linux Memory Analysis Tools

This repository contains some proof-of-concept tools for working with memory analysis on Linux. These work by locating the physical RAM address ranges by processing `/proc/iomem` and associating with regions in `/proc/kcore`. There are currently two tools provided here:

1. `dumpmemory` - Dumps the physical RAM of the system to a file on disk:
2. 
    ```
    dumpmemory <output_file>
    ```
3. `scanmemory` - Scans the system memory for a specified string pattern. Technically there's not many circumstances where you'd want this, but it serves as a POC of how something like this could work:
4. 
    ```
    scanmemory <pattern> <bytes_of_context>
    ```

## Disclaimer

Note that these tools are nothing more than experimental proofs-of-concept. They have not been extensively tested and I make no guarantee about their accuracy or completeness. 

## Building 

```bash
make
```
