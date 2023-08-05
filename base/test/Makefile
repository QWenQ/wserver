OBJECTS = 
CC = g++
FLAGS = -g -Wall

all : Logging_test AsyncLog_test
.PHONY : all

vpath %.cpp /home/daydayup/wserver : /home/daydayup/wserver/base
vpath %.h /home/daydayup/wserver : /home/daydayup/wserver/base


# todo
Logging_test : ...
	...

#todo
AsyncLog_test : ...
	...

#todo ??? other predependecies ??? answer: -MM
all : $(OBJECTS)

$(OBJECTS) : %.o : %.cpp
	$(CC) -c $(FLAGS) $< -o $@ -MM


.PHONY : clean

clean : 
	-rm -rf *.o %_testk
