/*******************************************************************************
 *	Purpose		:	Trigger Execution in Master and Slave Devices.			   *
 * 	Description	:	This is the Header for Trigger Execution.				   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	2-Jun-16												   *
 * 	DOLM		:	26-Dec-16												   *
 *******************************************************************************/

#ifndef _trigger_hpp
#define _trigger_hpp

/*-----CPP library-----*/
#include <iostream>
#include <thread>		/*-----thread functions-----*/
#include <chrono>		/*------Used to perform time operations.-------*/
#include <vector>		/*------Replacement for arrays (vectors are dynamicallly allocated).-------*/
#include <ctime>
#include <cassert>
#include <algorithm>
#include <string>

/*-----Curl Library-----*/
#include <curl/curl.h>

/*-----Socket API library-----*/
#include <sys/socket.h> /*-----Needed for the socket functions-----*/
#include <netdb.h>      /*-----Needed for the socket functions-----*/
#include <unistd.h>      /*-----Needed for the socket functions-----*/

/*-----local header files-----*/
#include "../include/mysql.hpp"
#include "../include/cJSON.hpp"


/*-----CGI Headers-----*/
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

/*-----P2P Headers -----*/
#include "../include/PPCS/PPCS_API.h"
#include "../include/AVSTREAM_IO_Proto.h"

using namespace std;
using namespace cgicc;

/*-----Independant Functions-----*/
void timerSecs(int timeInSecs,string param1,string param2,string param3,string param4,string param5,string param6,string param7,string param8,string param9,string param10,string param11,string param12,string param13,string param14);
bool isInt(const string& s, int base);
size_t CurlWrite_CallbackFunc_StdString(void *contents, size_t size, size_t nmemb, string *s);


enum integrationIdEnum
{
	categoryMedia, //Milan climax, Nuvo
	categoryIPIR, //IP2IR
	categoryAppliance, //IP2Relay
	categoryColorLight, //RGB
	categoryCurtain, //Curtain
	categoryLight, //Light
	categorySecurity, //sensor
	categoryVisitor, //Visitor
	categoryDoorLock, //DoorLock
	categoryThermostat, //Thermostat
	categoryIR,	//Cable box, television
	categoryScene,//Moodpro scene
	categoryNone //None of the above category
};

class trigger
{
	public:
		int triggerExecution(string sensorLocalInstanceId,string sensorPropertyId,string sensorPropertyValue,string sceneId,string dlUserId);
		int triggerExecution(vector<string> instanceId,vector<string> propertyId,vector<string> propertyValue,vector<string> userId,vector<string> delay,vector<string> type,vector<string> ref1);
		trigger();
		~trigger();
		testMysql t2;

	private:
		bool timeCheck(string startTimeInDb,string endTimeInDb);
		bool dayCheck(string daysOfWeek);
		void p2pData(string sceneId);
		string getTime();
		string rgb(string color);
		string urlEncode(string encodeData);
};

/******************************************************************************
 * class to send Json string												  *
 * ****************************************************************************/
class jsonSend
{
	public:
		string jsonCreate(string category,string userId,string instanceId,string integrationId,string param1,string param2,string param3,string param4,string param5,string param6,string param7,string param8,string param9,string param10);
		int socketSend(string jsonData);
		void curlRequests(string webQuery,string type1,string type2);
		int p2pSend(string did,string& jsonData);
		integrationIdEnum strToEnumMapping (string inString);
		jsonSend();
		~jsonSend();
		testMysql t3;
};

//jsonSend Class function definitions
/*******************************************************************************
 * Constructor for Class jsonSend.						 					   *
 *******************************************************************************/
jsonSend::jsonSend():t3("INFOWAY")
{

}

/*******************************************************************************
 * Destructor for Class jsonSend.						 					   *
 *******************************************************************************/
jsonSend::~jsonSend()
{
}

/*******************************************************************************
 * Function to get the current time.										   *
 *******************************************************************************/
string trigger::getTime()
{
	chrono::time_point<std::chrono::system_clock> presentTimeChronoType;
	presentTimeChronoType = chrono::system_clock::now();
	time_t presentTime = chrono::system_clock::to_time_t(presentTimeChronoType);
	return ctime(&presentTime);
}

/*******************************************************************************
 * Function to map integration Id to category.								   *
 *******************************************************************************/
integrationIdEnum jsonSend::strToEnumMapping (string inString)
{
	/*-----category = Media(Milan climax) and Nuvo-----*/
	if((inString == "10000")) //Nuvo not included&&((inString == "10001")))
		return categoryMedia;

	/*-----category = IP IR-----*/
	else if((inString == "20000")||(inString == "20001")||(inString == "20002")||(inString == "20003")||(inString == "20004")||(inString == "20005"))
		return categoryIPIR;

	/*-----category = IP Relay-----*/
	else if((inString == "30000")||(inString == "30001")||(inString == "30002")||(inString == "30003")||(inString == "30004")||(inString == "30005"))
		return categoryAppliance;

	/*-----category = Colorlight-----*/
	else if((inString == "40000")||(inString == "40003")) //category = curtain
		return categoryColorLight;

	/*-----category = curtain-----*/
	else if((inString == "50000")||(inString == "50001")||(inString == "50002")||(inString == "50003")||(inString == "50004")||(inString == "50005")||(inString == "50006")||(inString == "50007")||(inString == "50008")||(inString == "50009")||(inString == "50010"))
		return categoryCurtain;

	/*-----category = Light-----*/
	else if((inString == "60000")||(inString == "60001")||(inString == "60002")||(inString == "60003")||(inString == "60004")||(inString == "60005")||(inString == "60006")||(inString == "60007")||(inString == "60008")||(inString == "60009")||(inString == "60010")||(inString == "60011")||(inString == "60012"))
		return categoryLight;

	/*-----category = Security-----*/
	else if((inString == "70000")||(inString == "70001")||(inString == "70002")||(inString == "70003"))
		return categorySecurity;

	/*-----category = Visitor-----*/
	else if((inString == "80000")||(inString == "80001")||(inString == "80002")||(inString == "80003")||(inString == "80004")||(inString == "80005")||(inString == "80006")||(inString == "80007")||(inString == "80100")||(inString == "80101")||(inString == "80102")||(inString == "80103"))
		return categoryVisitor;

	/*-----category = Door Lock-----*/
	else if((inString == "90000")||(inString == "90001")||(inString == "90002"))
		return categoryDoorLock;

	/*-----category = Thermostat-----*/
	else if((inString == "110001")||(inString == "110002")||(inString == "110003")||(inString == "110004")||(inString == "110005"))
		return categoryThermostat;

	/*-----category = IR-----*/	/*-----category same as IP2IR-----*/
	else if((inString == "200000")||(inString == "200001")||(inString == "200002")||(inString == "200003")||(inString == "200004")||(inString == "200005")||(inString == "200006")||(inString == "200007")||(inString == "200008")||(inString == "200009")||(inString == "200010"))
		return categoryIR;

	/*-----category = IR-----*/	/*-----category same as IP2IR-----*/
	else if((inString == "200011")||(inString == "200015")||(inString == "200016")||(inString == "200017")||(inString == "200018")||(inString == "200019")||(inString == "200020")||(inString == "200021")||(inString == "200023")||(inString == "200033")||(inString == "200043"))
		return categoryIR;

	/*-----category = IR-----*/ /*-----category same as IP2IR-----*/
	else if((inString == "200053")||(inString == "200063")||(inString == "20000401")||(inString == "20000402")||(inString == "20000403")||(inString == "20000501")||(inString == "20000601"))
		return categoryIR;

	/*----------Category = scene-----------*/
	else if(inString == "NLMP01")
		return categoryScene;

	else
		return categoryNone;
}

/******************************************************************************
 * Member function to create Json string									  *
 * ****************************************************************************/
string jsonSend::jsonCreate(string category,string userId,string localInstanceId,string integrationId,string param1,string param2,string param3,string param4,string param5,string param6,string param7,string param8,string param9,string param10)
{
	cJSON *root,*arr,*fmt,*args;
	string jsonSent;

	/*-----case value for success-----*/
	root=cJSON_CreateObject();
	cJSON_AddItemToObject(root, "method",cJSON_CreateString("set"));
	cJSON_AddItemToObject(root, "params",arr=cJSON_CreateArray());
	cJSON_AddItemToArray(arr,fmt=cJSON_CreateObject());

	switch(strToEnumMapping (integrationId))
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
				cJSON_AddStringToObject(fmt,"category",category.c_str());
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
						arg[0] = param4.c_str();
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

		case categoryAppliance:
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
				//{"method":"set","params":[{"category":"Media","user_id":"USERID","inst_id":"LOCALINSTANCEID","model":"INTEGRATIONID","args":["DEVICEID","1","PLAY","","","","",""]}]}
				//{"method":"set","params":[{"category":"Media","user_id":"USERID","inst_id":"LOCALINSTANCEID","model":"INTEGRATIONID","args":["DEVICEID","3","PLAY_PLAYLIST","PLAYLISTID","","","",""]}]}
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

		case categoryThermostat:
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
				arg[4] =param5.c_str();
				arg[5] =param5.c_str();
				cJSON_AddItemToObject(fmt, "args", args=cJSON_CreateStringArray(arg,6));
			}
			break;

		case categoryDoorLock:
			{
				cJSON_AddStringToObject(fmt,"category",category.c_str());
				cJSON_AddStringToObject(fmt,"user_id",userId.c_str());
				cJSON_AddStringToObject(fmt,"inst_id",localInstanceId.c_str());
				cJSON_AddStringToObject(fmt,"model",integrationId.c_str());
				const char * arg[3];
				arg[0] =param1.c_str();
				arg[1] =param2.c_str();
				arg[2] =param3.c_str();
				cJSON_AddItemToObject(fmt, "args", args=cJSON_CreateStringArray(arg,3));
			}
			break;

		case categoryScene:
			{
				cJSON_AddStringToObject(fmt,"category",category.c_str());
				cJSON_AddStringToObject(fmt,"user_id",userId.c_str());
				cJSON_AddStringToObject(fmt,"scene_id",localInstanceId.c_str());
			}
			break;

		case categoryNone:
			if((category == "IR") || (category == "Channels"))
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
			else if(category == "Security")
			{
				cJSON_AddStringToObject(fmt,"category",category.c_str());
				cJSON_AddStringToObject(fmt,"user_id",userId.c_str());
				const char * arg[2];
				arg[0] =param1.c_str();
				arg[1] =param2.c_str();
				cJSON_AddItemToObject(fmt, "args", args=cJSON_CreateStringArray(arg,2));
			}
			else if(category == "Appliances")
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

		default:
			{
				#ifdef DEBUG
					cout << "Invalid Category(JSON).\n\n";
				#endif
			}
	}

	cJSON_Print(root);
	jsonSent=cJSON_PrintUnformatted(root);
	cJSON_Delete(root);
	return jsonSent;
}

