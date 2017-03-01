/*******************************************************************************
 *	Purpose		:	modBus AC Integration.							   	   *
 * 	Description	:	This program is the header for Driver Program.			   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	18-Jan-17												   *
 * 	DOLM		:	27-Jan-17												   *
 *******************************************************************************/

/*-----CPP Library-----*/
#include <iostream>
#include <vector>
#include <string>
#include <thread>

/*-----Socket API library-----*/
#include <sys/socket.h> /*-----Needed for the socket functions-----*/
#include <netdb.h>      /*-----Needed for the socket functions-----*/
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>

/*-----C library-----*/
#include <cstring> 		/*-----Library for C string Operations-----*/

/*-----Curl library-----*/
#include <curl/curl.h>

/*-----local header files-----*/
#include "../include/mysql.hpp"
#include "../include/uriParser.hpp"

using namespace std;

class modBus
{
	public:
		static bool ctrlAckRcvd,fbAckRcvd;
		void control();
		modBus();
		~modBus();
		friend size_t writeCallback(void *contents, size_t size, size_t nmemb, string *s);
								
	private:
		bool modeSet(string locInsId,string mode);
		bool tempSet(string locInsId,string temp);
		bool fanSet(string locInsId,string fan);
		bool fb(string locInsId);
		string getMyIp(string interface);
		void dbStore(string locInsId,string type,string param1);
		void curlRequest(string webQuery,string type);
		bool socketSend(string ip,string port, string lwCmd, string &lwResp);
		bool isInt(const string& s, int base);
};

//static variable initialisation
bool modBus::ctrlAckRcvd=0;
bool modBus::fbAckRcvd=0;

//constructor
modBus::modBus()
{
	
}

//destructor
modBus::~modBus()
{
	
}

