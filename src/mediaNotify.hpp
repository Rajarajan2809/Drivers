/*******************************************************************************
 *	Purpose		:	Media notification.							   			   *
 * 	Description	:	This is the header Main Program to send notifications 	   *
 * 					to media.												   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	03-Jan-17												   *
 * 	DOLM		:	09-Jan-17												   *
 *******************************************************************************/
/*-----CPP library-----*/
#include <iostream>

/*-----local header files-----*/
#include "../include/mysql.hpp"
#include "../include/cJSON.hpp"

/*-----CGI Headers-----*/
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

//C library for GNU
#include <unistd.h>
#include <string.h> /* for strncpy */

//Socket library
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

//curl library
#include <curl/curl.h>

using namespace std;
using namespace cgicc;

//class for media notification
class mediaNotify
{
	public:
		mediaNotify(string locInsId, string catId, string devStatus);
		~mediaNotify();
		testMysql db;
		friend size_t writeCallback(void *contents, size_t size, size_t nmemb, string *s);
		bool notify();
		string getMyIp(string interface);

	private:
		string locInsId,catId,devStatus;
		bool curlRequests(string ip,string port,short int type, string param1);
};

/*******************************************************************************
 * Constructor for Class scene.							 					   *
 *******************************************************************************/
mediaNotify::mediaNotify(string locInsId,string catId,string devStatus):db("Milan_SP")
{
	/*-----Progress flag is set and Stop flag is reset-----*/
	this -> locInsId = locInsId;
	this -> catId = catId;
	this -> devStatus = devStatus;
}

/*******************************************************************************
 * Destructor for Class testGateway.			 							   *
 *******************************************************************************/
mediaNotify::~mediaNotify()
{
	locInsId.clear();
	catId.clear();
	devStatus.clear();
}

/****************************************************************************************
 * Function to store curl response in DB.												*
 ****************************************************************************************/
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

//NOFICATION
//{"jsonrpc":"2.0","method":"GUI.ShowNotification","params":{"title":"Visitor at the Door","message":"You have a visitor at the Front Door","image":"http://www.milanity.com/assets/img/main/milan_dle.png","displaytime":3000},"id":1}
//VISITOR LIVE VIEW
//{"jsonrpc":"2.0","method":"Player.Open","params":{"item":{"file":"rtsp://admin:admin@35.65.2.191:555/12"}},"id":1}
/****************************************************************************************
 * Function to call media.																*
 ****************************************************************************************/
