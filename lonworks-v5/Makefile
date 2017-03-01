CFLAGS =-Wall -Werror -g -O3 -std=c++0x
LIBRARY=-lmysqlclient -lpthread -lcurl -lc

all: lonWorks lonWorks_debug
	rm -f *~ *.o *.log
	
lonWorks: lonWorks.cpp
	g++ lonWorks.cpp $(CFLAGS) -o lonWorks $(LIBRARY)
	
lonWorks_debug: lonWorks.cpp
	g++ lonWorks.cpp $(CFLAGS) -DDEBUG -o lonWorks_debug $(LIBRARY)
	
clean:
	rm -f *~ lonWorks lonWorks_debug *.gch *.o *.log
	
package:
	cp -rv lonWorks /var/www/Milan/Drivers/MILAN_EXE/CLIMATE/
	echo 'Cecots!*52764^Eloka' | sudo -S chmod 777 /var/www/Milan/Drivers/MILAN_EXE/CLIMATE/*
