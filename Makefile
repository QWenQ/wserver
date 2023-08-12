TARGET = http_server
SOURCES = main.cpp Acceptor.cpp Buffer.cpp Channel.cpp Epoll.cpp EventLoop.cpp EventLoopThread.cpp \
	  EventLoopThreadPool.cpp HttpContext.cpp HttpRequest.cpp HttpResponse.cpp HttpServer.cpp \
	  Socket.cpp TcpConnection.cpp TcpServer.cpp Thread.cpp TimerHeap.cpp \
	  base/AsyncLog.cpp base/CountDownLatch.cpp base/CurrentThread.cpp base/FileUtil.cpp \
	  base/LogFile.cpp base/LogStream.cpp base/Logging.cpp base/timeUtils.cpp
OBJECTS = $(SOURCES:.cpp=.o)

CC = g++
CPPFLAGS = -g -Wall -std=c++2a

vpath %.cpp base/
vpath %.h base/

$(TARGET) : $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(CPPFLAGS)

all : $(OBJECTS)

$(OBJECTS) : %.o : %.cpp %.d
	$(CC) -c $(CPPFLAGS) $< -o $@

%.d : %.cpp
	@set -e; rm -rf $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -rf $@.$$$$

DEPENDENCIES = $(SOURCES:.cpp=.d)
-include $(DEPENDENCIES)

.PHONY : clean

clean : 
	-rm -rf $(TARGET) $(OBJECTS) $(DEPENDENCIES)
