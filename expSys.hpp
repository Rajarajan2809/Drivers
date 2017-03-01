/*******************************************************************************
 *	Purpose		:	Expert System.											   *
 * 	Description	:	Header file for Expert System.							   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	2-Jul-16												   *
 * 	DOLM		:	1-Nov-16												   *
 *******************************************************************************/
/*******************************************************************************
allTestFlag variable value 1 -> DB update enable
allTestFlag variable value 0 -> DB update disable

Member function return value -> 1 //success
Member function return value -> 0 //Neither success nor failure
Member function return value -> 2 //failure

********************************************************************************/
#ifndef _expSys_hpp
#define _expSys_hpp

/*-----CPP Headers-----*/
#include <iostream>
#include <vector>
#include <string>
#include <chrono>

/*-----CGI Headers-----*/
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

/*-----Curl Library-----*/
#include <curl/curl.h>

/*-----Local library-----*/
#include "../include/mysql.hpp"
#include "../include/cJSON.hpp"

/*-----Socket API library-----*/
#include <sys/socket.h> /*-----Needed for the socket functions-----*/
#include <netdb.h>      /*-----Needed for the socket functions-----*/
#include <netinet/in.h>
#include <arpa/inet.h>

/*-----C library-----*/
#include <cstring> 		/*-----Library for C string Operations-----*/
#include <cstdlib>		/*-----C standard library-----*/
#include <cstddef>		/*-----C library for standard definitions-----*/
#include <unistd.h>

using namespace std;
using namespace cgicc;

class expSys
{
	bool jsonCreate(string userId, string instanceId, string localInstanceId, string testId, string propertyId, string status, string respId, string battery, string inactvTime, string friendlyName, string process);
	bool tcpCmd(string ip,string port,string cmd);
	long int timeCheck(string startTimeInDb);
	string userId,instanceId,propertyId,propertyValue,testId,respId,allTestFlag;
	void curlRequests(string webAddress,string type);
	bool curlRequests(string webAddress,string method,string item,string key,string keyValue,int id);
	bool isInt(const string& s, int base);
	int playerId;
		
	public:
		int pingIp();
		int pingDns();
		int pingRouter();
		int pingIpConflict();
		int zwaveDeviceStatus();
		int zwaveBatteryStatus();
		expSys(string userId,string instanceId,string propertyId,string propertyValue,string testId, string respId);
		expSys();
		testMysql t2;
		~expSys();
		void lanCheck();
		int irTest();
		int rgbTest();
		int relayTest();
		int climaxTest();
};

expSys::expSys(string userId,string instanceId,string propertyId,string propertyValue, string testId, string respId) : t2("Milan_SP")
{
	this -> userId = userId;
	this -> instanceId = instanceId;
	this -> propertyId = propertyId;
	this -> propertyValue = propertyValue;
	this -> testId = testId;
	this -> respId = respId;
	allTestFlag = "1";
}

expSys::expSys() : t2("Milan_SP")
{
}

expSys::~expSys()
{
	userId.clear();
	instanceId.clear();
	propertyId.clear();
	propertyValue.clear();
	respId.clear();
	allTestFlag.clear();
}

/******************************************************************************
 * Member function to create Json string									  *
 * ****************************************************************************/
bool expSys::jsonCreate(string userId, string instanceId, string localInstanceId, string testId, string propertyId, string status, string respId, string battery, string inactvTime, string friendlyName, string process)
{
	cJSON *root;
	string jsonSent;	
	
	//-----JSON string building process-----//
	root=cJSON_CreateObject();
	cJSON_AddStringToObject(root,"method","troubleshoot");
	cJSON_AddStringToObject(root,"category","expert_system");
	cJSON_AddStringToObject(root,"user_id",userId.c_str());
	cJSON_AddStringToObject(root,"inst_id",instanceId.c_str());
	cJSON_AddStringToObject(root,"local_inst_id",localInstanceId.c_str());
	cJSON_AddStringToObject(root,"test_id",testId.c_str());
	if((testId == "TS0051")||(testId == "TS0031"))
		cJSON_AddStringToObject(root,"ip",battery.c_str());
	cJSON_AddStringToObject(root,"friendly_name",friendlyName.c_str());
	cJSON_AddStringToObject(root,"property_id",propertyId.c_str());
	cJSON_AddStringToObject(root,"status",status.c_str());
	if((testId == "TS0028")||(testId == "TS0029")||(testId == "TS0030")||(testId == "TS0051")||(testId == "TS0031"))
		cJSON_AddStringToObject(root,"process",process.c_str());
	if((testId == "TS0050")||(testId == "TS0049"))
	{
		if(!battery.empty() && !inactvTime.empty())
		{	
			cJSON_AddStringToObject(root,"battery",battery.c_str());
			cJSON_AddStringToObject(root,"inactive_duration",inactvTime.c_str());
		}
		else
		{
			cJSON_AddStringToObject(root,"battery","NA");
			cJSON_AddStringToObject(root,"inactive_duration","NA");
		}
	}
	cJSON_AddStringToObject(root,"response_id",respId.c_str());
	
	cJSON_Print(root);
	jsonSent=cJSON_PrintUnformatted(root);

	cJSON_Delete(root);
	
	#ifdef DEBUG
		cout<<"JSON		:" << jsonSent << ".\n\n";
	#endif
	
	t2.mysqlQuery("UPDATE `ES_Response` SET `response_message`='" + jsonSent + "', `response_status`=1 WHERE `response_id`='" + respId + "';");
	return 1;
}

