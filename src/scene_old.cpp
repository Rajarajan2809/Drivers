/*******************************************************************************
 *	Purpose		:	Scene Execution.										   *
 * 	Description	:	Main file for scene Execution.							   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	30-Jul-16												   *
 * 	DOLM		:	23-Nov-16												   *
 *******************************************************************************/

/*-----local files-----*/
#include "scene.hpp"

using namespace std;
using namespace cgicc;

/*******************************************************************************
 * Member Function of Class Scene which controls the flow of scene execution   *
 * and sends JSON to json server.											   *
 *******************************************************************************/
int scene::sceneExecution()
{
	/*-----vector to store the scene params-----*/
	vector<string> category,integrationId,localInstanceId,level,ref2,ref3,ref4,ref5,ref6,ref9,ref7;
	
	/*-----scene Execution begins------*/
	string user,roomId,sunRiseTime,sunSetTime,sceneJson,delayJson;
	size_t loopNo1=0;
				
	/*-----Mysql parameters-----*/
	MYSQL_ROW row;
	t2.mysqlQuery("SELECT q2.user,q2.room_id FROM C_Mymilan_Users AS q1 LEFT JOIN Scene AS q2 ON q2.user = q1.id WHERE q2.id = '"+sceneId+"' AND q1.mymilan_user_id = '"+userId+"' AND q2.user IS NOT NULL AND q2.room_id IS NOT NULL;");
	if((row = mysql_fetch_row(t2.result)))
	{
		user = row[0];
		roomId = row[1];
	}
	
	#ifdef DEBUG
		cout << "user id\t\t\t:" << user << "\n";
		cout << "room id\t\t\t:" << roomId << "\n";
	#endif
	
	if(!user.empty())
	{
		/*-----User exists for this particular Scene-----*/
		t2.mysqlQuery("SELECT category,ref_1,ref_2,ref_3,ref_4,ref_5,ref_6,ref_9,ref_10,local_instance_id,ref_7 FROM Scene_Description WHERE scene_id='" +sceneId+ "' AND category IS NOT NULL AND ref_1 IS NOT NULL AND ref_2 IS NOT NULL AND ref_3 IS NOT NULL AND ref_4 IS NOT NULL AND ref_5 IS NOT NULL AND ref_6 IS NOT NULL AND ref_9 IS NOT NULL AND ref_10 IS NOT NULL AND local_instance_id IS NOT NULL AND ref_7 IS NOT NULL ORDER BY execution_order;");

		MYSQL_ROW row1,row2;
		while ((row1 = mysql_fetch_row(t2.result)))
		{
			/*-----Result of Query is stored here, data collected from Scene Description table-----*/
			category.push_back(row1[0]); //category
			integrationId.push_back(row1[1]);//ref1
			ref2.push_back(row1[2]);//ref2
			ref3.push_back(row1[3]);//ref3
			ref4.push_back(row1[4]);//ref4		
			ref5.push_back(row1[5]);//ref5
			ref6.push_back(row1[6]);//ref6
			ref9.push_back(row1[7]);//ref9
			level.push_back(row1[8]);//ref10
			localInstanceId.push_back(row1[9]);//local instance id
			ref7.push_back(row1[10]);
			//controlType.push_back(row1[10]);//controlType.push_back("NA");
		}
				
		t2.mysqlQuery("SELECT sunrise,sunset FROM Client_Information WHERE sunrise IS NOT NULL AND sunset IS NOT NULL;");
		if((row2 = mysql_fetch_row(t2.result)))
		{
			/*-----data collected from client Information table, used for timed scenario functions in Light, curtain and RGB-----*/
			sunRiseTime = row2[0];
			sunSetTime = row2[1];
		}
		
		#ifdef DEBUG
			cout << "No of Devices\t\t:" << to_string(category.size()) << "\n";
		#endif
		
		/*-----JSON Send class, object declaration-----*/
		jsonSend s1;
		
		//scene starts here
		#ifdef DEBUG
			cout<<"Scene start Json:"<<s1.jsonCreate("Scene",userId,roomId,"",sceneId,"0","","","","","","","","")<<"\n"<<"\n";
		#endif
		s1.socketSendSingleJson(s1.jsonCreate("Scene",userId,roomId,"",sceneId,"0","","","","","","","",""));
		
		thread *thread1 = new thread[category.size()];
		
		/*-----Thread spawning for sending JSON for Non-IR(Light, curtain and RGB) devices-----*/
		for(loopNo1=0;loopNo1 < category.size();loopNo1++) 
		{
			#ifdef DEBUG
				cout << "loop No.\t\t:" << loopNo1 << ",\n";
				cout << "category\t\t:" << category.at(loopNo1) << ",\n"; //category
				cout << "integration Id\t\t:" << integrationId.at(loopNo1) << ",\n";//ref1
				cout << "unique Id\t\t:" << ref2.at(loopNo1) << ",\n";//ref2
				cout << "ref 3\t\t\t:" << ref3.at(loopNo1) << ",\n";//ref3
				cout << "ref 4\t\t\t:" << ref4.at(loopNo1) << ",\n";//ref4		
				cout << "ref 5\t\t\t:" << ref5.at(loopNo1) << ",\n";//ref5
				cout << "ref 6\t\t\t:" << ref6.at(loopNo1) << ",\n";//ref6
				cout << "ref 9\t\t\t:" << ref9.at(loopNo1) << ",\n";//ref9
				cout << "ref 10\t\t\t:" << level.at(loopNo1) << ",\n";//ref10
				cout << "localInstanceId\t\t:" << localInstanceId.at(loopNo1) << ",\n";//local instance id
				cout << "ref 7\t\t\t:" << ref7.at(loopNo1) <<".\n\n";//ref7
			#endif
			
			if((category.at(loopNo1) == "Light")||(category.at(loopNo1) == "Curtain")||(category.at(loopNo1) == "Color Light"))
			{
				t2.mysqlQuery("SELECT flag,start_time,end_time,rise_delay,set_delay FROM Scene_Execution where category = '" + category.at(loopNo1) + "' and scene_id = '" + sceneId + "' AND flag IS NOT NULL AND start_time IS NOT NULL AND end_time IS NOT NULL AND rise_delay IS NOT NULL AND set_delay IS NOT NULL;");
				string flagForTiming,startTimeInDb,endTimeInDb,sunRiseTimeDelay,sunSetTimeDelay;
				MYSQL_ROW row3;
				if ((row3 = mysql_fetch_row(t2.result))) 
				{
					flagForTiming = row3[0];
					startTimeInDb = row3[1];
					endTimeInDb = row3[2];
					sunRiseTimeDelay = row3[3];
					sunSetTimeDelay = row3[4];
				}
				
				#ifdef DEBUG
					cout << "Scene Execution Parameters.\n";
					cout << "flag: 1 - always, 2 - b/w start and end time and 3 - b/w start+sunrisedelay and end+sunsetdelay.\n";
					cout << "flag\t\t\t:" << flagForTiming << ",\n";
					cout << "startTime\t\t:" << startTimeInDb << ",\n";
					cout << "endTime\t\t\t:" << endTimeInDb << ",\n";
					cout << "sunRiseDelay\t\t:" << sunRiseTimeDelay << ",\n";
					cout << "sunSetDelay\t\t:" << sunSetTimeDelay << ".\n\n";
				#endif
				if(!flagForTiming.empty())
				{
					switch(stoi(flagForTiming))
					{
						case 1:
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
															sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),"000,000,000",ref5.at(loopNo1),"","","","","","","");
														}
														if((ar[0] != "000") || (ar[1] != "000") || (ar[2] != "000")) 
														{
															if(!ref6.at(loopNo1).empty())
															{
																if(ref6.at(loopNo1) == "0")
																{
																	sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ar[3],ref5.at(loopNo1),"","","","","","","");
																}
																else
																{
																	thread1[loopNo1] = thread(timerSecs,stoi(ref6.at(loopNo1)),category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ar[3],ref5.at(loopNo1),"","","");
																}
															}
															else
															{
																sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ar[3],ref5.at(loopNo1),"","","","","","","");
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
																sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","","","","","");
															}
															else
															{
																thread1[loopNo1] = thread(timerSecs,stoi(ref6.at(loopNo1)),category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","");
															}
														}
														else
														{
															sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","","","","","");
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
														sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","","","","","");
													}
													else
													{
														thread1[loopNo1] = thread(timerSecs,stoi(ref6.at(loopNo1)),category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","");
													}
												}
												else
												{
													sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","","","","","");
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
														sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),"","","","","","","","");
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
													sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),"","","","","","","","");
											}
										}
									}
									else if(category.at(loopNo1) == "Curtain")
									{
										if(isInt(ref3.at(loopNo1),10))
										{
											if((stoi(ref3.at(loopNo1)) >=0) && (stoi(ref3.at(loopNo1)) <=100))
												sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),"","","","","","","","");
										}
									}
								}
							}
							break;
						
						case 2:
							{
								/*-----Timed, executes only when current time is between start and end time-----*/
								if(!startTimeInDb.empty() && !endTimeInDb.empty())
								{
									bool timeReturnValue = timeCheck(startTimeInDb,endTimeInDb,"","");
									if(timeReturnValue)
									{
										/*-----time is between start time and end time-----*/
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
																	sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),"000,000,000",ref5.at(loopNo1),"","","","","","","");
																}
																if((ar[0] != "000") || (ar[1] != "000") || (ar[2] != "000")) 
																{
																	if(!ref6.at(loopNo1).empty())
																	{
																		if(ref6.at(loopNo1) == "0")
																		{
																			sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ar[3],ref5.at(loopNo1),"","","","","","","");
																		}
																		else
																		{
																			thread1[loopNo1] = thread(timerSecs,stoi(ref6.at(loopNo1)),category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ar[3],ref5.at(loopNo1),"","","");
																		}
																	}
																	else
																	{
																		sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ar[3],ref5.at(loopNo1),"","","","","","","");
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
																		sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","","","","","");
																	}
																	else
																	{
																		thread1[loopNo1] = thread(timerSecs,stoi(ref6.at(loopNo1)),category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","");
																	}
																}
																else
																{
																	sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","","","","","");
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
																sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","","","","","");
															}
															else
															{
																thread1[loopNo1] = thread(timerSecs,stoi(ref6.at(loopNo1)),category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","");
															}
														}
														else
														{
															sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","","","","","");
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
																sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),"","","","","","","","");
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
															sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),"","","","","","","","");
													}
												}
											}	
											else if(category.at(loopNo1) == "Curtain")
											{
												if(isInt(ref3.at(loopNo1),10))
												{
													if((stoi(ref3.at(loopNo1)) >=0) && (stoi(ref3.at(loopNo1)) <=100))
														sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),"","","","","","","","");
												}
											}
										}
									}
									else
									{
										/*-----time is not between start time and end time-----*/
										#ifdef DEBUG
											cout << "Time is not between start time and end time.\n\n";
										#endif	
									}
								}
							}
							break;
						
						case 3:	
							if(!sunRiseTime.empty() && !sunSetTime.empty() && !sunRiseTimeDelay.empty() && !sunSetTimeDelay.empty())
							{
								/*-----Timed, executes only when current time is between sun rise time and sun set time-----*/
								bool timeReturnValue = timeCheck(sunRiseTime,sunSetTime,sunRiseTimeDelay,sunSetTimeDelay);
								if(timeReturnValue)
								{
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
																sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),"000,000,000",ref5.at(loopNo1),"","","","","","","");
															}
															if((ar[0] != "000") || (ar[1] != "000") || (ar[2] != "000")) 
															{
																if(!ref6.at(loopNo1).empty())
																{
																	if(ref6.at(loopNo1) == "0")
																	{
																		sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ar[3],ref5.at(loopNo1),"","","","","","","");
																	}
																	else
																	{
																		thread1[loopNo1] = thread(timerSecs,stoi(ref6.at(loopNo1)),category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ar[3],ref5.at(loopNo1),"","","");
																	}
																}
																else
																{
																	sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ar[3],ref5.at(loopNo1),"","","","","","","");
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
																	sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","","","","","");
																}
																else
																{
																	thread1[loopNo1] = thread(timerSecs,stoi(ref6.at(loopNo1)),category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","");
																}
															}
															else
															{
																sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","","","","","");
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
															sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","","","","","");
														}
														else
														{
															thread1[loopNo1] = thread(timerSecs,stoi(ref6.at(loopNo1)),category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","");
														}
													}
													else
													{
														sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref5.at(loopNo1),"","","","","","","");
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
															sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),"","","","","","","","");
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
														sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),"","","","","","","","");
												}
											}
										}
										else if(category.at(loopNo1) == "Curtain")
										{
											if(isInt(ref3.at(loopNo1),10))
											{
												if((stoi(ref3.at(loopNo1)) >=0) && (stoi(ref3.at(loopNo1)) <=100))
													sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),"","","","","","","","");
											}
										}
									}
								}
							}
							else
							{
								#ifdef DEBUG
									cout << "Current time is not between (start+sunRiseDelay) and (end+sunSetDelay).\n\n";
								#endif
							}
							break;
							
						default:
								#ifdef DEBUG
									cout << "Scene Execution:Inappropriate Value in Flag.\n\n";
								#endif
							break;
					}
				}
			}
			else if((category.at(loopNo1) == "Media")&&(ref3.at(loopNo1) != "NA"))
			{
				/*-----Nuvo params, strings created for understanding-----*/
				string zoneNameNuvo,favouriteNuvo,modeNuvo,sourceNuvo,volumeNuvo,playListClimax{},statusClimax{},volumeClimax{},shuffleClimax{},deviceIdClimax{},flagForTiming,startTimeInDb,endTimeInDb,sunRiseTimeDelay,sunSetTimeDelay;
				
				t2.mysqlQuery("SELECT flag,start_time,end_time,rise_delay,set_delay FROM Scene_Execution where category = '" + category.at(loopNo1) + "' and scene_id = '" + sceneId + "' AND flag IS NOT NULL AND start_time IS NOT NULL AND end_time IS NOT NULL AND rise_delay IS NOT NULL AND set_delay IS NOT NULL;");
				MYSQL_ROW row3;
				if ((row3 = mysql_fetch_row(t2.result))) 
				{
					flagForTiming = row3[0];
					startTimeInDb = row3[1];
					endTimeInDb = row3[2];
					sunRiseTimeDelay = row3[3];
					sunSetTimeDelay = row3[4];
				}
				
				#ifdef DEBUG
					cout << "Scene Execution Parameters.\n";
					cout << "flag: 1 - always, 2 - b/w start and end time and 3 - b/w start+sunrisedelay and end+sunsetdelay.\n";
					cout << "flag\t\t\t:" << flagForTiming << ",\n";
					cout << "startTime\t\t:" << startTimeInDb << ",\n";
					cout << "endTime\t\t\t:" << endTimeInDb << ",\n";
					cout << "sunRiseDelay\t\t:" << sunRiseTimeDelay << ",\n";
					cout << "sunSetDelay\t\t:" << sunSetTimeDelay << ".\n\n";
				#endif

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
					t2.mysqlQuery("SELECT MP.playlist_id,MP.status,MP.volume,MP.shuffle,MP.device_id FROM Music AS M LEFT JOIN Music_Preference AS MP ON MP.device_id=M.id AND MP.day_week = '"+to_string(timestruct->tm_wday)+"' AND MP.scene_id = '"+sceneId+"' WHERE M.local_instance_id = '"+localInstanceId[loopNo1]+"' AND MP.playlist_id IS NOT NULL AND M.id IS NOT NULL AND MP.status IS NOT NULL AND MP.volume IS NOT NULL AND MP.shuffle IS NOT NULL AND MP.device_id IS NOT NULL;");
					MYSQL_ROW row4;
					if ((row4 = mysql_fetch_row(t2.result)))
					{
						playListClimax = row4[0];
						statusClimax = row4[1];
						volumeClimax = row4[2];
						shuffleClimax = row4[3];
						deviceIdClimax = row4[4];
					}
				}
				
				#ifdef DEBUG
					cout << "Device id =" << deviceIdClimax << '\n';
					cout << "Playlist id =" << playListClimax << '\n'; 
					cout << "status (1-on/2-off)  =" << statusClimax << '\n';
					cout << "Volume =" << volumeClimax << '\n';
					cout << "shuffle(0-off,1-On) =" << shuffleClimax << ".\n\n";
				#endif
				if((!flagForTiming.empty())&&(flagForTiming =="1"))
				{
					/*-----Only always flag for these devices.-----*/
					if(integrationId.at(loopNo1) == "10000")
					{
						if(statusClimax == "1")//ON
						{
							sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),deviceIdClimax,"3","play_playlist",playListClimax,"","","","","","");
							sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),deviceIdClimax,"1","volume",volumeClimax,"","","","","","");
							if(shuffleClimax == "0")
								sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),deviceIdClimax,"1","shuffle_off","","","","","","","");
							else if(shuffleClimax == "1")
								sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),deviceIdClimax,"1","shuffle_on","","","","","","","");	
						}
						else if(statusClimax == "2")//OFF
						{
							sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),deviceIdClimax,"1","stop","","","","","","","");
						}
					}
					else if((integrationId.at(loopNo1) == "10001")||(integrationId.at(loopNo1) == "10002"))
					{	
						if(modeNuvo == "1")//ON
						{
							//ON
							sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),zoneNameNuvo,"1","ON","","","","","","","");
							
							//source select
							//sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),zoneNameNuvo,"1","shuffle_off","","","","","","","");
							
							//play_playlist
							sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),zoneNameNuvo,"3","play_playlist",favouriteNuvo,"","","","","","");
							
							//volume
							sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),zoneNameNuvo,"1","volume",volumeNuvo,"","","","","","");
						}
						else if(modeNuvo == "2")//OFF
						{
							sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),zoneNameNuvo,"1","OFF","","","","","","","");
						}
					}
				}
			}
			
			else if((category.at(loopNo1) == "IR")&&(ref3.at(loopNo1) != "NA"))
			{
				string flagForTiming,deviceModelId;
				t2.mysqlQuery("SELECT q1.flag, q2.device_model_id FROM Scene_Execution AS q1 LEFT JOIN IR_Devices AS q2 ON q2.id = '"+ref2.at(loopNo1)+"' WHERE q1.category = '"+category.at(loopNo1)+"' and q1.scene_id = '"+sceneId+"' AND q1.flag IS NOT NULL AND q2.device_model_id IS NOT NULL;");
				MYSQL_ROW row3;
				if ((row3 = mysql_fetch_row(t2.result))) 
				{
					flagForTiming = row3[0];
				}
				#ifdef DEBUG
					cout << "Scene Execution Parameters.\n";
					cout << "flag: 1 - always, 2 - b/w start and end time and 3 - b/w start+sunrisedelay and end+sunsetdelay.\n";
					cout << "flag\t\t\t:" << flagForTiming << ",\n";
				#endif
				deviceModelId=integrationId.at(loopNo1);
				if((!flagForTiming.empty())&&(flagForTiming == "1"))
				{
					/*-----Only always flag for these devices.-----*/
					if(ref3.at(loopNo1) == "OC")
						sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate("Channels",userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref5.at(loopNo1),"","","","","","","","");
					else
					{
						sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),"","","","","","","","");
										
						if(!ref4.at(loopNo1).empty() && !ref5.at(loopNo1).empty() && (ref3.at(loopNo1) != "2" ))
						{
							if(isInt(ref4.at(loopNo1),10))
							{
								if(deviceModelId == "33")
								{
									thread1[loopNo1] = thread(timerSecs,stoi(ref4.at(loopNo1)),"Channels",userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref5.at(loopNo1),"","","","");
								}
								else
								{
									thread1[loopNo1] = thread(timerSecs,stoi(ref4.at(loopNo1)),category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref5.at(loopNo1),"","","","");
								}
							}
						}
					}
				}
			}
			else if((category.at(loopNo1) == "Appliances")&&(ref3.at(loopNo1) != "NA"))
			{
				string flagForTiming;
				t2.mysqlQuery("SELECT flag FROM Scene_Execution where category = '" + category.at(loopNo1) + "' AND scene_id = '" + sceneId + "' AND flag IS NOT NULL;");
				MYSQL_ROW row3;
				if ((row3 = mysql_fetch_row(t2.result)))
				{
					flagForTiming = row3[0];
				}
				#ifdef DEBUG
					cout << "Scene Execution Parameters.\n";
					cout << "flag: 1 - always, 2 - b/w start and end time and 3 - b/w start+sunrisedelay and end+sunsetdelay.\n";
					cout << "flag\t\t\t:" << flagForTiming << ",\n";
				#endif
				if((!flagForTiming.empty())&&(flagForTiming == "1"))
				{
					/*-----Only always flag for these devices.-----*/
					if(!ref4.at(loopNo1).empty())
					{
						if(isInt(ref4.at(loopNo1),10))
						{
							sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),ref3.at(loopNo1),ref4.at(loopNo1),"","","","","","","");
						}
					}
				}
			}
			else if((category.at(loopNo1) == "Comfort")&&(ref3.at(loopNo1) != "NA"))
			{
				string flagForTiming;
				t2.mysqlQuery("SELECT flag FROM Scene_Execution where category = '" + category.at(loopNo1) + "' and scene_id = '" + sceneId + "' AND flag IS NOT NULL;");
				MYSQL_ROW row3;
				#ifdef DEBUG
					cout << "Scene Execution Parameters.\n";
					cout << "flag: 1 - always, 2 - b/w start and end time and 3 - b/w start+sunrisedelay and end+sunsetdelay.\n";
					cout << "flag\t\t\t:" << flagForTiming << ",\n";
				#endif
				if ((row3 = mysql_fetch_row(t2.result)))
				{
					flagForTiming = row3[0];
				}
				if((!flagForTiming.empty())&&(flagForTiming == "1"))
				{
					/*-----Only always flag for these devices.-----*/
					sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),"Mode",ref3.at(loopNo1),"","","","","","","");
					if(!ref4.at(loopNo1).empty() && !ref4.at(loopNo1).empty() && !ref5.at(loopNo1).empty() && (ref3.at(loopNo1) != "2" ))
					{
						thread1[loopNo1] = thread(timerSecs,stoi(ref4.at(loopNo1)),category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),"Setpoint","",ref5.at(loopNo1),"","");
					}
				}
			}
		}
		#ifdef DEBUG
			cout << "Bulk Json:\n" << sceneJson << ".\n\n";
		#endif
		if(!sceneJson.empty())
			s1.socketSend(sceneJson);
		for(loopNo1=0;loopNo1 < category.size();loopNo1++) 
		{
			if((category.at(loopNo1) == "Light")||(category.at(loopNo1) == "Curtain")||(category.at(loopNo1) == "Color Light"))
			{
				t2.mysqlQuery("SELECT flag,start_time,end_time,rise_delay,set_delay FROM Scene_Execution where category = '" + category.at(loopNo1) + "' and scene_id = '" + sceneId + "' AND flag IS NOT NULL AND start_time IS NOT NULL AND end_time IS NOT NULL AND rise_delay IS NOT NULL AND set_delay IS NOT NULL;");
				string flagForTiming,startTimeInDb,endTimeInDb,sunRiseTimeDelay,sunSetTimeDelay;
				MYSQL_ROW row3;
				if ((row3 = mysql_fetch_row(t2.result))) 
				{
					flagForTiming = row3[0];
					startTimeInDb = row3[1];
					endTimeInDb = row3[2];
					sunRiseTimeDelay = row3[3];
					sunSetTimeDelay = row3[4];
				}
				if(!flagForTiming.empty())
				{
					switch(stoi(flagForTiming))
					{
						case 1:
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
							break;
						
						case 2:
							{
								/*-----Timed, executes only when current time is between start and end time-----*/
								if(!startTimeInDb.empty() && !endTimeInDb.empty())
								{
									bool timeReturnValue = timeCheck(startTimeInDb,endTimeInDb,"","");
									if(timeReturnValue)
									{
										/*-----time is between start time and end time-----*/
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
													if(ref6.at(loopNo1) != "0")
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
									else
									{
										/*-----time is not between start time and end time-----*/
										#ifdef DEBUG
											cout << "Time is not between start time and end time" << "\n";
										#endif	
									}
								}
							}
							break;
						
						case 3:	
							if(!sunRiseTime.empty() && !sunSetTime.empty() && !sunRiseTimeDelay.empty() && !sunSetTimeDelay.empty())
							{
								/*-----Timed, executes only when current time is between sun rise time and sun set time-----*/
								bool timeReturnValue = timeCheck(sunRiseTime,sunSetTime,sunRiseTimeDelay,sunSetTimeDelay);
								if(timeReturnValue)
								{
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
																sceneJson = sceneJson + (sceneJson.empty()?"":"\r\n") + s1.jsonCreate(category.at(loopNo1),userId,localInstanceId.at(loopNo1),integrationId.at(loopNo1),ref2.at(loopNo1),"000,000,000",ref5.at(loopNo1),"","","","","","","");
															}
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
							else
							{
								#ifdef DEBUG
									cout << "Current time is between sun rise time and sun set time" << "\n";
								#endif
							}
							break;
							
						default:
								#ifdef DEBUG
									cout << "Inappropriate Value in Scene Execution." << "\n";
								#endif
							break;
					}
				}
			}
						
			else if((category.at(loopNo1) == "IR")&&(ref3.at(loopNo1) != "NA"))
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
			else if((category.at(loopNo1) == "Comfort")&&(ref3.at(loopNo1) != "NA"))
			{
				if(!ref2.at(loopNo1).empty() && !ref4.at(loopNo1).empty() && !ref5.at(loopNo1).empty() && (ref3.at(loopNo1) != "2" ))
				{
					thread1[loopNo1].join();
				}
			}
		}
		
		#ifdef DEBUG
			cout<<"Scene finish Json:"<<s1.jsonCreate("Scene",userId,roomId,"",sceneId,"1","","","","","","","","")<<".\n";
		#endif
		s1.socketSendSingleJson(s1.jsonCreate("Scene",userId,roomId,"",sceneId,"1","","","","","","","",""));
	}
	
	return 0;
}

