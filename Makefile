CFLAGS =-Wall -Werror -g -O3 -std=c++0x
LIBRARY=-lmysqlclient -lcgicc -lpthread -lcurl -lc

all:  scene util sun ping zwave expSys irRelayRgb climax trigger medNot
	rm -f *~ *.o *.log

debug: util_debug scene_debug sun_debug ping_debug zwave_debug expSys_debug irRelayRgb_debug climax_debug trigger_debug medNot_debug
	rm -f *~ *.o *.log

scene: scene.cpp 
	g++ scene.cpp $(CFLAGS) -o scene.cgi $(LIBRARY)

scene_debug: scene.cpp
	g++ scene.cpp $(CFLAGS) -DDEBUG -o scene_debug.cgi $(LIBRARY)

trigger: trigger.cpp 
	g++ trigger.cpp $(CFLAGS) -DLINUX -o trigger.cgi $(CURDIR)/../include/libPPCS_API.a -L $(CURDIR)/../include/libPPCS_API.so $(LIBRARY)
	g++ triggerSlave.cpp $(CFLAGS) -DLINUX -o triggerSlave.cgi $(CURDIR)/../include/libPPCS_API.a -L $(CURDIR)/../include/libPPCS_API.so $(LIBRARY)
	
trigger_debug: trigger.cpp 
	g++ $(CFLAGS) -DDEBUG -DLINUX trigger.cpp -o trigger_debug.cgi $(CURDIR)/../include/libPPCS_API.a -L $(CURDIR)/../include/libPPCS_API.so $(LIBRARY)
	g++ $(CFLAGS) -DDEBUG -DLINUX triggerSlave.cpp -o triggerSlave_debug.cgi $(CURDIR)/../include/libPPCS_API.a -L $(CURDIR)/../include/libPPCS_API.so $(LIBRARY)
	
util: util.cpp
	g++ util.cpp $(CFLAGS) -o util $(LIBRARY)
	
util_debug: util.cpp
	g++ util.cpp $(CFLAGS) -DDEBUG -o util_debug $(LIBRARY)

sun: sun.cpp
	g++ sun.cpp $(CFLAGS) -o sun $(LIBRARY)
	
sun_debug: sun.cpp
	g++ sun.cpp $(CFLAGS) -DDEBUG -o sun_debug $(LIBRARY)

ping: ping.cpp 
	g++ ping.cpp $(CFLAGS) -o ping.cgi $(LIBRARY)
	
ping_debug: ping.cpp 
	g++ ping.cpp $(CFLAGS) -DDEBUG -o ping_debug.cgi $(LIBRARY)	
	
zwave: zwave.cpp 
	g++ zwave.cpp $(CFLAGS) -o zwave.cgi $(LIBRARY)
	
zwave_debug: zwave.cpp 
	g++ zwave.cpp $(CFLAGS) -DDEBUG -o zwave_debug.cgi $(LIBRARY)	
	
expSys: expSys.cpp 
	g++ expSys.cpp $(CFLAGS) -o expSys.cgi $(LIBRARY)
	
expSys_debug: expSys.cpp 
	g++ expSys.cpp $(CFLAGS) -DDEBUG -o expSys_debug.cgi $(LIBRARY)	

irRelayRgb: irRelayRgb.cpp 
	g++ irRelayRgb.cpp $(CFLAGS) -o irRelayRgb.cgi $(LIBRARY)
	
irRelayRgb_debug: irRelayRgb.cpp 
	g++ irRelayRgb.cpp $(CFLAGS) -DDEBUG -o irRelayRgb_debug.cgi $(LIBRARY)	
	
climax: climax.cpp 
	g++ climax.cpp $(CFLAGS) -o climax.cgi $(LIBRARY)
	
climax_debug: climax.cpp 
	g++ climax.cpp $(CFLAGS) -DDEBUG -o climax_debug.cgi $(LIBRARY)
	
stream_debug: stream.cpp 
	g++ stream.cpp $(CFLAGS) -DDEBUG -o stream_debug.cgi $(LIBRARY)

stream: stream.cpp 
	g++ stream.cpp $(CFLAGS) -o stream.cgi $(LIBRARY)

cgiHosts: cgiHosts.cpp
	g++ cgiHosts.cpp $(CFLAGS) -o cgiHosts.cgi $(LIBRARY)

medNot: mediaNotify.cpp 
	g++ mediaNotify.cpp $(CFLAGS) -o medNot.cgi $(LIBRARY)

medNot_debug: mediaNotify.cpp
	g++ mediaNotify.cpp $(CFLAGS) -DDEBUG -o medNot_debug.cgi $(LIBRARY)
	
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