/******************************************************************************
 * Member function to send Json string										  *
 * ****************************************************************************/
bool expSys::tcpCmd(string ip,string port,string cmd)
{
	/*-----Tcp Client for sending IR commands.-----*/
	int socketStatus,connectStatus,socketFd;  /*-----socketFd -> socket descriptor for the socket connection.-----*/
	struct addrinfo host_info;       
	struct addrinfo *host_info_list; 
	
	bool retValue=0;
	
	struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
	
	memset(&host_info, 0, sizeof host_info);
	host_info.ai_family = AF_UNSPEC;     /*-----IP version not specified, Can be both IPv4 and IPv6.-----*/
	host_info.ai_socktype = SOCK_STREAM; /*-----Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.-----*/
	
	/*-----getting the status of the port on the ip.-----*/
	socketStatus = getaddrinfo(ip.c_str(), port.c_str(), &host_info, &host_info_list);
	if (socketStatus != 0)
	{
		/*-----Could not get address info.-----*/
		#ifdef DEBUG
			cout << "Could not get Server info.\n\n";
		#endif
		freeaddrinfo(host_info_list);
		/*-----address info not received return value-----*/
		return retValue;
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
			cout << "socket error.\n\n";
		#endif
		freeaddrinfo(host_info_list);
		/*-----socket not created return value-----*/
		return retValue;
	}
	
	if (setsockopt (socketFd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(timeout)) < 0)
		return retValue;
    
	/*-----Connecting to the socket.-----*/
	connectStatus = connect(socketFd, host_info_list->ai_addr, host_info_list->ai_addrlen);
	if (connectStatus == -1) 
	{
		/*-----not connected-----*/
		#ifdef DEBUG
			cout << "Socket could not be connected.\n\n";
		#endif
		retValue = 0;
	}
	else
	{
		/*-----connected-----*/
		if(!cmd.empty())
			send(socketFd, cmd.c_str(), strlen(cmd.c_str()), 0);
		retValue = 1;
	}
	
	sleep(1);
	freeaddrinfo(host_info_list);
	close(socketFd);
	return retValue;
}

/****************************************************************************************
 * Function to call curl request.														*
 ****************************************************************************************/
