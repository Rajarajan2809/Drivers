/*******************************************************************************
 *	Purpose		:	Scene Execution.							   			   *
 * 	Description	:	This is the header for Main Program to run the tests.	   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	30-Jul-16												   *
 * 	DOLM		:	3-Nov-16												   *
 *******************************************************************************/
 
#ifndef _sceneCgi_hpp
#define _sceneCgi_hpp

/*-----CPP library-----*/
#include <iostream>
#include <thread>		/*-----thread functions-----*/
#include <chrono>		/*------Used to perform time operations.-------*/
#include <vector>		/*------Replacement for arrays (vectors are dynamicallly allocated).-------*/

/*-----C library-----*/
#include <ctime>
#include <cassert>
#include <unistd.h>

/*-----Socket API library-----*/
#include <sys/socket.h> /*-----Needed for the socket functions-----*/
#include <netdb.h>      /*-----Needed for the socket functions-----*/

/*-----local header files-----*/
#include "../include/mysql.hpp"
#include "../include/cJSON.hpp"

/*-----CGI Headers-----*/
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

using namespace std;
using namespace cgicc;

/*-----Independant Functions-----*/
void timerSecs(int timeInSecs,string param1,string param2,string param3,string param4,string param5,string param6,string param7,string param8,string param9,string param10);

/*******************************************************************************
 * Class which perform Tests on IP2IR		                                   *
 *******************************************************************************/
class scene
{
	public:
			/*-----Public Member Declaration-----*/
			scene(string userId,string sceneId);
			~scene();
			int sceneExecution();
			testMysql t2;
			int dbCheck(string param1,string param2);
									
	private:
			/*-----Private Member declaration-----*/
			bool timeCheck(string startTimeInDb,string endTimeInDb,string sunRiseDelay,string sunSetDelay);
			string sceneId,userId;
			string getTime();
			bool isInt(const string& s, int base);
};

/*******************************************************************************
 * Constructor for Class scene.							 					   *
 *******************************************************************************/
scene::scene(string userId,string sceneId):t2("Milan_SP")
{
	/*-----Progress flag is set and Stop flag is reset-----*/
	#ifdef DEBUG
		cout << "Scene Execution Started at:" << getTime() << "\n\n";
	#endif
	this -> userId = userId;
	this -> sceneId = sceneId;
}

/*******************************************************************************
 * Destructor for Class testGateway.			 							   *
 *******************************************************************************/
scene::~scene()
{
	/*-----Progress flag is reset and strings related to Tests are terminated-----*/
	#ifdef DEBUG
		cout << "Scene Execution finished at:" << getTime() << "\n\n";
	#endif
	userId.clear();
	sceneId.clear();
}

/*******************************************************************************
 * Function to get the current time for log file.							   *
 *******************************************************************************/
string scene::getTime()
{
	chrono::time_point<std::chrono::system_clock> presentTimeChronoType;
	presentTimeChronoType = chrono::system_clock::now();
	time_t presentTime = chrono::system_clock::to_time_t(presentTimeChronoType);
	return ctime(&presentTime);
}

/*******************************************************************************
 * To check the string is number or alphabet								   *
 * *****************************************************************************/
bool isInt(const string& s, int base)
{
   if(s.empty() || std::isspace(s[0])) return false ;
   char * p ;
   int num = strtol(s.c_str(), &p, base) ;
   isdigit(num);
   return (*p == 0) ;
}

/*******************************************************************************
 * Enum to compare "category" of string	type								   *
 * *****************************************************************************/
enum categoryEnum 
{
	categoryLight,
	categoryCurtain,
	categoryColorLight,
	categoryIR,
	categoryChannels,
	categoryAppliances,
	categoryMedia,
	categoryComfort,
	categoryScene,
	categoryNone
};

/*******************************************************************************
 * Enum to string mapping													   *
 * *****************************************************************************/
categoryEnum strToEnumMapping (string inString)
{
	if(inString == "Light") 
		return categoryLight;
	else if(inString == "Curtain") 
		return categoryCurtain;
	else if(inString == "Color Light") 
		return categoryColorLight;
	else if(inString == "IR") 
		return categoryIR;
	else if(inString == "Channels") 
		return categoryChannels;
	else if(inString == "Appliances") 
		return categoryAppliances;
	else if(inString == "Media")
		return categoryMedia;
	else if(inString == "Comfort")
		return categoryComfort;
	else if(inString == "Scene")
		return categoryScene;
	else 
		return categoryNone;		
}

/******************************************************************************
 * class to send Json string												  *
 * ****************************************************************************/
