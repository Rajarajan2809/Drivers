/*******************************************************************************
 *	Purpose		:	SUn Rise / Sun set time.						   		   *
 * 	Description	:	This is the header for Main Program to calculate the Sun   *
 * 					rise and Sun set time.									   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	12-Sep-16												   *
 * 	DOLM		:	31-Oct-16												   *
 *******************************************************************************/
 
#ifndef _sun_exec_hpp
#define _sun_exec_hpp

/*-----CPP library-----*/
#include <iostream>
#include <chrono>		/*------Used to perform time operations.-------*/
#include <vector>		/*------Replacement for arrays (vectors are dynamicallly allocated).-------*/
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <string>
#include <stdexcept>

/*-----Curl Library-----*/
#include <curl/curl.h>

/*-----C Library-----*/
#include <ctime>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cassert>

/*-----local header files-----*/
#include "../include/mysql.hpp"
#include "../include/cJSON.hpp"

using namespace std;

size_t CurlWrite_CallbackFunc_StdString(void *contents, size_t size, size_t nmemb, string *s);
string curlRequests(string webAddress,string type);

/*******************************************************************************
 * Class which sets sunrise / sunset time.	                                   *
 *******************************************************************************/
class sun
{
	public:
			/*-----Public Member Declaration-----*/
			sun();
			~sun();
			int riseSetDBUpdate();
			void timeZoneUpdate(string timeZone);
			testMysql t2,db;
			
	private:
			string srlNo,macId;
			bool fileCreateFlag;
			void logFile(string logStr);
			string fileRead(string fileLocation);
			string getTime();
			string urlEncode(const string &s);
			string urlEncode2(string s);
			string fileRead2(string fileLocation);
			string exec(const char* cmd);
};

/*******************************************************************************
 * Constructor for Class testGateway.					 					   *
 *******************************************************************************/
sun::sun():t2("Milan_SP"),db("INFOWAY")
{
	 fileCreateFlag = 0;
	db.mysqlQuery("SELECT SERIAL FROM INFOWAY;");
	MYSQL_ROW row;
	if(mysql_num_rows(db.result) > 0)
	{
		while((row = mysql_fetch_row(db.result)))
		{
			srlNo = row[0];
		}
	}
	else
		exit(0);
	
	macId = fileRead("/sys/class/net/eth0/address");
	macId = ((macId.find("\n") != string::npos) ? macId.erase(macId.find("\n"),1):macId);
	
	#ifdef DEBUG
		cout << "macId :" << macId << ",\n";
		cout << "Serial No :" << srlNo << ".\n\n";
	#endif	
	
	/*-----Progress flag is set and Stop flag is reset-----*/
	if(!ifstream(srlNo+"_enc.txt")) 
	{
		fileCreateFlag = 1;
		ofstream outfile (srlNo+"_enc.txt");
	}
	
	if(!ifstream(srlNo+"_dec.txt")) 
	{
		fileCreateFlag = 1;
		ofstream outfile (srlNo+"_dec.txt");
	}
	
	if(!ifstream(macId+"_enc.txt")) 
	{
		fileCreateFlag = 1;
		ofstream outfile (macId+"_enc.txt");
	}
	
	if(!ifstream(macId+"_dec.txt")) 
	{
		fileCreateFlag = 1;
		ofstream outfile (macId+"_dec.txt");
	}
}

/*******************************************************************************
 * Destructor for Class testGateway.			 							   *
 *******************************************************************************/
sun::~sun()
{
	/*-----Progress flag is reset and strings related to Tests are terminated-----*/
	
}

/*******************************************************************************
 * Function to get the Latitude and Logitude in JSON format.				   *
 *******************************************************************************/
