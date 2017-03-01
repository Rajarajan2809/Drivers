/*******************************************************************************
 *	Purpose		:	Trigger Execution in Master Devices.					   *
 * 	Description	:	This is the Source for Trigger Execution.				   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	2-Jun-16												   *
 * 	DOLM		:	30-Jan-17												   *
 *******************************************************************************/

#include "trigger.hpp"

using namespace std;
using namespace cgicc;

bool trigger::timeCheck(string startTimeInDb,string endTimeInDb)
{
	/*-----time_t is a type to store time values-----*///02:00
	time_t Currenttime{},startTimeCheck{},endTimeCheck{};

	//string startTimeInDb = "07:00",endTimeInDb = "20:30";

	/*-----struct tm is a structure to store time values and which can be modified-----*/
	struct tm * startTime={0};
	struct tm * endTime={0};
	struct tm * currentTime={0};

	/*-----variables required for time operation-----*/
	string startHr{},endHr{},startMin{},endMin{},sunRiseHr{},sunRiseMin{},sunSetHr{},sunSetMin{},sign1{},sign2{};
	long int startCur{},endCur{},startEnd{};
	bool returnValue{};

	/*-----startTimeInDb & endTimeInDb will be like "01:02:03" but we need only "01:02",so use "find" one time only-----*/
	size_t pos1= startTimeInDb.find(":");
	size_t pos2= endTimeInDb.find(":");

	/*-----hours and mins extracted from function params,startTimeInDb and endTimeInDb-----*/
	startHr = startTimeInDb.substr(pos1 - 2,2);
	startMin = startTimeInDb.substr(pos1+1,2);

	#ifdef DEBUG
		cout << "From time hr:" << startHr << ",min:" << startMin << ".\n";
	#endif

	endHr = endTimeInDb.substr(pos2 - 2,2);
	endMin = endTimeInDb.substr(pos2+1,2);

	#ifdef DEBUG
		cout << "To time hr:" << endHr << ",min:" << endMin << ".\n\n";
	#endif

	/*-----getting the current time-----*/
	time ( &Currenttime );
	time ( &startTimeCheck );

	/*-----getting the time structure format-----*/
	startTime = localtime ( &startTimeCheck );
	currentTime = localtime ( &Currenttime );

	int curHr = currentTime->tm_hour;
	int curMin = currentTime->tm_min;

	/*-----getting the start time string in time format-----*/
	startTime->tm_hour = stoi(startHr);
	startTime->tm_min = stoi(startMin);
	startTime->tm_sec = 0;

	startTimeCheck = mktime(startTime);

	/*-----start time is constucted in time_t format-----*/
	time ( &endTimeCheck );
	endTime = localtime ( &endTimeCheck );
	endTime->tm_hour = stoi(endHr);
	endTime->tm_min = stoi(endMin);
	endTime->tm_sec = 59;

	/*-----converting to time format-----*/
	endTimeCheck = mktime(endTime);
	startCur = difftime(startTimeCheck,Currenttime);
	startEnd = difftime(startTimeCheck,endTimeCheck);
	endCur = difftime(endTimeCheck,Currenttime);

	#ifdef DEBUG
		cout << "before,Start Time:" << ctime(&startTimeCheck);
		cout << "Current Time:" << ctime(&Currenttime);
		cout << "End Time:" << ctime(&endTimeCheck) << endl;
		cout << "startCur:" << startCur << ",\n";
		cout << "endCur:" << endCur << ",\n";
		cout << "startEnd:" << startEnd << ".\n\n";
	#endif

	if(startEnd < 0)
	{
		//same day
	}

	if(startEnd == 0)
	{
		//same time
	}

	if(startEnd > 0)
	{
		//next day
		if(startCur > startEnd)
		{
			startTime->tm_hour = stoi(startHr);
			startTime->tm_min = stoi(startMin);
			startTime->tm_sec = 0;
			startTime->tm_mday = startTime->tm_mday -1;
			startTimeCheck = mktime(startTime);
		}
		else
		{
			size_t startMins = stoi(startHr)* 60 + stoi(startMin);
			size_t endMins = stoi(endHr)* 60 + stoi(endMin);
			size_t curMins = curHr * 60 + curMin;

			#ifdef DEBUG
				cout << "startMins:" << startMins << endl;
				cout << "curMins:" << curMins << endl;
				cout << "endMins:" << endMins << ".\n\n";
			#endif


			if(startMins > endMins)
			{
				if(curMins > startMins)
				{
					endTime->tm_hour = stoi(endHr);
					endTime->tm_min = stoi(endMin);
					endTime->tm_sec = 0;
					endTime->tm_mday = endTime->tm_mday +1;
					endTimeCheck = mktime(endTime);
				}
				else
				{
					startTime->tm_hour = stoi(startHr);
					startTime->tm_min = stoi(startMin);
					startTime->tm_sec = 0;
					startTime->tm_mday = startTime->tm_mday -1;
					startTimeCheck = mktime(startTime);
					//cout << "Start Time:" << ctime(&startTimeCheck);
				}
			}
		}
	}

	#ifdef DEBUG
		cout << "After,Start Time:" << ctime(&startTimeCheck);
		cout << "Current Time:" << ctime(&Currenttime);
		cout << "End Time:" << ctime(&endTimeCheck) << endl;
	#endif


	startCur = difftime(startTimeCheck,Currenttime);
	endCur = difftime(endTimeCheck,Currenttime);

	if((startCur <= 0)&&(endCur >= 0))
	{
		returnValue =1;
	}
	else
	{
		returnValue =0;
	}

	#ifdef DEBUG
		cout << "startCur:" << startCur << ",\n";
		cout << "endCur:" << endCur << ",\n";
		cout << "startEnd:" << startEnd << ",\n";
		cout << "Time Result:" << returnValue << ".\n\n";
	#endif

	//sudo date --set "31 Dec 2016 01:00:00"

	/*-----Time status is updated in returnValue-----*/
	return returnValue;
}

