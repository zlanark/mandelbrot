.DEFAULT_GOAL := mandelbrot

mandelbrot:
	g++ -std=c++17 -c *.cpp `libpng-config --cflags`
	g++ -std=c++17 -o ./bin/mandelbrot *.o `libpng-config --ldflags`
	rm -f  *.o *.gch

debug: clean
	g++ -std=c++17 -g -Og -O0 -c *.cpp `libpng-config --cflags`
	g++ -std=c++17 -o ./bin/mandelbrot *.o `libpng-config --ldflags`
	rm -f  *.o *.gch

all: clean mandelbrot

clean: 
	rm -f *.o *.gch
	rm -f ./bin/mandelbrot

run: ./bin/mandelbrot
