# Linux Memory Dumper

This repository contains a proof-of-concept tool for dumping the system memory of a Linux system. This works by locating the physical RAM address ranges by processing `/proc/iomem` and associating with regions in `/proc/kcore`. You can get a bit more background information on how this tool works from [my blog post](https://hackeradam.com/blog/taking-a-dive-into-linux-proc-kcore/) on the subject.

## Usage

```
make
sudo dumpmemory <output_file>
```

## Disclaimer

Note that this tool is nothing more than an experimental proof-of-concept. It has not been extensively tested and I make no guarantee about its accuracy or completeness.

## License

Linux Memory Dumper is licensed under the GNU GPL v3.0. See [LICENSE](/LICENSE).