//modeSet
bool modBus::modeSet(string locInsId,string mode)
{
	//format : #[ac no]@[command no]@[request data]$
	string acId{},cmd{},cmd2{},curMode{};
	testMysql db("Milan_SP");
	MYSQL_ROW rowMode{};
	db.mysqlQuery("SELECT node_id FROM Thermostat WHERE local_instance_id='"+locInsId+"' AND node_id IS NOT NULL;");
	while((rowMode = mysql_fetch_row(db.result)) != NULL)
	{
		acId = rowMode[0];
	}
	
	#ifdef DEBUG
		cout << "acId:" << acId << "\n\n";
	#endif
	
	db.mysqlQuery("SELECT mode FROM Climate_Status WHERE local_instance_id = '"+locInsId+"';");
	while((rowMode = mysql_fetch_row(db.result)) != NULL)
	{
		curMode = rowMode[0];

		if(curMode == "2")//off
		{
			curMode = "0";
		}
		else if(curMode == "50")//auto
		{
			curMode = "3";
		}
		else if(curMode == "49")//heat
		{
			curMode = "2";
		}
		else if(curMode == "47")//cool
		{
			curMode = "1";
		}
	}

	if(!acId.empty())
	{
		//cmd formation
		if(mode == "off")
		{
			cmd = '#'+acId+"@0@$";//off = 0
			cmd2 = '#'+acId+"@"+curMode+"@$";
			if(cmd == cmd2)
				cmd2 = '#'+acId+"@1@$";
		}
		else if(mode == "cool")
		{
			cmd = '#'+acId+"@1@$";//cool = 1
			cmd2 = '#'+acId+"@0@$";
			if(cmd == cmd2)
				cmd2 = '#'+acId+"@0@$";
		}
		else if(mode == "heat")
		{
			cmd = '#'+acId+"@2@$";//heat = 2
			cmd2 = '#'+acId+"@0@$";
			if(cmd == cmd2)
				cmd2 = '#'+acId+"@0@$";
		}
		else if(mode == "auto")
		{
			cmd = '#'+acId+"@3@$";//auto = 3
			cmd2 = '#'+acId+"@0@$";
			if(cmd == cmd2)
				cmd2 = '#'+acId+"@0@$";
		}
		
		if(!cmd.empty() && !cmd2.empty())
		{
			MYSQL_ROW rowMode{};
			string ip{},port{},response{},response2{};
			db.mysqlQuery("SELECT IP,port FROM IP_Port WHERE integration_id='110004' AND IP IS NOT NULL AND port IS NOT NULL;;");
			while((rowMode = mysql_fetch_row(db.result)) != NULL)
			{
				ip = rowMode[0];
				port = rowMode[1];
			}
			
			#ifdef DEBUG
				cout << "device ip:" << ip << endl;
				cout << "device port:" << port << endl;
				cout << "cmd2:" << cmd2 << endl;
				cout << "cmd:" << cmd << endl;
			#endif
			
			for(size_t loopNo1 =0; loopNo1 < 1; loopNo1++)
			{
				if(!ip.empty() && !port.empty() && (socketSend(ip,port,cmd2,response2) == 1))
				{
					sleep(1);
					socketSend(ip,port,cmd,response);
					#ifdef DEBUG
						cout << "response2:" << response2 << endl;
						cout << "response:" << response << endl;
					#endif
					if((response.find('#') != string::npos) && (response.find('$') != string::npos))
					{
						if((response[0] == '#') && (response[response.length()-1] == '$'))
						{
							string acIdVal = response.substr(1,response.find('@')-1);
							#ifdef DEBUG
								cout << "acId = " << acId << endl;
							#endif
							response.erase(0,response.find('@')+1);
							
							string cmdVal = response.substr(0,response.find('@'));
							#ifdef DEBUG
								cout << "cmd = " << cmdVal << endl;
							#endif
							response.erase(0,response.find('@')+1);
														
							string status = response.substr(0,response.find('$'));
							#ifdef DEBUG
								cout << "status = " << status << endl;
							#endif
							response.erase(0,response.find('$')+1);
							
							if(status == "1")
							{
								if(mode == "auto")
								{
									#ifdef DEBUG
										cout << "UPDATE Climate_Status SET mode='50',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';\n";
									#endif
									db.mysqlQuery("UPDATE Climate_Status SET mode='50',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';");
								}
								else if(mode == "cool")
								{
									#ifdef DEBUG
										cout << "UPDATE Climate_Status SET mode='47',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';\n";
									#endif
									db.mysqlQuery("UPDATE Climate_Status SET mode='47',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';");
								}
								else if(mode == "heat")
								{
									#ifdef DEBUG
										cout << "UPDATE Climate_Status SET mode='49',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';\n";
									#endif
									db.mysqlQuery("UPDATE Climate_Status SET mode='49',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';");
								}
								else if(mode == "off")
								{
									#ifdef DEBUG
										cout << "UPDATE Climate_Status SET mode='2',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';\n";
									#endif
									db.mysqlQuery("UPDATE Climate_Status SET mode='2',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';");
								}
								break;
							}
						}
					}
				}
			}
			return 1;
		}
	}
	return 0;
}

