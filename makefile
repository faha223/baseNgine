SHELL = /bin/sh
CXX = g++
FLAGS = -g -Wall
LIBS32 = -L/usr/lib32 -lSDL -lGLU
LIBS64 = -L/usr/lib64 -lSDL -lGLU

#change this to change the default target, or call the target explicitly
runme: ./bin/baseNgine64 #./bin/baseNgine32

x86: ./bin/baseNgine32

x64: ./bin/baseNgine64

# x86-64

./bin/baseNgine64: makefile ./temp/game64.o ./src/main.cpp ./temp/sound64.o
	$(CXX) $(FLAGS) -m64 ./libs/libVector64.o ./temp/quaternion64.o ./temp/texture64.o ./temp/model64.o ./temp/sound64.o ./temp/game64.o ./src/main.cpp -o ./bin/baseNgine64 $(LIBS64)

./temp/game64.o: ./src/game.h ./src/game.cpp ./temp/model64.o ./src/included.h ./src/userInput.h ./src/included.h
	$(CXX) $(FLAGS) -m64 -c ./src/game.cpp -o ./temp/game64.o $(LIBS64)

./temp/sound64.o: ./src/included.h ./src/sound.cpp ./src/sound.h
	$(CXX) $(FLAGS) -m64 -c ./src/sound.cpp -o ./temp/sound64.o $(LIBS64)

./temp/model64.o: ./src/model.h ./src/model.cpp ./temp/texture64.o ./temp/quaternion64.o ./libs/libVector64.o ./src/included.h
	$(CXX) $(FLAGS) -m64 -c ./src/model.cpp -o ./temp/model64.o

./temp/quaternion64.o: ./src/quaternion.h ./src/quaternion.cpp
	$(CXX) $(FLAGS) -m64 -c ./src/quaternion.cpp -o ./temp/quaternion64.o

./temp/texture64.o: ./src/texture.h ./src/texture.cpp ./src/included.h
	$(CXX) $(FLAGS) -m64 -c ./src/texture.cpp -o ./temp/texture64.o

# x86-32

./bin/baseNgine32: makefile ./temp/game32.o ./src/main.cpp ./temp/sound32.o
	$(CXX) $(FLAGS) -m32 ./libs/libVector32.o ./temp/quaternion32.o ./temp/texture32.o ./temp/model32.o ./temp/sound32.o ./temp/game32.o ./src/main.cpp -o ./bin/baseNgine32 $(LIBS32)

./temp/game32.o: ./src/game.h ./src/game.cpp ./temp/model32.o ./src/included.h ./src/userInput.h
	$(CXX) $(FLAGS) -m32 -c ./src/game.cpp -o ./temp/game32.o $(LIBS32)

./temp/sound32.o: ./src/included.h ./src/sound.cpp ./src/sound.h
	$(CXX) $(FLAGS) -m32 -c ./src/sound.cpp -o ./temp/sound32.o $(LIBS32)

./temp/model32.o: ./src/model.h ./src/model.cpp ./temp/texture32.o ./temp/quaternion32.o ./libs/libVector32.o
	$(CXX) $(FLAGS) -m32 -c ./src/model.cpp -o ./temp/model32.o

./temp/quaternion32.o: ./src/quaternion.h ./src/quaternion.cpp
	$(CXX) $(FLAGS) -m32 -c ./src/quaternion.cpp -o ./temp/quaternion32.o

./temp/texture32.o: ./src/texture.h ./src/texture.cpp
	$(CXX) $(FLAGS) -m32 -c ./src/texture.cpp -o ./temp/texture32.o

clean:
	if test -f ./bin/baseNgine64; then rm ./bin/baseNgine64; fi
	if test -f ./bin/baseNgine32; then rm ./bin/baseNgine32; fi
	if test -f ./temp/game64.o; then rm ./temp/game64.o; fi
	if test -f ./temp/model64.o; then rm ./temp/model64.o; fi
	if test -f ./temp/quaternion64.o; then rm ./temp/quaternion64.o; fi
	if test -f ./temp/texture64.o; then rm ./temp/texture64.o; fi
	if test -f ./temp/game32.o; then rm ./temp/game32.o; fi
	if test -f ./temp/model32.o; then rm ./temp/model32.o; fi
	if test -f ./temp/quaternion32.o; then rm ./temp/quaternion32.o; fi
	if test -f ./temp/texture32.o; then rm ./temp/texture32.o; fi
