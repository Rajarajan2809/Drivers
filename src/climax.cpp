/*******************************************************************************
 *	Purpose		:	Media test.												   *
 * 	Description	:	Source file for Media test.								   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	14-Sep-16												   *
 * 	DOLM		:	24-Dec-16												   *
 *******************************************************************************/

/*-----local files-----*/
#include "expSys.hpp"

using namespace std;
using namespace cgicc;

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

/****************************************************************************************
 * Function to call curl request.														*
 ****************************************************************************************/
bool expSys::curlRequests(string webAddress,string method,string item,string key,string keyValue,int id)
{
	CURL *curl;
    CURLcode res;
	string curlResponse{};
	struct curl_slist *headers = NULL;
    curl_global_init(CURL_GLOBAL_DEFAULT);
	
	/*-----JSON variable declaration----*/
	cJSON *mainObj=NULL,*paramsObj=NULL,*itemObj=NULL;
	cJSON *json=NULL,*spd=NULL,*playerid=NULL,*type=NULL,*error=NULL;
	
	/*-----JSON string construction begins-----*/
	mainObj=cJSON_CreateObject();
	cJSON_AddItemToObject(mainObj, "jsonrpc", cJSON_CreateString("2.0"));
	cJSON_AddItemToObject(mainObj, "method", cJSON_CreateString(method.c_str()));
	if(!item.empty() && !key.empty() && !keyValue.empty())
	{
		//curlRequests("http://"+climaxIp+":"+climaxPort+"/jsonrpc","Player.Stop","","playerid",to_string(playerId),1)
		if(method == "Player.Stop")
		{cJSON_AddNumberToObject(mainObj, "id", id);}
		cJSON_AddItemToObject(mainObj, "params", paramsObj=cJSON_CreateObject());
		cJSON_AddItemToObject(paramsObj, item.c_str(), itemObj=cJSON_CreateObject());
		cJSON_AddItemToObject(itemObj, key.c_str(), cJSON_CreateString(keyValue.c_str()));
		if(method != "Player.Stop")
		{cJSON_AddNumberToObject(mainObj, "id", id);}
	}
	else if(item.empty() && !key.empty() && !keyValue.empty())
	{
		//cout <<"isdigit status ->"<<isdigit(stoi(keyValue))<<endl;
		if(isInt(keyValue,10))
		{
			cJSON_AddItemToObject(mainObj, "params", paramsObj=cJSON_CreateObject());
			cJSON_AddItemToObject(paramsObj, key.c_str(), cJSON_CreateNumber(stoi(keyValue)));
			cJSON_AddNumberToObject(mainObj, "id", id);
		}
		else
		{
			cJSON_AddItemToObject(mainObj, "params", paramsObj=cJSON_CreateObject());
			cJSON_AddItemToObject(paramsObj, key.c_str(), cJSON_CreateString(keyValue.c_str()));
			cJSON_AddNumberToObject(mainObj, "id", id);
		}
	}
	else
	{
		cJSON_AddNumberToObject(mainObj, "id", id);
	}		
	cJSON_Print(mainObj);
	string queryJson = cJSON_PrintUnformatted(mainObj);
	#ifdef DEBUG
		cout << "queryJson:" << queryJson << ".\n\n";
	#endif
	
	cJSON_Delete(mainObj);
	
    curl = curl_easy_init();
    
    if(curl)
    {
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "charsets: utf-8");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_URL, webAddress.c_str());
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2L);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, queryJson.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlResponse);
        

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
	
	bool retVal = 0;
	string resultNumber{},resultString{},playerType{};
	//int resultNum{},speed{};
	#ifdef DEBUG
		cout << "curlResponse	:" << curlResponse << ".\n\n";
	#endif
	
	/*-----PARSING Json response from milan climax-----*/
	if(!curlResponse.empty())		
	{
		json = cJSON_GetObjectItem(cJSON_Parse(curlResponse.c_str()),"result");
		if(json != NULL)
		{
			spd = cJSON_GetObjectItem(json,"speed");
			if(spd != NULL)
			{
				//speed=spd->valueint;
				#ifdef DEBUG
					cout << "Player speed 		:" << spd->valueint << ".\n\n";
				#endif
				if(method == "Player.PlayPause")
				{
					retVal=1;
				}
			}	
			else if(cJSON_GetArraySize(json)>0)
			{
				for (int loopNumber1 = 0 ; loopNumber1 < cJSON_GetArraySize(json) ; loopNumber1++)
				{
					cJSON* subarray = cJSON_GetArrayItem(json, loopNumber1);
					playerid =  cJSON_GetObjectItem(subarray,"playerid");
					type =  cJSON_GetObjectItem(subarray,"type");
					playerId=playerid->valueint;
					playerType=type->valuestring;
					
					#ifdef DEBUG
						cout << "playerType		:" << playerType << ",\n";
						cout << "playerId 		:" << playerId << ".\n\n";
					#endif
					
					if(method == "Player.GetActivePlayers")
					{
						retVal=1;
						//testStatusCommandFlag=1;
					}
				}				
			}
			else
			{
				resultNumber = print_number(json);
				resultString = print_string(json);
				if((isInt(resultNumber,10))&&resultString.empty())
				{
					//resultNum=json->valueint;
					#ifdef DEBUG
						cout << "Volume 	:" << json->valueint << ".\n\n";
					#endif
					if((method == "Application.SetMute")||(method == "Application.SetVolume"))
					{
						retVal=1;
					}
				}
				else
				{
					resultString=json->valuestring;
					#ifdef DEBUG
						cout << "status 	:" << resultString << ".\n\n";
					#endif
					if((method == "Player.Stop")||(method == "JSONRPC.Ping")||(method == "Player.Open"))
					{
						retVal=1;
					}
				}
			}
		}
		else
		{
			error = cJSON_GetObjectItem(cJSON_Parse(curlResponse.c_str()),"error");
			if(error != NULL)
			{
				/*-----ERROR return value-----*/
				retVal = 0;
			}
		}
		
	}
	else
	{	
		retVal =0;
	}
	return retVal;
}