/*-----Member function to check day of week-----*/
bool trigger::dayCheck(string daysOfWeek)
{
	string  dayOfWeek[7];
	bool returnValue{};

	if(daysOfWeek.find(",") != string::npos)
	{
		dayOfWeek[0] = daysOfWeek.substr(daysOfWeek.find(",")-1,daysOfWeek.find(","));
		daysOfWeek.erase(0,daysOfWeek.find(",")+1);

		if(daysOfWeek.find(",") != string::npos)
		{
			dayOfWeek[1] = daysOfWeek.substr(daysOfWeek.find(",")-1,daysOfWeek.find(","));
			daysOfWeek.erase(0,daysOfWeek.find(",")+1);

			if(daysOfWeek.find(",") != string::npos)
			{
				dayOfWeek[2] = daysOfWeek.substr(daysOfWeek.find(",")-1,daysOfWeek.find(","));
				daysOfWeek.erase(0,daysOfWeek.find(",")+1);

				if(daysOfWeek.find(",") != string::npos)
				{

					dayOfWeek[3] = daysOfWeek.substr(daysOfWeek.find(",")-1,daysOfWeek.find(","));
					daysOfWeek.erase(0,daysOfWeek.find(",")+1);

					if(daysOfWeek.find(",") != string::npos)
					{
						dayOfWeek[4] = daysOfWeek.substr(daysOfWeek.find(",")-1,daysOfWeek.find(","));
						daysOfWeek.erase(0,daysOfWeek.find(",")+1);

						if(daysOfWeek.find(",") != string::npos)
						{
							dayOfWeek[5] = daysOfWeek.substr(daysOfWeek.find(",")-1,daysOfWeek.find(","));
							daysOfWeek.erase(0,daysOfWeek.find(",")+1);
							dayOfWeek[6] = daysOfWeek;
						}
					}
				}
			}
		}
	}

	/*for(int i=0;i<7;i++)
	{
		cout<<"dayOfWeek["<<i<<"] ->"<<dayOfWeek[i]<<"\n";
	}*/

	//const string DAY[]={"Sunday","Monday","Tueday","Wednesday","Thursday","Friday","Saturday"};
	time_t rawtime;
	tm * timeinfo;
	time(&rawtime);
	timeinfo=localtime(&rawtime);
    int wday=timeinfo->tm_wday;

    if(!dayOfWeek[0].empty() && !dayOfWeek[1].empty() && !dayOfWeek[2].empty() && !dayOfWeek[3].empty() && !dayOfWeek[4].empty() && !dayOfWeek[5].empty() && !dayOfWeek[6].empty())
    {
		switch(wday)
		{
			case 0:
				if((dayOfWeek[6] == "0")||(dayOfWeek[6] == "7"))
				{
					returnValue =1;
					#ifdef DEBUG
						cout << "Today is Sunday and Trigger configured for today.\n";
					#endif
				}
				else if(dayOfWeek[6] == "E")
				{
					returnValue =0;
					#ifdef DEBUG
						cout << "Today is Sunday and Trigger was not configured for today.\n";
					#endif
				}
				else
				{
					returnValue =0;
					#ifdef DEBUG
						cout << "Invalid day value at Sunday position.\n";
					#endif
				}
				break;

			case 1:
				if(dayOfWeek[0] == "1")
				{
					returnValue =1;
					#ifdef DEBUG
						cout << "Today is Monday and Trigger configured for today.\n";
					#endif
				}
				else if(dayOfWeek[0] == "E")
				{
					returnValue =0;
					#ifdef DEBUG
						cout << "Today is Monday and Trigger was not configured for today.\n";
					#endif
				}
				else
				{
					returnValue =0;
					#ifdef DEBUG
						cout << "Invalid day value at Monday position.\n";
					#endif
				}
				break;

			case 2:
				if(dayOfWeek[1] == "2")
				{
					returnValue =1;
					#ifdef DEBUG
						cout << "Today is Tuesday and Trigger configured for today.\n";
					#endif
				}
				else if(dayOfWeek[1] == "E")
				{
					returnValue =0;
					#ifdef DEBUG
						cout << "Today is Tuesday and Trigger was not configured for today.\n";
					#endif
				}
				else
				{
					returnValue =0;
					#ifdef DEBUG
						cout << "Invalid day value at Tuesday position.\n";
					#endif
				}
				break;

			case 3:
				if(dayOfWeek[2] == "3")
				{
					returnValue =1;
					#ifdef DEBUG
						cout << "Today is Wednesday and Trigger configured for today.\n";
					#endif
				}
				else if(dayOfWeek[2] == "E")
				{
					returnValue =0;
					#ifdef DEBUG
						cout << "Today is Wednesday and Trigger was not configured for today.\n";
					#endif
				}
				else
				{
					returnValue =0;
					#ifdef DEBUG
						cout << "Invalid day value at Wednesday position.\n";
					#endif
				}
				break;

			case 4:
				if(dayOfWeek[3] == "4")
				{
					returnValue =1;
					#ifdef DEBUG
						cout << "Today is Thursday and Trigger configured for today.\n";
					#endif
				}
				else if(dayOfWeek[3] == "E")
				{
					returnValue =0;
					#ifdef DEBUG
						cout << "Today is Thursday and Trigger was not configured for today.\n";
					#endif
				}
				else
				{
					returnValue =0;
					#ifdef DEBUG
						cout << "Invalid day value at Thursday position.\n";
					#endif
				}
				break;

			case 5:
				if(dayOfWeek[4] == "5")
				{
					returnValue =1;
					#ifdef DEBUG
						cout << "Today is Friday and Trigger configured for today.\n";
					#endif
				}
				else if(dayOfWeek[4] == "E")
				{
					returnValue =0;
					#ifdef DEBUG
						cout << "Today is Friday and Trigger was not configured for today.\n";
					#endif
				}
				else
				{
					returnValue =0;
					#ifdef DEBUG
						cout << "Invalid day value at Friday position.\n";
					#endif
				}
				break;

			case 6:
				if(dayOfWeek[5] == "6")
				{
					returnValue =1;
					#ifdef DEBUG
						cout << "Today is Saturday and Trigger configured for today.\n";
					#endif
				}
				else if(dayOfWeek[5] == "E")
				{
					returnValue =0;
					#ifdef DEBUG
						cout << "Today is Saturday and Trigger was not configured for today.\n";
					#endif
				}
				else
				{
					returnValue =0;
					#ifdef DEBUG
						cout << "Invalid day value at Saturday position.\n";
					#endif
				}
				break;

			default:
				{
					returnValue =0;
					#ifdef DEBUG
						cout << "Day Value doesnot corresponds to any day of week.\n";
					#endif
				}
		}
	}
	#ifdef DEBUG
		cout << "Day check status:" << returnValue << ".\n\n";
	#endif

	return returnValue;
}

