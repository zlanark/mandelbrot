Simple multi-threaded mandelbrot renderer.

## Dependencies
- [png++](http://savannah.nongnu.org/projects/pngpp/)
- [GNU C Library](https://sourceware.org/glibc/)

For Debian/Ubuntu Linux, these can be installed with:
`sudo apt install libpng++-dev libc-dev gcc`

## Build
Make sure that you have a version of [GCC](https://gcc.gnu.org/) capable of compiling C++ 17 installed and that `g++` is in your path.
```shell
cd path/to/source/directory
make -B
```
The executable will be at `bin/mandelbrot`.