# Copyright (c) 2021 by Thomas A. Early N7TAE

CPPFLAGS  = -g -Wall -std=c++11
LIBS    = -lpthread

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

EXE = qndmr

$(EXE) : $(OBJS)
		g++ $(OBJS) $(CFLAGS) $(LIBS) -o $(EXE)

%.o : %.cpp
	g++ -MMD -MD $(CPPFLAGS) -c $< -o $@

.PHONY : clean

clean :
	$(RM) $(EXE) *.o *.d

-include $(DEPS)
