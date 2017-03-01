/*******************************************************************************
 *	Purpose		:	Scene Execution.										   *
 * 	Description	:	Main file for scene Execution.							   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	30-Jul-16												   *
 * 	DOLM		:	26-Feb-17												   *
 *******************************************************************************/

/*-----local files-----*/
#include "scene.hpp"

using namespace std;
using namespace cgicc;

/*******************************************************************************
 * Member Function of Class Scene which controls the flow of scene execution   *
 * and sends JSON to json server.											   *
 *******************************************************************************/
int scene::sceneExecution(string userId,string sceneId)
{
	/*-----vector to store the scene params-----*/
	vector<string> category{},integrationId{},localInstanceId{},level{},ref2{},ref3{},ref4{},ref5{},ref6{},ref9{},ref7{},flagForTiming{},startTimeInDb{},endTimeInDb{},sunRiseTimeDelay{},sunSetTimeDelay{};
	
	/*-----scene Execution begins------*/
	string user{},roomId{},sunRiseTime{},sunSetTime{},sceneJson{};
	size_t loopNo1{};
				
	/*-----Mysql parameters-----*/
	MYSQL_ROW row{};
	db.mysqlQuery("SELECT S.user,S.room_id FROM C_Mymilan_Users AS MU LEFT JOIN Scene AS S ON S.user = MU.id WHERE MU.mymilan_user_id = '"+userId+"' AND S.id = '"+sceneId+"' AND S.user IS NOT NULL AND S.room_id IS NOT NULL;");
	if(mysql_num_rows(db.result) > 0)
	{
		if((row = mysql_fetch_row(db.result)))
		{
			user = row[0];
			roomId = row[1];
		}
	}
	
	#ifdef DEBUG
		cout << "user id:" << user << br();
		cout << "room id:" << roomId << br();
	#endif
	
	if(!user.empty())
	{
		/*-----User exists for this particular Scene-----*/
		db.mysqlQuery("SELECT SD.category,SD.ref_1,SD.ref_2,SD.ref_3,SD.ref_4,SD.ref_5,SD.ref_6,SD.ref_9,SD.ref_10,SD.local_instance_id,SD.ref_7,SE.flag,SE.start_time,SE.end_time,SE.rise_delay,SE.set_delay FROM Scene_Description AS SD LEFT JOIN Scene_Execution AS SE ON SD.scene_id = SE.scene_id AND SD.category = SE.category WHERE SD.scene_id = '"+sceneId+"' AND SD.category IS NOT NULL AND SD.ref_1 IS NOT NULL AND SD.ref_2 IS NOT NULL AND SD.ref_3 IS NOT NULL AND SD.ref_4 IS NOT NULL AND SD.ref_5 IS NOT NULL AND SD.ref_6 IS NOT NULL AND SD.ref_9 IS NOT NULL AND SD.ref_10 IS NOT NULL AND SD.local_instance_id IS NOT NULL AND SD.ref_7 IS NOT NULL AND SE.flag IS NOT NULL AND SE.start_time IS NOT NULL AND SE.end_time IS NOT NULL AND SE.rise_delay IS NOT NULL AND SE.set_delay IS NOT NULL ORDER BY SD.execution_order;");
		if(mysql_num_rows(db.result) > 0)
		{
			while ((row = mysql_fetch_row(db.result)))
			{
				/*-----Result of Query is stored here, data collected from Scene Description table-----*/
				category.push_back(row[0]); //category
				integrationId.push_back(row[1]);//ref1
				ref2.push_back(row[2]);//ref2
				ref3.push_back(row[3]);//ref3
				ref4.push_back(row[4]);//ref4		
				ref5.push_back(row[5]);//ref5
				ref6.push_back(row[6]);//ref6
				ref9.push_back(row[7]);//ref9
				level.push_back(row[8]);//ref10
				localInstanceId.push_back(row[9]);//local instance id
				ref7.push_back(row[10]);
				flagForTiming.push_back(row[11]);
				startTimeInDb.push_back(row[12]);
				endTimeInDb.push_back(row[13]);
				sunRiseTimeDelay.push_back(row[14]);
				sunSetTimeDelay.push_back(row[15]);
				//controlType.push_back(row[10]);//controlType.push_back("NA");
			}
		}
				
		db.mysqlQuery("SELECT sunrise,sunset FROM Client_Information WHERE sunrise IS NOT NULL AND sunset IS NOT NULL;");
		if(mysql_num_rows(db.result) > 0)
		{
			if((row = mysql_fetch_row(db.result)))
			{
				/*-----data collected from client Information table, used for timed scenario functions in Light, curtain and RGB-----*/
				sunRiseTime = row[0];
				sunSetTime = row[1];
			}
		}
		
		#ifdef DEBUG
			cout << "No of Devices\t\t:" << to_string(category.size()) << br();
		#endif
					
		//scene starts here
		#ifdef DEBUG
			cout << "Scene start Json:" << jsonCreate("Scene",userId,roomId,"",sceneId,"0","","","","","","","","") << br();
		#endif
		socketSend(jsonCreate("Scene",userId,roomId,"",sceneId,"0","","","","","","","",""),"64526");
		
		thread *thread1 = new thread[category.size()];
		
		/*-----Thread spawning for sending JSON for Non-IR(Light, curtain and RGB) devices-----*/
		for(loopNo1=0;loopNo1 < category.size();loopNo1++) 
		{
			#ifdef DEBUG
				cout << "loop_no:" << loopNo1 << br();
				cout << "category:" << category[loopNo1] << br(); //category
				cout << "integration Id:" << integrationId[loopNo1] << br();//ref1
				cout << "unique Id:" << ref2[loopNo1] << br();//ref2
				cout << "ref 3:" << ref3[loopNo1] << br();//ref3
				cout << "ref 4:" << ref4[loopNo1] << br();//ref4		
				cout << "ref 5:" << ref5[loopNo1] << br();//ref5
				cout << "ref 6:" << ref6[loopNo1] << br();//ref6
				cout << "ref 9:" << ref9[loopNo1] << br();//ref9
				cout << "ref 10:" << level[loopNo1] << br();//ref10
				cout << "localInstanceId:" << localInstanceId[loopNo1] << br();//local instance id
				cout << "ref 7:" << ref7[loopNo1] <<br();//ref7
				cout << "flag:" << flagForTiming[loopNo1] << br();
				cout << "startTime:" << startTimeInDb[loopNo1] << br();
				cout << "endTime:" << endTimeInDb[loopNo1] << br();
				cout << "sunRiseDelay:" << sunRiseTimeDelay[loopNo1] << br();
				cout << "sunSetDelay:" << sunSetTimeDelay[loopNo1] << br() << br();
			#endif
			
			if((category[loopNo1] == "Light")||(category[loopNo1] == "Curtain")||(category[loopNo1] == "Color Light"))
			{
				if(!flagForTiming[loopNo1].empty() && ((flagForTiming[loopNo1] == "1") || ((flagForTiming[loopNo1] == "2") && (!startTimeInDb[loopNo1].empty() && !endTimeInDb[loopNo1].empty()) && (timeCheck(startTimeInDb[loopNo1],endTimeInDb[loopNo1],"",""))) || ((flagForTiming[loopNo1] == "3") && (!startTimeInDb[loopNo1].empty() && !endTimeInDb[loopNo1].empty() && !sunRiseTimeDelay[loopNo1].empty() && !sunSetTimeDelay[loopNo1].empty()) && (timeCheck(startTimeInDb[loopNo1],endTimeInDb[loopNo1],sunRiseTimeDelay[loopNo1],sunSetTimeDelay[loopNo1])))))
				{
					/*-----Always flag is enabled-----*/
					if((!ref3.at(loopNo1).empty()) && (ref3.at(loopNo1) != "NA"))
					{
						if((category.at(loopNo1) == "Color Light")&&(!ref4.at(loopNo1).empty()))
						{
							if(ref5.at(loopNo1) == "0")//type 0
							{
								if(ref3.at(loopNo1).find(",") != string::npos)//rgb custom command validation
								{
									string ar[4];
									ar[2] = ref3.at(loopNo1);
									ar[0] = ar[2].substr(0,ar[2].find(","));
									ar[2].erase(0,ar[2].find(",")+1);
									ar[1] = ar[2].substr(0,ar[2].find(","));
									ar[2].erase(0,ar[2].find(",")+1);
									
									for(int i=0;ar[0].find(" ") != string::npos;i++)
										ar[0].erase(0,ar[0].find(" ")+1);
									
									for(int i=0;ar[1].find(" ") != string::npos;i++)
										ar[1].erase(0,ar[1].find(" ")+1);
									  
									 for(int i=0;ar[2].find(" ") != string::npos;i++)
										ar[2].erase(0,ar[2].find(" ")+1);   
									
									if(!ar[0].empty() && !ar[1].empty() && !ar[2].empty())
									{
										if(isInt(ar[0],10) && isInt(ar[1],10) && isInt(ar[2],10))					
										{
											if(ar[0].length() < 3)
												ar[0] = ((ar[0].length() == 1)? ("00"+ar[0]) : ((ar[0].length() == 2)? ("0"+ar[0]) : ar[0]));
											if(ar[1].length() < 3)
												ar[1] = ((ar[1].length() == 1)? ("00"+ar[1]) : ((ar[1].length() == 2)? ("0"+ar[1]) : ar[1]));
											if(ar[2].length() < 3)
												ar[2] = ((ar[2].length() == 1)? ("00"+ar[2]) : ((ar[2].length() == 2)? ("0"+ar[2]) : ar[2]));
										
											ar[3] = ar[0]+","+ar[1]+","+ar[2];
											
											if(ref4.at(loopNo1) == "2")//control type always 2
											{
												sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),"000,000,000",ref5.at(loopNo1),"","","","","","","");
											}
											if((ar[0] != "000") || (ar[1] != "000") || (ar[2] != "000")) 
											{
												if(!ref6.at(loopNo1).empty())
												{
													if(ref6.at(loopNo1) == "0")
													{
														sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ar[3],ref5.at(loopNo1),"","","","","","","");
													}
													else
													{
														thread1[loopNo1] = thread(timerSecs,stoi(ref6.at(loopNo1)),category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ar[3],ref5.at(loopNo1),"","","");
													}
												}
												else
												{
													sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ar[3],ref5.at(loopNo1),"","","","","","","");
												}
											}
										}
									}
								}
							}
							else if(ref5.at(loopNo1) == "1")//type 1
							{
								if(ref3.at(loopNo1).find(",") == string::npos)//rgb intensity command validation
								{
									if(isInt(ref3.at(loopNo1),10))//rgb data validation 
									{
										if(ref4.at(loopNo1) == "1")
										{
											if(!ref6.at(loopNo1).empty())
											{
												if(ref6.at(loopNo1) == "0")
												{
													sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","","","","","");
												}
												else
												{
													thread1[loopNo1] = thread(timerSecs,stoi(ref6.at(loopNo1)),category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","");
												}
											}
											else
											{
												sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","","","","","");
											}
										}
									}
								}
							}
							else if(ref5.at(loopNo1) == "2")//type 2
							{
								if((ref4.at(loopNo1) == "2") && (ref3.at(loopNo1) == "RND"))//control type
								{
									if(!ref6.at(loopNo1).empty())
									{
										if(ref6.at(loopNo1) == "0")
										{
											sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","","","","","");
										}
										else
										{
											thread1[loopNo1] = thread(timerSecs,stoi(ref6.at(loopNo1)),category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","");
										}
									}
									else
									{
										sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","","","","","");
									}
								}
							}
						}
						else if(category.at(loopNo1) == "Light")
						{
							if(!ref6.at(loopNo1).empty())
							{
								if(ref6.at(loopNo1) == "0")
								{	
									if(isInt(ref3.at(loopNo1),10))
									{	
										if((stoi(ref3.at(loopNo1)) >=0) && (stoi(ref3.at(loopNo1)) <=100))
											sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),"","","","","","","","");
									}
								}
								else
								{
									if(isInt(ref6.at(loopNo1),10))
									{
										if(isInt(ref3.at(loopNo1),10))
										{	
											if((stoi(ref3.at(loopNo1)) >=0) && (stoi(ref3.at(loopNo1)) <=100))
												thread1[loopNo1] = thread(timerSecs,stoi(ref6.at(loopNo1)),category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),"","","","");
										}
									}
								}
							}	
							else
							{	
								if(isInt(ref3.at(loopNo1),10))
								{	
									if((stoi(ref3.at(loopNo1)) >=0) && (stoi(ref3.at(loopNo1)) <=100))
										sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),"","","","","","","","");
								}
							}
						}
						else if(category.at(loopNo1) == "Curtain")
						{
							if(isInt(ref3.at(loopNo1),10))
							{
								if((stoi(ref3.at(loopNo1)) >=0) && (stoi(ref3.at(loopNo1)) <=100))
									sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),"","","","","","","","");
							}
						}
					}
				}
			}
			else if((category.at(loopNo1) == "Media")&&(ref3.at(loopNo1) != "NA"))
			{
				/*-----Nuvo params, strings created for understanding-----*/
				string zoneNameNuvo{},favouriteNuvo{},modeNuvo{},sourceNuvo{},volumeNuvo{},playListClimax{},statusClimax{},volumeClimax{},shuffleClimax{},deviceIdClimax{};
				if(ref7.at(loopNo1).empty() || (ref7.at(loopNo1) == "0"))
				{
					if(integrationId.at(loopNo1) == "10000")
					{
						deviceIdClimax = ref2.at(loopNo1);				//ref_2
						statusClimax = ref3.at(loopNo1);				//ref_3
						playListClimax = ref4.at(loopNo1);				//ref_4	
						volumeClimax = ref5.at(loopNo1);				//ref_5	
						shuffleClimax = ref6.at(loopNo1);				//ref_6
						
					}
					else if((integrationId.at(loopNo1) == "10001")||(integrationId.at(loopNo1) == "10002"))
					{
						zoneNameNuvo = ref2.at(loopNo1);			//ref_2
						modeNuvo = ref3.at(loopNo1);				//ref_3
						favouriteNuvo = ref4.at(loopNo1);			//ref_4
						volumeNuvo = ref5.at(loopNo1);				//ref_5	
						sourceNuvo = ref6.at(loopNo1);				//ref_6
					}
				}
				else if(ref7.at(loopNo1) == "1")//MUSIC PREFERENCE
				{
					/*-----to check year and month-----*/
					time_t timestamp;
					struct tm *timestruct;
					timestamp = time(NULL);
					timestruct = localtime(&timestamp);
					db.mysqlQuery("SELECT MP.playlist_id,MP.status,MP.volume,MP.shuffle,MP.device_id FROM Music AS M LEFT JOIN Music_Preference AS MP ON MP.device_id=M.id AND MP.day_week = '"+to_string(timestruct->tm_wday)+"' AND MP.scene_id = '"+sceneId+"' WHERE M.local_instance_id = '"+localInstanceId[loopNo1]+"' AND MP.playlist_id IS NOT NULL AND M.id IS NOT NULL AND MP.status IS NOT NULL AND MP.volume IS NOT NULL AND MP.shuffle IS NOT NULL AND MP.device_id IS NOT NULL;");
					if(mysql_num_rows(db.result) > 0)
					{
						if ((row = mysql_fetch_row(db.result)))
						{
							playListClimax = row[0];
							statusClimax = row[1];
							volumeClimax = row[2];
							shuffleClimax = row[3];
							deviceIdClimax = row[4];
						}
					}
				}
				
				#ifdef DEBUG
					cout << "Device id =" << deviceIdClimax << br();
					cout << "Playlist id =" << playListClimax << br(); 
					cout << "status (1-on/2-off)  =" << statusClimax << br();
					cout << "Volume =" << volumeClimax << br();
					cout << "shuffle(0-off,1-On) =" << shuffleClimax << br() << br();
				#endif
				if((!flagForTiming[loopNo1].empty())&&(flagForTiming[loopNo1] =="1"))
				{
					/*-----Only always flag for these devices.-----*/
					if(integrationId.at(loopNo1) == "10000")
					{
						if(statusClimax == "1")//ON
						{
							sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),deviceIdClimax,"3","play_playlist",playListClimax,"","","","","","");
							sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),deviceIdClimax,"1","volume",volumeClimax,"","","","","","");
							if(shuffleClimax == "0")
								sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),deviceIdClimax,"1","shuffle_off","","","","","","","");
							else if(shuffleClimax == "1")
								sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),deviceIdClimax,"1","shuffle_on","","","","","","","");	
						}
						else if(statusClimax == "2")//OFF
						{
							sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),deviceIdClimax,"1","stop","","","","","","","");
						}
					}
					else if((integrationId.at(loopNo1) == "10001")||(integrationId.at(loopNo1) == "10002"))
					{	
						if(modeNuvo == "1")//ON
						{
							//ON
							sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),zoneNameNuvo,"1","ON","","","","","","","");
							
							//source select
							//sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),zoneNameNuvo,"1","shuffle_off","","","","","","","");
							
							//play_playlist
							sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),zoneNameNuvo,"3","play_playlist",favouriteNuvo,"","","","","","");
							
							//volume
							sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),zoneNameNuvo,"1","volume",volumeNuvo,"","","","","","");
						}
						else if(modeNuvo == "2")//OFF
						{
							sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),zoneNameNuvo,"1","OFF","","","","","","","");
						}
					}
				}
			}
			
			else if((category.at(loopNo1) == "IR")&&(ref3.at(loopNo1) != "NA"))
			{
				//driver handled directly from scene and json server not invloved.
				string deviceModelId{},url = "http://localhost:6161/Milan/Drivers/MILAN_EXE/IR/ir.cgi?";
				db.mysqlQuery("SELECT device_model_id FROM IR_Devices WHERE id = '"+ref2[loopNo1]+"' AND device_model_id IS NOT NULL;");
				if ((row = mysql_fetch_row(db.result))) 
				{
					deviceModelId = row[0];
				}
				
				deviceModelId=integrationId[loopNo1];
				#ifdef DEBUG
					cout << "deviceModelId:" << deviceModelId << br();
				#endif
				if((!flagForTiming[loopNo1].empty())&&(flagForTiming[loopNo1] == "1"))
				{
					/*-----Only always flag for these devices.-----*/
					if(ref3.at(loopNo1) == "OC")
					{
						//sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate("Channels",userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref5.at(loopNo1),"","","","","","","","");
						url = url+"l_instid="+localInstanceId[loopNo1]+"&irid="+ref2[loopNo1]+"&cat="+category[loopNo1]+"&userid="+userId+"&cmd1=NA&cmd2=NA&cmd3="+ref3[loopNo1]+"&cmd4=NA";
						webQuery(url,"GET");
					}
					else
					{
						//sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),"","","","","","","","");
						url = url+"l_instid="+localInstanceId[loopNo1]+"&irid="+ref2[loopNo1]+"&cat="+category[loopNo1]+"&userid="+userId+"&cmd1="+ref3[loopNo1]+"&cmd2=NA&cmd3=NA&cmd4=NA";
						webQuery(url,"GET");
						if(!ref4.at(loopNo1).empty() && !ref5.at(loopNo1).empty() && (ref3.at(loopNo1) != "2" ))
						{
							if(isInt(ref4.at(loopNo1),10))
							{
								if(deviceModelId == "33")
								{
									thread1[loopNo1] = thread(timerSecs,stoi(ref4[loopNo1]),"Channels",userId,localInstanceId[loopNo1],integrationId[loopNo1],ref2[loopNo1],ref5[loopNo1],"","","","");
								}
								else
								{
									thread1[loopNo1] = thread(timerSecs,stoi(ref4[loopNo1]),category[loopNo1],userId,localInstanceId[loopNo1],integrationId[loopNo1],ref2[loopNo1],ref5[loopNo1],"","","","");
								}
							}
						}
					}
				}
			}
			else if((category[loopNo1] == "Appliances")&&(ref3[loopNo1] != "NA"))
			{
				if((!flagForTiming[loopNo1].empty())&&(flagForTiming[loopNo1] == "1"))
				{
					/*-----Only always flag for these devices.-----*/
					if(!ref4.at(loopNo1).empty())
					{
						if(isInt(ref4.at(loopNo1),10))
						{
							sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref4.at(loopNo1),"","","","","","","");
						}
					}
				}
			}
			else if((category[loopNo1] == "Comfort")&&(ref3[loopNo1] != "NA"))
			{
				if((!flagForTiming[loopNo1].empty())&&(flagForTiming[loopNo1] == "1"))
				{
					/*-----Only always flag for these devices.-----*/
					sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),"Mode",ref3.at(loopNo1),"","","","","","","");
					if(!ref4.at(loopNo1).empty() && !ref4.at(loopNo1).empty() && !ref5.at(loopNo1).empty() && (ref3.at(loopNo1) != "2" ))
					{
						thread1[loopNo1] = thread(timerSecs,stoi(ref4.at(loopNo1)),category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),"Set Point","",ref5.at(loopNo1),"","");
					}
				}
			}
		}
		
		#ifdef DEBUG
			cout << "Bulk Json:\n" << sceneJson << br() << br();
		#endif
		
		if(!sceneJson.empty())
			socketSend(sceneJson,"64527");
		
		for(loopNo1=0;loopNo1 < category.size();loopNo1++) 
		{
			if((category.at(loopNo1) == "Light")||(category.at(loopNo1) == "Curtain")||(category.at(loopNo1) == "Color Light"))
			{
				if(!flagForTiming[loopNo1].empty() && ((flagForTiming[loopNo1] == "1") || ((flagForTiming[loopNo1] == "2") && (!startTimeInDb[loopNo1].empty() && !endTimeInDb[loopNo1].empty()) && (timeCheck(startTimeInDb[loopNo1],endTimeInDb[loopNo1],"",""))) || ((flagForTiming[loopNo1] == "3") && (!startTimeInDb[loopNo1].empty() && !endTimeInDb[loopNo1].empty() && !sunRiseTimeDelay[loopNo1].empty() && !sunSetTimeDelay[loopNo1].empty()) && (timeCheck(startTimeInDb[loopNo1],endTimeInDb[loopNo1],sunRiseTimeDelay[loopNo1],sunSetTimeDelay[loopNo1])))))
				{
					{
						/*-----Always flag is enabled-----*/
						if((!ref3.at(loopNo1).empty()) && (ref3.at(loopNo1) != "NA"))
						{
							if((category.at(loopNo1) == "Color Light")&&(!ref4.at(loopNo1).empty()))
							{
								if(ref5.at(loopNo1) == "0")//type 0
								{
									if(ref3.at(loopNo1).find(",") != string::npos)//rgb custom command validation
									{
										string ar[4];
										ar[2] = ref3.at(loopNo1);
										ar[0] = ar[2].substr(0,ar[2].find(","));
										ar[2].erase(0,ar[2].find(",")+1);
										ar[1] = ar[2].substr(0,ar[2].find(","));
										ar[2].erase(0,ar[2].find(",")+1);
										
										for(int i=0;ar[0].find(" ") != string::npos;i++)
											ar[0].erase(0,ar[0].find(" ")+1);
										
										for(int i=0;ar[1].find(" ") != string::npos;i++)
											ar[1].erase(0,ar[1].find(" ")+1);
										  
										 for(int i=0;ar[2].find(" ") != string::npos;i++)
											ar[2].erase(0,ar[2].find(" ")+1);   
										
										if(!ar[0].empty() && !ar[1].empty() && !ar[2].empty())
										{
											if(isInt(ar[0],10) && isInt(ar[1],10) && isInt(ar[2],10))					
											{
												if(ar[0].length() < 3)
													ar[0] = ((ar[0].length() == 1)? ("00"+ar[0]) : ((ar[0].length() == 2)? ("0"+ar[0]) : ar[0]));
												if(ar[1].length() < 3)
													ar[1] = ((ar[1].length() == 1)? ("00"+ar[1]) : ((ar[1].length() == 2)? ("0"+ar[1]) : ar[1]));
												if(ar[2].length() < 3)
													ar[2] = ((ar[2].length() == 1)? ("00"+ar[2]) : ((ar[2].length() == 2)? ("0"+ar[2]) : ar[2]));
											
												ar[3] = ar[0]+","+ar[1]+","+ar[2];
												
												if((ar[0] != "000") && (ar[1] != "000") && (ar[2] != "000")) 
												{
													if(!ref6.at(loopNo1).empty())
													{
														if(ref6.at(loopNo1) == "0")
														{}
														else
														{
															thread1[loopNo1].join();
														}
													}
												}
											}
										}
									}
								}
								else if(ref5.at(loopNo1) == "1")//type 1
								{
									if(ref3.at(loopNo1).find(",") == string::npos)//rgb intensity command validation
									{
										if(isInt(ref3.at(loopNo1),10))//rgb data validation 
										{
											if(ref4.at(loopNo1) == "1")
											{
												if(!ref6.at(loopNo1).empty())
												{
													if(ref6.at(loopNo1) == "0")
													{}
													else
													{
														thread1[loopNo1].join();
													}
												}
											}
										}
									}
								}
								else if(ref5.at(loopNo1) == "2")//type 2
								{
									if((ref4.at(loopNo1) == "2") && (ref3.at(loopNo1) == "RND"))//control type
									{
										if(!ref6.at(loopNo1).empty())
										{
											if(ref6.at(loopNo1) == "0")
											{}
											else
											{
												thread1[loopNo1].join();
											}
										}
									}
								}
							}
							else if(category.at(loopNo1) == "Light")
							{
								if(!ref6.at(loopNo1).empty())
								{
									if(ref6.at(loopNo1) == "0")
									{}
									else
									{
										if(isInt(ref6.at(loopNo1),10))
										{
											if(isInt(ref3.at(loopNo1),10))
											{	
												if((stoi(ref3.at(loopNo1)) >=0) && (stoi(ref3.at(loopNo1)) <=100))
													thread1[loopNo1].join();
											}
										}
									}
								}
							}
						}
					}
				}
			}
						
			else if((category.at(loopNo1) == "IR")&&(ref3.at(loopNo1) != "NA"))
			{
				if((!flagForTiming[loopNo1].empty())&&(flagForTiming[loopNo1] == "1"))
				{
					if((ref3.at(loopNo1) != "OC") && !ref4.at(loopNo1).empty() && !ref5.at(loopNo1).empty() &&ref3.at(loopNo1) != "2")
					{
						if(isInt(ref4.at(loopNo1),10))
						{
							if(integrationId.at(loopNo1) == "33")
							{
								thread1[loopNo1].join();
							}
							else
							{
								thread1[loopNo1].join();
							}
						}
					}
				}
			}
			else if((category.at(loopNo1) == "Comfort")&&(ref3.at(loopNo1) != "NA"))
			{
				if((!flagForTiming[loopNo1].empty())&&(flagForTiming[loopNo1] == "1"))
				{
					if(!ref2.at(loopNo1).empty() && !ref4.at(loopNo1).empty() && !ref5.at(loopNo1).empty() && (ref3.at(loopNo1) != "2" ))
					{
						thread1[loopNo1].join();
					}
				}
			}
		}
		
		#ifdef DEBUG
			cout << "Scene finish Json:" << jsonCreate("Scene",userId,roomId,"",sceneId,"1","","","","","","","","") << br();
		#endif
		socketSend(jsonCreate("Scene",userId,roomId,"",sceneId,"1","","","","","","","",""),"64526");
	}
	
	return 0;
}