bool modBus::tempSet(string locInsId,string temp)//local instance id,temp
{
	//format : #[ac no]@[command no]@[request data]$
	string acId{},cmd{};
	testMysql db("Milan_SP");
	MYSQL_ROW rowTemp{};
	db.mysqlQuery("SELECT node_id FROM Thermostat WHERE local_instance_id='"+locInsId+"' AND node_id IS NOT NULL;");
	while((rowTemp = mysql_fetch_row(db.result)) != NULL)
	{
		acId = rowTemp[0];
	}
	
	#ifdef DEBUG
		cout << "acId:" << acId << "\n\n";
	#endif
	if(!acId.empty())
	{
		//cmd formation
		if(stoi(temp) >= 0)
		{
			cmd = '#'+acId+"@5@"+temp+"$";//temp
		}
			
		if(!cmd.empty())
		{
			string ip{},port{},response{};
			db.mysqlQuery("SELECT IP,port FROM IP_Port WHERE integration_id='110004' AND IP IS NOT NULL AND port IS NOT NULL;");
			while((rowTemp = mysql_fetch_row(db.result)) != NULL)
			{
				ip = rowTemp[0];
				port = rowTemp[1];
			}
			
			#ifdef DEBUG
				cout << "device ip:" << ip << endl;
				cout << "device port:" << port << endl;
				cout << "cmd:" << cmd << endl;
			#endif
			
			for(size_t loopNo1 =0; loopNo1 < 1; loopNo1++)
			{
				if(!ip.empty() && !port.empty() && (socketSend(ip,port,cmd,response) == 1))
				{
					#ifdef DEBUG
						cout << "response:" << response << endl;
					#endif
					if((response.find('#') != string::npos) && (response.find('$') != string::npos))
					{
						if((response[0] == '#') && (response[response.length()-1] == '$'))
						{
							string acIdVal = response.substr(1,response.find('@')-1);
							#ifdef DEBUG
								cout << "acId = " << acId << endl;
							#endif
							response.erase(0,response.find('@')+1);
							
							string cmdVal = response.substr(0,response.find('@'));
							#ifdef DEBUG
								cout << "cmd = " << cmdVal << endl;
							#endif
							response.erase(0,response.find('@')+1);
														
							string status = response.substr(0,response.find('$'));
							#ifdef DEBUG
								cout << "status = " << status << endl;
							#endif
							response.erase(0,response.find('$')+1);
							
							if(status == "1")
							{
								string mode{};
								db.mysqlQuery("SELECT mode FROM Climate_Status WHERE local_instance_id='"+locInsId+"' AND mode IS NOT NULL;");
								while((rowTemp = mysql_fetch_row(db.result)) != NULL)
								{
									mode = rowTemp[0];
								}
								if(mode == "50")
								{
									#ifdef DEBUG
										cout << "UPDATE Climate_Status SET auto_changeover_setpoint='"+temp+"',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';\n";
									#endif
									db.mysqlQuery("UPDATE Climate_Status SET auto_changeover_setpoint='"+temp+"',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';");
								}
								else if(mode == "47")
								{
									#ifdef DEBUG
										cout << "UPDATE Climate_Status SET cool_setpoint='"+temp+"',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';\n";
									#endif
									db.mysqlQuery("UPDATE Climate_Status SET cool_setpoint='"+temp+"',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';");
								}
								else if(mode == "49")
								{
									#ifdef DEBUG
										cout << "UPDATE Climate_Status SET heat_setpoint='"+temp+"',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';\n";
									#endif
									db.mysqlQuery("UPDATE Climate_Status SET heat_setpoint='"+temp+"',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';");
								}
								break;
							}
						}
					}
				}
			}
			return 1;
		}
	}
	return 0;
}