void expSys::curlRequests(string webAddress,string type)
{
	CURL *curl;
    CURLcode res;
	curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    
    if(curl)
    {
		curl_easy_setopt(curl, CURLOPT_URL, webAddress.c_str());
		if(type == "POST")
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
        

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
}

bool expSys::isInt(const string& s, int base)
{
   if(s.empty() || std::isspace(s[0])) return false ;
   char * p ;
   int num = strtol(s.c_str(), &p, base) ;
   isdigit(num);
   return (*p == 0) ;
}

int expSys::pingIp()
{
	//Ping IP DUT function.
	//cout <<"Send the Ping Command to Device Under Test of Instance ID: "<<instanceId<<"\n";
	string cmd = "ping -W4 -c4 -A ",ip,localInstanceId,friendlyName,port;
	MYSQL_ROW row1;
	t2.mysqlQuery("SELECT IPD.ip AS ip, IPD.port AS port, UAI.local_instance_id, IPD.name AS name FROM C_User_Addon_Instances AS UAI LEFT JOIN IR_Devices AS IRD ON IRD.local_instance_id = UAI.local_instance_id LEFT JOIN IP_IR_Relay AS IIR ON IIR.ir_device_id = IRD.id LEFT JOIN IP_Devices AS IPD ON IPD.id = IIR.device_id WHERE UAI.instance_id = '"+instanceId+"' AND IPD.name IS NOT NULL AND IPD.ip IS NOT NULL AND IPD.port IS NOT NULL AND UAI.local_instance_id IS NOT NULL UNION ALL SELECT IPD.ip AS ip, IPD.port AS port, UAI.local_instance_id AS local_instance_id, IPD.name AS name FROM C_User_Addon_Instances AS UAI LEFT JOIN Relay_Devices AS RD ON RD.local_instance_id = UAI.local_instance_id LEFT JOIN IP_IR_Relay AS IIR ON IIR.ir_device_id = RD.id LEFT JOIN IP_Devices AS IPD ON IPD.id = IIR.device_id WHERE UAI.instance_id = '"+instanceId+"' AND IPD.name IS NOT NULL AND IPD.ip IS NOT NULL AND IPD.port IS NOT NULL AND UAI.local_instance_id IS NOT NULL UNION ALL SELECT ID.ip AS ip, ID.port AS port, UAI.local_instance_id AS local_instance_id, ID.name AS name FROM C_User_Addon_Instances AS UAI LEFT JOIN Relay_Devices AS RD ON RD.local_instance_id = UAI.local_instance_id LEFT JOIN IP_IR_Relay AS IIR ON IIR.ir_device_id = RD.id LEFT JOIN IP_Devices AS ID ON ID.id = IIR.device_id WHERE UAI.instance_id = '"+instanceId+"' AND ID.ip IS NOT NULL AND ID.port IS NOT NULL AND ID.local_instance_id IS NOT NULL AND ID.name IS NOT NULL UNION ALL SELECT L.ref_1 AS ip, L.ref_2 AS port, UAI.local_instance_id AS local_instance_id, L.name AS name FROM C_User_Addon_Instances AS UAI LEFT JOIN Lighting AS L ON L.local_instance_id = UAI.local_instance_id WHERE UAI.instance_id = '"+instanceId+"' AND L.category = 'Color Light' AND L.integration_id = '40000' AND L.name IS NOT NULL AND L.ref_1 IS NOT NULL AND L.ref_2 IS NOT NULL AND UAI.local_instance_id IS NOT NULL UNION ALL SELECT M.ip AS ip, M.port AS port, M.local_instance_id AS local_instance_id, M.name AS name FROM C_User_Addon_Instances AS UAI LEFT JOIN Music AS M ON M.local_instance_id = UAI.local_instance_id WHERE UAI.instance_id = '"+instanceId+"' AND M.ip IS NOT NULL AND M.port IS NOT NULL AND M.local_instance_id IS NOT NULL AND M.name IS NOT NULL UNION ALL SELECT S.lan_ip, S.port, S.local_instance_id, S.camera_name FROM C_User_Addon_Instances AS UAI LEFT JOIN Surveilance AS S ON S.local_instance_id = UAI.local_instance_id WHERE UAI.instance_id = '"+instanceId+"' AND S.lan_ip IS NOT NULL AND S.port IS NOT NULL AND S.local_instance_id IS NOT NULL AND S.camera_name IS NOT NULL;");
	while ((row1 = mysql_fetch_row(t2.result)))
	{
		ip = row1[0];
		port = row1[1];
		localInstanceId	= row1[2];
		friendlyName = row1[3];
	}
	#ifdef DEBUG
		cout <<"ip					:" << ip << ",\n";
		cout <<"port				:" << port<< ",\n";
		cout <<"localInstanceId		:" << localInstanceId << ",\n";
		cout <<"friendlyName		:" << friendlyName << ".\n\n";
	#endif
	if(!ip.empty() && !port.empty() && !localInstanceId.empty() && !friendlyName.empty() && !respId.empty())
	{
		cmd = cmd +ip;
	
		//Terminal "Ping" sent via system() command.
		#ifdef DEBUG
			cout << "cmd					:" << cmd << ".\n\n";
		#endif
		switch(system(cmd.c_str()))
		{
			case 0:
				{	
					if(allTestFlag != "0")
						jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"1",respId,"","",friendlyName,"");
					return 1;
				}
				break;
					
			default:
				{
					if(allTestFlag != "0")
						jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",friendlyName,"");
					return 0;
				}
				break;
		}
	}
	else
	{
		#ifdef DEBUG
			cout << "Device Instance id is Invalid.\n\n";
		#endif
	}
	return 2;
}

