Logging_test : Logging_test.o Logging.o LogStream.o LogFile.o FileUtil.o
	g++ -o Logging_test \
		Logging_test.o Logging.o LogStream.o LogFile.o FileUtil.o\
		-g -Wall -std=c++2a

Logging_test.o : Logging_test.cpp ../Logging.h ../LogFile.h
	g++ -c Logging_test.cpp -std=c++2a -g

Logging.o : ../Logging.cpp ../Logging.h ../CurrentThread.h
	g++ -c ../Logging.cpp -std=c++2a -g

LogStream.o : ../LogStream.cpp ../LogStream.h
	g++ -c ../LogStream.cpp -std=c++2a -g

LogFile.o : ../LogFile.cpp ../LogFile.h ../FileUtil.h
	g++ -c ../LogFile.cpp -std=c++2a -g

FileUtil.o : ../FileUtil.cpp ../FileUtil.h
	g++ -c ../FileUtil.cpp -std=c++2a -g

clean:
	rm -rf Logging_test *.o