int expSys::climaxTest()
{
	MYSQL_ROW row;
	string climaxName{},climaxIp{},climaxPort{},localInstanceId{};
	t2.mysqlQuery("SELECT M.name AS name, M.ip AS ip, M.port AS port, M.local_instance_id AS local_instance_id FROM C_User_Addon_Instances AS UAI LEFT JOIN Music AS M ON M.local_instance_id = UAI.local_instance_id WHERE UAI.instance_id = '"+instanceId+"' AND M.ip IS NOT NULL AND M.port IS NOT NULL AND M.local_instance_id IS NOT NULL AND M.name IS NOT NULL;");
	while((row = mysql_fetch_row(t2.result)))
	{
		climaxName = row[0];
		climaxIp = row[1];
		climaxPort = row[2];
		localInstanceId = row[3];
	}
	
	#ifdef DEBUG
		cout << "climaxName      : " << climaxName << ",\n";
		cout << "climaxIp        : " << climaxIp << ",\n";
		cout << "climaxPort      : " << climaxPort << ",\n";
		cout << "localInstanceId : " << localInstanceId << ".\n\n";
	#endif
		
	if(!climaxName.empty() && !climaxIp.empty() && !climaxPort.empty() && !localInstanceId.empty())
	{
		if(testId == "TS0051")
		{
			//if(curlRequests("http://"+climaxIp+":"+climaxPort+"/jsonrpc","JSONRPC.Ping","","","",1))
			//{
				if(curlRequests("http://"+climaxIp+":"+climaxPort+"/jsonrpc","Player.Open","item","file","/sdcard/Movies/demo.mp4",1))
				{
					if(curlRequests("http://"+climaxIp+":"+climaxPort+"/jsonrpc","Player.GetActivePlayers","","","",1))
					{
						//if(curlRequests("http://"+climaxIp+":"+climaxPort+"/jsonrpc","Player.PlayPause","","playerid",to_string(playerId),1))
						//{
							//if(curlRequests("http://"+climaxIp+":"+climaxPort+"/jsonrpc","Application.SetMute","","mute","toggle",2))
							//{
								if(curlRequests("http://"+climaxIp+":"+climaxPort+"/jsonrpc","Application.SetVolume","","volume","50",1))
								{
									sleep(4);
									if(curlRequests("http://"+climaxIp+":"+climaxPort+"/jsonrpc","Player.Stop","","playerid",to_string(playerId),1))
									{
										jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"1",respId,climaxIp,"",climaxName,"STOP");
									}
									else
										jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,climaxIp,"",climaxName,"STOP");
								}
								else
									jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,climaxIp,"",climaxName,"VOLUME");
							//}
						//}
					}
					else
						jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,climaxIp,"",climaxName,"PLAY_STATUS");
					
				}
				else
					jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,climaxIp,"",climaxName,"PLAY");
		}
		else if(testId == "TS0031")
		{
			allTestFlag = "0";
			if(pingIp())
			{
				if(pingIpConflict())
				{
					//if(curlRequests("http://"+climaxIp+":"+climaxPort+"/jsonrpc","JSONRPC.Ping","","","",1))
					//{
						if(curlRequests("http://"+climaxIp+":"+climaxPort+"/jsonrpc","Player.Open","item","file","storage/sdcard0/Download/terminator_genisys.mkv",1))
						{
							if(curlRequests("http://"+climaxIp+":"+climaxPort+"/jsonrpc","Player.GetActivePlayers","","","",1))
							{
								//if(curlRequests("http://"+climaxIp+":"+climaxPort+"/jsonrpc","Player.PlayPause","","playerid",to_string(playerId),1))
								//{
									//if(curlRequests("http://"+climaxIp+":"+climaxPort+"/jsonrpc","Application.SetMute","","mute","toggle",2))
									//{
										if(curlRequests("http://"+climaxIp+":"+climaxPort+"/jsonrpc","Application.SetVolume","","volume","50",1))
										{
											sleep(4);
											if(curlRequests("http://"+climaxIp+":"+climaxPort+"/jsonrpc","Player.Stop","","playerid",to_string(playerId),1))
											{
												jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"1",respId,climaxIp,"",climaxName,"STOP");
											}
											else
												jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,climaxIp,"",climaxName,"STOP");
										}
										else
											jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,climaxIp,"",climaxName,"VOLUME");
									//}
								//}
							}
							else
								jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,climaxIp,"",climaxName,"PLAY_STATUS");
							
						}
						else
							jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,climaxIp,"",climaxName,"PLAY");
					//}
					//else
					//{}
				}
				else
				{
					jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,climaxIp,"",climaxName,"IP_CONFLICT");
				}
			}
			else
			{
				jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,climaxIp,"",climaxName,"LAN");
			}
		}
	}
	return 0;
}