int expSys::pingIpConflict()
{
	//Ping IP DUT function.
	string ip,localInstanceId,port,friendlyName;
	MYSQL_ROW row1;
	t2.mysqlQuery("SELECT IPD.ip AS ip, IPD.port AS port, UAI.local_instance_id, IPD.name AS name FROM C_User_Addon_Instances AS UAI LEFT JOIN IR_Devices AS IRD ON IRD.local_instance_id = UAI.local_instance_id LEFT JOIN IP_IR_Relay AS IIR ON IIR.ir_device_id = IRD.id LEFT JOIN IP_Devices AS IPD ON IPD.id = IIR.device_id WHERE UAI.instance_id = '"+instanceId+"' AND IPD.name IS NOT NULL AND IPD.ip IS NOT NULL AND IPD.port IS NOT NULL AND UAI.local_instance_id IS NOT NULL UNION ALL SELECT IPD.ip AS ip, IPD.port AS port, UAI.local_instance_id AS local_instance_id, IPD.name AS name FROM C_User_Addon_Instances AS UAI LEFT JOIN Relay_Devices AS RD ON RD.local_instance_id = UAI.local_instance_id LEFT JOIN IP_IR_Relay AS IIR ON IIR.ir_device_id = RD.id LEFT JOIN IP_Devices AS IPD ON IPD.id = IIR.device_id WHERE UAI.instance_id = '"+instanceId+"' AND IPD.name IS NOT NULL AND IPD.ip IS NOT NULL AND IPD.port IS NOT NULL AND UAI.local_instance_id IS NOT NULL UNION ALL SELECT ID.ip AS ip, ID.port AS port, UAI.local_instance_id AS local_instance_id, ID.name AS name FROM C_User_Addon_Instances AS UAI LEFT JOIN Relay_Devices AS RD ON RD.local_instance_id = UAI.local_instance_id LEFT JOIN IP_IR_Relay AS IIR ON IIR.ir_device_id = RD.id LEFT JOIN IP_Devices AS ID ON ID.id = IIR.device_id WHERE UAI.instance_id = '"+instanceId+"' AND ID.ip IS NOT NULL AND ID.port IS NOT NULL AND ID.local_instance_id IS NOT NULL AND ID.name IS NOT NULL UNION ALL SELECT L.ref_1 AS ip, L.ref_2 AS port, UAI.local_instance_id AS local_instance_id, L.name AS name FROM C_User_Addon_Instances AS UAI LEFT JOIN Lighting AS L ON L.local_instance_id = UAI.local_instance_id WHERE UAI.instance_id = '"+instanceId+"' AND L.category = 'Color Light' AND L.integration_id = '40000' AND L.name IS NOT NULL AND L.ref_1 IS NOT NULL AND L.ref_2 IS NOT NULL AND UAI.local_instance_id IS NOT NULL UNION ALL SELECT M.ip AS ip, M.port AS port, M.local_instance_id AS local_instance_id, M.name AS name FROM C_User_Addon_Instances AS UAI LEFT JOIN Music AS M ON M.local_instance_id = UAI.local_instance_id WHERE UAI.instance_id = '"+instanceId+"' AND M.ip IS NOT NULL AND M.port IS NOT NULL AND M.local_instance_id IS NOT NULL AND M.name IS NOT NULL UNION ALL SELECT S.lan_ip, S.port, S.local_instance_id, S.camera_name FROM C_User_Addon_Instances AS UAI LEFT JOIN Surveilance AS S ON S.local_instance_id = UAI.local_instance_id WHERE UAI.instance_id = '"+instanceId+"' AND S.lan_ip IS NOT NULL AND S.port IS NOT NULL AND S.local_instance_id IS NOT NULL AND S.camera_name IS NOT NULL;");
	while ((row1 = mysql_fetch_row(t2.result)))
	{
		ip = row1[0];
		port = row1[1];
		localInstanceId	= row1[2];
		friendlyName = row1[3];
	}
	#ifdef DEBUG
		cout<<"ip					:" << ip << ",\n";
		cout<<"port					:" << port << ",\n";
		cout<<"localInstanceId		:" << localInstanceId << ",\n";
		cout<<"friendlyName			:" << friendlyName << ".\n\n";
	#endif
	if(!ip.empty() && !port.empty() && !localInstanceId.empty() && !friendlyName.empty() && !respId.empty())
	{
		//Terminal "Ping" sent via system() command.
		switch(tcpCmd(ip,port,""))
		{
			case 0:
				//IP conflict exists
				{	
					if(allTestFlag != "0")
						jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",friendlyName,"");
					return 0;
				}
				break;
					
			case 1:
				//IP conflict does not exists
				{
					if(allTestFlag != "0")
						jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"1",respId,"","",friendlyName,"");
					return 1;
				}
				break;
		}
	}
	else
	{
		#ifdef DEBUG
			cout << "Device Instance id is Invalid.\n\n";
		#endif
	}
	return 0;
}

#endif
