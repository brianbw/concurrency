OBJS = fullTest.o
CC = g++ -Wall -pedantic -g -std=c++11 -pthread
DEPS = coarseList.h coarseTable.h fineList.h fineTable.h

all: fullTest

fullTest: fullTest.cpp $(OBJS)
	$(CC) -o fullTest $(OBJS)

fullTest.o: fullTest.cpp $(DEPS)
	$(CC) -c fullTest.cpp

clean:
	rm $(OBJS) fullTest

