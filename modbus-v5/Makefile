CFLAGS =-Wall -Werror -g -O3 -std=c++0x
LIBRARY=-lmysqlclient -lpthread -lcurl -lc

all: modBus modBus_debug
	rm -f *~ *.o *.log
	
modBus: modBus.cpp
	g++ modBus.cpp $(CFLAGS) -o modBus $(LIBRARY)
	
modBus_debug: modBus.cpp
	g++ modBus.cpp $(CFLAGS) -DDEBUG -o modBus_debug $(LIBRARY)
	
clean:
	rm -f *~ modBus modBus_debug *.gch *.o *.log
	
package:
	cp -rv modBus /var/www/Milan/Drivers/MILAN_EXE/CLIMATE/
	echo 'Cecots!*52764^Eloka' | sudo -S chmod 777 /var/www/Milan/Drivers/MILAN_EXE/CLIMATE/*
