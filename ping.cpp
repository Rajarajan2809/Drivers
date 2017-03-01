/*******************************************************************************
 *	Purpose		:	Ping test.												   *
 * 	Description	:	Source file for Ping Test.								   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	2-Jul-16												   *
 * 	DOLM		:	1-Nov-16												   *
 *******************************************************************************/

/*-----local files-----*/
#include "expSys.hpp"

using namespace std;
using namespace cgicc;

/******************************************************************************
 * Ping Router																  *
 * ****************************************************************************/
int expSys::pingRouter()
{
	//Ping IP DUT function.
	//cout <<"Send the Ping Command to Device Under Test of Instance ID: "<<instanceId<<br();
	string cmd = "ping -W4 -c4 -A ",ip{},localInstanceId{},friendlyName{},port{};
	MYSQL_ROW row1;
	t2.mysqlQuery("SELECT ID.ip AS ip, ID.port AS port, UAI.local_instance_id AS local_instance_id, ID.name AS name FROM C_User_Addon_Instances AS UAI LEFT JOIN IR_Devices AS IRD ON IRD.local_instance_id = UAI.local_instance_id LEFT JOIN IP_IR_Relay AS IIR ON IIR.ir_device_id = IRD.id LEFT JOIN IP_Devices AS ID ON ID.id = IIR.device_id WHERE UAI.instance_id = '"+instanceId+"' AND ID.ip IS NOT NULL AND ID.port IS NOT NULL AND ID.local_instance_id IS NOT NULL AND ID.name IS NOT NULL UNION ALL SELECT ID.ip AS ip, ID.port AS port, UAI.local_instance_id AS local_instance_id, ID.name AS name FROM C_User_Addon_Instances AS UAI LEFT JOIN Relay_Devices AS RD ON RD.local_instance_id = UAI.local_instance_id LEFT JOIN IP_IR_Relay AS IIR ON IIR.ir_device_id = RD.id LEFT JOIN IP_Devices AS ID ON ID.id = IIR.device_id WHERE UAI.instance_id = '"+instanceId+"' AND ID.ip IS NOT NULL AND ID.port IS NOT NULL AND ID.local_instance_id IS NOT NULL AND ID.name IS NOT NULL UNION ALL SELECT L.ref_1 AS ip, L.ref_2 AS port, L.local_instance_id AS local_instance_id, L.name AS name FROM C_User_Addon_Instances AS UAI LEFT JOIN Lighting AS L ON L.local_instance_id = UAI.local_instance_id WHERE UAI.instance_id = '"+instanceId+"' AND L.ref_1 IS NOT NULL AND L.ref_2 IS NOT NULL AND L.local_instance_id IS NOT NULL AND L.name IS NOT NULL UNION ALL SELECT M.ip AS ip, M.port AS port, M.local_instance_id AS local_instance_id, M.name AS name FROM C_User_Addon_Instances AS UAI LEFT JOIN Music AS M ON M.local_instance_id = UAI.local_instance_id WHERE UAI.instance_id = '"+instanceId+"' AND M.ip IS NOT NULL AND M.port IS NOT NULL AND M.local_instance_id IS NOT NULL AND M.name IS NOT NULL UNION ALL SELECT S.lan_ip, S.port, S.local_instance_id, S.camera_name FROM C_User_Addon_Instances AS UAI LEFT JOIN Surveilance AS S ON S.local_instance_id = UAI.local_instance_id WHERE UAI.instance_id = '"+instanceId+"' AND S.lan_ip IS NOT NULL AND S.port IS NOT NULL AND S.local_instance_id IS NOT NULL AND S.camera_name IS NOT NULL;");
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
		string delimiter = ".",token[4];
		size_t delimiterPos1 = 0;
		for(int loopNo1=0; loopNo1<4 ;loopNo1++) /*-----similiar operation -> ((pos = s.find(delimiter)) != string::npos)-----*/
		{
			delimiterPos1 = ip.find(delimiter);
			token[loopNo1] =ip.substr(0, delimiterPos1);
			ip.erase(0, delimiterPos1 + delimiter.length());
		}
		
		/*-----Router IP generation.-----*/
		ip =token[0]+"."+token[1]+"."+token[2]+".1";
		cmd = cmd +ip;
		#ifdef DEBUG
			cout << "cmd					:" << cmd << ".\n\n";
		#endif
		//Terminal "Ping" sent via system() command.
		switch(system(cmd.c_str()))
		{
			case 0:
				//Ping success JSON sent
				{	
					if(allTestFlag != "0")
						jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"1",respId,"","",friendlyName,"");
					return 1;
				}
				break;
					
			default:
				//Ping failure JSON sent
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

int expSys::pingDns()
{
	//Ping IP DUT function.
	string cmd = "ping -W4 -c4 -A 8.8.8.8",ip{},localInstanceId{},friendlyName{},port{};
	MYSQL_ROW row1;
	t2.mysqlQuery("SELECT ID.ip AS ip,ID.port AS port,ID.local_instance_id AS local_instance_id,ID.name AS name FROM C_User_Addon_Instances AS UAI LEFT JOIN IP_Devices AS ID ON ID.local_instance_id = UAI.local_instance_id WHERE UAI.instance_id = '"+instanceId+"' AND ID.ip IS NOT NULL AND ID.port IS NOT NULL AND ID.local_instance_id IS NOT NULL AND ID.name IS NOT NULL UNION ALL SELECT L.ref_1 AS ip,L.ref_2 AS port,L.local_instance_id AS local_instance_id,L.name AS name FROM C_User_Addon_Instances AS UAI LEFT JOIN Lighting AS L ON L.local_instance_id = UAI.local_instance_id WHERE UAI.instance_id = '"+instanceId+"' AND L.ref_1 IS NOT NULL AND L.ref_2 IS NOT NULL AND L.local_instance_id IS NOT NULL AND L.name IS NOT NULL UNION ALL SELECT M.ip AS ip,M.port AS port,M.local_instance_id AS local_instance_id,M.name AS name FROM C_User_Addon_Instances AS UAI LEFT JOIN Music AS M ON M.local_instance_id = UAI.local_instance_id WHERE UAI.instance_id = '"+instanceId+"' AND M.ip IS NOT NULL AND M.port IS NOT NULL AND M.local_instance_id IS NOT NULL AND M.name IS NOT NULL UNION ALL SELECT S.lan_ip, S.port, S.local_instance_id, S.camera_name FROM C_User_Addon_Instances AS UAI LEFT JOIN Surveilance AS S ON S.local_instance_id = UAI.local_instance_id WHERE UAI.instance_id = '"+instanceId+"' AND S.lan_ip IS NOT NULL AND S.port IS NOT NULL AND S.local_instance_id IS NOT NULL AND S.camera_name IS NOT NULL;");
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
		#ifdef DEBUG
			cout << "cmd					:" << cmd << ".\n\n";
		#endif	
		//Terminal "Ping" sent via system() command.
		switch(system(cmd.c_str()))
		{
			case 0:
				//Ping success JSON sent
				{	
					if(allTestFlag != "0")
						jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"1",respId,"","",friendlyName,"");
					return 1;
				}
				break;
					
			default:
				//Ping failure JSON sent
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

int main()
{
	try
	{
		/*-----Declaration-----*/
		Cgicc cgi;
				
		/*-----getting the User id-----*/
		string testId{},instanceId{},userId{},propertyId{},propertyValue{},respId{};
		for(int loopNo1=1;loopNo1 <= 6;loopNo1++)
		{
			switch(loopNo1)
			{
				case 1:
					{
						form_iterator i1 = cgi.getElement("test_id");
						if(i1 != cgi.getElements().end())
						{
							testId = i1->getValue();
						}
					}
					break;
					
				case 2:
					{
						form_iterator i1 = cgi.getElement("relationship_id");
						if(i1 != cgi.getElements().end())
						{
							userId = i1->getValue();
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
			cout << "Test id 			: " << testId << ",\n";
			cout << "User id 			: " << userId << ",\n";
			cout << "Instance id 		: " << instanceId << ",\n";
			cout << "Property id 		: " << propertyId << ",\n";
			cout << "Property Value 	: " << propertyValue << ",\n";
			cout << "Resp id 			: " << respId << ".\n\n";
		#endif		
		//testId = XXXXXXXXXXXXXXXX;
		//instanceId = XXXXXXXXXXXXX;
		//Example query:http://localhost:6161/Milan/Drivers_V5/MILAN_EXE/expert_system/ping.cgi?test_id=TS0010&relationship_id=XXXXXXXXXXXXXX&instance_id=XXXXXXXXXXXXXXXX&property_id=XXXXXXXXXXXXXXXXXXXX&property_value=XXXXXXXXXXXXXXXX&resp_id=XXXXXXXXXXXXX
		if(!testId.empty() && !userId.empty() && !instanceId.empty() && !propertyId.empty() && !propertyValue.empty() && !respId.empty())
		{
			expSys e1(userId,instanceId,propertyId,propertyValue,testId,respId);
			if(testId == "TS0005")//Device power test
			{	
				e1.pingIp();
			}
			else if(testId == "TS0008")//Router test
			{
				e1.pingIpConflict();
			}
			
			else if(testId == "TS0010")//Router test
			{
				e1.pingRouter();
			}
			else if(testId == "TS0015")//Router test
			{
				e1.pingDns();
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
			cout << "Ping Test finished successfully.";
		#endif
	}
	catch(exception& e) 
	{
		/*-----handle any errors - omitted for brevity-----*/
		#ifdef DEBUG
			cout << "Exception handled at Ping Test, Program terminated, please check for errors.";
		#endif
	}
}