int sun::riseSetDBUpdate()
{
	string latLongJson{},lat{},lng{},timeZoneJson{},timeZone{},riseSetQuery{},riseSetJson{},rise{},set{},ntpCommand{},sysRes{};
	vector<string> clientId{},Api_Key{};
	MYSQL_ROW row;
	size_t rowsReturned{};
	int randNum{},loopNo1=0;
	
	do
	{
		ntpCommand ="echo 'Cecots!*52764^Eloka' ; sudo -S service ntp stop ; sudo ntpdate -s "+to_string(loopNo1)+".ubuntu.pool.ntp.org ; sudo service ntp start";
		sysRes = exec(ntpCommand.c_str());
		#ifdef DEBUG
			cout << "ntp server result :" << sysRes << "\n\n";
		#endif
		loopNo1++;
		if(loopNo1 > 4)
			return 0;
	}
	while(sysRes.empty() && loopNo1 < 4);
	t2.mysqlQuery("SELECT Api_Key_Google FROM Api_Key;");
	if (t2.result)
	{
		rowsReturned = mysql_num_rows( t2.result );
	}
	if(rowsReturned > 0)
	{
		while((row = mysql_fetch_row(t2.result)))
		{
			Api_Key.push_back(row[0]);
		}
	}
	else
		return 1;
	srand (time(NULL));
	randNum = rand() % 100 + 20;
	#ifdef DEBUG
		cout << "Random Delay : " << randNum << ",\n";
	#endif
	
	//Random Delay
	sleep(randNum);
	
	for(size_t loopNo2=0;loopNo2<Api_Key.size();loopNo2++)
	{
		if(!exec("ping -W4 -c4 -A www.google.com").empty())
		{
			#ifdef DEBUG
				cout<<"\n\nNow, Api_Key used id :"<<Api_Key.at(loopNo2)<<"\n\n";
			#endif
			
			latLongJson = curlRequests("https://www.googleapis.com/geolocation/v1/geolocate?key="+Api_Key.at(loopNo2),"POST");
			
			#ifdef DEBUG
				cout << "latLongJson:" << latLongJson << "\n\n";
			#endif
			
			if((latLongJson.find("error") != string::npos) || (latLongJson.empty()))
			{
				//error json case
				continue;
			}
			else
			{
				//json parsing for latitude and longitude
				cJSON *json1 = cJSON_GetObjectItem(cJSON_Parse(latLongJson.c_str()),"location");
				cJSON *json2 = cJSON_GetObjectItem(json1,"lat");
				lat = to_string(json2->valuedouble);
				cJSON *json3 = cJSON_GetObjectItem(json1,"lng");
				lng = to_string(json3->valuedouble);
					
				//curl request for timezone
				timeZoneJson = curlRequests("https://maps.googleapis.com/maps/api/timezone/json?location="+lat+","+lng+"&timestamp=1458000000&key=AIzaSyAQ3REK7FupiKL8p5BrnSczXvluDPEGXZU","POST"); //works also in GET
				
				#ifdef DEBUG
					cout << "timeZoneJson:" << timeZoneJson << "\n\n";
				#endif
				
				if((timeZoneJson.find("timeZoneId") != string::npos) || (!timeZoneJson.empty()))
				{
					cJSON *json4 = cJSON_GetObjectItem(cJSON_Parse(timeZoneJson.c_str()),"timeZoneId");
					timeZone = json4->valuestring;
													
					if(timeZone != fileRead("/etc/timezone"))
					{
						string cmd = "echo '"+timeZone+"' >/etc/timezone && dpkg-reconfigure -f noninteractive tzdata && export TZ="+timeZone;
						if(!exec(cmd.c_str()).empty())
						{
							t2.mysqlQuery("SET GLOBAL time_zone='"+timeZone+"';");
							#ifdef DEBUG
								cout << "Time zone updated.\n\n";
							#endif
						}
						else
						{
							#ifdef DEBUG
								cout << "Time zone update failed.\n\n";
							#endif
						}
					}
					
					//Serial Number checking
					size_t decTry =0;
					LOOP:
					if(!macId.empty() && !srlNo.empty())
					{
						string encSrlNo{},encMacId{};
						if(fileCreateFlag == 1)
						{
							string cmd1 = "/var/www/Milan/Drivers/encrypt/RNCryptor-C-master/rn_encrypt " +macId+" "+macId+"_enc.txt";
							string cmd2 = "/var/www/Milan/Drivers/encrypt/RNCryptor-C-master/rn_encrypt "+srlNo+" "+srlNo+"_enc.txt";
								
							if(!exec(cmd1.c_str()).empty() && !exec(cmd2.c_str()).empty())
							{
								encSrlNo = fileRead(srlNo+"_enc.txt");
								encMacId = fileRead(macId+"_enc.txt");
								
								#ifdef DEBUG
									cout << "\n\n";
									cout << "encrypted macId:" << encMacId << ",\n";
									cout << "encrypted sl.no:" << encSrlNo << ".\n\n";
								#endif
														
								//serial no. decryption.
								string cmd3 = "/var/www/Milan/Drivers/encrypt/RNCryptor-C-master/rn_decrypt "+encSrlNo+" "+srlNo+"_dec.txt";
								string cmd4 = "/var/www/Milan/Drivers/encrypt/RNCryptor-C-master/rn_decrypt "+encMacId+" "+macId+"_dec.txt";
							
								if(!exec(cmd3.c_str()).empty() && !exec(cmd4.c_str()).empty())
								{
									string decSrlNo = fileRead(srlNo+"_dec.txt");
									string decMacId = fileRead(macId+"_dec.txt");
									
									#ifdef DEBUG
										cout << "\n\n";
										cout << "decrypted Serialno:" << decSrlNo << ",\n";
										cout << "decrypted macId:" << decMacId << ".\n\n";
									#endif
								
									decTry++;
									if((macId == decMacId) && (srlNo == decSrlNo))
									{
										#ifdef DEBUG
											cout << "Macid and SerialNo. Encryption success.\n\n";
										#endif
									}
									else
									{	//max no of tries decryption takesplace
										if(decTry < 10)
											goto LOOP;
										else
										{
											string fileName = srlNo+"_enc.txt";
											remove(fileName.c_str());
											fileName = srlNo+"_dec.txt";
											remove(fileName.c_str());
											fileName = macId+"_enc.txt";
											remove(fileName.c_str());
											fileName = macId+"_dec.txt";
											remove(fileName.c_str());
											return 3;
										}
									}
								}
							}
						}
						else
						{
							encSrlNo = fileRead(srlNo+"_enc.txt");
							encMacId = fileRead(macId+"_enc.txt");
							#ifdef DEBUG
								cout << "encrypted macId :" << encMacId << ",\n";
								cout << "encrypted sl.no :" << encSrlNo << ".\n\n";
							#endif
						}
						string cloudUrl = "http://milancloud.com/milan_api_v2/public/infomap/sunrise_sunset?serial_number="+urlEncode2(encSrlNo)+"&physical_address="+urlEncode2(encMacId)+"&latitude="+urlEncode2(lat)+"&longitude="+urlEncode2(lng)+"&timezone="+urlEncode2(timeZone);
						#ifdef DEBUG
							cout<<"cloud url :" << cloudUrl << ".\n\n";
						#endif
								
						riseSetJson = curlRequests(cloudUrl,"GET");
							
						#ifdef DEBUG
							cout << "riseSetJson :" << riseSetJson << ".\n\n";
						#endif
									
						if(((riseSetJson.find("sunrise") != string::npos) && (riseSetJson.find("sunset") != string::npos)) || (!riseSetJson.empty()))
						{
							cJSON *json5 = cJSON_GetObjectItem(cJSON_Parse(riseSetJson.c_str()),"sunrise");
							rise = json5->valuestring;
							cJSON *json6 = cJSON_GetObjectItem(cJSON_Parse(riseSetJson.c_str()),"sunset");
							set = json6->valuestring;
													
							t2.mysqlQuery("SELECT id FROM Client_Information;");
							if(mysql_num_rows(t2.result) > 0)
							{
								while((row = mysql_fetch_row(t2.result)))
								{
									clientId.push_back(row[0]);
								}
								for(size_t loopNo3=0;loopNo3<clientId.size();loopNo3++)
								{
									t2.mysqlQuery("UPDATE `Client_Information` SET `sunrise`='"+rise+"', `sunset`='"+set+"',`latitude`='"+lat+"',`longitude`='"+lng+"' WHERE `id`='"+clientId.at(loopNo3)+"';");
								}
							}
							else
							{
								t2.mysqlQuery("INSERT INTO `Client_Information` (`latitude`, `longitude`, `sunrise`, `sunset`) VALUES ('"+lat+"', '"+lng+"', '"+rise+"', '"+set+"');");
							}
							return 4;
						}
						else
						{
							sleep(5);
							continue;
						}
					}
					else
					{
						return 2;
					}
				}
				else
				{
					continue;
				}
			}
		}
		else
			return 0;
	}
	return 5;
}