//Member Function of Class scene checks the Time, whether it is between startTimeInDb and endTimeInDb with or without sunRiseDelay and sunSetDelay included
bool scene::timeCheck(string startTimeInDb,string endTimeInDb,string sunRiseDelay,string sunSetDelay)
{
	if(!startTimeInDb.empty() && !endTimeInDb.empty() && !sunRiseDelay.empty() && !sunSetDelay.empty())
	{
		//time_t is a type to store time values
		time_t Currenttime,startTimeCheck,endTimeCheck;
		
		//struct tm is a structure to store time values and which can be modified
		struct tm * startTime={0};
		struct tm * endTime={0};
		
		//variables required for time operation
		string startHr,endHr,startMin,endMin,sunRiseHr,sunRiseMin,sunSetHr,sunSetMin,sign1,sign2;
		long int startTimeGap,endTimeGap;
		bool returnValue;
		
		//hours and mins extracted from function params,startTimeInDb and endTimeInDb
		startHr = startTimeInDb.substr(startTimeInDb.find(":") - 2,startTimeInDb.find(":"));
		startMin = startTimeInDb.substr(startTimeInDb.find(":")+1,startTimeInDb.find(":") + 2);
		
		endHr = endTimeInDb.substr(endTimeInDb.find(":") - 2,endTimeInDb.find(":"));
		endMin = endTimeInDb.substr(endTimeInDb.find(":")+1,endTimeInDb.find(":") + 2);
		
		//checks for Sun rise delay and sun set delay is given in function params or empty
		if(!sunRiseDelay.empty() && !sunSetDelay.empty())
		{
			//if "-" sign present, then start time - sun rise delay
			int q2 = sunRiseDelay.find("-");
			if(q2 >= 0)
			{
				sign1 = sunRiseDelay.substr(sunRiseDelay.find("-"),sunRiseDelay.find("-")+1);
			}
			else
			{
				//otherwise , then start time + sun rise delay
				sign1 = sunRiseDelay.substr(sunRiseDelay.find("+"),sunRiseDelay.find("+")+1);
			}
			
			//sun rise hr and min calculated  here.
			sunRiseHr = sunRiseDelay.substr(sunRiseDelay.find(":") - 2,sunRiseDelay.find(":"));
			sunRiseMin = sunRiseDelay.substr(sunRiseDelay.find(":")+1,sunRiseDelay.find(":") + 2);
			
			//if "-" sign present, then start time - sun rise delay
			q2 = sunSetDelay.find("-");
			if(q2 >= 0)
			{
				sign2 = sunSetDelay.substr(sunSetDelay.find("-"),sunSetDelay.find("-")+1);
			}
			else
			{
				//otherwise , then start time + sun rise delay
				sign2 = sunSetDelay.substr(sunSetDelay.find("+"),sunSetDelay.find("+")+1);
			}
			
			//sun set hr and min calculated  here.
			sunSetHr = sunSetDelay.substr(sunSetDelay.find(":") - 2,sunSetDelay.find(":"));
			sunSetMin = sunSetDelay.substr(sunSetDelay.find(":")+1,sunSetDelay.find(":") + 2);
		}
			
		//getting the current time
		time ( &Currenttime );

		//getting the start time string in time format
		startTime = localtime ( &Currenttime );
		if(!sunRiseDelay.empty() && !sunSetDelay.empty())
		{
			if(sign1 == "+" )
			{
				startTime->tm_hour = stoi(startHr) + stoi(sunRiseHr);
				startTime->tm_min = stoi(startMin) + stoi(sunRiseMin);
			}
			else if(sign1 == "-" )
			{
				startTime->tm_hour = stoi(startHr) - stoi(sunRiseHr);
				startTime->tm_min = stoi(startMin) - stoi(sunRiseMin);		
			}
		}
		else
		{
			startTime->tm_hour = stoi(startHr);
			startTime->tm_min = stoi(startMin);
		}
		
		//start time is constucted in time_t format
		startTimeCheck = mktime(startTime);
		
		//cout<<"Start Time "<<ctime(&startTimeCheck)<<br();
		
		//getting the start time gap, difference in time between start time and current time
		startTimeGap = difftime(startTimeCheck,Currenttime);
		
		//getting the end time string in time format
		endTime = localtime ( &Currenttime );
		if(!sunRiseDelay.empty() && !sunSetDelay.empty())
		{
			if(sign2 == "+")
			{
				endTime->tm_hour = stoi(endHr) + stoi(sunSetHr);
				endTime->tm_min = stoi(endMin) + stoi(sunSetMin);
			}
			else
			{
				endTime->tm_hour = stoi(endHr) - stoi(sunSetHr);
				endTime->tm_min = stoi(endMin) - stoi(sunSetMin);
			}
		}
		else
		{
			endTime->tm_hour = stoi(endHr);
			endTime->tm_min = stoi(endMin);
		}
		
		//end time is constucted in time_t format
		endTimeCheck = mktime(endTime);
			
		//getting the end time gap, difference in time between end time and current time
		endTimeGap = difftime(endTimeCheck,Currenttime);
				
		if((startTimeGap <0)&&(endTimeGap >= 0))
		{
			returnValue =1;
		}
		else
		{
			returnValue =0;
		}
		
		#ifdef DEBUG
			cout << "ReturnValue (Time comparison):" << returnValue << br();
		#endif
		
		//Time status is updated in returnValue
		return returnValue;
	}
	else
		return 0;
}

