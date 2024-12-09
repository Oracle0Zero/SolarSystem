g++ -std=c++11 -c main.cpp Utils.cpp sphere.cpp Torus.cpp;
g++ -c  glad.c;
g++ -g main.o -o main.exec glad.o Utils.o sphere.o Torus.o -lGL -lGLU -lSOIL -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi;
./main.exec;

