PROG = app.exe
CC = g++
CPP_FLAGS = -g -Wall
LINKER_FLAGS = -lSDL2main -lSDL2 -lSDL2_ttf
OBJS = text_interface.o main.o

$(PROG) : $(OBJS)
	$(CC) -o $(PROG) $(OBJS) $(LINKER_FLAGS)
text_interface.o: 
	$(CC) $(CPP_FLAGS) -c text_interface.cpp
main.o: 
	$(CC) $(CPP_FLAGS) -c main.cpp
clean:
	del /Q /S $(PROG) $(OBJS)