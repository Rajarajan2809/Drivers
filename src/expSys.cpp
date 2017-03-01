/*******************************************************************************
 *	Purpose		:	lan check.												   *
 * 	Description	:	Main file for scene Execution.							   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	13-Sep-16												   *
 * 	DOLM		:	13-Sep-16												   *
 *******************************************************************************/

#include "expSys.hpp"

using namespace std;

//lanCheck
void expSys::lanCheck()
{
	vector<string> ipDevices,tabRef,instanceId,name;//irRelayDevices -> ip of IP2IR and IP2Relay, rgb -> ip of RGB, surveilDevices -> Surveilance device ip and userId-> users to send notification.
	//getting IPs of devices, their friendly name and instance ids 
	MYSQL_ROW row;
	//unsigned long int rowsReturned;
	t2.mysqlQuery("SELECT q1.ip, q1.port,q1.name, 1 AS tab_ref, q2.instance_id FROM IP_Devices AS q1 LEFT JOIN C_User_Addon_Instances AS q2 ON q1.local_instance_id = q2.local_instance_id UNION SELECT q3.ref_1, q3.ref_2,q3.name, 2 AS tab_ref, q4.instance_id FROM Lighting AS q3 LEFT JOIN C_User_Addon_Instances AS q4 ON q3.local_instance_id = q4.local_instance_id WHERE q3.category = 'Color Light' UNION SELECT q5.lan_ip, q5.port,q5.camera_name, 3 AS tab_ref, q6.instance_id FROM Surveilance AS q5 LEFT JOIN C_User_Addon_Instances AS q6 ON q5.local_instance_id = q6.local_instance_id UNION SELECT q7.ip, q7.port,q7.name, 4 AS tab_ref, q8.instance_id FROM Music AS q7 LEFT JOIN C_User_Addon_Instances AS q8 ON q7.local_instance_id = q8.local_instance_id WHERE q7.integration_id = '10000';");
	while((row = mysql_fetch_row(t2.result)))
	{
		ipDevices.push_back(row[0]);
		name.push_back(row[2]);
		tabRef.push_back(row[3]);
		instanceId.push_back(row[4]);
	}
				
	//this for loop checks the device status
	for(size_t loopNo1=0;loopNo1<ipDevices.size();loopNo1++)
	{
		#ifdef DEBUG
			cout<<"ip		:" << ipDevices.at(loopNo1) << ",\n";
		#endif
		string cmd = "ping -W4 -c1 -A "+ipDevices.at(loopNo1);
		
		#ifdef DEBUG
			cout<<"cmd		:" << cmd << ".\n\n";
		#endif
		if(system(cmd.c_str()))
		{
			#ifdef DEBUG
				cout << "\"" << ipDevices.at(loopNo1) << "\" is Offline.\n\n";
			#endif
			string name1 = name.at(loopNo1);
			for(size_t i=0; name.at(loopNo1).find(" ") != string::npos; i++)
			{
				name.at(loopNo1).replace(name.at(loopNo1).find(" "),1,"%20");	
			}
			
			t2.mysqlQuery("INSERT INTO `ES_Notification` (`category`, `test_id`, `medium`, `instance_id`, `reference`) VALUES ('6', 'TS0005', 'push', '"+instanceId.at(loopNo1)+"', '"+name1+" is offline');");
			
			#ifdef DEBUG
				cout << "Push web query :http://localhost:6161/Milan/Drivers/MILAN_EXE/PUSH/push.cgi?category=6&action=IP_Device&medium=push&relationship_id=&instance_id="+instanceId.at(loopNo1)+"&friendly_name=&REF_1="+name.at(loopNo1)+"%20is%20offline&REF_2=&REF_3=&REF_4=&REF_5=,\n\n";
			#endif
			//curlRequests("http://localhost:6161/Milan/Drivers/MILAN_EXE/PUSH/push.cgi?category=6&action=IP_Device&medium=push&relationship_id=&instance_id="+instanceId.at(loopNo1)+"&friendly_name=&REF_1="+name.at(loopNo1)+"%20is%20offline&REF_2=&REF_3=&REF_4=&REF_5=","POST");
			
			t2.mysqlQuery("DELETE FROM  ES_Notification WHERE 20 < (SELECT * FROM (SELECT COUNT(*) AS nos FROM ES_Notification) AS TMP) LIMIT 1");
		}
		else
		{
			#ifdef DEBUG
				cout << "\"" <<ipDevices.at(loopNo1) << "\" is Online.\n\n";
			#endif
		}
	}
}

int main()
{
	try
	{
		/*-----Declaration-----*/
		Cgicc cgi;
						
		//Example query:http://localhost/Milan/Drivers_V5/MILAN_EXE/SCENE/expSys.cgi
		
		expSys e1;
		e1.lanCheck();
	}
	catch(exception& e) 
	{
		/*-----handle any errors - omitted for brevity-----*/
	}
}