/*-----Trigger part-----*/
int trigger::triggerExecution(string sensorLocalInstanceId,string sensorPropertyId,string sensorPropertyValue,string sceneId, string dlUserId)
{
	vector<string> instanceIdTrigger{},propertyIdTrigger{},propertyValueTrigger{},flag{},daysOfWeek{},fromTime{},toTime{},targetMode{},targetValue{},userId{},message{};

	if(!sensorLocalInstanceId.empty() && !sensorPropertyId.empty() && sceneId.empty())
	{
		MYSQL_ROW row1;
		/*-----Query to check Trigger for device of "localInstanceId", "propertyId" and "propertyValue"-----*/
		if(dlUserId.empty())
			t2.mysqlQuery("SELECT UAI.instance_id,T.Property_Id,T.Property_Value,T.Flag,T.Days_Of_Week,T.From_Time,T.To_Time,T.Target_Mode,T.Target_Value,T.user_id,IF((T.message IS NULL) && (T.Target_Mode = 'scene'),'NA',T.message) FROM C_User_Addon_Instances AS UAI JOIN Triggers AS T ON T.Instance_Id = UAI.instance_id AND T.Property_Id = '"+sensorPropertyId+"' AND T.Property_Value = '"+sensorPropertyValue+"' WHERE UAI.instance_ref='' AND UAI.local_instance_id = '"+sensorLocalInstanceId+"'AND T.Is_Active = 1 AND T.Instance_Id IS NOT NULL AND T.Property_Id IS NOT NULL AND T.Property_Value IS NOT NULL AND T.Flag IS NOT NULL AND T.Days_Of_Week IS NOT NULL AND T.From_Time IS NOT NULL AND T.To_Time IS NOT NULL AND T.Target_Mode IS NOT NULL AND T.Target_Value IS NOT NULL AND T.user_id IS NOT NULL AND IF((T.message IS NULL) && (T.Target_Mode = 'scene'), 'NA', T.message) IS NOT NULL UNION ALL SELECT UAIREF.instance_id, T.Property_Id, T.Property_Value, T.Flag, T.Days_Of_Week, T.From_Time, T.To_Time, T.Target_Mode, T.Target_Value, T.user_id, IF((T.message IS NULL) && (T.Target_Mode = 'scene'), 'NA', T.message) FROM  C_User_Addon_Instances AS UAI JOIN C_User_Addon_Instances UAIREF ON UAI.instance_id=UAIREF.instance_ref JOIN Triggers AS T ON T.Instance_Id = UAIREF.instance_id AND T.Property_Id = '"+sensorPropertyId+"' AND T.Property_Value = '"+sensorPropertyValue+"' WHERE UAI.instance_ref != '' AND UAI.local_instance_id = '"+sensorLocalInstanceId+"' AND T.Is_Active = 1 AND T.Instance_Id IS NOT NULL AND T.Property_Id IS NOT NULL AND T.Property_Value IS NOT NULL AND T.Flag IS NOT NULL AND T.Days_Of_Week IS NOT NULL AND T.From_Time IS NOT NULL AND T.To_Time IS NOT NULL AND T.Target_Mode IS NOT NULL AND T.Target_Value IS NOT NULL AND T.user_id IS NOT NULL AND IF((T.message IS NULL) && (T.Target_Mode = 'scene'), 'NA', T.message) IS NOT NULL;");
		else
			t2.mysqlQuery("SELECT UAI.instance_id,T.Property_Id,T.Property_Value,T.Flag,T.Days_Of_Week,T.From_Time,T.To_Time,T.Target_Mode,T.Target_Value,T.user_id,IF((T.message IS NULL) && (T.Target_Mode = 'scene'),'NA',T.message) FROM C_User_Addon_Instances AS UAI JOIN Triggers AS T ON T.Instance_Id = UAI.instance_id AND T.Property_Id = '"+sensorPropertyId+"' AND T.Property_Value = '"+sensorPropertyValue+"'  AND T.Target_Value = '"+dlUserId+"' WHERE UAI.instance_ref = '' AND UAI.local_instance_id = '"+sensorLocalInstanceId+"' AND T.Is_Active = 1 AND T.Instance_Id IS NOT NULL AND T.Property_Id IS NOT NULL AND T.Property_Value IS NOT NULL AND T.Flag IS NOT NULL AND T.Days_Of_Week IS NOT NULL AND T.From_Time IS NOT NULL AND T.To_Time IS NOT NULL AND T.Target_Mode IS NOT NULL AND T.Target_Value IS NOT NULL AND T.user_id IS NOT NULL AND IF((T.message IS NULL) && (T.Target_Mode = 'scene'),'NA',T.message) IS NOT NULL UNION ALL SELECT UAIREF.instance_id,T.Property_Id,T.Property_Value,T.Flag,T.Days_Of_Week,T.From_Time,T.To_Time,T.Target_Mode,T.Target_Value,T.user_id,IF((T.message IS NULL) && (T.Target_Mode = 'scene'),'NA',T.message) FROM C_User_Addon_Instances AS UAI JOIN C_User_Addon_Instances UAIREF ON UAI.instance_id = UAIREF.instance_ref JOIN Triggers AS T ON T.Instance_Id = UAIREF.instance_id AND T.Property_Id = '"+sensorPropertyId+"' AND T.Property_Value = '"+sensorPropertyValue+"' AND T.T.Target_Value = '"+dlUserId+"' WHERE UAI.instance_ref != '' AND UAI.local_instance_id = '"+sensorLocalInstanceId+"' AND T.Is_Active = 1 AND T.Instance_Id IS NOT NULL AND T.Property_Id IS NOT NULL AND T.Property_Value IS NOT NULL AND T.Flag IS NOT NULL AND T.Days_Of_Week IS NOT NULL AND T.From_Time IS NOT NULL AND T.To_Time IS NOT NULL AND T.Target_Mode IS NOT NULL AND T.Target_Value IS NOT NULL AND T.user_id IS NOT NULL AND IF((T.message IS NULL) && (T.Target_Mode = 'scene'),'NA',T.message) IS NOT NULL;");
		while ((row1 = mysql_fetch_row(t2.result)))
		{
			//Result of Query is stored here, data collected from Scene Description table
			instanceIdTrigger.push_back(row1[0]);
			propertyIdTrigger.push_back(row1[1]);
			propertyValueTrigger.push_back(row1[2]);
			flag.push_back(row1[3]);
			daysOfWeek.push_back(row1[4]);
			fromTime.push_back(row1[5]);
			toTime.push_back(row1[6]);
			targetMode.push_back(row1[7]);
			targetValue.push_back(row1[8]);
			userId.push_back(row1[9]);
			message.push_back(row1[10]);
		}

		thread *thread1 = new thread[instanceIdTrigger.size()];

		for(size_t loopNo1=0;loopNo1<instanceIdTrigger.size();loopNo1++)
		{
			#ifdef DEBUG
				cout << "Instance Id:" << instanceIdTrigger.at(loopNo1) << ",\n";
				cout <<	"Property Id:" << propertyIdTrigger.at(loopNo1) << ",\n";
				cout << "Property Value:" << propertyValueTrigger.at(loopNo1) << ",\n";
				cout << "Flag:" << flag.at(loopNo1) << ",\n";
				cout << "Weekdays:" << daysOfWeek.at(loopNo1) << ",\n";
				cout << "From Time:" << fromTime.at(loopNo1) << ",\n";
				cout << "To Time:" << toTime.at(loopNo1) << ",\n";
				cout << "Target mode:" << targetMode.at(loopNo1) << ",\n";
				cout << "Target value:" << targetValue.at(loopNo1) << ",\n";
				cout << "User id:" << userId.at(loopNo1) << ",\n";
				cout << "Message:" << message.at(loopNo1) << ".\n\n";
			#endif

			if(!instanceIdTrigger.at(loopNo1).empty() && !propertyIdTrigger.at(loopNo1).empty() && !propertyValueTrigger.at(loopNo1).empty() && !flag.at(loopNo1).empty() && !targetMode.at(loopNo1).empty() && !targetValue.at(loopNo1).empty() && !userId.at(loopNo1).empty() && !message.at(loopNo1).empty())
			{
				if(targetMode.at(loopNo1) == "scene")
				{
					if((flag[loopNo1] == "0") || ((flag[loopNo1] == "1") && (!daysOfWeek.at(loopNo1).empty() && !fromTime.at(loopNo1).empty() && !toTime.at(loopNo1).empty()) && (dayCheck(daysOfWeek.at(loopNo1))) && (timeCheck(fromTime.at(loopNo1),toTime.at(loopNo1)))))
					{
						thread1[loopNo1] = thread(&trigger::p2pData,trigger(),targetValue.at(loopNo1));
					}
				}
				else if((targetMode[loopNo1] == "email") || (targetMode[loopNo1] == "sms") || (targetMode[loopNo1] == "push"))
				{
					if((flag[loopNo1] == "0") || ((flag[loopNo1] == "1") && (!daysOfWeek.at(loopNo1).empty() && !fromTime.at(loopNo1).empty() && !toTime.at(loopNo1).empty()) && (dayCheck(daysOfWeek.at(loopNo1))) && (timeCheck(fromTime.at(loopNo1),toTime.at(loopNo1)))))
					{
						string mailData = "localhost:6161/Milan/Drivers/MILAN_EXE/PUSH/push.cgi?category=7&action=TRIGGER_NOTIFY&medium="+targetMode[loopNo1]+"&relationship_id="+userId[loopNo1]+"&instance_id="+instanceIdTrigger[loopNo1]+"&friendly_name=&REF_1="+urlEncode(message[loopNo1])+"&REF_2="+targetValue[loopNo1]+"&REF_3=&REF_4=&REF_5=";

						#ifdef DEBUG
							cout << "query:" << mailData << ".\n\n";
						#endif

						thread1[loopNo1] = thread(&jsonSend::curlRequests,jsonSend(),mailData,"POST","push");
					}
				}
				else
				{
					#ifdef DEBUG
						cout << "Target Mode is Invalid.\n\n";
					#endif
				}
			}
		}

		for(size_t loopNo1=0;loopNo1<instanceIdTrigger.size();loopNo1++)
		{
			if(!instanceIdTrigger.at(loopNo1).empty() && !propertyIdTrigger.at(loopNo1).empty() && !propertyValueTrigger.at(loopNo1).empty() && !flag.at(loopNo1).empty() && !targetMode.at(loopNo1).empty() && !targetValue.at(loopNo1).empty() && !userId.at(loopNo1).empty() && !message.at(loopNo1).empty())
			{
				if(targetMode.at(loopNo1) == "scene")
				{
					if((flag[loopNo1] == "0") || ((flag[loopNo1] == "1") && (!daysOfWeek.at(loopNo1).empty() && !fromTime.at(loopNo1).empty() && !toTime.at(loopNo1).empty()) && (dayCheck(daysOfWeek.at(loopNo1))) && (timeCheck(fromTime.at(loopNo1),toTime.at(loopNo1)))))
					{
						thread1[loopNo1].join();
					}
				}
				else if((targetMode[loopNo1] == "email") || (targetMode[loopNo1] == "sms") || (targetMode[loopNo1] == "push"))
				{
					if((flag[loopNo1] == "0") || ((flag[loopNo1] == "1") && (!daysOfWeek.at(loopNo1).empty() && !fromTime.at(loopNo1).empty() && !toTime.at(loopNo1).empty()) && (dayCheck(daysOfWeek.at(loopNo1))) && (timeCheck(fromTime.at(loopNo1),toTime.at(loopNo1)))))
					{
						thread1[loopNo1].join();
					}
				}
			}
		}

	}
	else if(sensorLocalInstanceId.empty() && sensorPropertyId.empty() && sensorPropertyValue.empty() && !sceneId.empty())
	{
		#ifdef DEBUG
			cout << "Scene execution to begin.\n";
		#endif
		p2pData(sceneId);
	}
	return 0;
}