/******************************************************************************
 * Member function to send Json string	throuugh tcp socket.				  *
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
		#ifdef DEBUG
			cout << "Could not get address info.\n\n";
		#endif
		/*-----Could not get address info.-----*/
		freeaddrinfo(host_info_list);
		/*-----address info not received return value-----*/
		return 1;
	}

	/*-----Opening the socket.-----*/
	socketFd = socket(host_info_list->ai_family, host_info_list->ai_socktype,host_info_list->ai_protocol);
	if (socketFd == -1)
	{
		#ifdef DEBUG
			cout << "Could not create socket.\n\n";
		#endif
		/*-----socket error-----*/
		freeaddrinfo(host_info_list);
		/*-----socket not created return value-----*/
		return 2;
	}

	/*-----Connecting to the socket.-----*/
	connectStatus = connect(socketFd, host_info_list->ai_addr, host_info_list->ai_addrlen);
	if (connectStatus == -1)
	{
		#ifdef DEBUG
		/*-----not connected-----*/
			cout << "Could not connect to socket.\n\n";
		#endif
		close(socketFd);
		freeaddrinfo(host_info_list);
		/*-----socket not connected return value-----*/
		return 3;
	}


	send(socketFd, jsonData.c_str(), strlen(jsonData.c_str()), 0);
	freeaddrinfo(host_info_list);
	close(socketFd);
	return 4;
}

/**********************************************************************************
 * Member function to send Json string	through p2p for another nano or through   *
 * TCP socket incase of local.													  *
 * *******************************************************************************/
int jsonSend::p2pSend(string did,string& jsonData)
{
	if(!did.empty())
	{
		#ifdef DEBUG
			cout<<"P2P id\t\t\t\t:" << did << ".\n\n";
		#endif

		string curP2pId{};
		//jsonData = urlEncode(jsonData);
		MYSQL_ROW row1;
		t3.mysqlQuery("SELECT P2P FROM INFOWAY WHERE P2P IS NOT NULL;");
		row1 = mysql_fetch_row(t3.result);
		curP2pId = row1[0];

		if(!curP2pId.empty())
		{
			if(curP2pId != did)
			{
				cJSON *root,*serv_call,*arr,*value;
				string jsonSent{};

				/*-----case value for success-----*/
				root=cJSON_CreateObject();
				cJSON_AddStringToObject(root, "KEY","1234567890123456789");
				cJSON_AddItemToObject(root, "SERVICE_CALL",serv_call=cJSON_CreateObject());
				cJSON_AddItemToObject(serv_call, "POST",arr=cJSON_CreateArray());
				cJSON_AddItemToArray(arr,value=cJSON_CreateObject());

				string slaveLoc{};
				#ifdef DEBUG
					slaveLoc = "http://localhost:6161/Milan/Drivers/MILAN_EXE/TRIGGER/triggerSlave_debug.cgi";
				#else
					slaveLoc = "http://localhost:6161/Milan/Drivers/MILAN_EXE/TRIGGER/triggerSlave.cgi";
				#endif

				cJSON_AddStringToObject(value, "URL",slaveLoc.c_str());
				cJSON_AddStringToObject(value, "VALUE",jsonData.c_str());
				cJSON_AddItemToObject(value, "RESPONSE",cJSON_CreateString("YES"));
				cJSON_Print(root);
				jsonSent=cJSON_PrintUnformatted(root);
				cJSON_Delete(root);

				INT32 ret;

				ret = PPCS_Initialize((CHAR*)"EFGBFFBDKPILGKJDFJHPFAELGDNAHGMAHMFBBIDHAOJBLKKODOACDEPFGLKFIKLBADNJKKDJOLNLBNCKJMMI");
				st_PPCS_NetInfo NetInfo;
				ret = PPCS_NetworkDetect(&NetInfo, 0);

				INT32 SessionHandle;
				SessionHandle = PPCS_Connect(did.c_str(), 1, 0);
				if(SessionHandle >=0)
				{
					st_PPCS_Session Sinfo;
					PPCS_Check(ret, &Sinfo);
					jsonSent = jsonSent + "\r\n";
					char json_string[jsonSent.length()];// = jsonSent.c_str();//=jsonSent.c_str();
					strcpy(json_string,jsonSent.c_str());

					#ifdef DEBUG
						cout << "JSON 	:" << json_string << ".\n\n";
					#endif

					PPCS_Write(SessionHandle, 0, json_string, strlen(json_string));
					sleep(3);
				}
				PPCS_Close(SessionHandle);
				jsonData.clear();
			}
			else
			{
				#ifdef DEBUG
					jsonData = "localhost:6161/Milan/Drivers/MILAN_EXE/TRIGGER/triggerSlave_debug.cgi?"+jsonData;
				#else
					jsonData = "localhost:6161/Milan/Drivers/MILAN_EXE/TRIGGER/triggerSlave.cgi?"+jsonData;
				#endif

				curlRequests(jsonData, "GET", "triggerSlave");
				jsonData.clear();
			}
		}
	}
	return 0;
}

/****************************************************************************************
 * Function to call curl request.														*
 ****************************************************************************************/
