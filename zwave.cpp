/*******************************************************************************
 *	Purpose		:	Zwave test.												   *
 * 	Description	:	Source file for Zwave test.								   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	2-Jul-16												   *
 * 	DOLM		:	1-Nov-16												   *
 *******************************************************************************/

/*-----local files-----*/
#include "expSys.hpp"

using namespace std;
using namespace cgicc;

/****************************************************************************************
 * Need to call zwave Driver for getting live values (future implementation)			*
 * **************************************************************************************/
int expSys::zwaveBatteryStatus()
{
	string nodeId{},localInstanceId{},batteryPercent{},friendlyName{},elapMin{};
	MYSQL_ROW row1;
	t2.mysqlQuery("SELECT UAI.instance_id, UAI.instance_friendly_name, DEV . *, COALESCE(IF((SELECT ZWS.current_status) IS NULL, (SELECT status FROM ZW_Wireless_Devices WHERE node_id = DEV.node_id), ZWS.current_status),'') AS status, COALESCE(TIMESTAMPDIFF(MINUTE, (SELECT updated_time FROM ZW_Wireless_Status WHERE node_id = DEV.node_id AND status_name = 'Battery Level' ORDER BY id DESC LIMIT 1), NOW()),'') AS ELAP_MIN FROM C_User_Addon_Instances UAI JOIN (SELECT reference_id AS node_id, local_instance_id, 'doorlock' AS device_cat FROM Door_Lock UNION ALL SELECT ref_1 AS node_id, local_instance_id, 'light' AS device_cat FROM Lighting UNION ALL SELECT zone_id AS node_id, local_instance_id, 'security' AS device_cat FROM Security UNION ALL SELECT node_id, local_instance_id, 'thermostat' AS device_cat FROM Thermostat UNION ALL SELECT ref_1 AS node_id, local_instance_id, 'appliance' AS device_cat FROM Wireless_Appliances UNION ALL SELECT node_id, local_instance_id, 'other' AS device_cat FROM Wireless_Supporting_Devices) AS DEV ON UAI.local_instance_id = DEV.local_instance_id LEFT JOIN ZW_Device_Supported_Commands AS ZDSC ON ZDSC.node_id = DEV.node_id AND ZDSC.class_name = 'COMMAND_CLASS_BATTERY' LEFT JOIN ZW_Wireless_Status AS ZWS ON ZWS.node_id = ZDSC.node_id and ZWS.status_name = 'Battery Level' WHERE UAI.instance_id = '"+instanceId+"' AND UAI.instance_id IS NOT NULL AND UAI.instance_friendly_name IS NOT NULL AND DEV.node_id IS NOT NULL;");
	while ((row1 = mysql_fetch_row(t2.result)))
	{
		friendlyName = row1[1];
		nodeId = row1[2];
		localInstanceId	= row1[3];
		batteryPercent = row1[5];
		elapMin = row1[6];
	}
	
	#ifdef DEBUG
		cout << "friendlyName		:" << friendlyName << ",\n";
		cout << "nodeId\t\t\t\t:" << nodeId << ",\n";
		cout << "localInstanceId\t:" << localInstanceId << ",\n";
		cout << "batteryPercent\t\t:" << batteryPercent << ",\n";
		cout << "elapsed Mins\t\t:" << elapMin << ",\n";
		cout << "User id\t\t\t:" << userId << ",\n";
		cout << "Instance id\t\t:" << instanceId << ",\n";
		cout << "Property id\t\t:" << propertyId << ",\n";
		cout << "Property Value\t\t:" << propertyValue << ",\n";
		cout << "Resp id\t\t\t:" << respId << ".\n\n";
	#endif
	
	if(!friendlyName.empty() && !nodeId.empty() && !localInstanceId.empty() && !batteryPercent.empty() && !respId.empty())
	{
		if(!elapMin.empty())
		{
			if(stol(elapMin) <= 20)
			{
				if(allTestFlag != "0")
					jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"1",respId,batteryPercent,elapMin,friendlyName,"");
				return 1;
			}
			else
			{
				if(allTestFlag != "0")
					jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,batteryPercent,elapMin,friendlyName,"");
				return 0;
			}
		}
		else
		{
			if(batteryPercent == "alive")
			{
				if(allTestFlag != "0")
					jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"1",respId,"","",friendlyName,"");
				return 1; 
			}
			if(batteryPercent == "dead")
			{
				if(allTestFlag != "0")
					jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",friendlyName,"");
				return 0;
			}
		}
	}
	else
	{
		#ifdef DEBUG
			cout << "Device Instance id is Invalid.\n\n";
		#endif
	}
	/*SELECT UAI.instance_id, UAI.instance_friendly_name, DEV . *, COALESCE(IF((SELECT ZWS.current_status) IS NULL, (SELECT status FROM ZW_Wireless_Devices WHERE node_id = DEV.node_id), ZWS.current_status),'') AS status, COALESCE(TIMESTAMPDIFF(MINUTE, (SELECT updated_time FROM ZW_Wireless_Status WHERE node_id = DEV.node_id AND status_name = 'Battery Level' ORDER BY id DESC LIMIT 1), NOW()),'') AS ELAP_MIN FROM C_User_Addon_Instances UAI JOIN (SELECT reference_id AS node_id, local_instance_id, 'doorlock' AS device_cat FROM Door_Lock UNION ALL SELECT ref_1 AS node_id, local_instance_id, 'light' AS device_cat FROM Lighting UNION ALL SELECT zone_id AS node_id, local_instance_id, 'security' AS device_cat FROM Security UNION ALL SELECT node_id, local_instance_id, 'thermostat' AS device_cat FROM Thermostat UNION ALL SELECT ref_1 AS node_id, local_instance_id, 'appliance' AS device_cat FROM Wireless_Appliances UNION ALL SELECT node_id, local_instance_id, 'other' AS device_cat FROM Wireless_Supporting_Devices) AS DEV ON UAI.local_instance_id = DEV.local_instance_id LEFT JOIN ZW_Device_Supported_Commands AS ZDSC ON ZDSC.node_id = DEV.node_id AND ZDSC.class_name = 'COMMAND_CLASS_BATTERY' LEFT JOIN ZW_Wireless_Status AS ZWS ON ZWS.node_id = ZDSC.node_id and ZWS.status_name = 'Battery Level' WHERE UAI.instance_id = '9917FAE0-5BFB-452D-AEF2-71EA9DAAEFD4' AND UAI.instance_id IS NOT NULL AND UAI.instance_friendly_name IS NOT NULL AND DEV.node_id IS NOT NULL;*/
		 return 0;
}

