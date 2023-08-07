OBJECTS = AsyncLog_test.o Logging_test.o AsyncLog.o Logging.o LogStream.o LogFile.o FileUtil.o CurrentThread.o \
	  timeUtils.o Thread.o CountDownLatch.o
CC = g++
FLAGS = -g -Wall

all : Logging_test AsyncLog_test
.PHONY : all

vpath %.cpp /home/daydayup/wserver : /home/daydayup/wserver/base
vpath %.h /home/daydayup/wserver : /home/daydayup/wserver/base

# generating prerequisities automatically
%.d : %.c
	@set -e; rm -rf $@; \
	$(CC) -MM $(FLAGS) $< > $@.$$$$; \
	sed 's, \($*\)\.o[ :],\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -rf $@.$$$$

SOUCRCES = AsyncLog_test.cpp Logging_test.cpp AsyncLog.cpp Logging.cpp LogStream.cpp LogFile.cpp FileUtil.cpp \
	   CurrentThread.cpp timeUtils.cpp Thread.cpp CountDownLatch.cpp

include $(SOURCES:.cpp=.d)


# todo
# Logging_test : ...
#	...

# todo
# AsyncLog_test : ...
#	...



#todo
all : $(OBJECTS)

$(OBJECTS) : %.o : %.cpp
	$(CC) -c $(FLAGS) $< -o $@



.PHONY : clean

clean : 
	-rm -rf *.o %_test