void jsonSend::curlRequests(string webAddress,string type1,string type2)
{
	CURL *curl;
    CURLcode res;
	string curlResponse{};
    curl_global_init(CURL_GLOBAL_DEFAULT);

	//webAddress = urlEncode(webAddress);
    #ifdef DEBUG
		cout << "Url:" << webAddress << ".\n\n";
    #endif

    curl = curl_easy_init();

    if(curl)
    {
		curl_easy_setopt(curl, CURLOPT_URL, webAddress.c_str());
		if(type1 == "POST")
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
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
		cout << "query result:" << curlResponse << ".\n\n";
	#endif
}

//Trigger Class function definitions
/*******************************************************************************
 * Constructor for Class trigger.						 					   *
 *******************************************************************************/
trigger::trigger():t2("Milan_SP")
{
	/*-----Progress flag is set and Stop flag is reset-----*/
}

/*******************************************************************************
 * Destructor for Class trigger.					 					   	   *
 *******************************************************************************/
trigger::~trigger()
{
	/*-----Progress flag is set and Stop flag is reset-----*/
}

/*******************************************************************************
 * Function to construct the query for Slave devices. 					   	   *
 *******************************************************************************/
void trigger::p2pData(string sceneId)
{
	jsonSend s1;
	vector<string> p2pId{},instanceId{},propertyId{},propertyValue{},userId{},delay{},type{},ref1{};
	MYSQL_ROW row2;
	string queryString{};
	t2.mysqlQuery("SELECT DID,Instance_Id,Property_Id,Property_Value,User_Id,delay,exec_method,ref_1 FROM Milan_SP.Trigger_Scene WHERE Scene_Id='" + sceneId + "' AND DID IS NOT NULL AND Instance_Id IS NOT NULL AND Property_Id IS NOT NULL AND Property_Value IS NOT NULL AND ref_1 IS NOT NULL AND User_Id IS NOT NULL AND delay IS NOT NULL AND exec_method IS NOT NULL ORDER BY DID ;");
	while ((row2 = mysql_fetch_row(t2.result)))
	{
		p2pId.push_back(row2[0]);
		instanceId.push_back(row2[1]);
		propertyId.push_back(row2[2]);
		propertyValue.push_back(row2[3]);
		userId.push_back(row2[4]);
		delay.push_back(row2[5]);
		type.push_back(row2[6]);
		ref1.push_back(row2[7]);
	}
	if(!p2pId.empty() && !instanceId.empty() && !propertyId.empty() && !propertyValue.empty() && !userId.empty() && !delay.empty() && !type.empty() && !ref1.empty() )
	{
		for(size_t loopNo1=0;loopNo1<p2pId.size();loopNo1++)
		{
			#ifdef DEBUG
				cout<< "p2pId[" << loopNo1 << "]\t\t\t:" << p2pId.at(loopNo1) << ",\n";
				cout<< "instanceId[" << loopNo1 << "]\t\t\t:" << instanceId.at(loopNo1) << ",\n";
				cout<< "propertyId[" << loopNo1 << "]\t\t\t:" << propertyId.at(loopNo1) << ",\n";
				cout<< "propertyValue[" << loopNo1 << "]\t\t\t:" << propertyValue.at(loopNo1) << ",\n";
				cout<< "userId[" << loopNo1 << "]\t\t\t:" << userId.at(loopNo1) << ",\n";
				cout<< "delay[" << loopNo1 << "]\t\t\t:" << delay.at(loopNo1) << ",\n";
				cout<< "type[" << loopNo1 << "]\t\t\t:" << type.at(loopNo1) << ",\n";
				cout<< "ref1[" << loopNo1 << "]\t\t\t:" << ref1.at(loopNo1) << ".\n\n";
			#endif

			if(!p2pId.at(loopNo1).empty() && !instanceId.at(loopNo1).empty() && !propertyId.at(loopNo1).empty() && !propertyValue.at(loopNo1).empty() && !userId.at(loopNo1).empty() && !delay.at(loopNo1).empty() && !type.at(loopNo1).empty() && !ref1.at(loopNo1).empty())
			{
				if(loopNo1 == 0)
				{
					//first element
					queryString += "instanceId="+instanceId.at(loopNo1)+"&propertyId="+propertyId.at(loopNo1)+"&propertyValue="+urlEncode(propertyValue.at(loopNo1))+"&userId="+userId.at(loopNo1)+"&delay="+delay.at(loopNo1)+"&type="+type.at(loopNo1)+"&ref1="+ref1.at(loopNo1);
					//one element
					if(loopNo1 == p2pId.size() - 1)
						s1.p2pSend(p2pId.at(loopNo1),queryString);
				}
				else if(loopNo1 == p2pId.size() - 1)
				{
					if(p2pId.at(loopNo1) == p2pId.at(loopNo1 -1))
					{
						//both p2pids are same
						queryString += "&instanceId="+instanceId.at(loopNo1)+"&propertyId="+propertyId.at(loopNo1)+"&propertyValue="+urlEncode(propertyValue.at(loopNo1))+"&userId="+userId.at(loopNo1)+"&delay="+delay.at(loopNo1)+"&type="+type.at(loopNo1)+"&ref1="+ref1.at(loopNo1);
						s1.p2pSend(p2pId.at(loopNo1),queryString);
					}
					else
					{
						//both p2pids are different
						s1.p2pSend(p2pId.at(loopNo1 -1),queryString);

						//clear the previous data
						queryString += "instanceId="+instanceId.at(loopNo1)+"&propertyId="+propertyId.at(loopNo1)+"&propertyValue="+urlEncode(propertyValue.at(loopNo1))+"&userId="+userId.at(loopNo1)+"&delay="+delay.at(loopNo1)+"&type="+type.at(loopNo1)+"&ref1="+ref1.at(loopNo1);
						s1.p2pSend(p2pId.at(loopNo1),queryString);
					}
				}
				else if((loopNo1 != 0)&&(loopNo1 != p2pId.size() -1))
				{
					if(p2pId.at(loopNo1) == p2pId.at(loopNo1 -1))
					{
						//both p2pids are same
						queryString += "&instanceId="+instanceId.at(loopNo1)+"&propertyId="+propertyId.at(loopNo1)+"&propertyValue="+urlEncode(propertyValue.at(loopNo1))+"&userId="+userId.at(loopNo1)+"&delay="+delay.at(loopNo1)+"&type="+type.at(loopNo1)+"&ref1="+ref1.at(loopNo1);
					}
					else
					{
						//both p2pids are different
						s1.p2pSend(p2pId.at(loopNo1 -1),queryString);

						//clear the previous data
						queryString += "instanceId="+instanceId.at(loopNo1)+"&propertyId="+propertyId.at(loopNo1)+"&propertyValue="+urlEncode(propertyValue.at(loopNo1))+"&userId="+userId.at(loopNo1)+"&delay="+delay.at(loopNo1)+"&type="+type.at(loopNo1)+"&ref1="+ref1.at(loopNo1);
					}
				}
			}
		}
		for(size_t loopNo1=0;loopNo1<userId.size();loopNo1++)
		{
			if(!userId.at(loopNo1).empty() && !p2pId.at(loopNo1).empty() && !instanceId.at(loopNo1).empty() && !propertyId.at(loopNo1).empty() && !propertyValue.at(loopNo1).empty() && !delay.at(loopNo1).empty() && !type.at(loopNo1).empty() && !ref1.at(loopNo1).empty())
			{
				//first element
				if(loopNo1 == 0)
				{
					//only one element
					if(loopNo1 == userId.size() - 1)
					{
						t2.mysqlQuery("CALL INFOMAP_USAGE_HISTORY('scene_tri', '"+userId.at(loopNo1)+"', '"+sceneId+"', '0', 'NA', 'control')");
					}
				}
				//last element
				else if(loopNo1 == userId.size() - 1)
				{
					//both p2pids are same
					if(userId.at(loopNo1) == userId.at(loopNo1 -1))
					{
						t2.mysqlQuery("CALL INFOMAP_USAGE_HISTORY('scene_tri', '"+userId.at(loopNo1)+"', '"+sceneId+"', '0', 'NA', 'control')");
					}
					//both p2pids are different
					else
					{
						t2.mysqlQuery("CALL INFOMAP_USAGE_HISTORY('scene_tri', '"+userId.at(loopNo1 - 1)+"', '"+sceneId+"', '0', 'NA', 'control')");

						//clear the previous data
						t2.mysqlQuery("CALL INFOMAP_USAGE_HISTORY('scene_tri', '"+userId.at(loopNo1)+"', '"+sceneId+"', '0', 'NA', 'control')");
					}
				}
				//middle elements
				else if((loopNo1 != 0)&&(loopNo1 != p2pId.size() -1))
				{
					if(p2pId.at(loopNo1) == p2pId.at(loopNo1 -1))
					{
						//both p2pids are same
					}
					else
					{
						//both p2pids are different
						t2.mysqlQuery("CALL INFOMAP_USAGE_HISTORY('scene_tri', '"+userId.at(loopNo1 - 1)+"', '"+sceneId+"', '0', 'NA', 'control')");

						//clear the previous data
						t2.mysqlQuery("CALL INFOMAP_USAGE_HISTORY('scene_tri', '"+userId.at(loopNo1)+"', '"+sceneId+"', '0', 'NA', 'control')");
					}
				}
			}
		}
	}
}

/********************************************************************************
 * Trigger Slave function 														*
 * ******************************************************************************/
int trigger::triggerExecution(vector<string> instanceId,vector<string> propertyId,vector<string> propertyValue,vector<string> userId,vector<string> delay,vector<string> type,vector<string> ref1)
{
	/*-----Listing out the Devices-----*/
	jsonSend s1{};
	string triggerJson{};
	thread *thread1 = new thread[instanceId.size()];
	#ifdef DEBUG
		cout << "Trigger Slave Query Params list:\n";
		cout << "If (delay == 0) && (type)\t:sequential execution,\n";
		cout << "If (delay > 0) && (type == 1)\t:parallel execution with delay,\n";
		cout << "If (delay > 0) && (type == 0)\t:sequential execution with delay.\n\n";
	#endif

	for(size_t loopNo1=0; loopNo1 < instanceId.size(); loopNo1++)
	{
		#ifdef DEBUG
			cout << "Instance id[" << loopNo1 << "]\t\t\t:" << instanceId.at(loopNo1) << ",\n";
			cout << "Property id[" << loopNo1 << "]\t\t\t:" << propertyId.at(loopNo1) << ",\n";
			cout << "Property Value[" << loopNo1 << "]\t\t:" << propertyValue.at(loopNo1) << ",\n";
			cout << "User Id[" << loopNo1 << "]\t\t\t:" << userId.at(loopNo1) << ",\n";
			cout << "Delay[" << loopNo1 << "]\t\t\t:" << delay.at(loopNo1) << ",\n";
			cout << "Type[" << loopNo1 << "]\t\t\t" << ((loopNo1 > 9)?"":"\t") << ":" << type.at(loopNo1) << ",\n";
			cout << "ref1[" << loopNo1 << "]\t\t\t" << ((loopNo1 > 9)?"":"\t") << ":" << ref1.at(loopNo1) << ",\n";
		#endif

		if((propertyId.at(loopNo1) == "15") || (propertyId.at(loopNo1) == "16") || (propertyId.at(loopNo1) == "17") || (propertyId.at(loopNo1) == "18"))
		{
			if(!instanceId.at(loopNo1).empty() && !propertyId.at(loopNo1).empty() && !propertyValue.at(loopNo1).empty() && !userId.at(loopNo1).empty() && !delay.at(loopNo1).empty() && !type.at(loopNo1).empty())
			{
				string passCode{};
				MYSQL_ROW row1;
				#ifdef DEBUG
					cout << "Device Category\t\t\t:Security.\n\n";
				#endif
				t2.mysqlQuery("SELECT passcode FROM C_Mymilan_Users where mymilan_user_id='"+userId.at(loopNo1)+"';");
				while((row1 = mysql_fetch_row(t2.result)))
				{
					passCode = row1[0];
				}
				if(propertyId.at(loopNo1) == "15")//Away Arm
				{
					if(isInt(delay.at(loopNo1),10))
					{
						if(delay.at(loopNo1) == "0")
						{
							//Bulk json construction
							triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Security",userId.at(loopNo1),"","","01",passCode,"","","","","","","","");
						}
						else
						{
							if(type.at(loopNo1) == "1")
							{
								//Thread json construction
								thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"Security",userId.at(loopNo1),"","","01",passCode,"","","","","","","","");
							}
							else
							{
								//sequential json construction
								if(!triggerJson.empty())
								{
									#ifdef DEBUG
										cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
									#endif
									s1.socketSend(triggerJson);
									triggerJson.clear();
								}
								this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
								triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Security",userId.at(loopNo1),"","","01",passCode,"","","","","","","","");
							}
						}
					}
				}
				else if(propertyId.at(loopNo1) == "16")//Night Arm
				{
					if(isInt(delay.at(loopNo1),10))
					{
						if(delay.at(loopNo1) == "0")
						{
							//Bulk json construction
							triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Security",userId.at(loopNo1),"","","03",passCode,"","","","","","","","");
						}
						else
						{
							if(type.at(loopNo1) == "1")
							{
								//Thread json construction
								thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"Security",userId.at(loopNo1),"","","03",passCode,"","","","","","","","");
							}
							else
							{
								//sequential json construction
								if(!triggerJson.empty())
								{
									#ifdef DEBUG
										cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
									#endif
									s1.socketSend(triggerJson);
									triggerJson.clear();
								}
								this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
								triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Security",userId.at(loopNo1),"","","03",passCode,"","","","","","","","");
							}
						}
					}
				}
				else if(propertyId.at(loopNo1) == "17")//stay arm
				{
					if(isInt(delay.at(loopNo1),10))
					{
						if(delay.at(loopNo1) == "0")
						{
							//Bulk json construction
							triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Security",userId.at(loopNo1),"","","02",passCode,"","","","","","","","");
						}
						else
						{
							if(type.at(loopNo1) == "1")
							{
								//Thread json construction
								thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"Security",userId.at(loopNo1),"","","02",passCode,"","","","","","","","");
							}
							else
							{
								//sequential json construction
								if(!triggerJson.empty())
								{
									#ifdef DEBUG
										cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
									#endif
									s1.socketSend(triggerJson);
									triggerJson.clear();
								}
								this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
								triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Security",userId.at(loopNo1),"","","02",passCode,"","","","","","","","");
							}
						}
					}
				}
				else if(propertyId.at(loopNo1) == "18")//disarm
				{
					if(isInt(delay.at(loopNo1),10))
					{
						if(delay.at(loopNo1) == "0")
						{
							//Bulk json construction
							triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Security",userId.at(loopNo1),"","","00",passCode,"","","","","","","","");
						}
						else
						{
							if(type.at(loopNo1) == "1")
							{
								//Thread json construction
								thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"Security",userId.at(loopNo1),"","","00",passCode,"","","","","","","","");
							}
							else
							{
								//sequential json construction
								if(!triggerJson.empty())
								{
									#ifdef DEBUG
										cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
									#endif
									s1.socketSend(triggerJson);
									triggerJson.clear();
								}
								this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
								triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Security",userId.at(loopNo1),"","","00",passCode,"","","","","","","","");
							}
						}
					}
				}
			}
		}
		else
		{
			if(!instanceId.at(loopNo1).empty() && !propertyId.at(loopNo1).empty() && !propertyValue.at(loopNo1).empty() && !userId.at(loopNo1).empty() && !delay.at(loopNo1).empty() && !type.at(loopNo1).empty())
			{
				//cout<<"Device is not security."<<"\n";
				MYSQL_ROW row1;

				string localInstanceId{},addonId1{},addonId2{},integrationId{},propertyIdCheck{},comProtocol{},prodCat{},propertyName{};
				//testMysql t2("Milan_SP");
				//t2.mysqlQuery("SELECT q2.local_instance_id,q2.addon_id,q3.addon_id,q3.mymilan_user_id,q4.integration_id,q5.property_id FROM C_User_Addon_Instances AS q2 LEFT JOIN C_User_Addons AS q3 ON q2.addon_id = q3.id LEFT JOIN C_Addons AS q4 ON q3.addon_id = q4.id LEFT JOIN C_Addons_Properties AS q5 ON q5.addon_id = q3.addon_id AND q5.property_id = '"+propertyId.at(loopNo1)+"' WHERE q2.Instance_Id ='"+instanceId.at(loopNo1)+"';");
				t2.mysqlQuery("SELECT q2.local_instance_id, q2.addon_id, q3.addon_id, q4.integration_id, q6.property_id, q2.comm_protocol_id, q4.product_category, q6.property_name FROM C_User_Addon_Instances AS q2 LEFT JOIN C_User_Addons AS q3 ON q2.addon_id = q3.id LEFT JOIN C_Addons AS q4 ON q3.addon_id = q4.id LEFT JOIN C_Product_Type AS q5 ON q5.pro_type_primary_id = q4.product_type LEFT JOIN C_Addons_Properties AS q6 ON q6.addon_id = q3.addon_id AND q6.property_id = '"+propertyId.at(loopNo1)+"' WHERE q2.Instance_Id = '"+instanceId.at(loopNo1)+"' AND q2.local_instance_id IS NOT NULL AND q2.addon_id IS NOT NULL AND q3.addon_id IS NOT NULL AND q4.integration_id IS NOT NULL AND q6.property_id IS NOT NULL AND q2.comm_protocol_id IS NOT NULL;");
				while((row1 = mysql_fetch_row(t2.result)))
				{
					localInstanceId = row1[0];
					addonId1 = row1[1];
					addonId2 = row1[2];
					integrationId = row1[3];
					propertyIdCheck = row1[4];
					comProtocol = row1[5];
					prodCat = row1[6];
					propertyName = row1[7];
				}

				#ifdef DEBUG
					cout << "localInstanceId[" << loopNo1 << "]\t\t:" << localInstanceId << ",\n";
					cout << "addonId1[" << loopNo1 << "]\t\t\t:" << addonId1 << ",\n";
					cout << "addonId2[" << loopNo1 << "]\t\t\t:" << addonId2 << ",\n";
					cout << "integrationId[" << loopNo1 << "]\t\t:" << integrationId << ",\n";
					cout << "propertyIdCheck[" << loopNo1 << "]\t\t:" << propertyIdCheck << ",\n";
					cout << "comProtocol[" << loopNo1 << "]\t\t\t:" << comProtocol << ",\n";
					cout << "propertyName[" << loopNo1 << "]\t\t\t:" << propertyName << ",\n";
				#endif

				if(propertyId.at(loopNo1) == propertyIdCheck)
				{
					switch(s1.strToEnumMapping(integrationId))
					{
						case categoryMedia:
							{
								MYSQL_ROW row2;
								t2.mysqlQuery("SELECT id FROM Milan_SP.Music where local_instance_id='"+localInstanceId+"' AND id IS NOT NULL;");
								if((row2 = mysql_fetch_row(t2.result)))
								{
									string deviceIdClimax = row2[0];
									#ifdef DEBUG
										cout << "Device Category\t\t\t:Media." << comProtocol << ",\n";
										cout << "deviceId\t\t\t:" << deviceIdClimax << ".\n\n";
									#endif

									if(isInt(delay.at(loopNo1),10))
									{
										if(delay.at(loopNo1) == "0")
										{
											//Bulk json construction
											triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Media",userId.at(loopNo1),localInstanceId,integrationId,deviceIdClimax,(propertyIdCheck == "144"?"3":"1"),(propertyIdCheck == "403"? "volume":(propertyIdCheck == "404"?"reboot":propertyIdCheck)),((propertyIdCheck == "144") || (propertyIdCheck == "403")?propertyValue.at(loopNo1):""),"","","","","","");
										}
										else
										{
											if(type.at(loopNo1) == "1")
											{
												//Thread json construction
												thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"Media",userId.at(loopNo1),localInstanceId,integrationId,deviceIdClimax,(propertyIdCheck == "144"?"3":"1"),(propertyIdCheck == "403"? "volume":(propertyIdCheck == "404"?"reboot":propertyIdCheck)),((propertyIdCheck == "144") || (propertyIdCheck == "403")?propertyValue.at(loopNo1):""),"","","","","","");
											}
											else
											{
												//sequential json construction
												if(!triggerJson.empty())
												{
													#ifdef DEBUG
														cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
													#endif
													s1.socketSend(triggerJson);
													triggerJson.clear();
												}
												this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
												triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Media",userId.at(loopNo1),localInstanceId,integrationId,deviceIdClimax,(propertyIdCheck == "144"?"3":"1"),(propertyIdCheck == "403"? "volume":(propertyIdCheck == "404"?"reboot":propertyIdCheck)),((propertyIdCheck == "144") || (propertyIdCheck == "403")?propertyValue.at(loopNo1):""),"","","","","","");
											}
										}
									}
								}
							}
							break;

						case categoryAppliance:
							{
								MYSQL_ROW row2;
								t2.mysqlQuery("SELECT id,control_type,duration FROM Relay_Devices WHERE local_instance_id='"+localInstanceId+"' AND id IS NOT NULL AND control_type IS NOT NULL AND duration IS NOT NULL;");
								if((row2 = mysql_fetch_row(t2.result)))
								{
									string uniqueId = row2[0],controlType = row2[1],delay1= row2[2];
									#ifdef DEBUG
										cout << "Device Category\t\t\t:Appliances.,\n";
										cout << "uniqueId\t\t\t:" << uniqueId << ",\n";
										cout << "controlType\t\t\t:" << controlType << ",\n";
										cout << "delay\t\t\t:" << delay1 << ".\n\n";
									#endif
									if(delay.at(loopNo1) == "0")
									{
										//Bulk json construction
										//1 -> toggle, 2 -> on/off
										triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Appliances",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,(controlType == "1")?"2":(propertyId.at(loopNo1) == "2")?"0":propertyId.at(loopNo1),(controlType == "1")?delay1:"0","","","","","","","");
									}
									else
									{
										if(type.at(loopNo1) == "1")
										{
											//Thread json construction
											thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"Appliances",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,(controlType == "1")?"2":(propertyId.at(loopNo1) == "2")?"0":propertyId.at(loopNo1),(controlType == "1")?delay1:"0","","","","","","","");
										}
										else
										{
											//sequential json construction
											if(!triggerJson.empty())
											{
												#ifdef DEBUG
													cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
												#endif
												s1.socketSend(triggerJson);
												triggerJson.clear();
											}
											this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
											triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Appliances",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,(controlType == "1")?"2":(propertyId.at(loopNo1) == "2")?"0":propertyId.at(loopNo1),(controlType == "1")?delay1:"0","","","","","","","");
										}
									}
								}
							}
							break;

						case categoryColorLight:
							{
								MYSQL_ROW row2;
								t2.mysqlQuery("SELECT id,category FROM Lighting where local_instance_id='" +localInstanceId+ "' AND id IS NOT NULL AND category IS NOT NULL;");
								if((row2 = mysql_fetch_row(t2.result)))
								{
									string uniqueId = row2[0],category = row2[1];
									#ifdef DEBUG
										cout << "Device Category\t\t\t:Color Light,\n";
										cout << "uniqueId\t\t\t:" << uniqueId << ",\n";
										cout << "category\t\t\t:" << category << ",\n";
										if(propertyId.at(loopNo1) != "7")
											cout << "\n";
									#endif

									if((propertyId.at(loopNo1) == "9")||(propertyId.at(loopNo1) == "385"))//Random ON & OFF
									{
										if(isInt(delay.at(loopNo1),10))
										{
											if(delay.at(loopNo1) == "0")
											{
												//Bulk json construction
												triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("RGB",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"RND","2",propertyValue.at(loopNo1),"","","","","","");
											}
											else
											{
												if(type.at(loopNo1) == "1")
												{
													//Thread json construction
													thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"RGB",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"RND","2",propertyValue.at(loopNo1),"","","","","","");
												}
												else
												{
													//sequential json construction
													if(!triggerJson.empty())
													{
														#ifdef DEBUG
															cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
														#endif
														s1.socketSend(triggerJson);
														triggerJson.clear();
													}
													this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
													triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("RGB",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"RND","2",propertyValue.at(loopNo1),"","","","","","");
												}
											}
										}
									}
									else if((propertyId.at(loopNo1) == "1")||(propertyId.at(loopNo1) == "2")||(propertyId.at(loopNo1) == "8"))//ON/OFF and color preset
									{
										if(isInt(delay.at(loopNo1),10))
										{
											if(delay.at(loopNo1) == "0")
											{
												//Bulk json construction
												triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("RGB",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyValue.at(loopNo1),"0","","","","","","","");
											}
											else
											{
												if(type.at(loopNo1) == "1")
												{
													//Thread json construction
													thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"RGB",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyValue.at(loopNo1),"0","","","","","","","");
												}
												else
												{
													//sequential json construction
													if(!triggerJson.empty())
													{
														#ifdef DEBUG
															cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
														#endif
														s1.socketSend(triggerJson);
														triggerJson.clear();
													}
													this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
													triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("RGB",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyValue.at(loopNo1),"0","","","","","","","");
												}
											}
										}
									}
									else if(propertyId.at(loopNo1) == "7")//Color value
									{
										string color = rgb(propertyValue.at(loopNo1));
										#ifdef DEBUG
											cout << "color\t\t\t:" << color << ".\n\n";
										#endif
										if(isInt(delay.at(loopNo1),10))
										{
											if(delay.at(loopNo1) == "0")
											{
												//Bulk json construction
												triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("RGB",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,color,"0","","","","","","","");
											}
											else
											{
												if(type.at(loopNo1) == "1")
												{
													//Thread json construction
													thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"RGB",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,color,"0","","","","","","","");
												}
												else
												{
													//sequential json construction
													if(!triggerJson.empty())
													{
														#ifdef DEBUG
															cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
														#endif
														s1.socketSend(triggerJson);
														triggerJson.clear();
													}
													this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
													triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("RGB",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,color,"0","","","","","","","");
												}
											}
										}
									}
								}
							}
							break;

						case categoryLight:
							{
								MYSQL_ROW row2;
								t2.mysqlQuery("SELECT id,category FROM Lighting where local_instance_id='" + localInstanceId + "' AND id IS NOT NULL AND category IS NOT NULL;");
								if((row2 = mysql_fetch_row(t2.result)))
								{
									string uniqueId = row2[0],category = row2[1];
									#ifdef DEBUG
										cout << "Device Category\t\t\t:Light,\n";
										cout << "uniqueId\t\t\t:"<< uniqueId << ",\n";
										cout << "category\t\t\t:" << category << ".\n\n";
									#endif
									if(isInt(delay.at(loopNo1),10))
									{
										if(delay.at(loopNo1) == "0")
										{
											//Bulk json construction
											triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate(category,userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyValue.at(loopNo1),"","","","","","","","");
										}
										else
										{
											if(type.at(loopNo1) == "1")
											{
												//Thread json construction
												thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),category,userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyValue.at(loopNo1),"","","","","","","","");
											}
											else
											{
												//sequential json construction
												if(!triggerJson.empty())
												{
													#ifdef DEBUG
														cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
													#endif
													s1.socketSend(triggerJson);
													triggerJson.clear();
												}
												this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
												triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate(category,userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyValue.at(loopNo1),"","","","","","","","");
											}
										}
									}
								}
							}
							break;

						case categoryCurtain:
							{
								MYSQL_ROW row2;
								t2.mysqlQuery("SELECT id,category FROM Lighting where local_instance_id='" + localInstanceId + "' AND id IS NOT NULL AND category IS NOT NULL;");
								if((row2 = mysql_fetch_row(t2.result)))
								{
									string uniqueId = row2[0],category = row2[1];
									#ifdef DEBUG
										cout << "Device Category\t\t\t:curtain,\n";
										cout << "uniqueId\t\t\t:"<< uniqueId << ",\n";
										cout << "category\t\t\t:" << category << ".\n\n";
									#endif
									if(isInt(delay.at(loopNo1),10))
									{
										if(delay.at(loopNo1) == "0")
										{
											//Bulk json construction
											triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate(category,userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyValue.at(loopNo1),"","","","","","","","");
										}
										else
										{
											if(type.at(loopNo1) == "1")
											{
												//Thread json construction
												thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),category,userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyValue.at(loopNo1),"","","","","","","","");
											}
											else
											{
												//sequential json construction
												if(!triggerJson.empty())
												{
													#ifdef DEBUG
														cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
													#endif
													s1.socketSend(triggerJson);
													triggerJson.clear();
												}
												this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
												triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate(category,userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyValue.at(loopNo1),"","","","","","","","");
											}
										}
									}
								}
							}
							break;

						case categoryIR:
							{
								if(comProtocol == "22")
								{
									MYSQL_ROW row2;
									t2.mysqlQuery("SELECT id FROM IR_Devices where local_instance_id='" + localInstanceId + "' AND id IS NOT NULL;");
									if((row2 = mysql_fetch_row(t2.result)))
									{
										string uniqueId = row2[0];
										#ifdef DEBUG
											cout << "Device Category\t\t\t:IR,\n";
											cout << "uniqueId\t\t\t:" << uniqueId << ",\n";
											cout << "property id\t\t\t:" << propertyValue.at(loopNo1) << ".\n\n";
										#endif
										if(propertyId.at(loopNo1) == "39")
										{
											if(isInt(delay.at(loopNo1),10))
											{
												if(delay.at(loopNo1) == "0")
												{
													//Bulk json construction
													triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Channels",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyId.at(loopNo1),"","","","","","","","");
												}
												else
												{
													if(type.at(loopNo1) == "1")
													{
														//Thread json construction
														thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"Channels",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyId.at(loopNo1),"","","","","","","","");
													}
													else
													{
														//sequential json construction
														if(!triggerJson.empty())
														{
															#ifdef DEBUG
																cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
															#endif
															s1.socketSend(triggerJson);
															triggerJson.clear();
														}
														this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
														triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Channels",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyId.at(loopNo1),"","","","","","","","");
													}
												}
											}
										}
										else
										{
											if(isInt(delay.at(loopNo1),10))
											{
												if(delay.at(loopNo1) == "0")
												{
													//Bulk json construction
													triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("IR",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,(propertyId[loopNo1] != "399")?propertyId[loopNo1]:propertyValue[loopNo1],"","","","","","","","");
												}
												else
												{
													if(type.at(loopNo1) == "1")
													{
														//Thread json construction
														thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"IR",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,(propertyId[loopNo1] != "399")?propertyId[loopNo1]:propertyValue[loopNo1],"","","","","","","","");
													}
													else
													{
														//sequential json construction
														if(!triggerJson.empty())
														{
															#ifdef DEBUG
																cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
															#endif
															s1.socketSend(triggerJson);
															triggerJson.clear();
														}
														this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
														triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("IR",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,(propertyId[loopNo1] != "399")?propertyId[loopNo1]:propertyValue[loopNo1],"","","","","","","","");
													}
												}
											}
										}
									}
								}
							}
							break;

						case categoryDoorLock:
							{
								MYSQL_ROW row2;
								t2.mysqlQuery("SELECT (SELECT id FROM Door_Lock WHERE local_instance_id = '"+localInstanceId+"' AND id IS NOT NULL),(SELECT passcode FROM C_Mymilan_Users WHERE mymilan_user_id = '"+userId.at(loopNo1)+"' AND passcode IS NOT NULL); ");
								if((row2 = mysql_fetch_row(t2.result)))
								{
									string uniqueId = row2[0];
									string passcode = row2[1];
									#ifdef DEBUG
										cout << "Device Category\t\t\t:Doorlock,\n";
										cout << "uniqueId\t\t\t:" << uniqueId << ",\n";
										cout << "property id\t\t\t:" << propertyValue.at(loopNo1) << ".\n\n";
									#endif
									if(isInt(delay.at(loopNo1),10))
									{
										if(delay.at(loopNo1) == "0")
										{
											//Bulk json construction
											triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Door Lock",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyValue.at(loopNo1),passcode,"","","","","","","");
										}
										else
										{
											if(type.at(loopNo1) == "1")
											{
												//Thread json construction
												thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"Door Lock",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyValue.at(loopNo1),passcode,"","","","","","","");
											}
											else
											{
												//sequential json construction
												if(!triggerJson.empty())
												{
													#ifdef DEBUG
														cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
													#endif
													s1.socketSend(triggerJson);
													triggerJson.clear();
												}
												this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
												triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Door Lock",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyValue.at(loopNo1),passcode,"","","","","","","");
											}
										}
									}
								}
								t2.mysqlQuery("UPDATE `Door_Lock` SET `status`='"+propertyValue.at(loopNo1)+"' WHERE `local_instance_id`='"+localInstanceId+"';");
							}
							break;

						case categoryNone:
							{
								if(comProtocol.find("22") != string::npos)//IR com protocol
								{
									MYSQL_ROW row2;
									t2.mysqlQuery("SELECT id FROM IR_Devices where local_instance_id='" + localInstanceId + "' AND id IS NOT NULL;");
									if((row2 = mysql_fetch_row(t2.result)))
									{
										string uniqueId = row2[0];
										#ifdef DEBUG
											cout << "Device Category\t\t\t:None/IR,\n";
											cout << "uniqueId\t\t\t:" << uniqueId << ",\n";
											cout << "property id\t\t\t:" << propertyId.at(loopNo1) << ".\n\n";
										#endif
										if(propertyId.at(loopNo1) == "39")
										{
											if(isInt(delay.at(loopNo1),10))
											{
												if(delay.at(loopNo1) == "0")
												{
													//Bulk json construction
													triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Channels",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyId.at(loopNo1),propertyValue.at(loopNo1),"","","","","","","");
												}
												else
												{
													if(type.at(loopNo1) == "1")
													{
														//Thread json construction
														thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"Channels",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyId.at(loopNo1),propertyValue.at(loopNo1),"","","","","","","");
													}
													else
													{
														//sequential json construction
														if(!triggerJson.empty())
														{
															#ifdef DEBUG
																cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
															#endif
															s1.socketSend(triggerJson);
															triggerJson.clear();
														}
														this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
														triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Channels",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,propertyId.at(loopNo1),propertyValue.at(loopNo1),"","","","","","","");
													}
												}
											}
										}
										else
										{
											if(isInt(delay.at(loopNo1),10))
											{
												if(delay.at(loopNo1) == "0")
												{
													//Bulk json construction
													triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("IR",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,(propertyId[loopNo1] != "399")?propertyId[loopNo1]:propertyValue[loopNo1],"","","","","","","","");
												}
												else
												{
													if(type.at(loopNo1) == "1")
													{
														//Thread json construction
														thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"IR",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,(propertyId[loopNo1] != "399")?propertyId[loopNo1]:propertyValue[loopNo1],"","","","","","","","");
													}
													else
													{
														//sequential json construction
														if(!triggerJson.empty())
														{
															#ifdef DEBUG
																cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
															#endif
															s1.socketSend(triggerJson);
															triggerJson.clear();
														}
														this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
														triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("IR",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,(propertyId[loopNo1] != "399")?propertyId[loopNo1]:propertyValue[loopNo1],"","","","","","","","");
													}
												}
											}
										}
									}
								}
								else if(comProtocol.find("10") != string::npos)//Relay comm protocol
								{
									MYSQL_ROW row2;
									t2.mysqlQuery("SELECT id,control_type,duration FROM Relay_Devices WHERE local_instance_id='"+localInstanceId+"' AND id IS NOT NULL AND control_type IS NOT NULL AND duration IS NOT NULL;");
									if((row2 = mysql_fetch_row(t2.result)))
									{
										string uniqueId = row2[0],controlType = row2[1],delay1= row2[2];
										#ifdef DEBUG
											cout << "Device Category\t\t\t:Apppliance,\n";
											cout << "uniqueId\t\t\t:" << uniqueId << ",\n";
											cout << "controlType\t\t\t:" << controlType << ",\n";
											cout << "delay\t\t\t\t:" << delay1 << ".\n\n";
										#endif
										if(delay.at(loopNo1) == "0")
										{
											//Bulk json construction
											//1 -> toggle, 2 -> on/off
											triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Appliances",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,(controlType == "1")?"2":(propertyId.at(loopNo1) == "2")?"0":propertyId.at(loopNo1),(controlType == "1")?delay1:"0","","","","","","","");
										}
										else
										{
											if(type.at(loopNo1) == "1")
											{
												//Thread json construction
												thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"Appliances",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,(controlType == "1")?"2":(propertyId.at(loopNo1) == "2")?"0":propertyId.at(loopNo1),(controlType == "1")?delay1:"0","","","","","","","");
											}
											else
											{
												//sequential json construction
												if(!triggerJson.empty())
												{
													#ifdef DEBUG
														cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
													#endif
													s1.socketSend(triggerJson);
													triggerJson.clear();
												}
												this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
												triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Appliances",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,(controlType == "1")?"2":(propertyId.at(loopNo1) == "2")?"0":propertyId.at(loopNo1),(controlType == "1")?delay1:"0","","","","","","","");
											}
										}
									}
								}
							}
							break;

						case categoryThermostat:
							{
								MYSQL_ROW row2;
								t2.mysqlQuery("SELECT node_id FROM Thermostat WHERE local_instance_id='" + localInstanceId + "' AND node_id IS NOT NULL;");
								if((row2 = mysql_fetch_row(t2.result)))
								{
									string uniqueId = row2[0];
									#ifdef DEBUG
										cout << "Device Category\t\t\t:Thermostat,\n";
										cout << "uniqueId\t\t\t:" << uniqueId << ".\n\n";
									#endif
									if((propertyId.at(loopNo1) == "2")||(propertyId.at(loopNo1) == "47")||(propertyId.at(loopNo1) == "48")||(propertyId.at(loopNo1) == "49")||(propertyId.at(loopNo1) == "50"))//OFF,heat,cool,auto,dry
									{
										if(isInt(delay.at(loopNo1),10))
										{
											if(delay.at(loopNo1) == "0")
											{
												//Bulk json construction
												triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Thermostat",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"Mode",propertyId.at(loopNo1),"","","","","","","");
											}
											else
											{
												if(type.at(loopNo1) == "1")
												{
													//Thread json construction
													thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"Thermostat",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"Mode",propertyId.at(loopNo1),"","","","","","","");
												}
												else
												{
													//sequential json construction
													if(!triggerJson.empty())
													{
														#ifdef DEBUG
															cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
														#endif
														s1.socketSend(triggerJson);
														triggerJson.clear();
													}
													this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
													triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Comfort",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"Mode",propertyId.at(loopNo1),"","","","","","","");
												}
											}
										}
									}
									else if(propertyId.at(loopNo1) == "295")//MODE
									{
										if(isInt(delay.at(loopNo1),10))
										{
											if(delay.at(loopNo1) == "0")
											{
												//Bulk json construction
												triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Comfort",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"Mode",propertyValue.at(loopNo1),"","","","","","","");
											}
											else
											{
												if(type.at(loopNo1) == "1")
												{
													//Thread json construction
													thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"Comfort",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"Mode",propertyValue.at(loopNo1),"","","","","","","");
												}
												else
												{
													//sequential json construction
													if(!triggerJson.empty())
													{
														#ifdef DEBUG
															cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
														#endif
														s1.socketSend(triggerJson);
														triggerJson.clear();
													}
													this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
													triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Comfort",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"Mode",propertyValue.at(loopNo1),"","","","","","","");
												}
											}
										}
									}
									//set point temperature only
									else if((propertyId.at(loopNo1) == "24")||(propertyId.at(loopNo1) == "54")||(propertyId.at(loopNo1) == "55")||(propertyId.at(loopNo1) == "56")||(propertyId.at(loopNo1) == "57")||(propertyId.at(loopNo1) == "58")||(propertyId.at(loopNo1) == "59")||(propertyId.at(loopNo1) == "60")||(propertyId.at(loopNo1) == "61")||(propertyId.at(loopNo1) == "62")||(propertyId.at(loopNo1) == "63")||(propertyId.at(loopNo1) == "64")||(propertyId.at(loopNo1) == "65")||(propertyId.at(loopNo1) == "66") ||(propertyId.at(loopNo1) == "67")||(propertyId.at(loopNo1) == "68")||(propertyId.at(loopNo1) == "69")||(propertyId.at(loopNo1) == "70"))
									{
										if(isInt(delay.at(loopNo1),10))
										{
											if(delay.at(loopNo1) == "0")
											{
												//Bulk json construction
												triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Comfort",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"Setpoint","",propertyValue.at(loopNo1),"","","","","","");
											}
											else
											{
												if(type.at(loopNo1) == "1")
												{
													//Thread json construction
													thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"Comfort",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"Setpoint","",propertyValue.at(loopNo1),"","","","","","");
												}
												else
												{
													//sequential json construction
													if(!triggerJson.empty())
													{
														#ifdef DEBUG
															cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
														#endif
														s1.socketSend(triggerJson);
														triggerJson.clear();
													}
													this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
													triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Comfort",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"Setpoint","",propertyValue.at(loopNo1),"","","","","","");
												}
											}
										}
									}
									//set point with mode
									else if((propertyId.at(loopNo1) == "71")||(propertyId.at(loopNo1) == "72")||(propertyId.at(loopNo1) == "74")||(propertyId.at(loopNo1) == "75")||(propertyId.at(loopNo1) == "76"))
									{
										if(isInt(delay.at(loopNo1),10))
										{
											if(delay.at(loopNo1) == "0")
											{
												//Bulk json construction
												triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Comfort",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"Setpoint","",propertyValue.at(loopNo1),"","","","","","");
											}
											else
											{
												if(type.at(loopNo1) == "1")
												{
													//Thread json construction
													thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"Comfort",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"Setpoint","",propertyValue.at(loopNo1),"","","","","","");
												}
												else
												{
													//sequential json construction
													if(!triggerJson.empty())
													{
														#ifdef DEBUG
															cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
														#endif
														s1.socketSend(triggerJson);
														triggerJson.clear();
													}
													this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
													triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Comfort",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"Setpoint","",propertyValue.at(loopNo1),"","","","","","");
												}
											}
										}
									}
									//fanmode
									else if((propertyId.at(loopNo1) == "82")||(propertyId.at(loopNo1) == "83")||(propertyId.at(loopNo1) == "84")||(propertyId.at(loopNo1) == "85")||(propertyId.at(loopNo1) == "86")||(propertyId.at(loopNo1) == "87"))
									{
										if(isInt(delay.at(loopNo1),10))
										{
											if(delay.at(loopNo1) == "0")
											{
												//Bulk json construction
												triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Comfort",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"Fan Mode",propertyValue.at(loopNo1),"","","","","","","");
											}
											else
											{
												if(type.at(loopNo1) == "1")
												{
													//Thread json construction
													thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"Comfort",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"Fan Mode",propertyValue.at(loopNo1),"","","","","","","");
												}
												else
												{
													//sequential json construction
													if(!triggerJson.empty())
													{
														#ifdef DEBUG
															cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
														#endif
														s1.socketSend(triggerJson);
														triggerJson.clear();
													}
													this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
													triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Comfort",userId.at(loopNo1),localInstanceId,integrationId,uniqueId,"Fan Mode",propertyValue.at(loopNo1),"","","","","","","");
												}
											}
										}
									}
								}
							}
							break;

						case categoryScene:
							{
								MYSQL_ROW row2;
								t2.mysqlQuery("SELECT id FROM Scene where id='" + propertyValue.at(loopNo1) + "' AND user=(SELECT id FROM C_Mymilan_Users WHERE mymilan_user_id='"+userId.at(loopNo1)+"') AND id IS NOT NULL AND user IS NOT NULL;");
								if((row2 = mysql_fetch_row(t2.result)))
								{
									string sceneId = row2[0];
									#ifdef DEBUG
										cout << "Device Category :scene,\n";
										cout << "sceneId         :" << sceneId << ",\n\n";
									#endif
									if(isInt(delay.at(loopNo1),10))
									{
										if(delay.at(loopNo1) == "0")
										{
											//Bulk json construction
											triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Scene",userId.at(loopNo1),propertyValue.at(loopNo1),integrationId,"","","","","","","","","","");
										}
										else
										{
											if(type.at(loopNo1) == "1")
											{
												//Thread json construction
												thread1[loopNo1] = thread(timerSecs,stoi(delay.at(loopNo1)),"Scene",userId.at(loopNo1),propertyValue.at(loopNo1),integrationId,"","","","","","","","","","");
											}
											else
											{
												//sequential json construction
												if(!triggerJson.empty())
												{
													#ifdef DEBUG
														cout <<"JSON(before seq delay):"<< triggerJson << ".\n\n";
													#endif
													s1.socketSend(triggerJson);
													triggerJson.clear();
												}
												this_thread::sleep_for (chrono::seconds(stoi(delay.at(loopNo1))));
												triggerJson = triggerJson + (triggerJson.empty()?"":"\r\n") + s1.jsonCreate("Scene",userId.at(loopNo1),propertyValue.at(loopNo1),integrationId,"","","","","","","","","","");
											}
										}
									}
								}
							}
							break;

						default:
							{
								#ifdef DEBUG
									cout << "Device Category\t\t\t: Invalid(main).\n\n";
								#endif
							}
							break;
					}
				}
        else
  			{
  				#ifdef DEBUG
  					cout << "Property Id check(main).\n\n";
  				#endif
  			}
      }
		}
	}

	if(!triggerJson.empty())
	{
		#ifdef DEBUG
			cout <<"Json Objects created:"<< triggerJson << ".\n\n";
		#endif
		s1.socketSend(triggerJson);
	}

	for(size_t loopNo1=0;loopNo1<instanceId.size();loopNo1++)
	{
		if((propertyId.at(loopNo1) == "15") || (propertyId.at(loopNo1) == "16") || (propertyId.at(loopNo1) == "17") || (propertyId.at(loopNo1) == "18"))
		{
			if(!instanceId.at(loopNo1).empty() && !propertyId.at(loopNo1).empty() && !propertyValue.at(loopNo1).empty() && !userId.at(loopNo1).empty() && !delay.at(loopNo1).empty() && !type.at(loopNo1).empty())
			{
				string passCode{};
				MYSQL_ROW row1;
				t2.mysqlQuery("SELECT passcode FROM C_Mymilan_Users where mymilan_user_id='"+userId.at(loopNo1)+"';");
				while((row1 = mysql_fetch_row(t2.result)))
				{
					passCode = row1[0];
				}
				if(propertyId.at(loopNo1) == "15")//Away Arm
				{
					if(isInt(delay.at(loopNo1),10))
					{
						if(delay.at(loopNo1) != "0")
						{
							if(type.at(loopNo1) == "1")
							{
								//Thread json construction
								#ifdef DEBUG
									cout << "Category\t\t\t: Security(Thread, Away Arm).\n\n";
								#endif
								thread1[loopNo1].join();
							}
						}
					}
				}
				else if(propertyId.at(loopNo1) == "16")//Night Arm
				{
					if(isInt(delay.at(loopNo1),10))
					{
						if(delay.at(loopNo1) != "0")
						{
							if(type.at(loopNo1) == "1")
							{
								//Thread json construction
								#ifdef DEBUG
									cout << "Category		: Security(Thread, Night Arm).\n\n";
								#endif
								thread1[loopNo1].join();
							}
						}
					}
				}
				else if(propertyId.at(loopNo1) == "17")//stay arm
				{
					if(isInt(delay.at(loopNo1),10))
					{
						if(delay.at(loopNo1) != "0")
						{
							if(type.at(loopNo1) == "1")
							{
								//Thread json construction
								#ifdef DEBUG
									cout << "Category\t\t\t: Security(Thread, Stay Arm).\n\n";
								#endif
								thread1[loopNo1].join();
							}
						}
					}
				}
				else if(propertyId.at(loopNo1) == "18")//disarm
				{
					if(isInt(delay.at(loopNo1),10))
					{
						if(delay.at(loopNo1) != "0")
						{
							if(type.at(loopNo1) == "1")
							{
								//Thread json construction
								#ifdef DEBUG
									cout << "Category\t\t\t: Security(Thread, disarm).\n\n";
								#endif
								thread1[loopNo1].join();
							}
						}
					}
				}
			}
		}
		else
		{
			if(!instanceId.at(loopNo1).empty() && !propertyId.at(loopNo1).empty() && !propertyValue.at(loopNo1).empty() && !userId.at(loopNo1).empty() && !delay.at(loopNo1).empty() && !type.at(loopNo1).empty())
			{
				MYSQL_ROW row1;

				string localInstanceId{},addonId1{},addonId2{},integrationId{},propertyIdCheck{},comProtocol{},prodCat{};
				t2.mysqlQuery("SELECT q2.local_instance_id,q2.addon_id,q3.addon_id,q4.integration_id,q6.property_id,q4.communication_protocol_id,q4.product_category FROM C_User_Addon_Instances AS q2 LEFT JOIN C_User_Addons AS q3 ON q2.addon_id = q3.id LEFT JOIN C_Addons AS q4 ON q3.addon_id = q4.id LEFT JOIN C_Product_Type AS q5 ON q5.pro_type_primary_id = q4.product_type LEFT JOIN C_Addons_Properties AS q6 ON q6.addon_id = q3.addon_id AND q6.property_id = '"+propertyId.at(loopNo1)+"' WHERE q2.Instance_Id = '"+instanceId.at(loopNo1)+"' AND q2.local_instance_id IS NOT NULL AND q2.addon_id IS NOT NULL AND q3.addon_id IS NOT NULL AND q4.integration_id IS NOT NULL AND q6.property_id IS NOT NULL AND q4.communication_protocol_id IS NOT NULL;");
				while((row1 = mysql_fetch_row(t2.result)))
				{
					localInstanceId = row1[0];
					addonId1 = row1[1];
					addonId2 = row1[2];
					integrationId = row1[3];
					propertyIdCheck = row1[4];
					comProtocol = row1[5];
					prodCat = row1[6];
				}

				if(propertyId.at(loopNo1) == propertyIdCheck)
				{
					switch(s1.strToEnumMapping(integrationId))
					{
						case categoryMedia:
							{
								MYSQL_ROW row2;
								t2.mysqlQuery("SELECT id FROM Milan_SP.Music where local_instance_id='"+localInstanceId+"' AND id IS NOT NULL;");
								if((row2 = mysql_fetch_row(t2.result)))
								{
									string deviceIdClimax = row2[0];
									if(isInt(delay.at(loopNo1),10))
									{
										if(delay.at(loopNo1) != "0")
										{
											if(type.at(loopNo1) == "1")
											{
												//Thread json construction
												#ifdef DEBUG
													cout << "Category\t\t\t:Media(Thread).\n\n";
												#endif
												thread1[loopNo1].join();
											}
										}
									}
								}
							}
							break;

						case categoryAppliance:
							{
								MYSQL_ROW row2;
								t2.mysqlQuery("SELECT id,control_type,duration FROM Relay_Devices WHERE local_instance_id='"+localInstanceId+"' AND id IS NOT NULL AND control_type IS NOT NULL AND duration IS NOT NULL;");
								if((row2 = mysql_fetch_row(t2.result)))
								{
									string uniqueId = row2[0],controlType = row2[1],delay1= row2[2];
									if(isInt(delay.at(loopNo1),10))
									{
										if(delay.at(loopNo1) != "0")
										{
											if(type.at(loopNo1) == "1")
											{
												//Thread json construction
												#ifdef DEBUG
													cout << "Category\t\t\t: Appliance(Thread).\n\n";
												#endif
												thread1[loopNo1].join();
											}
										}
									}
								}
							}
							break;

						case categoryColorLight:
							{
								MYSQL_ROW row2;
								t2.mysqlQuery("SELECT id,category FROM Lighting where local_instance_id='" +localInstanceId+ "' AND id IS NOT NULL AND category IS NOT NULL;");
								if((row2 = mysql_fetch_row(t2.result)))
								{
									string uniqueId = row2[0],category = row2[1];

									if((propertyId.at(loopNo1) == "9")||(propertyId.at(loopNo1) == "385"))//Random ON & OFF
									{
										if(isInt(delay.at(loopNo1),10))
										{
											if(delay.at(loopNo1) != "0")
											{
												if(type.at(loopNo1) == "1")
												{
													//Thread json construction
													#ifdef DEBUG
														cout << "Category\t\t\t: Color light(Thread,Random).\n\n";
													#endif
													thread1[loopNo1].join();
												}
											}
										}
									}
									else if((propertyId.at(loopNo1) == "1")||(propertyId.at(loopNo1) == "2")||(propertyId.at(loopNo1) == "8"))//ON/OFF and color preset
									{
										if(isInt(delay.at(loopNo1),10))
										{
											if(delay.at(loopNo1) != "0")
											{
												if(type.at(loopNo1) == "1")
												{
													//Thread json construction
													#ifdef DEBUG
														cout << "Category\t\t\t:Color light(Thread,color preset).\n\n";
													#endif
													thread1[loopNo1].join();
												}
											}
										}
									}
									else if(propertyId.at(loopNo1) == "7")//Color value
									{
										if(isInt(delay.at(loopNo1),10))
										{
											if(delay.at(loopNo1) != "0")
											{
												if(type.at(loopNo1) == "1")
												{
													//Thread json construction
													#ifdef DEBUG
														cout << "Category\t\t\t: Color light(Thread,custom color).\n\n";
													#endif
													//string color = rgb(propertyValue.at(loopNo1));
													thread1[loopNo1].join();
												}
											}
										}
									}
								}
							}
							break;

						case categoryLight:
							{
								MYSQL_ROW row2;
								t2.mysqlQuery("SELECT id,category FROM Lighting where local_instance_id='" + localInstanceId + "' AND id IS NOT NULL AND category IS NOT NULL;");
								if((row2 = mysql_fetch_row(t2.result)))
								{
									string uniqueId = row2[0],category = row2[1];
									if(isInt(delay.at(loopNo1),10))
									{
										if(delay.at(loopNo1) != "0")
										{
											if(type.at(loopNo1) == "1")
											{
												//Thread json construction
												#ifdef DEBUG
													cout << "Category\t\t\t: Light(Thread).\n\n";
												#endif
												thread1[loopNo1].join();
											}
										}
									}
								}
							}
							break;

						case categoryCurtain:
							{
								MYSQL_ROW row2;
								t2.mysqlQuery("SELECT id,category FROM Lighting where local_instance_id='" + localInstanceId + "' AND id IS NOT NULL AND category IS NOT NULL;");
								if((row2 = mysql_fetch_row(t2.result)))
								{
									string uniqueId = row2[0],category = row2[1];
									if(isInt(delay.at(loopNo1),10))
									{
										if(delay.at(loopNo1) != "0")
										{
											if(type.at(loopNo1) == "1")
											{
												//Thread json construction
												#ifdef DEBUG
													cout << "Category\t\t\t: Curtain(Thread).\n\n";
												#endif
												thread1[loopNo1].join();
											}
										}
									}
								}
							}
							break;

						case categoryIR:
							{
								if(comProtocol == "22")
								{
									MYSQL_ROW row2;
									t2.mysqlQuery("SELECT id FROM IR_Devices where local_instance_id='" + localInstanceId + "' AND id IS NOT NULL;");
									if((row2 = mysql_fetch_row(t2.result)))
									{
										string uniqueId = row2[0];
										if(propertyId.at(loopNo1) == "39")
										{
											if(isInt(delay.at(loopNo1),10))
											{
												if(delay.at(loopNo1) != "0")
												{
													if(type.at(loopNo1) == "1")
													{
														//Thread json construction
														#ifdef DEBUG
															cout << "Category\t\t\t: IR(Thread,propertyId(39)).\n\n";
														#endif
														thread1[loopNo1].join();
													}
												}
											}
										}
										else
										{
											if(isInt(delay.at(loopNo1),10))
											{
												if(delay.at(loopNo1) != "0")
												{
													if(type.at(loopNo1) == "1")
													{
														//Thread json construction
														#ifdef DEBUG
															cout << "Category\t\t\t: IR(Thread).\n\n";
														#endif
														thread1[loopNo1].join();
													}
												}
											}
										}
									}
								}
							}
							break;

						case categoryDoorLock:
							{
								MYSQL_ROW row2;
								t2.mysqlQuery("SELECT (SELECT id FROM Door_Lock WHERE local_instance_id = '"+localInstanceId+"' AND id IS NOT NULL),(SELECT passcode FROM C_Mymilan_Users WHERE mymilan_user_id = '"+userId.at(loopNo1)+"' AND passcode IS NOT NULL); ");
								if((row2 = mysql_fetch_row(t2.result)))
								{
									string uniqueId = row2[0];
									string passcode = row2[1];
									if(isInt(delay.at(loopNo1),10))
									{
										if(delay.at(loopNo1) != "0")
										{
											if(type.at(loopNo1) == "1")
											{
												//Thread json construction
												#ifdef DEBUG
													cout << "Category\t\t\t: Doorlock(Thread).\n\n";
												#endif
												thread1[loopNo1].join();
											}
										}
									}
								}
								t2.mysqlQuery("UPDATE `Door_Lock` SET `status`='"+propertyValue.at(loopNo1)+"' WHERE `local_instance_id`='"+localInstanceId+"';");
							}
							break;

						case categoryScene:
							{
								MYSQL_ROW row2;
								t2.mysqlQuery("SELECT id FROM Scene where id='" + propertyValue[loopNo1] + "' AND user=(SELECT id FROM C_Mymilan_Users WHERE mymilan_user_id='"+userId[loopNo1]+"') AND id IS NOT NULL AND user IS NOT NULL;");
								if((row2 = mysql_fetch_row(t2.result)))
								{
									string sceneId = row2[0];
									if(delay.at(loopNo1) != "0")
									{
										if(type.at(loopNo1) == "1")
										{
											//Thread json construction
											#ifdef DEBUG
												cout << "Category\t\t\t: Scene(Thread).\n\n";
											#endif
											thread1[loopNo1].join();
										}
									}
								}
							}
							break;

						case categoryNone:
							{
								if(comProtocol.find("22") != string::npos)//IR comm protocol
								{
									MYSQL_ROW row2;
									t2.mysqlQuery("SELECT id FROM IR_Devices where local_instance_id='" + localInstanceId + "' AND id IS NOT NULL;");
									if((row2 = mysql_fetch_row(t2.result)))
									{
										string uniqueId = row2[0];
										if(propertyId.at(loopNo1) == "39")
										{
											if(isInt(delay.at(loopNo1),10))
											{
												if(delay.at(loopNo1) != "0")
												{
													if(type.at(loopNo1) == "1")
													{
														//Thread json construction
														#ifdef DEBUG
															cout << "Category\t\t\t: None/IR (Thread, propertyId(39)).\n\n";
														#endif
														thread1[loopNo1].join();
													}
												}
											}
										}
										else
										{
											if(isInt(delay.at(loopNo1),10))
											{
												if(delay.at(loopNo1) != "0")
												{
													if(type.at(loopNo1) == "1")
													{
														//Thread json construction
														#ifdef DEBUG
															cout << "Category\t\t\t: None/IR (Thread).\n\n";
														#endif
														thread1[loopNo1].join();
													}
												}
											}
										}
									}
								}
								else if(comProtocol.find("10") != string::npos)//Relay comm protocol
								{
									MYSQL_ROW row2;
									t2.mysqlQuery("SELECT id,control_type,duration FROM Relay_Devices WHERE local_instance_id='"+localInstanceId+"' AND id IS NOT NULL AND control_type IS NOT NULL AND duration IS NOT NULL;");
									if((row2 = mysql_fetch_row(t2.result)))
									{
										string uniqueId = row2[0],controlType = row2[1],delay1= row2[2];
										if(isInt(delay.at(loopNo1),10))
										{
											if(delay.at(loopNo1) != "0")
											{
												if(type.at(loopNo1) == "1")
												{
													//Thread json construction
													#ifdef DEBUG
														cout << "Category\t\t\t: Appliance(Thread).\n\n";
													#endif
													thread1[loopNo1].join();
												}
											}
										}
									}
								}
							}
							break;

						case categoryThermostat:
							{
								if((propertyId.at(loopNo1) == "2")||(propertyId.at(loopNo1) == "47")||(propertyId.at(loopNo1) == "48")||(propertyId.at(loopNo1) == "49")||(propertyId.at(loopNo1) == "50"))//OFF,heat,cool,auto,dry
								{
									if(isInt(delay.at(loopNo1),10))
									{
										if(delay.at(loopNo1) != "0")
										{
											if(type.at(loopNo1) == "1")
											{
												//Thread json construction
												#ifdef DEBUG
													cout << "Category\t\t\t: Thermostat(Thread,Mode).\n\n";
												#endif
												thread1[loopNo1].join();
											}
										}
									}
								}
								else if(propertyId.at(loopNo1) == "295")//MODE
								{
									if(isInt(delay.at(loopNo1),10))
									{
										if(delay.at(loopNo1) != "0")
										{
											if(type.at(loopNo1) == "1")
											{
												//Thread json construction
												#ifdef DEBUG
													cout << "Category\t\t\t: Thermostat(Thread,Mode).\n\n";
												#endif
												thread1[loopNo1].join();
											}
										}
									}
								}
								//set point temperature only
								else if((propertyId.at(loopNo1) == "24")||(propertyId.at(loopNo1) == "54")||(propertyId.at(loopNo1) == "55")||(propertyId.at(loopNo1) == "56")||(propertyId.at(loopNo1) == "57")||(propertyId.at(loopNo1) == "58")||(propertyId.at(loopNo1) == "59")||(propertyId.at(loopNo1) == "60")||(propertyId.at(loopNo1) == "61")||(propertyId.at(loopNo1) == "62")||(propertyId.at(loopNo1) == "63")||(propertyId.at(loopNo1) == "64")||(propertyId.at(loopNo1) == "65")||(propertyId.at(loopNo1) == "66")||(propertyId.at(loopNo1) == "67")||(propertyId.at(loopNo1) == "68")||(propertyId.at(loopNo1) == "69")||(propertyId.at(loopNo1) == "70"))
								{
									if(isInt(delay.at(loopNo1),10))
									{
										if(delay.at(loopNo1) != "0")
										{
											if(type.at(loopNo1) == "1")
											{
												//Thread json construction
												#ifdef DEBUG
													cout << "Category\t\t\t: Thermostat(Thread,set point).\n\n";
												#endif
												thread1[loopNo1].join();
											}
										}
									}
								}
								//set point with mode
								else if((propertyId.at(loopNo1) == "71")||(propertyId.at(loopNo1) == "72")||(propertyId.at(loopNo1) == "74")||(propertyId.at(loopNo1) == "75")||(propertyId.at(loopNo1) == "76"))
								{
									if(isInt(delay.at(loopNo1),10))
									{
										if(delay.at(loopNo1) != "0")
										{
											if(type.at(loopNo1) == "1")
											{
												//Thread json construction
												#ifdef DEBUG
													cout << "Category\t\t\t: Thermostat(Thread,set point with mode).\n\n";
												#endif
												thread1[loopNo1].join();
											}
										}
									}
								}
								//fanmode
								else if((propertyId.at(loopNo1) == "82")||(propertyId.at(loopNo1) == "83")||(propertyId.at(loopNo1) == "84")||(propertyId.at(loopNo1) == "85")||(propertyId.at(loopNo1) == "86")||(propertyId.at(loopNo1) == "87"))
								{
									if(isInt(delay.at(loopNo1),10))
									{
										if(delay.at(loopNo1) != "0")
										{
											if(type.at(loopNo1) == "1")
											{
												//Thread json construction
												#ifdef DEBUG
													cout << "Category\t\t\t: Thermostat(Thread,fan mode).\n\n";
												#endif
												thread1[loopNo1].join();
											}
										}
									}
								}
							}
							break;

						default:
							{
								#ifdef DEBUG
									cout << "Device Category\t\t\t: Invalid(thread).\n\n";
								#endif
							}
							break;
					}
				}
				else
				{
					#ifdef DEBUG
						cout << "Property Id check failed(thread).\n\n";
					#endif
				}
			}
		}
	}
	return 0;
}