/*******************************************************************************
 * Member Function of Class scene checks the database.						   *
 *******************************************************************************/
int scene::dbCheck(string param1,string param2)
{
	string res1,res2;
	int returnValue=1;
	MYSQL_ROW row1,row2;
	t2.mysqlQuery("SELECT 1 FROM Scene where id='"+param1+"';");
	if((row1 = mysql_fetch_row(t2.result)))
		res1 = row1[0];
			
	t2.mysqlQuery("SELECT 1 FROM C_Mymilan_Users where mymilan_user_id='"+param2+"';");
	if((row2 = mysql_fetch_row(t2.result)))
		res2 = row2[0];
	
	if((res1 == "1")&&(res2 =="1"))
		returnValue = 1;
	else if((res1.empty())&&(res2.empty()))
		returnValue = 2;
	else if((stoi(res1) > 1)&&(stoi(res2)>1))
		returnValue = 3;
	else
		returnValue = 4;
		
	return returnValue;
}

/*******************************************************************************
 * Member Function of Class scene checks the Time, whether it is between 	   *
 * startTimeInDb and endTimeInDb with or without sunRiseDelay and sunSetDelay  *
 * included.																   *
 *******************************************************************************/
bool scene::timeCheck(string startTimeInDb,string endTimeInDb,string sunRiseDelay,string sunSetDelay)
{
	if(!startTimeInDb.empty() && !endTimeInDb.empty() && !sunRiseDelay.empty() && !sunSetDelay.empty())
	{
		/*-----time_t is a type to store time values-----*/
		time_t Currenttime,startTimeCheck,endTimeCheck;
		
		/*-----struct tm is a structure to store time values and which can be modified-----*/
		struct tm * startTime={0};
		struct tm * endTime={0};
		
		/*-----variables required for time operation-----*/
		string startHr,endHr,startMin,endMin,sunRiseHr,sunRiseMin,sunSetHr,sunSetMin,sign1,sign2;
		long int startTimeGap,endTimeGap;
		bool returnValue;
		
		/*-----hours and mins extracted from function params,startTimeInDb and endTimeInDb-----*/
		startHr = startTimeInDb.substr(startTimeInDb.find(":") - 2,startTimeInDb.find(":"));
		startMin = startTimeInDb.substr(startTimeInDb.find(":")+1,startTimeInDb.find(":") + 2);
		
		endHr = endTimeInDb.substr(endTimeInDb.find(":") - 2,endTimeInDb.find(":"));
		endMin = endTimeInDb.substr(endTimeInDb.find(":")+1,endTimeInDb.find(":") + 2);
		
		/*-----checks for Sun rise delay and sun set delay is given in function params or empty-----*/
		if(!sunRiseDelay.empty() && !sunSetDelay.empty())
		{
			/*-----if "-" sign present, then start time - sun rise delay-----*/
			int q2 = sunRiseDelay.find("-");
			if(q2 >= 0)
			{
				sign1 = sunRiseDelay.substr(sunRiseDelay.find("-"),sunRiseDelay.find("-")+1);
			}
			else
			{
				/*-----otherwise , then start time + sun rise delay-----*/
				sign1 = sunRiseDelay.substr(sunRiseDelay.find("+"),sunRiseDelay.find("+")+1);
			}
			
			/*-----sun rise hr and min calculated  here.-----*/
			sunRiseHr = sunRiseDelay.substr(sunRiseDelay.find(":") - 2,sunRiseDelay.find(":"));
			sunRiseMin = sunRiseDelay.substr(sunRiseDelay.find(":")+1,sunRiseDelay.find(":") + 2);
			
			/*-----if "-" sign present, then start time - sun rise delay-----*/
			q2 = sunSetDelay.find("-");
			if(q2 >= 0)
			{
				sign2 = sunSetDelay.substr(sunSetDelay.find("-"),sunSetDelay.find("-")+1);
			}
			else
			{
				/*-----otherwise , then start time + sun rise delay-----*/
				sign2 = sunSetDelay.substr(sunSetDelay.find("+"),sunSetDelay.find("+")+1);
			}
			
			/*-----sun set hr and min calculated  here.-----*/
			sunSetHr = sunSetDelay.substr(sunSetDelay.find(":") - 2,sunSetDelay.find(":"));
			sunSetMin = sunSetDelay.substr(sunSetDelay.find(":")+1,sunSetDelay.find(":") + 2);
		}
			
		/*-----getting the current time-----*/
		time ( &Currenttime );

		/*-----getting the start time string in time format-----*/
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
		
		/*-----start time is constucted in time_t format-----*/
		startTimeCheck = mktime(startTime);
		
		//cout<<"Start Time "<<ctime(&startTimeCheck)<<"<br>\n";
		
		/*-----getting the start time gap, difference in time between start time and current time-----*/
		startTimeGap = difftime(startTimeCheck,Currenttime);
		
		/*-----getting the end time string in time format-----*/
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
		
		/*-----end time is constucted in time_t format-----*/
		endTimeCheck = mktime(endTime);
			
		/*-----getting the end time gap, difference in time between end time and current time-----*/
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
			cout << "ReturnValue (Time comparison) -> " << returnValue << "\n";
		#endif
		
		/*-----Time status is updated in returnValue-----*/
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
		/*-----Declaration-----*/
		Cgicc cgi;
				
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
		
		//userId = "RID-542-24636973855539201";
		//sceneId = "24841326453325924";
		//Example query:http://localhost/Milan/Drivers/MILAN_EXE/SCENE/scene.cgi?user_id=15882735709066137&room_id=15882735709066151&scene_id=15882735709066152
		if(!userId.empty()  && !sceneId.empty())
		{
			#ifdef DEBUG
				cout << "user id\t\t\t:" << userId << ",\n";
				cout << "Scene id\t\t:" << sceneId << ".\n\n";
			#endif
			
			scene t1(userId,sceneId);
			int dbStatus{};
			
			/*-----DB check for the scene id-----*/
			dbStatus = t1.dbCheck(sceneId,userId); //"SELECT id FROM Scene WHERE id=;"
			
			#ifdef DEBUG
				cout<<"dbStatus\t\t:"<<dbStatus<<",\n";
			#endif
			
			switch(dbStatus)
			{
				case 1:
					t1.sceneExecution();
					break;
				
				default:
					{
						#ifdef DEBUG
							cout<<"Invalid User Id and Scene Id combination.\n";
						#endif
					}
					break;
			}
		}
		else
		{
			if(userId.empty())
			{
				#ifdef DEBUG
					cout << "User id is empty.\n\n";
				#endif
				throw 1;
			}
			
			if(sceneId.empty())
			{
				#ifdef DEBUG
					cout << "Scene id is empty.\n\n";
				#endif
				throw 1;
			}
		}
		#ifdef DEBUG
			cout << "Scene Execution finished successfully.\n\n";
		#endif
	}
	catch(exception& e) 
	{
		/*-----handle any errors - omitted for brevity-----*/
		#ifdef DEBUG
			cout<<"Exception handled, Scene Execution terminated, Please Check for errors.";
		#endif
	}
}
