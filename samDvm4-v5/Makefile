CFLAGS =-Wall -Werror -g -O3 -std=c++0x
LIBRARY=-lmysqlclient -lpthread -lcurl -lc

all: samdvm4 samdvm4_debug
	rm -f *~ *.o *.log
	
samdvm4: samsungSerial.cpp
	g++ samsungSerial.cpp $(CFLAGS) -o samdvm4 $(LIBRARY)
	
samdvm4_debug: samsungSerial.cpp
	g++ samsungSerial.cpp $(CFLAGS) -DDEBUG -o samdvm4_debug $(LIBRARY)
	
clean:
	rm -f *~ samdvm4 samdvm4_debug *.gch *.o *.log
	
package:
	cp -rv samdvm4 /var/www/Milan/Drivers/MILAN_EXE/CLIMATE/
	echo 'Cecots!*52764^Eloka' | sudo -S chmod 777 /var/www/Milan/Drivers/MILAN_EXE/CLIMATE/*