class jsonSend
{
	public:
		string jsonCreate(string category,string userId,string localInstanceId,string integrationId,string param1,string param2,string param3,string param4,string param5,string param6,string param7,string param8,string param9,string param10);
		int socketSend(string jsonData);
		int socketSendSingleJson(string jsonData);
};

/******************************************************************************
 * Member function to create Json string									  *
 * ****************************************************************************/
string jsonSend::jsonCreate(string category,string userId,string localInstanceId,string integrationId,string param1,string param2,string param3,string param4,string param5,string param6,string param7,string param8,string param9,string param10)
{
	cJSON *root,*arr,*fmt,*args;
	string jsonSent;	
	/*-----case value for success-----*/
	root=cJSON_CreateObject();
	if(category != "Scene")
		cJSON_AddItemToObject(root, "method",cJSON_CreateString("set"));
	else
		cJSON_AddItemToObject(root, "method",cJSON_CreateString("feedback all"));
	cJSON_AddItemToObject(root, "params",arr=cJSON_CreateArray());
	cJSON_AddItemToArray(arr,fmt=cJSON_CreateObject());
	switch(strToEnumMapping (category))
	{
		case categoryLight:
			{
				cJSON_AddStringToObject(fmt,"category",category.c_str());
				cJSON_AddStringToObject(fmt,"user_id",userId.c_str());
				cJSON_AddStringToObject(fmt,"inst_id",localInstanceId.c_str());
				cJSON_AddStringToObject(fmt,"model",integrationId.c_str());
				const char * arg[2];
				arg[0] = param1.c_str();
				arg[1] =param2.c_str();
				cJSON_AddItemToObject(fmt, "args", args=cJSON_CreateStringArray(arg,2));
			}
			break;
		
		case categoryCurtain:
			{
				cJSON_AddStringToObject(fmt,"category",category.c_str());
				cJSON_AddStringToObject(fmt,"user_id",userId.c_str());
				cJSON_AddStringToObject(fmt,"inst_id",localInstanceId.c_str());
				cJSON_AddStringToObject(fmt,"model",integrationId.c_str());
				const char * arg[2];
				arg[0] = param1.c_str();
				arg[1] =param2.c_str();
				cJSON_AddItemToObject(fmt, "args", args=cJSON_CreateStringArray(arg,2));
			}	
			break;
		
		case categoryColorLight:
			{
				cJSON_AddStringToObject(fmt,"category","RGB");
				cJSON_AddStringToObject(fmt,"user_id",userId.c_str());
				cJSON_AddStringToObject(fmt,"inst_id",localInstanceId.c_str());
				cJSON_AddStringToObject(fmt,"control_id",param1.c_str());
				cJSON_AddStringToObject(fmt,"model",integrationId.c_str());
				cJSON_AddStringToObject(fmt,"type",param3.c_str());
				if(param3 == "0")
				{
					//type 0 but Specific color
					string ar[2];
					ar[0] = param2.substr(0,param2.find(","));
					param2.erase(0,param2.find(",")+1);
					ar[1] = param2.substr(0,param2.find(","));
					param2.erase(0,param2.find(",")+1);
					
					const char * arg[3];
					arg[0] =ar[0].c_str();
					arg[1] = ar[1].c_str();
					arg[2] = param2.c_str();
					cJSON_AddItemToObject(fmt, "args", args=cJSON_CreateStringArray(arg,3));
				}
				else if(param3 == "1")
				{
					const char * arg[1];
					arg[0] = param2.c_str();
					cJSON_AddItemToObject(fmt, "args", args=cJSON_CreateStringArray(arg,1));
				}
				else if(param3 == "2")
				{
					if(param2 == "RND")
					{
						//type 2 but Random
						const char * arg[1];
						arg[0] = "1";
						cJSON_AddItemToObject(fmt, "args", args=cJSON_CreateStringArray(arg,1));
					}
				}	
			}
			break;
		
		case categoryIR:
			{
				cJSON_AddStringToObject(fmt,"category",category.c_str());
				cJSON_AddStringToObject(fmt,"user_id",userId.c_str());
				cJSON_AddStringToObject(fmt,"inst_id",localInstanceId.c_str());
				cJSON_AddStringToObject(fmt,"model",integrationId.c_str());
				const char * arg[1];
				cJSON_AddStringToObject(fmt,"control_id",param1.c_str());
				arg[0] =param2.c_str();
				cJSON_AddItemToObject(fmt, "args", args=cJSON_CreateStringArray(arg,1));
			}	
			break;
		
		case categoryChannels:
			{
				cJSON_AddStringToObject(fmt,"category",category.c_str());
				cJSON_AddStringToObject(fmt,"user_id",userId.c_str());
				cJSON_AddStringToObject(fmt,"inst_id",localInstanceId.c_str());
				cJSON_AddStringToObject(fmt,"model",integrationId.c_str());
				const char * arg[1];
				cJSON_AddStringToObject(fmt,"control_id",param1.c_str());
				arg[0] =param2.c_str();
				cJSON_AddItemToObject(fmt, "args", args=cJSON_CreateStringArray(arg,1));
			}
			break;
		
		case categoryAppliances:
			{
				cJSON_AddStringToObject(fmt,"category",category.c_str());
				cJSON_AddStringToObject(fmt,"user_id",userId.c_str());
				cJSON_AddStringToObject(fmt,"inst_id",localInstanceId.c_str());
				cJSON_AddStringToObject(fmt,"model",integrationId.c_str());
				const char * arg[2];
				cJSON_AddStringToObject(fmt,"control_id",param1.c_str());
				arg[0] =param2.c_str();
				arg[1] =param3.c_str();
				cJSON_AddItemToObject(fmt, "args", args=cJSON_CreateStringArray(arg,2));
			}
			break;
		
		case categoryMedia:
			{
				cJSON_AddStringToObject(fmt,"category",category.c_str());
				cJSON_AddStringToObject(fmt,"user_id",userId.c_str());
				cJSON_AddStringToObject(fmt,"inst_id",localInstanceId.c_str());
				cJSON_AddStringToObject(fmt,"model",integrationId.c_str());
				const char * arg[8];
				arg[0] =param1.c_str();
				arg[1] =param2.c_str();
				arg[2] =param3.c_str();
				arg[3] =param4.c_str();
				arg[4] =param5.c_str();
				arg[5] =param6.c_str();
				arg[6] =param7.c_str();
				arg[7] =param8.c_str();
				cJSON_AddItemToObject(fmt, "args", args=cJSON_CreateStringArray(arg,8));
			}
			break;
			
		case categoryComfort:
			{
				cJSON_AddStringToObject(fmt,"category","comfort");
				cJSON_AddStringToObject(fmt,"user_id",userId.c_str());
				cJSON_AddStringToObject(fmt,"control_id",param1.c_str());
				cJSON_AddStringToObject(fmt,"inst_id",localInstanceId.c_str());
				cJSON_AddStringToObject(fmt,"model",integrationId.c_str());
				const char * arg[6];
				arg[0] =param2.c_str();
				arg[1] =param3.c_str();
				arg[2] =param4.c_str();
				arg[3] =param5.c_str();
				arg[4] =param6.c_str();
				arg[5] =param7.c_str();
				cJSON_AddItemToObject(fmt, "args", args=cJSON_CreateStringArray(arg,6));
			}
			break;
		
		case categoryScene:
			{
				cJSON_AddStringToObject(fmt,"category",category.c_str());
				cJSON_AddStringToObject(fmt,"user_id",userId.c_str());
				cJSON_AddStringToObject(fmt,"room_id",localInstanceId.c_str());
				cJSON_AddStringToObject(fmt,"scene_id",param1.c_str());
				cJSON_AddStringToObject(fmt,"type",param2.c_str());
			}
			break;	
			
		case categoryNone:
			#ifdef DEBUG
				cout << "Invalid category." << "\n\n";
			#endif
			break;						
	}

	cJSON_Print(root);
	jsonSent=cJSON_PrintUnformatted(root);
	cJSON_Delete(root);	
	return jsonSent;
}