/****************************************************************************
 * function to convert hex  values to string								*
 * **************************************************************************/
string trigger::rgb(string color)
{
	int redVal,greenVal,blueVal;
	color.erase(0,1);

	#ifdef DEBUG
		cout << "Color without hash\t:" << color << ".\n";
	#endif

	string red="0x"+color.substr(0,2);
	string green="0x"+color.substr(2,2);
	string blue="0x"+color.substr(4,2);

	redVal = stoi(red,NULL,0);
	#ifdef DEBUG
		cout << "red(hex)\t\t\t:" << redVal << ",\n";
	#endif
	redVal = (int)((redVal*100)/255);

	greenVal = stoi(green,NULL,0);
	#ifdef DEBUG
		cout << "green(hex)\t\t\t:" << greenVal << ",\n";
	#endif
	greenVal = (int)((greenVal*100)/255);

	blueVal = stoi(blue,NULL,0);
	#ifdef DEBUG
		cout << "blue(hex)\t\t\t:" << blueVal << ".\n";
	#endif
	blueVal = (int)((blueVal*100)/255);

	#ifdef DEBUG
		cout << "red\t\t\t\t:" << redVal << ",\n";
		cout << "green\t\t\t\t:" << greenVal << ",\n";
		cout << "blue\t\t\t\t:" << blueVal << ".\n\n";
	#endif

	red = ((to_string(redVal).length() == 1)? ("00"+to_string(redVal)) : ((to_string(redVal).length() == 2)? ("0"+to_string(redVal)) : to_string(redVal)));
	green = ((to_string(greenVal).length() == 1)? ("00"+to_string(greenVal)) : ((to_string(greenVal).length() == 2)? ("0"+to_string(greenVal)) : to_string(greenVal)));
	blue = ((to_string(blueVal).length() == 1)? ("00"+to_string(blueVal)) : ((to_string(blueVal).length() == 2)? ("0"+to_string(blueVal)) : to_string(blueVal)));

	string rgbCmd = red+","+green+","+blue;

	return rgbCmd;
}

