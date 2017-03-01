CFLAGS =-Wall -Werror -g -O3 -std=c++0x
LIBRARY=-lmysqlclient -lcgicc -lpthread -lcurl -lc
SRC = src/
BIN = bin/

all:  scene util sun ping zwave expSys irRelayRgb climax trigger medNot
	rm -f *~ *.o *.log

debug: util_debug scene_debug sun_debug ping_debug zwave_debug expSys_debug irRelayRgb_debug climax_debug trigger_debug medNot_debug
	rm -f *~ *.o *.log

scene:
	g++ $(SRC)scene.cpp $(CFLAGS) -o $(BIN)scene.cgi $(LIBRARY)

scene_debug:
	g++ $(SRC)scene.cpp $(CFLAGS) -DDEBUG -o $(BIN)scene_debug.cgi $(LIBRARY)

trigger:
	g++ trigger.cpp $(CFLAGS) -DLINUX -o $(BIN)trigger.cgi $(CURDIR)/../include/libPPCS_API.a -L $(CURDIR)/../include/libPPCS_API.so $(LIBRARY)
	g++ triggerSlave.cpp $(CFLAGS) -DLINUX -o $(BIN)triggerSlave.cgi $(CURDIR)/../include/libPPCS_API.a -L $(CURDIR)/../include/libPPCS_API.so $(LIBRARY)
	
trigger_debug:
	g++ $(CFLAGS) -DDEBUG -DLINUX $(SRC)trigger.cpp -o $(BIN)trigger_debug.cgi $(CURDIR)/../include/libPPCS_API.a -L $(CURDIR)/../include/libPPCS_API.so $(LIBRARY)
	g++ $(CFLAGS) -DDEBUG -DLINUX $(SRC)triggerSlave.cpp -o $(BIN)triggerSlave_debug.cgi $(CURDIR)/../include/libPPCS_API.a -L $(CURDIR)/../include/libPPCS_API.so $(LIBRARY)
	
util: util.cpp
	g++ $(SRC)util.cpp $(CFLAGS) -o $(BIN)util $(LIBRARY)
	
util_debug: util.cpp
	g++ $(SRC)util.cpp $(CFLAGS) -DDEBUG -o $(BIN)util_debug $(LIBRARY)

sun: sun.cpp
	g++ $(SRC)sun.cpp $(CFLAGS) -o $(BIN)sun $(LIBRARY)
	
sun_debug: sun.cpp
	g++ $(SRC)sun.cpp $(CFLAGS) -DDEBUG -o $(BIN)sun_debug $(LIBRARY)

ping: ping.cpp 
	g++ $(SRC)ping.cpp $(CFLAGS) -o $(BIN)ping.cgi $(LIBRARY)
	
ping_debug: ping.cpp 
	g++ $(SRC)ping.cpp $(CFLAGS) -DDEBUG -o $(BIN)ping_debug.cgi $(LIBRARY)	
	
zwave: zwave.cpp 
	g++ zwave.cpp $(CFLAGS) -o $(BIN)zwave.cgi $(LIBRARY)
	
zwave_debug: zwave.cpp 
	g++ $(SRC)zwave.cpp $(CFLAGS) -DDEBUG -o $(BIN)zwave_debug.cgi $(LIBRARY)	
	
expSys: expSys.cpp 
	g++ $(SRC)expSys.cpp $(CFLAGS) -o $(BIN)expSys.cgi $(LIBRARY)
	
expSys_debug: expSys.cpp 
	g++ $(SRC)expSys.cpp $(CFLAGS) -DDEBUG -o $(BIN)expSys_debug.cgi $(LIBRARY)	

irRelayRgb: irRelayRgb.cpp 
	g++ $(SRC)irRelayRgb.cpp $(CFLAGS) -o $(BIN)irRelayRgb.cgi $(LIBRARY)
	
irRelayRgb_debug: irRelayRgb.cpp 
	g++ $(SRC)irRelayRgb.cpp $(CFLAGS) -DDEBUG -o $(BIN)irRelayRgb_debug.cgi $(LIBRARY)	
	
climax: climax.cpp 
	g++ $(SRC)climax.cpp $(CFLAGS) -o $(BIN)climax.cgi $(LIBRARY)
	
climax_debug: climax.cpp 
	g++ $(SRC)climax.cpp $(CFLAGS) -DDEBUG -o $(BIN)climax_debug.cgi $(LIBRARY)
	
stream_debug: stream.cpp 
	g++ $(SRC)stream.cpp $(CFLAGS) -DDEBUG -o $(BIN)stream_debug.cgi $(LIBRARY)

stream: stream.cpp 
	g++ $(SRC)stream.cpp $(CFLAGS) -o $(BIN)stream.cgi $(LIBRARY)

cgiHosts:
	g++ $(SRC)cgiHosts.cpp $(CFLAGS) -o $(BIN)cgiHosts.cgi $(LIBRARY)

medNot: mediaNotify.cpp 
	g++ $(SRC)mediaNotify.cpp $(CFLAGS) -o $(BIN)medNot.cgi $(LIBRARY)

medNot_debug: mediaNotify.cpp
	g++ $(SRC)mediaNotify.cpp $(CFLAGS) -DDEBUG -o $(BIN)medNot_debug.cgi $(LIBRARY)
	
clean:
	rm -f *~ *.cgi util util_debug sun sun_debug *.gch *.o *.log
	
package:
	cp -rv trigger.cgi triggerSlave.cgi /var/www/Milan/Drivers/MILAN_EXE/TRIGGER/
	cp -rv scene.cgi /var/www/Milan/Drivers/MILAN_EXE/SCENE/
	cp -rv util /var/www/Milan/Drivers/MILAN_EXE/UTIL/
	cp -rv sun /var/www/Milan/Drivers/MILAN_EXE/SUNRISE_SET/
	cp -rv expSys.cgi ping.cgi zwave.cgi irRelayRgb.cgi climax.cgi /var/www/Milan/Drivers/MILAN_EXE/EXPSYS/
	echo 'Cecots!*52764^Eloka' | sudo -S chmod 777 /var/www/Milan/Drivers/MILAN_EXE/SCENE/*
	echo 'Cecots!*52764^Eloka' | sudo -S chmod 777 /var/www/Milan/Drivers/MILAN_EXE/TRIGGER/*
	echo 'Cecots!*52764^Eloka' | sudo -S chmod 777 /var/www/Milan/Drivers/MILAN_EXE/EXPSYS/*