/******************************************************************************
 * Member function to send Json string										  *
 * ****************************************************************************/
int jsonSend::socketSend(string jsonData)
{
	/*-----Tcp Client for sending IR commands.-----*/
	int socketStatus,connectStatus,socketFd;  /*-----socketFd -> socket descriptor for the socket connection.-----*/
	struct addrinfo host_info;       
	struct addrinfo *host_info_list; 
	
	memset(&host_info, 0, sizeof host_info);
	host_info.ai_family = AF_UNSPEC;     /*-----IP version not specified, Can be both IPv4 and IPv6.-----*/
	host_info.ai_socktype = SOCK_STREAM; /*-----Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.-----*/
	
	/*-----getting the status of the port on the ip.-----*/
	socketStatus = getaddrinfo("localhost", "64527", &host_info, &host_info_list);
	if (socketStatus != 0)
	{
		/*-----Could not get address info.-----*/
		#ifdef DEBUG
			cout << "JSON Server(mass-exec):Could not get address info.\n\n";
		#endif
		freeaddrinfo(host_info_list);
		/*-----address info not received return value-----*/
		return 1;
	}
	else
	{
		/*-----address info received-----*/
	}	
		
	/*-----Opening the socket.-----*/
	socketFd = socket(host_info_list->ai_family, host_info_list->ai_socktype,host_info_list->ai_protocol);
	if (socketFd == -1)  
	{
		/*-----socket error-----*/
		#ifdef DEBUG
			cout << "JSON Server(mass-exec):socket error.\n\n";
		#endif
		freeaddrinfo(host_info_list);
		/*-----socket not created return value-----*/
		return 2;
	}
	
	/*-----Connecting to the socket.-----*/
	connectStatus = connect(socketFd, host_info_list->ai_addr, host_info_list->ai_addrlen);
	if (connectStatus == -1) 
	{
		/*-----not connected-----*/
		#ifdef DEBUG
			cout << "JSON Server(mass-exec):not connected.\n\n";
		#endif
		close(socketFd);
		freeaddrinfo(host_info_list);
		/*-----socket not connected return value-----*/
		return 3;
	}
	else
	{
		/*-----connected-----*/
	}
	
	send(socketFd, jsonData.c_str(), strlen(jsonData.c_str()), 0);
	freeaddrinfo(host_info_list);
	close(socketFd);
	return 4;
}	

