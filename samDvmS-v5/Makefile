CFLAGS =-Wall -Werror -g -O3 -std=c++0x
LIBRARY=-lmysqlclient -lpthread -lcurl -lc

all: samdvms samdvms_debug
	rm -f *~ *.o *.log
	
samdvms: samsungSerial.cpp
	g++ samsungSerial.cpp $(CFLAGS) -o samdvms $(LIBRARY)
	
samdvms_debug: samsungSerial.cpp
	g++ samsungSerial.cpp $(CFLAGS) -DDEBUG -o samdvms_debug $(LIBRARY)
	
clean:
	rm -f *~ samdvms samdvms_debug *.gch *.o *.log
	
package:
	cp -rv samdvms /var/www/Milan/Drivers/MILAN_EXE/CLIMATE/
	echo 'Cecots!*52764^Eloka' | sudo -S chmod 777 /var/www/Milan/Drivers/MILAN_EXE/CLIMATE/*
