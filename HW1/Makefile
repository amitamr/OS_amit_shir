# Makefile for the smash program
CC = g++
CXXFLAGS = -g -Wall
CCLINK = $(CC)
OBJS = smash.o commands.o signals.o jobs.o
TARGET = smash
RM = rm -f

# Creating the  executable
$(TARGET): $(OBJS)
	$(CCLINK) -o $(TARGET) $(OBJS)
# Creating the object files
commands.o: commands.cpp commands.hpp jobs.hpp
	$(CC) $(CXXFLAGS) -c commands.cpp
smash.o: smash.cpp commands.hpp signals.hpp jobs.hpp
	$(CC) $(CXXFLAGS) -c smash.cpp
signals.o: signals.cpp signals.hpp jobs.hpp
	$(CC) $(CXXFLAGS) -c signals.cpp
jobs.o: jobs.cpp jobs.hpp
	$(CC) $(CXXFLAGS) -c jobs.cpp

# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*

