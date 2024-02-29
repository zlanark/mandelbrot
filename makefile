# testwriter:
# 	g++ -g -Og -c testwriter.cpp `libpng-config --cflags`
# 	g++ -o testwriter testwriter.o `libpng-config --ldflags`
# 	rm testwriter.o

# mandelbrot:
# 	g++ -g -Og -c mandelbrot.cpp `libpng-config --cflags`
# 	g++ -o ./bin/mandelbrot mandelbrot.o `libpng-config --ldflags`
# 	rm mandelbrot.o

mandelbrot:
	g++ -g -Og -c main.cpp `libpng-config --cflags`
	g++ -o ./bin/mandelbrot main.o `libpng-config --ldflags`
	rm main.o

# test:
# 	g++ -Og test.cpp -o ./bin/test