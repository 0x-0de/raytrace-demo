CC = g++
CPPFLAGS = -std=c++11 -Wall -g -c

LIBDIR = -L./
LIBS = -lpthread -lopengl32 -lgdi32 -lglfw3 -lglew32

OBJLIST = main.o utilmath.o mesh.o renderer.o shader.o camera.o

default: $(OBJLIST)
	$(CC) $(OBJLIST) $(LIBDIR) $(LIBS) -o Game.exe
	
main.o: ../src/main.cpp
	$(CC) $(CPPFLAGS) ../src/main.cpp

utilmath.o: ../src/utils/utilmath.h ../src/utils/utilmath.cpp
	$(CC) $(CPPFLAGS) ../src/utils/utilmath.cpp

mesh.o: ../src/rendering/mesh.h ../src/rendering/mesh.cpp
	$(CC) $(CPPFLAGS) ../src/rendering/mesh.cpp

renderer.o: ../src/rendering/renderer.h ../src/rendering/renderer.cpp
	$(CC) $(CPPFLAGS) ../src/rendering/renderer.cpp

shader.o: ../src/rendering/shader.h ../src/rendering/shader.cpp
	$(CC) $(CPPFLAGS) ../src/rendering/shader.cpp

font.o: ../src/utils/font.h ../src/utils/font.cpp
	$(CC) $(CPPFLAGS) ../src/utils/font.cpp

texture.o: ../src/utils/texture.h ../src/utils/texture.cpp
	$(CC) $(CPPFLAGS) ../src/utils/texture.cpp

camera.o: ../src/rendering/camera.h ../src/rendering/camera.cpp
	$(CC) $(CPPFLAGS) ../src/rendering/camera.cpp

clean:
	-rm Game.exe *.o