bool mediaNotify::notify()
{
	//mysql variables
	MYSQL_ROW row1;
	
	//media list
	vector<string> medName{},medIp{},medPort{},devId{},devName{},camLink{},flag{};
	if(locInsId != "NA")//for devices, local instance id is not "NA" and is a value.
	{
		db.mysqlQuery("SELECT M.name,M.ip,M.port,D.zone_id,D.name,D.rtsp_url_medium,D.ref FROM Music AS M JOIN (SELECT name, id AS zone_id, '' AS rtsp_url_medium, 1 AS ref FROM Security SEC WHERE SEC.local_instance_id = '"+locInsId+"' UNION ALL SELECT camera_name AS name,lan_ip AS zone_id,rtsp_url_medium,2 AS ref FROM Surveilance S WHERE S.local_instance_id = '"+locInsId+"' UNION ALL SELECT friendly_name AS name,'' AS zone_id,'' AS rtsp_url_medium,4 AS ref FROM Door_Lock DL WHERE DL.local_instance_id = '"+locInsId+"') AS D WHERE M.integration_id = '10000' AND M.name IS NOT NULL AND M.ip IS NOT NULL AND M.port IS NOT NULL AND D.zone_id IS NOT NULL AND D.name IS NOT NULL AND D.rtsp_url_medium IS NOT NULL AND D.ref IS NOT NULL AND M.visitor_video=1;");
		unsigned int numRsMed = mysql_num_rows(db.result);
		#ifdef DEBUG
			cout << "No of Media:" << numRsMed << br();
		#endif
		if(numRsMed > 0)
		{
			//media device present
			while((row1 = mysql_fetch_row(db.result)))
			{
				medName.push_back(row1[0]);
				medIp.push_back(row1[1]);
				medPort.push_back(row1[2]);
				devId.push_back(row1[3]);
				devName.push_back(row1[4]);
				camLink.push_back(row1[5]);
				flag.push_back(row1[6]);
			}
			
			for(size_t loopNo1 =0; loopNo1 < numRsMed;loopNo1++)
			{
				switch(stoi(flag[loopNo1]))
				{
					case 1://type:Notification; device:door sensor.cat id =1
						switch(stoi(devStatus))
						{
							case 1:
								{
									curlRequests(medIp[loopNo1],medPort[loopNo1], stoi(flag[loopNo1]), devName[loopNo1]+" is secure");
								}
								break;
								
							case 2:
								{
									curlRequests(medIp[loopNo1],medPort[loopNo1], stoi(flag[loopNo1]), devName[loopNo1]+" is insecure");
								}
								break;
								
							case 3:
								{
									curlRequests(medIp[loopNo1],medPort[loopNo1], stoi(flag[loopNo1]), devName[loopNo1]+" is opened");
								}
								break;
								
							case 4:
								{
									curlRequests(medIp[loopNo1],medPort[loopNo1], stoi(flag[loopNo1]), devName[loopNo1]+" is closed");
								}
								break;			
						}
						break;

					case 2://type:notification & streaming; device:camera.cat id =2
						switch(stoi(devStatus))
						{
							case 1:
								#ifdef DEBUG
									cout << "No of Media:" << camLink[loopNo1] << br();
								#endif
													
								curlRequests(medIp[loopNo1],medPort[loopNo1],stoi("1"),"You have Visitor at "+devName[loopNo1]);//notification
								sleep(5);	
								curlRequests(medIp[loopNo1],medPort[loopNo1],stoi(flag[loopNo1]),camLink[loopNo1]);//camera stream							
								
								break;

							case 2:
								curlRequests(medIp[loopNo1],medPort[loopNo1],stoi(flag[loopNo1]),"");//camera stream
								break;
								
							case 3:
								#ifdef DEBUG
									cout << "No of Media:" << camLink[loopNo1] << br();
								#endif
								curlRequests(medIp[loopNo1],medPort[loopNo1],stoi("1"),"You have Visitor at "+devName[loopNo1]);//notification
								break; 
						}
						break;
 
					case 4://type:notification; device:doorlock.cat id =4
						switch(stoi(devStatus))
						{
							case 20://close
								{
									curlRequests(medIp[loopNo1],medPort[loopNo1], stoi(flag[loopNo1]), devName[loopNo1]+" is locked");
								}
								break;
								
							case 21://open
								{
									curlRequests(medIp[loopNo1],medPort[loopNo1], stoi(flag[loopNo1]), devName[loopNo1]+" is unlocked");
								}
								break;
						}
						break;
				}
			}
		}
	}
	else
	{//for Security, local instance id is "NA" and is not a value.
		db.mysqlQuery("SELECT name,ip,port FROM Music;");
		unsigned int numRsMed = mysql_num_rows(db.result);
		#ifdef DEBUG
			cout << "No of Media:" << numRsMed << br();
		#endif
		if(numRsMed > 0)
		{
			while((row1 = mysql_fetch_row(db.result)))
			{
				medName.push_back(row1[0]);
				medIp.push_back(row1[1]);
				medPort.push_back(row1[2]);
			}
			for(size_t loopNo1 =0; loopNo1 < numRsMed;loopNo1++)
			{
				if(catId == "3")//type:Notification
				{
					switch(stoi(devStatus))
					{
						case 15://away arm
							{
								curlRequests(medIp[loopNo1],medPort[loopNo1],3,"Away Armed");
							}
							break;
							
						case 16://night arm
							{
								curlRequests(medIp[loopNo1],medPort[loopNo1],3,"Night Armed");
							}
							break;
							
						case 17://stay arm
							{
								curlRequests(medIp[loopNo1],medPort[loopNo1],3,"Stay Armed");
							}
							break;
							
						case 18://disarm
							{
								curlRequests(medIp[loopNo1],medPort[loopNo1],3,"Disarmed");
							}
							break;			
					}
				}
			}
		}
	}
	return 0;
}

