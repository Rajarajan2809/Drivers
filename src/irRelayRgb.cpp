/*******************************************************************************
 *	Purpose		:	IR test.												   *
 * 	Description	:	Source file for IR test.								   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	14-Sep-16												   *
 * 	DOLM		:	1-Nov-16												   *
 *******************************************************************************/

/*-----local files-----*/
#include "expSys.hpp"

using namespace std;
using namespace cgicc;

int expSys::irTest()
{
	MYSQL_ROW row;
	string irName{},irIp{},irIpPort{},irPort{},localInstanceId{},irCmd{};
	t2.mysqlQuery("SELECT IPD.name, IPD.ip, IPD.port AS IR_IP_PORT, IIR.port AS IR_PORT, UAI.local_instance_id, AP.attribute_ref_1 FROM C_User_Addon_Instances AS UAI LEFT JOIN IR_Devices AS IRD ON IRD.local_instance_id = UAI.local_instance_id LEFT JOIN IP_IR_Relay AS IIR ON IIR.ir_device_id = IRD.id LEFT JOIN IP_Devices AS IPD ON IPD.id = IIR.device_id LEFT JOIN C_User_Addons AS UA ON UA.id =  UAI.addon_id LEFT JOIN C_Addons_Properties AS AP ON AP.addon_id = UA.addon_id AND AP.property_id = '1' WHERE UAI.instance_id = '"+instanceId+"' AND IPD.name IS NOT NULL AND IPD.ip IS NOT NULL AND IPD.port IS NOT NULL AND IIR.port IS NOT NULL AND UAI.local_instance_id IS NOT NULL AND AP.attribute_ref_1 IS NOT NULL;");
	while((row = mysql_fetch_row(t2.result)))
	{
		irName = row[0];
		irIp = row[1];
		irIpPort = row[2];
		irPort = row[3];
		localInstanceId = row[4];
		irCmd = row[5];
	}
	#ifdef DEBUG
		cout << "irName\t\t\t\t:" << irName << ",\n";
		cout << "irIp\t\t\t\t:" << irIp << ",\n";
		cout << "irIpPort\t\t\t:" << irIpPort << ",\n";
		cout << "irPort\t\t\t\t:" << irPort << ",\n";
		cout << "localInstanceId\t:" << localInstanceId << ",\n";
		cout << "irCmd\t\t\t\t:" << irCmd << ".\n\n";
	#endif
		
	if(!irName.empty() && !irIp.empty() && !irIpPort.empty() && !irPort.empty() && !localInstanceId.empty() && !irCmd.empty())
	{
		if(testId == "TS0045")
		{
			if(tcpCmd(irIp,irIpPort,"EM"+irPort+propertyValue+"\r\n"))
				jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"1",respId,"","",irName,"IR_COMMAND");
			else
				jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",irName,"IR_COMMAND");
		}
		else if(testId == "TS0028")
		{
			allTestFlag = "0";
			if(pingIp())
			{
				if(pingIpConflict())
				{
					if(tcpCmd(irIp,irIpPort,"EM"+irPort+irCmd+"\r\n"))
						jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"1",respId,"","",irName,"IR_COMMAND");
					else
						jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",irName,"IR_COMMAND");
				}
				else
				{
					jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",irName,"IP_CONFLICT");
				}
			}
			else
			{
				jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",irName,"LAN");
			}
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

int expSys::rgbTest()
{
	MYSQL_ROW row;
	string rgbName{},rgbIp{},rgbPort{},localInstanceId{};
	t2.mysqlQuery("SELECT L.name, L.ref_1, L.ref_2, UAI.local_instance_id FROM C_User_Addon_Instances AS UAI LEFT JOIN Lighting AS L ON L.local_instance_id = UAI.local_instance_id WHERE UAI.instance_id = '"+instanceId+"' AND L.category = 'Color Light' AND L.integration_id = '40000' AND L.name IS NOT NULL AND L.ref_1 IS NOT NULL AND L.ref_2 IS NOT NULL;");
	while((row = mysql_fetch_row(t2.result)))
	{
		rgbName = row[0];
		rgbIp = row[1];
		rgbPort = row[2];
		localInstanceId = row[3];
	}
	
	#ifdef DEBUG
		cout << "rgbName 			:" << rgbName << ",\n";
		cout << "rgbIp 				:" << rgbIp << ",\n";
		cout << "rgbPort 			:" << rgbPort << ",\n";
		cout << "localInstanceId 	:" << localInstanceId << ".\n\n";
	#endif
		
	if(!rgbName.empty() && !rgbIp.empty() && !rgbPort.empty() && !localInstanceId.empty())
	{
		if(testId == "TS0046")
		{
			/*bool status[3];
			//rgb color red
			status[0] = tcpCmd(rgbIp,rgbPort,"EM001:100,000,000\r\n");
			if(status[0])
			{}
			else
				jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",rgbName,"RGB_COMMAND");
						
			//rgb color green after 2 second delay
			sleep(2);
			status[1] = tcpCmd(rgbIp,rgbPort,"EM001:000,100,000\r\n");
			if(status[1])
			{}
			else
				jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",rgbName,"RGB_COMMAND");
			
			//rgb color blue after 2 second delay
			sleep(2);
			status[2] = tcpCmd(rgbIp,rgbPort,"EM001:000,000,100\r\n");
			if(status[0] && status[1] && status[2])
			{
				jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"1",respId,"","",rgbName,"RGB_COMMAND");
			}
			else
				jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",rgbName,"RGB_COMMAND");*/
			if(tcpCmd(rgbIp,rgbPort,"EM001:100,100,100\r\n"))
			{	
				jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"1",respId,"","",rgbName,"RGB_COMMAND");
				sleep(2);
				tcpCmd(rgbIp,rgbPort,"EM001:000,000,000\r\n");
			}
			else
				jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",rgbName,"RGB_COMMAND");
				
		}
		else if(testId == "TS0029")
		{
			allTestFlag = "0";
			if(pingIp())
			{
				if(pingIpConflict())
				{
					if(tcpCmd(rgbIp,rgbPort,"EM001:100,100,100\r\n"))
					{	
						jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"1",respId,"","",rgbName,"RGB_COMMAND");
						sleep(2);
						tcpCmd(rgbIp,rgbPort,"EM001:000,000,000\r\n");
					}
					else
						jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",rgbName,"RGB_COMMAND");
				}
				else
				{
					jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",rgbName,"IP_CONFLICT");
				}
			}
			else
			{
				jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",rgbName,"LAN");
			}
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

int expSys::relayTest()
{
	MYSQL_ROW row;
	string relayName{},relayIp{},relayIpPort{},relayPort{},controlType{},relayDelay{},localInstanceId{};
	t2.mysqlQuery("SELECT IPD.name AS NAME, IPD.ip AS IP, IPD.port AS RELAY_IP_PORT, IIR.port AS RELAY_PORT, RD.control_type TYPE, RD.duration DUR, UAI.local_instance_id AS local_instance_id FROM C_User_Addon_Instances AS UAI LEFT JOIN Relay_Devices AS RD ON RD.local_instance_id = UAI.local_instance_id LEFT JOIN IP_IR_Relay AS IIR ON IIR.ir_device_id = RD.id LEFT JOIN IP_Devices AS IPD ON IPD.id = IIR.device_id WHERE UAI.instance_id = '"+instanceId+"' AND IPD.name IS NOT NULL AND IPD.ip IS NOT NULL AND IPD.port IS NOT NULL AND IIR.port IS NOT NULL AND RD.control_type IS NOT NULL AND RD.duration IS NOT NULL AND UAI.local_instance_id IS NOT NULL;");
	while((row = mysql_fetch_row(t2.result)))
	{
		relayName = row[0];
		relayIp = row[1];
		relayIpPort = row[2];
		relayPort = row[3];
		controlType = row[4];
		relayDelay = row[5];
		localInstanceId = row[6];
	}
	
	#ifdef DEBUG
		cout << "relayName				:" << relayName << ",\n";
		cout << "relayIp				:" << relayIp << ",\n";
		cout << "relayIpPort			:" << relayIpPort << ",\n";
		cout << "relayPort				:" << relayPort << ",\n";
		cout << "controlType			:" << controlType << ",\n";
		cout << "relayDelay				:" << relayDelay << ",\n";
		cout << "localInstanceId		:" << localInstanceId << ".\n\n";
	#endif
	
	if(!relayName.empty() && !relayIp.empty() && !relayIpPort.empty() && !relayPort.empty() &&	!controlType.empty() && !relayDelay.empty()	&& !localInstanceId.empty())
	{
		if(testId == "TS0047")
		{
			//(controlType == "1")?"2":(propertyId.at(loopNo1) == "2")?"0":propertyId.at(loopNo1)//(controlType == "1")?delay1:"0"
			//control type -> 1 -> toggle
			//control type -> 2 -> on/off
			
			bool status[2];
			
			//relay on
			status[0] = tcpCmd(relayIp,relayIpPort,"ER"+relayPort+"N\r\n");
			if(status[0])
			{}
			else
				jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",relayName,"RELAY_COMMAND");
						
			//relay off after 2 second delay
			sleep(2);
			status[1] = tcpCmd(relayIp,relayIpPort,"ER"+relayPort+"F\r\n");
			if(status[1])
			{}
			else
				jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",relayName,"RELAY_COMMAND");
		}
		else if(testId == "TS0030")
		{
			allTestFlag = "0";
			if(pingIp())
			{
				if(pingIpConflict())
				{
					//relay on
					if(tcpCmd(relayIp,relayIpPort,"ER"+relayPort+"N\r\n"))
					{	
						jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"1",respId,"","",relayName,"RELAY_COMMAND");
						//relay off after 2 second delay
						sleep(2);
						tcpCmd(relayIp,relayIpPort,"ER"+relayPort+"F\r\n");
					}
					else
						jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",relayName,"RELAY_COMMAND");
				}
				else
					jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",relayName,"IP_CONFLICT");
			}
			else
				jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",relayName,"LAN");
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
			cout << "User id 			:" << userId << ",\n";
			cout << "Test id 			:" << testId << ",\n";
			cout << "Instance id 		:" << instanceId << ",\n";
			cout << "Property id 		:" << propertyId << ",\n";
			cout << "Property Value 	:" << propertyValue << ",\n";
			cout << "Resp id 			:" << respId << ".\n\n";
		#endif
				
		//testId = XXXXXXXXXXXXXXXX;
		//instanceId = XXXXXXXXXXXXX;
		//Example query:http://localhost:6161/Milan/Drivers/MILAN_EXE/expert_system/ping.cgi?test_id=TS0010&relationship_id=XXXXXXXXXXXXXX&instance_id=XXXXXXXXXXXXXXXX&property_id=XXXXXXXXXXXXXXXXXXXX&property_value=XXXXXXXXXXXXXXXX&resp_id=XXXXXXXXXXXXX
		if(!testId.empty() && !userId.empty() && !instanceId.empty() && !propertyId.empty() && !propertyValue.empty() && !respId.empty())
		{
			expSys e1(userId,instanceId,propertyId,propertyValue,testId,respId);
			if((testId == "TS0045")||(testId == "TS0028"))//IR test
			{	
				e1.irTest();
			}
			else if((testId == "TS0046")||(testId == "TS0029"))//RGB test
			{
				e1.rgbTest();
			}
			else if((testId == "TS0047")||(testId == "TS0030"))//Relay test
			{
				e1.relayTest();
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
			cout << "IR-RELAY-RGB Test finished successfully.";
		#endif
	}
	catch(exception& e) 
	{
		/*-----handle any errors - omitted for brevity-----*/
		#ifdef DEBUG
			cout<<"Exception handled, IR-RELAY-RGB Test terminated, please check for errors.";
		#endif
	}
}