bool modBus::fanSet(string locInsId,string fan)//local instance id,fan
{
	//format : #[ac no]@[command no]@[request data]$
	string acId{},cmd{};
	testMysql db("Milan_SP");
	MYSQL_ROW rowFan{};
	db.mysqlQuery("SELECT node_id FROM Thermostat WHERE local_instance_id='"+locInsId+"' AND node_id IS NOT NULL;");
	while((rowFan = mysql_fetch_row(db.result)) != NULL)
	{
		acId = rowFan[0];
	}
	
	#ifdef DEBUG
		cout << "acId:" << acId << "\n\n";
	#endif
	if(!acId.empty())
	{
		//cmd formation
		if(fan == "auto")
		{
			cmd = '#'+acId+"@6@$";//fan auto
		}
		else if(fan == "on")
		{
			cmd = '#'+acId+"@7@$";//fan on
		}
			
		if(!cmd.empty())
		{
			string ip{},port{},response{};
			db.mysqlQuery("SELECT IP,port FROM IP_Port WHERE integration_id='110004' AND IP IS NOT NULL AND port IS NOT NULL;;");
			while((rowFan = mysql_fetch_row(db.result)) != NULL)
			{
				ip = rowFan[0];
				port = rowFan[1];
			}
			
			#ifdef DEBUG
				cout << "device ip:" << ip << endl;
				cout << "device port:" << port << endl;
				cout << "cmd:" << cmd << endl;
			#endif
			
			for(size_t loopNo1 =0; loopNo1 < 1; loopNo1++)
			{
				if(!ip.empty() && !port.empty() && (socketSend(ip,port,cmd,response) == 1))
				{
					#ifdef DEBUG
						cout << "response:" << response << endl;
					#endif
					if((response.find('#') != string::npos) && (response.find('$') != string::npos))
					{
						if((response[0] == '#') && (response[response.length()-1] == '$'))
						{
							string acIdVal = response.substr(1,response.find('@')-1);
							#ifdef DEBUG
								cout << "acId = " << acId << endl;
							#endif
							response.erase(0,response.find('@')+1);
							
							string cmdVal = response.substr(0,response.find('@'));
							#ifdef DEBUG
								cout << "cmd = " << cmdVal << endl;
							#endif
							response.erase(0,response.find('@')+1);
														
							string status = response.substr(0,response.find('$'));
							#ifdef DEBUG
								cout << "status = " << status << endl;
							#endif
							response.erase(0,response.find('$')+1);
							
							if(status == "1")
							{
								if(fan == "auto")
								{
									#ifdef DEBUG
										cout << "UPDATE Climate_Status SET fan_mode='87',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';\n";
									#endif
									db.mysqlQuery("UPDATE Climate_Status SET fan_mode='87',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';");
								}
								else if(fan == "on")
								{
									#ifdef DEBUG
										cout << "UPDATE Climate_Status SET fan_mode='85',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';\n";
									#endif
									db.mysqlQuery("UPDATE Climate_Status SET fan_mode='85',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';");
								}
								break;
							}
						}
					}
				}
			}
			return 1;
		}
	}
	return 0;
}

