#OBJS specifies which files to compile as part of the project
OBJS = CardItem.cpp fileIO.cpp JSONHandler.cpp logger.cpp network.cpp Profit.cpp

#CC specifies which compiler we're using
CC = g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I${CURDIR}\include -IC:\local\boost_1_71_0

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -LC:\MinGW\lib -LC:\local\boost_1_71_0\libs

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
#COMPILER_FLAGS = -w -Wl,-subsystem,windows
COMPILER_FLAGS = -std=c++14 -static-libstdc++ -w

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lmingw32 -lwsock32 -lws2_32

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = Profit

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
