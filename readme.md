Simple multi-threaded mandelbrot renderer.

## Dependencies
- [png++](http://savannah.nongnu.org/projects/pngpp/)
- [GNU C Library](https://sourceware.org/glibc/)
- [gcc] (https://gcc.gnu.org/)

For Debian/Ubuntu Linux, these can be installed with `sudo apt install libpng++-dev libc-dev gcc`

## Build
```shell
cd path/to/source/directory
make -B
```
The executable will be at `bin/mandelbrot`.