int main()
{
	try
	{
		/*-----Declaration-----*/
		Cgicc cgi;
		
		#ifdef DEBUG
			cout << "\t\t\t Climax Test Begins\t\t\t\n\n";
		#endif
				
		/*-----getting the User id-----*/
		//http://35.65.2.218:6161/Milan/Drivers/MILAN_EXE/EXPSYS/climax.cgi?relationship_id=NA&test_id=TS0051&instance_id=288201D2-0412-4156-AB87-6657BBD2FBB8&local_instance_id=NA&property_id=NA&property_value=NA&response_id=428
		string testId{},instanceId{},userId{},propertyId{},propertyValue{},respId{};
		for(int loopNo1=1;loopNo1 <= 6;loopNo1++)
		{
			switch(loopNo1)
			{
				case 1:
					{
						form_iterator i1 = cgi.getElement("relationship_id");
						if(i1 != cgi.getElements().end())
						{
							userId = i1->getValue();
						}
					}
					break;
					
				case 2:
					{
						form_iterator i1 = cgi.getElement("test_id");
						if(i1 != cgi.getElements().end())
						{
							testId = i1->getValue();
						}
					}
					break;
				
				case 3:
					{
						form_iterator i1 = cgi.getElement("instance_id");
						if(i1 != cgi.getElements().end())
						{
							instanceId = i1->getValue();
						}
					}
					break;
					
				case 4:
					{
						form_iterator i1 = cgi.getElement("property_id");
						if(i1 != cgi.getElements().end())
						{
							propertyId = i1->getValue();
						}
					}
					break;
					
				case 5:
					{
						form_iterator i1 = cgi.getElement("property_value");
						if(i1 != cgi.getElements().end())
						{
							propertyValue = i1->getValue();
						}
					}
					break;
					
				case 6:
					{
						form_iterator i1 = cgi.getElement("response_id");
						if(i1 != cgi.getElements().end())
						{
							respId = i1->getValue();
						}
					}
					break;
					
			}
		}
		
		#ifdef DEBUG
			cout << "User id\t\t: " << userId << ",\n";
			cout << "Test id\t\t: " << testId << ",\n";
			cout << "Instance id\t: " << instanceId << ",\n";
			cout << "Property id\t: " << propertyId << ",\n";
			cout << "Property Value\t: " << propertyValue << ",\n";
			cout << "Resp id\t\t: " << respId << ".\n\n";
		#endif
				
		//testId = XXXXXXXXXXXXXXXX;
		//instanceId = XXXXXXXXXXXXX;
		//Example query:http://localhost:6161/Milan/Drivers/MILAN_EXE/expert_system/ping.cgi?test_id=TS0010&relationship_id=XXXXXXXXXXXXXX&instance_id=XXXXXXXXXXXXXXXX&property_id=XXXXXXXXXXXXXXXXXXXX&property_value=XXXXXXXXXXXXXXXX&resp_id=XXXXXXXXXXXXX
		if(!testId.empty() && !userId.empty() && !instanceId.empty() && !propertyId.empty() && !propertyValue.empty() && !respId.empty())
		{
			expSys e1(userId,instanceId,propertyId,propertyValue,testId,respId);
			if((testId == "TS0051")||(testId == "TS0031"))//Media test
			{	
				e1.climaxTest();
			}
		}
		else
		{
			if(testId.empty())
			{
				#ifdef DEBUG
					cout << "test_id is empty.\n\n";
				#endif
				throw 1;
			}
			if(instanceId.empty())
			{
				#ifdef DEBUG
					cout << "instance_id is empty.\n\n";
				#endif
				throw 1;
			}
			if(userId.empty())
			{
				#ifdef DEBUG
					cout << "user_id is empty.\n\n";
				#endif
				throw 1;
			}
			if(propertyId.empty())
			{
				#ifdef DEBUG
					cout << "property_id is empty.\n\n";
				#endif
				throw 1;
			}
			if(propertyValue.empty())
			{
				#ifdef DEBUG
					cout << "property_value is empty.\n\n";
				#endif
				throw 1;
			}
			if(respId.empty())
			{
				#ifdef DEBUG
					cout << "resp_id is empty.\n\n";
				#endif
				throw 1;
			}
		}
			
		/*-----Command to Close the HTML document------*/
		#ifdef DEBUG
			cout << "Media Test finished successfully.";
		#endif
	}
	catch(exception& e) 
	{
		/*-----handle any errors - omitted for brevity-----*/
		#ifdef DEBUG
			cout << "Exception handled, Media Test terminated, please check for errors.";
		#endif
	}
}
