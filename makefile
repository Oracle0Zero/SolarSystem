vpath %.cpp src
vpath %.c src
vpath %.h include

CC = g++
CFLAGS = -c
CPPFLAGS = -fopenmp
COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
OUTPUT_OPTION = -o $@
LDLIBS = -fopenmp -lGL -lGLU -lSOIL -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi

main.exec: main.o Utils.o sphere.o Torus.o glad.o
	$(LINK.C) $^ $(LOADLIBES) $(LDLIBS) -o $@
	rm -r *.o

main.o: main.cpp
	$(COMPILE.c) $(OUTPUT_OPTION) $<

Utils.o: Utils.cpp
	$(COMPILE.c) $(OUTPUT_OPTION) $<

sphere.o: sphere.cpp
	$(COMPILE.c) $(OUTPUT_OPTION) $<

Torus.o: Torus.cpp
	$(COMPILE.c) $(OUTPUT_OPTION) $<

glad.o: glad.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<