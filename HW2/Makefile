
CC = g++
CXXFLAGS = -g -Wall -std=c++11 -Werror -pedantic-errors -DNDEBUG
CCLINK = $(CC)
OBJS = bank.o atm.o BankAccount.o
TARGET = Bank
RM = rm -f

# Creating the executable
$(TARGET): $(OBJS)
	$(CCLINK) -o $(TARGET) $(OBJS) -pthread

# Creating the object files
atm.o: atm.cpp atm.hpp
	$(CC) $(CXXFLAGS) -c atm.cpp -o atm.o

bank.o: bank.cpp atm.hpp bank.hpp
	$(CC) $(CXXFLAGS) -c bank.cpp -o bank.o

BankAccount.o: BankAccount.cpp BankAccount.hpp
	$(CC) $(CXXFLAGS) -c BankAccount.cpp -o BankAccount.o

# Cleaning old files before new make
clean:
	$(RM) $(TARGET) $(OBJS) *~ "#" core.*
