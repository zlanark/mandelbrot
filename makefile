.DEFAULT_GOAL := mandelbrot

# For testing png writer

# testwriter:
# 	g++ -g -Og -c testwriter.cpp `libpng-config --cflags`
# 	g++ -o testwriter testwriter.o `libpng-config --ldflags`
# 	rm testwriter.o

# Debug

mandelbrot:
	g++ -std=c++17 -g -Og -O0 -c main.cpp `libpng-config --cflags`
	g++ -std=c++17 -o ./bin/mandelbrot main.o `libpng-config --ldflags`
	rm main.o

# mandelbrot:
# 	g++ -std=c++17 -c main.cpp `libpng-config --cflags`
# 	g++ -std=c++17 -o ./bin/mandelbrot main.o `libpng-config --ldflags`
# 	rm main.o

all: clean mandelbrot

clean: 
	rm -f main.o
	rm -f ./bin/mandelbrot

run: ./bin/mandelbrot