//function to check localtime zone from linux system
string sun::fileRead(string fileLocation)
{
	// open a file in read mode.
	ifstream infile;
	string data; 
	infile.open(fileLocation.c_str()); 

	infile >> data; 
	
	// close the opened file.
	infile.close();
	
	return data;
}

void sun::timeZoneUpdate(string timeZone)
{
	#ifdef DEBUG
		cout << "Time zone : " << timeZone << "\n\n";
	#endif
	//timeZone2 = "Asia/Manila";
	if(!timeZone.empty())
	{
		string cmd = "echo "+timeZone+" > /etc/timezone && dpkg-reconfigure -f noninteractive tzdata && export TZ="+timeZone;
		#ifdef DEBUG
			cout << "cmd :	"<< cmd << "\n\n";
		#endif
		if(!exec(cmd.c_str()).empty())
		{
			#ifdef DEBUG
				cout << "Time zone update Success.\n\n";
			#endif
		}
		else
		{
			#ifdef DEBUG
				cout << "Time zone update failed.\n\n";
			#endif
		}
	}
	else
	{
		#ifdef DEBUG
			cout << "Time zone is empty.\n\n";
		#endif
	}
}

//log file creation for debugging cron functionality
void sun::logFile(string logStr)
{
	logStr = getTime() +"\n"+logStr + "\n\n";
	ofstream myfile;
	myfile.open ("error_log.txt", fstream::in | fstream::out | fstream::app);
	myfile << logStr;
	myfile.close();
}