/*******************************************************************************
 * Function to run a threaded timer.										   *
 * timerTh.detach();should be used in program								   *
 * allowing timer to run independently						    			   *
 *******************************************************************************/
void timerSecs(int timeInSecs,string param1,string param2,string param3,string param4,string param5,string param6,string param7,string param8,string param9,string param10,string param11,string param12,string param13,string param14)
{
	/*-----timer_flag=1;-----*/
	/*-----cout <<"timer_flag--->"<<timer_flag<<endl;-----*/
	/*-----if timer is used in program "timer_flag" should be decclared.-----*/
	this_thread::sleep_for (chrono::seconds(timeInSecs));
	jsonSend s1;
	#ifdef DEBUG
		cout << "Thread json:" << s1.jsonCreate(param1,param2,param3,param4,param5,param6,param7,param8,param9,param10,param11,param12,param13,param14) << ".\n\n";
	#endif
	s1.socketSend(s1.jsonCreate(param1,param2,param3,param4,param5,param6,param7,param8,param9,param10,param11,param12,param13,param14));
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
 * To url encode the string													   *
 * *****************************************************************************/
string trigger::urlEncode(string encodeData)
{
	CURL *curl = curl_easy_init();
	string encodedData{};
	if(curl)
	{
		encodedData = curl_easy_escape(curl, encodeData.c_str(), encodeData.length());
		if(!encodedData.empty())
		{}
		//curl_free(output);
	}
	return encodedData;
}

//callback function for curl
size_t CurlWrite_CallbackFunc_StdString(void *contents, size_t size, size_t nmemb, string *s)
{
    size_t newLength = size*nmemb;
    size_t oldLength = s->size();
    try
    {
        s->resize(oldLength + newLength);
    }
    catch(std::bad_alloc &e)
    {
        //handle memory problem
        return 0;
    }

    copy((char*)contents,(char*)contents+newLength,s->begin()+oldLength);
    return size*nmemb;
}
#endif