bool modBus::fb(string locInsId)//fb
{
	//format : #[ac no]@[command no]@[request data]$
	string acId{},cmd{};
	testMysql db("Milan_SP");
	MYSQL_ROW rowfb{};
	db.mysqlQuery("SELECT node_id FROM Thermostat WHERE local_instance_id='"+locInsId+"' AND node_id IS NOT NULL;");
	while((rowfb = mysql_fetch_row(db.result)) != NULL)
	{
		acId = rowfb[0];
	}
	
	if(!acId.empty())
	{
		//cmd formation
		cmd = '#'+acId+"@8@$";//fb
				
		if(!cmd.empty())
		{
			string ip{},port{},response{};
			db.mysqlQuery("SELECT IP,port FROM IP_Port WHERE integration_id='110004' AND IP IS NOT NULL AND port IS NOT NULL;;");
			while((rowfb = mysql_fetch_row(db.result)) != NULL)
			{
				ip = rowfb[0];
				port = rowfb[1];
			}
			
			#ifdef DEBUG
				cout << "device ip:" << ip << endl;
				cout << "device port:" << port << endl;
				cout << "cmd:" << cmd << endl;
			#endif
			
			for(size_t loopNo1 =0; loopNo1 < 1; loopNo1++)
			{
				if(!ip.empty() && !port.empty() && (socketSend(ip,port,cmd,response) == 1))
				{
					#ifdef DEBUG
						cout << "response:" << response << endl;
					#endif
					if(response.find('-') != string::npos)
					{
						string devStatus{},setTemp{},curTemp{},curMode{};
						devStatus = response.substr(0,response.find('-'));
						#ifdef DEBUG
							cout << "devStatus = " << devStatus << endl;
						#endif
						response.erase(0,response.find('-')+1);
						
						setTemp = response.substr(0,response.find('-'));
						#ifdef DEBUG
							cout << "setTemp = " << setTemp << endl;
						#endif
						response.erase(0,response.find('-')+1);
						
						if(response.find('-') != string::npos)
						{
							curTemp = response;
						}
						else
						{
							curTemp = "NA";
						}
						
						//current mode
						db.mysqlQuery("SELECT mode FROM Climate_Status WHERE local_instance_id = '"+locInsId+"';");
						while((rowfb = mysql_fetch_row(db.result)) != NULL)
						{
							curMode = rowfb[0];
						}

						#ifdef DEBUG
							cout << "curTemp = " << curTemp << endl;
							cout << "curMode = " << curMode << endl;
						#endif

						if(((devStatus == "ON") && ((curMode == "50") || (curMode == "47") || (curMode == "49") || (curMode == "2"))) || (devStatus == "OFF"))
						{
							if((isInt(setTemp,10) && (stoi(setTemp) > 0))) //if((isInt(setTemp,10) && (stoi(setTemp) > 0)) && (isInt(curTemp,10) && (stoi(curTemp) > 0))) 
							{
								if(devStatus == "OFF")
								{
									#ifdef DEBUG
										cout << "UPDATE Climate_Status SET mode='2',current_temp='"+((!curTemp.empty())?curTemp:"NA")+"',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';\n";
									#endif
									db.mysqlQuery("UPDATE Climate_Status SET mode='2',current_temp='"+((!curTemp.empty())?curTemp:"NA")+"',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';");
								}
								else if(devStatus == "ON")
								{
									if(curMode == "50")
									{
										#ifdef DEBUG
											cout << "UPDATE Climate_Status SET auto_changeover_setpoint='"+setTemp+"',current_temp='"+((!curTemp.empty())?curTemp:"NA")+"',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';\n";
										#endif
										db.mysqlQuery("UPDATE Climate_Status SET auto_changeover_setpoint='"+setTemp+"',current_temp='"+((!curTemp.empty())?curTemp:"NA")+"',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';");
									}
									else if(curMode == "47")
									{
										#ifdef DEBUG
											cout << "UPDATE Climate_Status SET cool_setpoint='"+setTemp+"',current_temp='"+((!curTemp.empty())?curTemp:"NA")+"',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';\n";
										#endif
										db.mysqlQuery("UPDATE Climate_Status SET cool_setpoint='"+setTemp+"',current_temp='"+((!curTemp.empty())?curTemp:"NA")+"',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';");
									}
									else if(curMode == "49")
									{
										#ifdef DEBUG
											cout << "UPDATE Climate_Status SET heat_setpoint='"+setTemp+"',current_temp='"+((!curTemp.empty())?curTemp:"NA")+"',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';\n";
										#endif
										db.mysqlQuery("UPDATE Climate_Status SET heat_setpoint='"+setTemp+"',current_temp='"+((!curTemp.empty())?curTemp:"NA")+"',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';");
									}
									else if(curMode == "2")
									{
										#ifdef DEBUG
											cout << "UPDATE Climate_Status SET mode='47',cool_setpoint='"+setTemp+"',current_temp='"+((!curTemp.empty())?curTemp:"NA")+"',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';\n";
										#endif
										db.mysqlQuery("UPDATE Climate_Status SET mode='47',cool_setpoint='"+setTemp+"',current_temp='"+((!curTemp.empty())?curTemp:"NA")+"',auto_count = auto_count + 1, timestamp = CURRENT_TIMESTAMP WHERE local_instance_id = '"+locInsId+"';");
									}

								}
								curlRequest("http://localhost:6161/Milan/Drivers/MILAN_EXE/CLIMATE/climate_feed.cgi?l_instid="+locInsId,"GET");
							}
						}
					}
				}
			}
			return 1;
		}
	}
	return 0;
}

/******************************************************************************
 * Member function to send Json string										  *
 * ****************************************************************************/
