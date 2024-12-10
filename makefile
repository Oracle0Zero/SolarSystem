main.o: main.cpp, Utils.cpp, sphere.cpp, Torus.cpp
	g++ -std=c++11 -c main.cpp Utils.cpp sphere.cpp Torus.cpp;

Utils.cpp: Utils.h, stb_image.h