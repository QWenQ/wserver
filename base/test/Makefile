Logging_test : Logging_test.o Logging.o LogStream.o LogFile.o FileUtil.o
	g++ -o Logging_test \
		Logging_test.o Logging.o LogStream.o LogFile.o FileUtil.o\
		-g -Wall -std=c++2a

Logging_test.o : Logging_test.cpp ../Logging.h ../LogFile.h
	g++ -c Logging_test.cpp -std=c++2a

Logging.o : ../Logging.cpp ../Logging.h ../CurrentThread.h
	g++ -c ../Logging.cpp

LogStream.o : ../LogStream.cpp ../LogStream.h
	g++ -c ../LogStream.cpp

LogFile.o : ../LogFile.cpp ../LogFile.h ../FileUtil.h
	g++ -c ../LogFile.cpp

FileUtil.o : ../FileUtil.cpp ../FileUtil.h
	g++ -c ../FileUtil.cpp

clean:
	rm -rf Logging_test *.o