bool modBus::socketSend(string ip,string port, string lwCmd, string &lwResp)
{
	int socketStatus,connectStatus,socketFd;  //socketFd -> socket descriptor for the socket connection.
	struct addrinfo host_info;       
	struct addrinfo *host_info_list; 
	
	bool retValue=0;
	
	struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
	
	memset(&host_info, 0, sizeof host_info);
	host_info.ai_family = AF_UNSPEC;     //IP version not specified, Can be both IPv4 and IPv6.
	host_info.ai_socktype = SOCK_STREAM; //Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
	
	//getting the status of the port on the ip.
	socketStatus = getaddrinfo(ip.c_str(), port.c_str(), &host_info, &host_info_list);
	if (socketStatus != 0)
	{
		//Could not get address info.
		#ifdef DEBUG
			cout << "Could not get Server info.\n\n";
		#endif
		freeaddrinfo(host_info_list);
		//address info not received return value
		return retValue;
	}
	else
	{
		//Opening the socket.
		socketFd = socket(host_info_list->ai_family, host_info_list->ai_socktype,host_info_list->ai_protocol);
		if (socketFd == -1)  
		{
			//socket error
			#ifdef DEBUG
				cout << "socket error.\n\n";
			#endif
			freeaddrinfo(host_info_list);
			//socket not created return value
			return retValue;
		}
		
		if (setsockopt (socketFd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(timeout)) < 0)
			return retValue;
		else
		{
			//Connecting to the socket.
			connectStatus = connect(socketFd, host_info_list->ai_addr, host_info_list->ai_addrlen);
			if (connectStatus == -1) 
			{
				//not connected
				#ifdef DEBUG
					cout << "Socket could not be connected.\n\n";
				#endif
				retValue = 0;
			}
			else
			{
				//connected
				if(!lwCmd.empty())
				{
					send(socketFd, lwCmd.c_str(), strlen(lwCmd.c_str()), 0);
					char resPonse[1000];
					int resp = recv(socketFd, resPonse,1000, 0);
					if (resp == -1)
					{
						/*-----receive error!-----*/
						#ifdef DEBUG
							cout<<"receive error"<<endl;
						#endif
						retValue = 0;
						sleep(1);
					}
					else
					{
						lwResp = resPonse;
						retValue = 1;
					}
				}
			}
		}
	}
	freeaddrinfo(host_info_list);
	close(socketFd);
	return retValue;
}

/*
				//curl request to get local_inst_id for feedback
				MYSQL_ROW row4;
				db.mysqlQuery("SELECT local_instance_id FROM Climate_Status WHERE id=(SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006');");
				string locInstId{};
				while(((row4 = mysql_fetch_row(db.result))))
				{
					locInstId = row4[0];
				}
				curlRequest("http://localhost:6161/Milan/Drivers/MILAN_EXE/CLIMATE/climate_feed.cgi?l_instid="+locInstId,"GET");
			}
			else
			{
				#ifdef DEBUG
					cout<<"CRC check failed.\n\n";
				#endif
			}
		}
	}
}*/

//friend function
size_t writeCallback(void *contents, size_t size, size_t nmemb, string *s)
{
    size_t newLength = size*nmemb;
    size_t oldLength = s->size();
    try
    {
        s->resize(oldLength + newLength);
    }
    catch(bad_alloc &e)
    {
        //handle memory problem
        return 0;
    }

    copy((char*)contents,(char*)contents+newLength,s->begin()+oldLength);
    return size*nmemb;
}

/****************************************************************************************
 * Function to call curl request.														*
 ****************************************************************************************/
void modBus::curlRequest(string webQuery,string type)
{
	#ifdef DEBUG
		cout << "webQuery:" << webQuery << ",\n";
		cout << "Query type:" << type << ",\n";
	#endif
	CURL *curl;
    CURLcode res;
	string curlResponse{};
    curl_global_init(CURL_GLOBAL_DEFAULT);
	
    curl = curl_easy_init();
    
    if(curl)
    {
		curl_easy_setopt(curl, CURLOPT_URL, webQuery.c_str());
		if(type == "POST")
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlResponse);
        
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
        {
			#ifdef DEBUG
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			#endif
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    #ifdef DEBUG
		cout << "curlResponse:" << curlResponse << ".\n\n";
	#endif
}

//class member to get device ip
string modBus::getMyIp(string interface)
{
	//declaration
	int fd;
	string ip{};
	struct ifreq ifr;
	
	//process
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
	ip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
	return ip;
}

//check for integer
bool modBus::isInt(const string& s, int base)
{
   if(s.empty() || std::isspace(s[0])) return false ;
   char * p ;
   int num = strtol(s.c_str(), &p, base) ;
   isdigit(num);
   return (*p == 0) ;
}
