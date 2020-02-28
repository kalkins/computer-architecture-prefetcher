# M5 Prefetcher simulation

## Installation
First, download the modified M5 simulator and SPEC CPU2000 benchmarks suite from https://goo.gl/Jg7oFm (NB! 564 MiB large file). Located on OneDrive and requires NTNU login,
Software requirements (specific Debian/Ubuntu packages mentioned in paren- theses):
* 3.4.6 <= g++ <= 4.8
* Python and libpython >= 2.4 (python and python-dev)  Scons > 0.98.1 (scons)
* SWIG >= 1.3.31 (swig)
* zlib (zlib1g-dev)
* m4 (m4)

To install all required packages in one go, issue instructions to apt-get:
```sh
sudo apt-get install g++-4.8 python-dev scons swig zlib1g-dev m4
```

The simulator framework comes packaged as a gzipped tarball. Start the adventure by unpacking with
```sh
tar xvzf prefetcher.tgz
```

This will create a directory named framework.

## Building
M5 uses the scons build system:
```sh
cd prefetcher/m5/
scons -j2 ./build/ALPHA_SE/m5.opt
```
builds the optimized version of the M5 binaries.
-j2 specifies that the build process should build two targets in parallel. This is a useful option to cut down on compile time if your machine has several processors or cores.
The included build script compile.sh encapsulates the necessary build commands and options.

## Benchmark
The test prefetcher.py script can be used to evaluate the performance of your prefetcher against the SPEC CPU2000 benchmarks. It runs a selected suite of CPU2000 tests with your prefetcher, and compares the results to some reference prefetchers.
```sh
cd ../prefetcher/
make test
```

The per-test statistics that M5 generates are written to output/<testname-prefetcher>/stats.txt. The statistics most relevant for hardware prefetching are then filtered and aggregated to a stats.txt file in the framework base directory.
```sh
  cat output/*/stats.txt
```