//class member to get device ip
string mediaNotify::getMyIp(string interface)
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
	#ifdef DEBUG
		cout << "my ip:" << ip << br();
	#endif
	
	return ip;
}

//request for jsonrpc
bool mediaNotify::curlRequests(string ip,string port,short int type,string param1)
{
	CURL *curl;
	CURLcode res;
	string curlResponse{},imgLoc{};
	struct curl_slist *headers = NULL;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	#ifdef DEBUG
		cout << "Media IP: " << ip << br();
		cout << "Media port: " << port << br();
	#endif
	
	//image selection
	switch(type)
	{
		case 1:
		case 2:
			imgLoc = "http://"+getMyIp("eth0")+":6161/images_notify/door_camera.png";
			break;

		case 3:
			imgLoc = "http://"+getMyIp("eth0")+":6161/images_notify/security.png";
			break;

		case 4:
			imgLoc = "http://"+getMyIp("eth0")+":6161/images_notify/door_lock.png";
			break;
	}
	
	/*-----JSON variable declaration----*/
	cJSON *mainObj=NULL,*paramsObj=NULL,*itemObj=NULL;
	/*-----JSON string construction begins-----*/
	//{"jsonrpc":"2.0","method":"GUI.ShowNotification","params":{"title":"Visitor at the Door","message":"You have a visitor at the Front Door","image":"http://www.milanity.com/assets/img/main/milan_dle.png","displaytime":3000},"id":1}
	mainObj=cJSON_CreateObject();
	cJSON_AddStringToObject(mainObj, "jsonrpc", "2.0");
	if((type == 1) || (type ==3) || (type ==4))//door sensor, camera(notification before streaming) and security
	{
		cJSON_AddStringToObject(mainObj, "method", "GUI.ShowNotification");
		cJSON_AddItemToObject(mainObj, "params", paramsObj=cJSON_CreateObject());
		cJSON_AddStringToObject(paramsObj, "title", (type == 1)?"Visitor":(type == 3)?"Security":"Door Lock");//use this message in switch case
		cJSON_AddStringToObject(paramsObj, "message", param1.c_str());//for type 1 , param1 is message
		cJSON_AddStringToObject(paramsObj, "image", imgLoc.c_str());
		cJSON_AddNumberToObject(paramsObj, "displaytime", 10000);
	}
	//{"jsonrpc":"2.0","method":"Player.Open","params":{"item":{"file":"rtsp://admin:admin@35.65.2.191:555/12"}},"id":1}
	else if(type == 2)//camera streaming
	{
		switch(stoi(devStatus))
		{
			case 1:
				{
					cJSON_AddStringToObject(mainObj, "method", "Player.Open");
					cJSON_AddItemToObject(mainObj, "params", paramsObj=cJSON_CreateObject());
					cJSON_AddItemToObject(paramsObj, "item", itemObj=cJSON_CreateObject());
					cJSON_AddStringToObject(itemObj, "file", param1.c_str());//for type 2, param1 is rtsp link
				}
				break;
			case 2:
				{
					cJSON_AddStringToObject(mainObj, "method", "Player.Stop");
					cJSON_AddItemToObject(mainObj, "params", paramsObj=cJSON_CreateObject());
					cJSON_AddNumberToObject(paramsObj, "playerid", 1);
					cJSON_AddNumberToObject(mainObj, "id", 1);
				}
				break;
		}
	}
	cJSON_AddNumberToObject(mainObj, "id", 1);
	cJSON_Print(mainObj);
	string queryJson = cJSON_PrintUnformatted(mainObj);
	cJSON_Delete(mainObj);
	
	#ifdef DEBUG
		cout << "queryJson:" << queryJson << br() << br();
	#endif
			
	curl = curl_easy_init();
	if(curl)
	{
		string webAddress = "http://"+ip+":"+port+"/jsonrpc";
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "charsets: utf-8");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_URL, webAddress.c_str());
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, queryJson.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlResponse);
		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		#ifdef DEBUG
			cout << "curl response:" << res << br() << br();
		#endif
		/* Check for errors */
		if(res != CURLE_OK)
		{
			#ifdef DEBUG
				fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
			#endif
			return 0;
		}
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	return 1;
}