/*******************************************************************************
 * Main program to perofrm scene Execution.									   *
 *******************************************************************************/
int main()
{
	string sceneId{},userId{};
	try
	{
		Cgicc cgi;
		timeval start;
		gettimeofday(&start, NULL);
		const CgiEnvironment& env = cgi.getEnvironment();
		
		cout << HTTPHTMLHeader();
        // Output the HTML 4.0 DTD info
		cout << HTMLDoctype(HTMLDoctype::eStrict) << br();
		cout << html().set("lang", "en").set("dir", "ltr");
		// Set up the page's header and title.
		// I will put in lfs to ease reading of the produced HTML. 
		cout << head();

		// Output the style sheet portion of the header
		cout << title() << "Moodpro Scenes." << title();
		cout << head();
		
		// Start the HTML body
		cout << body();
		/*-----getting the User id-----*/
		form_iterator i1 = cgi.getElement("user_id");
		if(i1 != cgi.getElements().end())
		{
			userId = i1->getValue();
		}
		
		form_iterator i2 = cgi.getElement("scene_id");
		if(i2 != cgi.getElements().end())
		{
			sceneId = i2->getValue();
		}
		
		#ifdef DEBUG
			cout << "Method:" << env.getRequestMethod() << br();
			cout << "Postdata:" << env.getPostData() << br();
			cout << "Pathinfo:" << env.getPathInfo() << br();
			cout << "QueryString:" << env.getQueryString() << br();
			cout << "ContentLength:" << env.getContentLength() << br();
			cout << "Remote Address:" << env.getRemoteAddr() << br() << br();
		#endif

		//userId = "RID-542-24636973855539201";
		//sceneId = "24841326453325924";
		//Example query:http://localhost/Milan/Drivers/MILAN_EXE/SCENE/scene.cgi?user_id=15882735709066137&room_id=15882735709066151&scene_id=15882735709066152
		if(!userId.empty()  && !sceneId.empty())
		{
			#ifdef DEBUG
				cout << "user id:" << userId << br();
				cout << "Scene id:" << sceneId << br() << br();
			#endif
			
			scene s1;
			#ifdef DEBUG
				s1.sceneExecution(userId,sceneId);
			#else
			if((s1.getMyIp("wlan0") == env.getRemoteAddr()) || (s1.getMyIp("eth0") == env.getRemoteAddr()) || (s1.getMyIp("lo") == env.getRemoteAddr()))
				s1.sceneExecution(userId,sceneId);
			#endif
		}
		else
		{
			if(userId.empty())
			{
				#ifdef DEBUG
					cout << "User id is empty." << br();
				#endif
			}
			
			if(sceneId.empty())
			{
				#ifdef DEBUG
					cout << "Scene id is empty." << br();
				#endif
			}
		}

		timeval end;
		gettimeofday(&end, NULL);

		#ifdef DEBUG
			cout << "Scene Execution finished successfully." << br() << body() << html();
			long us = ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec);
			cout << "Total time for request :" << us << "us" << " (" << (double) (us/1000000.0) << "s)";
		#endif

	}
	catch(exception& e) 
	{
		/*-----handle any errors - omitted for brevity-----*/
		#ifdef DEBUG
			cout  << br() <<"Exception handled, Scene Execution terminated, Please Check for errors." << br() << body() << html();
		#endif
	}
}