/****************************************************************************************
 * Need to call zwave Driver for getting live values (future implementation)			*
 * 
 * **************************************************************************************/
int expSys::zwaveDeviceStatus()
{
	string nodeId{},localInstanceId{},friendlyName{},batteryPercent{},elapTime{};
	MYSQL_ROW row1;
	t2.mysqlQuery("SELECT UAI.instance_id, UAI.instance_friendly_name, DEV . *, COALESCE(IF((SELECT ZWS.current_status) IS NULL, (SELECT status FROM ZW_Wireless_Devices WHERE node_id = DEV.node_id), ZWS.current_status),'') AS status, COALESCE(TIMESTAMPDIFF(MINUTE, (SELECT updated_time FROM ZW_Wireless_Status WHERE node_id = DEV.node_id AND status_name = 'Battery Level' ORDER BY id DESC LIMIT 1), NOW()),'') AS ELAP_MIN FROM C_User_Addon_Instances UAI JOIN (SELECT reference_id AS node_id, local_instance_id, 'doorlock' AS device_cat FROM Door_Lock UNION ALL SELECT ref_1 AS node_id, local_instance_id, 'light' AS device_cat FROM Lighting UNION ALL SELECT zone_id AS node_id, local_instance_id, 'security' AS device_cat FROM Security UNION ALL SELECT node_id, local_instance_id, 'thermostat' AS device_cat FROM Thermostat UNION ALL SELECT ref_1 AS node_id, local_instance_id, 'appliance' AS device_cat FROM Wireless_Appliances UNION ALL SELECT node_id, local_instance_id, 'other' AS device_cat FROM Wireless_Supporting_Devices) AS DEV ON UAI.local_instance_id = DEV.local_instance_id LEFT JOIN ZW_Device_Supported_Commands AS ZDSC ON ZDSC.node_id = DEV.node_id AND ZDSC.class_name = 'COMMAND_CLASS_BATTERY' LEFT JOIN ZW_Wireless_Status AS ZWS ON ZWS.node_id = ZDSC.node_id and ZWS.status_name = 'Battery Level' WHERE UAI.instance_id = '"+instanceId+"' AND UAI.instance_id IS NOT NULL AND UAI.instance_friendly_name IS NOT NULL AND DEV.node_id IS NOT NULL;");
	while ((row1 = mysql_fetch_row(t2.result)))
	{
		friendlyName = row1[1];
		nodeId = row1[2];
		localInstanceId	= row1[3];
		batteryPercent = row1[5];//device status for mains powered and battery percent for zwave devices
		elapTime = row1[6];
	}
	#ifdef DEBUG
		cout << "friendlyName		:" << friendlyName << ",\n";
		cout << "nodeId\t\t\t\t:" << nodeId << ",\n";
		cout << "localInstanceId\t:" << localInstanceId << ",\n";
		cout << "batteryPercent\t\t:" << batteryPercent << ",\n";
		cout << "elapsed Mins\t\t:" << elapTime << ",\n";
		cout << "User id\t\t\t:" << userId << ",\n";
		cout << "Instance id\t\t:" << instanceId << ",\n";
		cout << "Property id\t\t:" << propertyId << ",\n";
		cout << "Property Value\t\t:" << propertyValue << ",\n";
		cout << "Resp id\t\t\t:" << respId << ".\n\n";
	#endif
	
	if(!friendlyName.empty() && !nodeId.empty() && !localInstanceId.empty() && !respId.empty() && !batteryPercent.empty())
	{
		if(!elapTime.empty())
		{
			if(stol(elapTime) <= 20)
			{
				if(allTestFlag != "0")
					jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"1",respId,batteryPercent,elapTime,friendlyName,"");
				return 1;
			}
			else
			{
				if(allTestFlag != "0")
					jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,batteryPercent,elapTime,friendlyName,"");
				return 0;
			}
		}
		else
		{
			if(batteryPercent == "alive")
			{
				if(allTestFlag != "0")
					jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"1",respId,"","",friendlyName,"");
				return 1;
			}
			if(batteryPercent == "dead")
			{
				if(allTestFlag != "0")
					jsonCreate(userId,instanceId,localInstanceId,testId,propertyId,"0",respId,"","",friendlyName,"");
				return 0;
			}
		}
	}
	else
	{
		#ifdef DEBUG
			cout << "Device Instance id is Invalid.\n\n";
		#endif
	}
	/*SELECT * FROM(SELECT UAI.instance_id, UAI.instance_friendly_name, DEV . *, IF((SELECT ZWS.current_status) IS NULL, (SELECT status FROM WHERE node_id = DEV.node_id AND status), ZWS.current_status) AS status, TIMESTAMPDIFF(MINUTE, (SELECT updated_time FROM ZW_Wireless_Status where node_id = DEV.node_id AND status_name = 'Battery Level' ORDER BY id DESC LIMIT 1), NOW()) AS ELAP_MIN FROM C_User_Addon_Instances UAI JOIN (SELECT reference_id AS node_id, local_instance_id, 'doorlock' AS device_cat FROM Door_Lock UNION ALL SELECT ref_1 AS node_id, local_instance_id, 'light' AS device_cat FROM Lighting UNION ALL SELECT zone_id AS node_id, local_instance_id, 'security' AS device_cat FROM Security UNION ALL SELECT node_id, local_instance_id, 'thermostat' AS device_cat FROM Thermostat UNION ALL SELECT ref_1 AS node_id, local_instance_id, 'appliance' AS device_cat FROM Wireless_Appliances UNION ALL SELECT node_id, local_instance_id, 'other' AS device_cat FROM Wireless_Supporting_Devices) AS DEV ON UAI.local_instance_id = DEV.local_instance_id LEFT JOIN ZW_Device_Supported_Commands AS ZDSC ON ZDSC.node_id = DEV.node_id AND ZDSC.class_name = 'COMMAND_CLASS_BATTERY' LEFT JOIN ZW_Wireless_Status AS ZWS ON ZWS.node_id = ZDSC.node_id and ZWS.status_name = 'Battery Level' WHERE UAI.instance_id = '5C5D4F70-F942-4D5E-A3BF-4B36202133A5' AND UAI.instance_id IS NOT NULL AND UAI.instance_friendly_name IS NOT NULL AND DEV.node_id IS NOT NULL) AS ZWBTST WHERE status IS NOT NULL AND ELAP_MIN IS NOT NULL;*/
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
			cout << "Test id\t\t\t\t:" << testId << ",\n";
			cout << "User id\t\t\t\t:" << userId << ",\n";
			cout << "Instance id\t\t\t:" << instanceId << ",\n";
			cout << "Property id\t\t\t:" << propertyId << ",\n";
			cout << "Property Value\t\t\t:" << propertyValue << ",\n";
			cout << "Resp id\t\t\t\t:" << respId << ".\n\n";
		#endif
		
		//testId = XXXXXXXXXXXXXXXX;
		//instanceId = XXXXXXXXXXXXX;
		//Example query:http://localhost:6161/Milan/Drivers_V5/MILAN_EXE/expert_system/zwave.cgi?test_id=TS0049&relationship_id=XXXXXXXXXXXXXX&instance_id=XXXXXXXXXXXXXXXX&property_id=XXXXXXXXXXXXXXXXXXXX&property_value=XXXXXXXXXXXXXXXX
		if(!testId.empty() && !userId.empty() && !instanceId.empty() && !propertyId.empty() && !propertyValue.empty() && !respId.empty())
		{
			expSys e1(userId,instanceId,propertyId,propertyValue,testId,respId);
			if(testId == "TS0049")//Device status get
			{	
				e1.zwaveDeviceStatus();
			}
			else if(testId == "TS0050")//Battery status get
			{
				e1.zwaveBatteryStatus();
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
		#ifdef DEBUG
			cout << "Zwave Test finished successfully.";
		#endif
	}
	catch(exception& e) 
	{
		/*-----handle any errors - omitted for brevity-----*/
		#ifdef DEBUG
			cout << "Exception handled, Zwave Test terminated, please check for errors.";
		#endif
	}
}