/******************************************************************************
 * Member function to send Json string										  *
 * ****************************************************************************/
int jsonSend::socketSendSingleJson(string jsonData)
{
	/*-----Tcp Client for sending IR commands.-----*/
	int socketStatus,connectStatus,socketFd;  /*-----socketFd -> socket descriptor for the socket connection.-----*/
	struct addrinfo host_info;       
	struct addrinfo *host_info_list; 
	
	memset(&host_info, 0, sizeof host_info);
	host_info.ai_family = AF_UNSPEC;     /*-----IP version not specified, Can be both IPv4 and IPv6.-----*/
	host_info.ai_socktype = SOCK_STREAM; /*-----Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.-----*/
	
	/*-----getting the status of the port on the ip.-----*/
	socketStatus = getaddrinfo("localhost", "64526", &host_info, &host_info_list);
	if (socketStatus != 0)
	{
		/*-----Could not get address info.-----*/
		#ifdef DEBUG
			cout << "JSON Server(norm-exec):Could not get address info.\n\n";
		#endif
		freeaddrinfo(host_info_list);
		/*-----address info not received return value-----*/
		return 1;
	}
	else
	{
		/*-----address info received-----*/
	}	
		
	/*-----Opening the socket.-----*/
	socketFd = socket(host_info_list->ai_family, host_info_list->ai_socktype,host_info_list->ai_protocol);
	if (socketFd == -1)  
	{
		/*-----socket error-----*/
		#ifdef DEBUG
			cout << "JSON Server(mass-exec):socket error.\n\n";
		#endif
		freeaddrinfo(host_info_list);
		/*-----socket not created return value-----*/
		return 2;
	}
	
	/*-----Connecting to the socket.-----*/
	connectStatus = connect(socketFd, host_info_list->ai_addr, host_info_list->ai_addrlen);
	if (connectStatus == -1) 
	{
		/*-----not connected-----*/
		#ifdef DEBUG
			cout << "JSON Server(mass-exec):not connected.\n\n";
		#endif
		close(socketFd);
		freeaddrinfo(host_info_list);
		/*-----socket not connected return value-----*/
		return 3;
	}
	else
	{
		/*-----connected-----*/
	}
	
	send(socketFd, jsonData.c_str(), strlen(jsonData.c_str()), 0);
	freeaddrinfo(host_info_list);
	close(socketFd);
	return 4;
}

/*******************************************************************************
 * Function to run a threaded timer.										   *
 * timerTh.detach();should be used in program								   *
 * allowing timer to run independently						    			   *
 *******************************************************************************/
void timerSecs(int timeInSecs,string param1,string param2,string param3,string param4,string param5,string param6,string param7,string param8,string param9,string param10)
{
	/*-----timer_flag=1;-----*/
	/*-----cout <<"timer_flag--->"<<timer_flag<<endl;-----*/
	/*-----if timer is used in program "timer_flag" should be decclared.-----*/
	//for(int i=0;i<timeInSecs;i++)
		//this_thread::sleep_for (chrono::seconds(1));
	sleep(timeInSecs+2);	
	jsonSend s1;
	#ifdef DEBUG
		cout<<"Thread json:"<<s1.jsonCreate(param1,param2,param3,param4,param5,param6,param7,param8,param9,param10,"","","","")<<".\n\n";
	#endif
	s1.socketSend(s1.jsonCreate(param1,param2,param3,param4,param5,param6,param7,param8,param9,param10,"","","",""));
}	

//check for integer
bool scene::isInt(const string& s, int base)
{
   if(s.empty() || std::isspace(s[0])) return false ;
   char * p ;
   int num = strtol(s.c_str(), &p, base) ;
   isdigit(num);
   return (*p == 0) ;
}
#endif