int main()
{
	try
	{
		Cgicc cgi;

		string localInstanceId{},propertyId{},propertyValue{},sceneId{},userId{};//locInsIdName{},propIdName{},propValueName{};


		#ifdef DEBUG
			cout << "\t\t\t Trigger Execution Starts\t\t\t\n\n";
		#endif

		//sample query -> http://35.65.2.218:6161/Milan/Drivers/MILAN_EXE/TRIGGER/trigger.cgi?local_instance_id=XXXXX&property_id=XXXXX&property_value=XXXXX
		//sample query -> http://35.65.2.218:6161/Milan/Drivers/MILAN_EXE/TRIGGER/trigger.cgi?scene_id=XXXXX

		/*-----getting the Instance id-----*/
		form_iterator i1 = cgi.getElement("local_instance_id");
		if(i1 != cgi.getElements().end())
		{
			localInstanceId = i1->getValue();
		}

		/*-----getting the Property Id-----*/
		i1 = cgi.getElement("property_id");
		if(i1 != cgi.getElements().end())
		{
			propertyId = i1->getValue();
		}

		/*-----getting the Property Value-----*/
		i1 = cgi.getElement("property_value");
		if(i1 != cgi.getElements().end())
		{
			propertyValue = i1->getValue();
		}

		/*-----getting the Property Value-----*/
		i1 = cgi.getElement("scene_id");
		if(i1 != cgi.getElements().end())
		{
			sceneId = i1->getValue();
		}

		/*-----getting the Property Value-----*/
		i1 = cgi.getElement("user_id");
		if(i1 != cgi.getElements().end())
		{
			userId = i1->getValue();
		}
		//sceneId="57bae62f84f01";
		//localInstanceId = "1";
		//propertyId = "2";
		//propertyValue = "3";

		if((!localInstanceId.empty() && !propertyValue.empty() && !propertyId.empty() && sceneId.empty() && userId.empty()) || (localInstanceId.empty() && propertyValue.empty() && propertyId.empty() && userId.empty() && !sceneId.empty()) || (!localInstanceId.empty() && !propertyValue.empty() && !propertyId.empty() && sceneId.empty() && !userId.empty()))
		{
			#ifdef DEBUG
				cout << "Local instance id\t\t\t:" << localInstanceId << ",\n";
				cout << "Property id\t\t\t:" << propertyId << ",\n";
				cout << "Property value\t\t\t:" << propertyValue << ",\n";
				cout << "User id\t\t\t:" << userId << ".\n\n";
			#endif

			trigger t1;
			t1.triggerExecution(localInstanceId,propertyId,propertyValue,sceneId,userId);
		}
		else
		{
			if(sceneId.empty())
			{
				#ifdef DEBUG
					cout << "Scene id is empty along with that local instance id, property id and property value are also empty.\n\n";
				#endif
			}
		
			if(localInstanceId.empty())
			{
				#ifdef DEBUG
					cout << "local instance id is empty.\n\n";
				#endif
			}
			
			if(propertyId.empty())
			{
				#ifdef DEBUG
					cout << "Property id is empty.\n\n";
				#endif
			}
			
			if(propertyValue.empty())
			{
				#ifdef DEBUG
					cout << "Property value is empty.\n\n";
				#endif
			}

			if(userId.empty())
			{
				#ifdef DEBUG
					cout << "user id is empty.\n\n";
				#endif
			}
		}

		/*-----Command to Close the HTML document------*/
		#ifdef DEBUG
			cout<<"Trigger Execution finished successfully.";
		#endif
	}
	catch(exception& e)
	{
		#ifdef DEBUG
			cout<<"Exception handled, Trigger Execution terminated, please check for errors.";
		#endif
	}
	return EXIT_SUCCESS;
}
