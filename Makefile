#OBJS specifies which files to compile as part of the project
OBJS = watch.c

#CC specifies which compiler we're using
CC = gcc
COMPILER_FLAGS = -w

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_ttf -lSDL2_image

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = hahaha

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