//get time
string sun::getTime()
{
	chrono::time_point<std::chrono::system_clock> presentTimeChronoType;
	presentTimeChronoType = chrono::system_clock::now();
	time_t presentTime = chrono::system_clock::to_time_t(presentTimeChronoType);
	return ctime(&presentTime);
}

//URL encoding using discrete functions
string sun::urlEncode(const string &s)
{
    static const char lookup[]= "0123456789abcdef";
    stringstream e;
    for(int i=0, ix=s.length(); i<ix; i++)
    {
        const char& c = s[i];
        if ( (48 <= c && c <= 57) /*0-9*/ || (65 <= c && c <= 90) /*abc...xyz*/ || (97 <= c && c <= 122) /*ABC...XYZ*/ || (c=='-' || c=='_' || c=='.' || c=='~'))
        {
            e << c;
        }
        else
        {
            e << '%';
            e << lookup[ (c&0xF0)>>4 ];
            e << lookup[ (c&0x0F) ];
        }
    }
    return e.str();
}

//URL encoding using curl easy escape
string sun::urlEncode2(string encodeData)
{
	CURL *curl = curl_easy_init();
	string encodedData{};
	if(curl) 
	{
		encodedData = curl_easy_escape(curl, encodeData.c_str(), encodeData.length());
		if(!encodedData.empty()) 
		{}
	}
	return encodedData;
}

//shell command returns string
string sun::exec(const char* cmd) 
{
    char buffer[128];
    string result{};
    shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) 
		throw runtime_error("popen() failed!");
    while (!feof(pipe.get())) 
    {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

// /* get us the resource without a body! */
//curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
//deb timezone cmd -> "mysql_tzinfo_to_sql /usr/share/zoneinfo | mysql -u root mysql" and "service mysql restart"
//mysql -uuser -hhostname -PPORT -ppassword
//sudo apt-get install ntp
//service ntpd restart
#endif

