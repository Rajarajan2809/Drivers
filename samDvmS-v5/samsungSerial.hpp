/*******************************************************************************
 *	Purpose		:	Samsung DVM S AC Integration.							   *
 * 	Description	:	This program is the header for Driver Program.			   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	01-Jun-16												   *
 * 	DOLM		:	17-Dec-16												   *
 *******************************************************************************/

/*-----CPP Library-----*/
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>		/*------Used to perform time operations.-------*/
#include <sstream>      // std::stringstream

/*-----Socket API library-----*/
#include <sys/socket.h> /*-----Needed for the socket functions-----*/
#include <netdb.h>      /*-----Needed for the socket functions-----*/

/*-----C library-----*/
#include <cstring> 		/*-----Library for C string Operations-----*/

/*-----Curl library-----*/
#include <curl/curl.h>

/*-----Serial Libraries (POSIX APIs)-----*/
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/file.h>

/*-----local header files-----*/
#include "../include/mysql.hpp"
#include "../include/uriParser.hpp"

using namespace std;

static const unsigned short crc16tab[256]= {
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};
  
unsigned short crc16_ccitt(unsigned char *buf, int len)
{
	register int counter;
	register unsigned short crc = 0;
	for( counter = 0; counter < len; counter++)
		crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *(unsigned char *)buf++)&0x00FF];
	return crc;
}

class samsungSerial
{
	public:
		static bool ctrlAckRcvd,fbAckRcvd;
		static int fd;
		void serialThread();
		void webQueryThread();
		samsungSerial();
		~samsungSerial();
		friend size_t writeCallback(void *contents, size_t size, size_t nmemb, string *s);
								
	private:
		void serialRead();
		void feedbackProcessing(unsigned char myBuf[1024]);
		int readSerialHex(int localFd,unsigned char recvBuf[],size_t bufLen, long timeout);
		int writeSerialHex(int localFd,unsigned char sendBuf[],size_t bufLen);
		void indoorUnitCtrl(int localFd,unsigned char ctrlParams[]);
		void indoorUnitMtr(int localFd,unsigned char mtrParams[]);
		bool hex2Binary(unsigned char byte,int k);
		void dbStore(unsigned char ctrlParams[]);
		void curlRequest(string webQuery,string type);
};

//static variable initialisation
bool samsungSerial::ctrlAckRcvd=0;
bool samsungSerial::fbAckRcvd=0;
int samsungSerial::fd=0;

//constructor
samsungSerial::samsungSerial()
{
}	

//destructor
samsungSerial::~samsungSerial()
{	
}
//feedback part
void samsungSerial::serialRead()
{
	unsigned char serialBuffer[1024];
	if(fd >= 0)
	{
		while(1)
		{
			size_t frameLength =0,serialDataLength=0;	
			serialDataLength = readSerialHex(fd,serialBuffer, 1, 100);
			if (serialDataLength == 1)
			{
				switch(serialBuffer[0]) 
				{
					case 0x32:
					{
						//SOF found
						#ifdef DEBUG
							printf("SOF Found=%x\n",serialBuffer[0]);
						#endif
									
						//find the length;
						serialDataLength = readSerialHex(fd,serialBuffer+1, 2, 100);
						if(serialBuffer[1] == 0x00)
						{
							frameLength = serialBuffer[2];
							#ifdef DEBUG
								printf("Frame Length = %x\n",serialBuffer[2]);
							#endif
							
							//Now get the rest of the frame
							serialDataLength = readSerialHex(fd,serialBuffer+3,frameLength-1,100);
							
							//print the frame
							for(unsigned int i=0;i<serialDataLength+3;i++)
							{
								#ifdef DEBUG
									printf("The Frame is = %x\n",serialBuffer[i]);
								#endif
							}
												
							if(serialBuffer[serialDataLength+2] == 0x34)
							{
								feedbackProcessing(serialBuffer);
							}
						}
						tcdrain(fd);
					}
					break;
					
					default:
						#ifdef DEBUG
							printf("SOF not Found and data received is =%x\nERROR! Out of frame flow!!\n",serialBuffer[0]);
						#endif
						break;
				}
			}
			usleep(1000);
		}
	}	
}

//serial read part
int samsungSerial::readSerialHex(int localFd,unsigned char recvBuf[],size_t bufLen,long timeout)
{
	int bytes = 0;
	size_t total = 0;
	struct timeval tv;
	fd_set fs;

	while (total < bufLen) 
	{
		FD_ZERO (&fs);
		FD_SET (localFd,&fs);
		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		bytes = select (FD_SETSIZE,&fs,NULL,NULL,&tv);

		// 0 bytes or error
		if( bytes <= 0 )
		{
			return total;
		}

		bytes = read (localFd,recvBuf+total,bufLen-total);
		total += bytes;
	}
	tcdrain(localFd);
	return total;
}

//write the buffer
int samsungSerial::writeSerialHex(int localFd,unsigned char sendBuf[],size_t bufLen)
{
	int write_fd = write(localFd,sendBuf,bufLen);
	tcdrain(localFd);
	return write_fd;
}

//feedback processing part
void samsungSerial::feedbackProcessing(unsigned char serialBuffer[1024])
{
	testMysql db("Milan_SP");
	//Indoor unit id check
	if((serialBuffer[3] == 0x20)&&(serialBuffer[6] == 0x6a)&&(serialBuffer[7] == 0xee)&&(serialBuffer[8] == 0xff))
	{
		//control feedback received
		//32 00 0E 20 00 00 6A EE FF C0 16 00 00 81 29 34
		if((serialBuffer[9] == 0xC0)&&(serialBuffer[10] == 0x16)&&(serialBuffer[11] == 0x00))
		{
			if(serialBuffer[12] == 0x00)//dataset check
			{
				//CRC
				unsigned short crc;
				crc = crc16_ccitt(serialBuffer+3, 10);//12-3+1
				if((serialBuffer[13] == crc >> 8)&&(serialBuffer[14] == (crc & 0xFF)))
				{
					#ifdef DEBUG
						cout<<"CRC check success.\n\n";
					#endif
					ctrlAckRcvd = 1;
				}
				else
				{
					#ifdef DEBUG
						cout<<"CRC check failed.\n\n";
					#endif
				}
			}
		}
		
		//monitor feedback received
		//32 00 2F 20 00 00 6A EE FF C0 15 00 09 40 00 00 40 01 00 40 06 00 40 11 00 40 27 00 42 01 00 F0 42 03 00 F4 02 02 00 65 04 09 00 00 00 00 7F F5 34
		else if((serialBuffer[9] == 0xC0)&&(serialBuffer[10] == 0x15)&&(serialBuffer[11] == 0x00)&&(serialBuffer[12] == 0x09))
		{
			unsigned short crc;
			fbAckRcvd =1;
			crc = crc16_ccitt(serialBuffer+3, 43);//45-3+1
			#ifdef DEBUG
				printf("crc1:%x, ",crc >> 8);
				printf("crc2:%x.\n\n",(crc & 0xFF));
			#endif
			
			if((serialBuffer[46] == crc >> 8)&&(serialBuffer[47] == (crc & 0xFF)))
			{
				string nodeId = ((serialBuffer[4]>9)?to_string(serialBuffer[4]):"0"+to_string(serialBuffer[4])) + ((serialBuffer[5]>9)?to_string(serialBuffer[5]):"0"+to_string(serialBuffer[5]));
				//operation ON/OFF check
				if((serialBuffer[13] == 0x40)&&(serialBuffer[14] == 0x00)&&((serialBuffer[15] >=0)||(serialBuffer[15] < 2)))
				{
					//operation mode check
					if((serialBuffer[16] == 0x40)&&(serialBuffer[17] == 0x01) && ((serialBuffer[18] >=0)||(serialBuffer[18] < 5)))
					{
						//air flow check
						if((serialBuffer[19] == 0x40) && (serialBuffer[20] == 0x06) && ((serialBuffer[21] >=0)||(serialBuffer[21] < 4)))
						{
							string fanMode{},swing{};
							switch(serialBuffer[21])
							{
								//86 Fan_Auto
								//83 Fan_Low
								//85 Fan_High
								//87 Fan_Medium
								case 0://auto
									{
										fanMode = "86";
										#ifdef DEBUG
											cout << "wind speed:auto\n";
											cout << "fan_mode=" << fanMode << '\n';
										#endif
									}
									break;
									
								case 1://low
									{
										fanMode = "83";
										#ifdef DEBUG
											cout << "wind speed:low\n";
											cout << "fan_mode=" << fanMode << '\n';
										#endif
									}
									break;
									
								case 2://medium
									{
										fanMode = "87";
										#ifdef DEBUG
											cout << "wind speed:medium\n";
											cout << "fan_mode=" << fanMode << '\n';
										#endif
									}
									break;
									
								case 3://high
									{
										fanMode = "85";
										#ifdef DEBUG
											cout << "wind speed:high\n";
											cout << "fan_mode=" << fanMode << '\n';
										#endif
									}
							}
						
							//swing check
							if((serialBuffer[22] == 0x40)&&(serialBuffer[23] == 0x11)&&((serialBuffer[24] == 0)||(serialBuffer[24] == 1)))
							{
								if(serialBuffer[24] == 0)
								{
									swing = "46";
									#ifdef DEBUG
										cout << "swing:on\n";
										cout << "swing=" << swing << '\n';
									#endif
								}
								else if(serialBuffer[24] == 1)
								{
									swing = "45";
									#ifdef DEBUG
										cout << "swing:off\n";
										cout << "swing=" << swing << '\n';
									#endif
								}
							
								//Filter alarm status check
								if((serialBuffer[25] == 0x40)&&(serialBuffer[26] == 0x27)&&((serialBuffer[27] == 0)||(serialBuffer[27] == 1)))
								{
									//desired temperature check
									if((serialBuffer[28] == 0x42)&&(serialBuffer[29] == 0x01))
									{
										unsigned int desTemp = (serialBuffer[30] == 1)? (256+serialBuffer[31])/10 : serialBuffer[31]/10;
										#ifdef DEBUG
											printf("Set Temp:%d,\n",desTemp);
										#endif
										if((desTemp >=18)&&(desTemp <= 30))
										{
											//Room temperature check
											if((serialBuffer[32] == 0x42)&&(serialBuffer[33] == 0x03))
											{
												unsigned int currentTemp = (serialBuffer[34] == 1)? (256+serialBuffer[35])/10 : serialBuffer[35]/10;
												#ifdef DEBUG
													printf("Current Temperature:%d,\n",currentTemp);
												#endif
												if((currentTemp >=18) && (currentTemp <= 30))
												{
													if(serialBuffer[15] == 0x00)//Operation ON/OFF
													{
														#ifdef DEBUG
															cout << "mode:off\n";
															cout << "UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006') AS T2 SET T1.fan_mode='"+fanMode+"',T1.swing_status='"+swing+"',T1.mode='2',T1.misc_status='2',T1.current_temp='"+to_string(currentTemp)+"', T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n";
														#endif
														db.mysqlQuery("UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006') AS T2 SET T1.fan_mode='"+fanMode+"',T1.swing_status='"+swing+"',T1.mode='2',T1.misc_status='2',T1.current_temp='"+to_string(currentTemp)+"',T1.auto_count = T1.auto_count + 1,T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n");
													}
													else
													{
														string mode{};
														switch(serialBuffer[18])//mode
														{
															case 0x00:
																{
																	mode = "50";
																	#ifdef DEBUG
																		cout << "mode:auto\n";
																		cout << "mode=" << mode << '\n';
																	#endif
																}
															break;
														
															case 0x01:
																{
																	mode = "47";
																	#ifdef DEBUG
																		cout << "mode:cool\n";
																		cout << "mode=" << mode << '\n';
																	#endif
																}
															break;
															
															case 0x02:
																{
																	mode = "48";
																	#ifdef DEBUG
																		cout << "mode:dry";
																		cout << "mode=" << mode << '\n';
																	#endif
																}
															break;
														
															case 0x03:
																{
																	mode = "378";
																	#ifdef DEBUG
																		cout << "mode:fan(ventillation)";
																		cout << "mode=" << mode << '\n';
																	#endif
																}
															break;
														
															case 0x04:
																{
																	mode = "49";
																	#ifdef DEBUG				
																		cout << "mode:heat\n";
																		cout << "mode=" << mode << '\n';
																	#endif
																}
															break;
														}
														#ifdef DEBUG
															cout << "UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006') AS T2 SET T1.fan_mode='"+fanMode+"',T1.swing_status='"+swing+"',T1.mode='"+mode+"',T1.current_temp='"+to_string(currentTemp)+"',T1.misc_status='1',T1.auto_count = T1.auto_count + 1,T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n";
														#endif
														db.mysqlQuery("UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006') AS T2 SET T1.fan_mode='"+fanMode+"',T1.swing_status='"+swing+"',T1.mode='"+mode+"',T1.current_temp='"+to_string(currentTemp)+"',T1.misc_status='1',T1.auto_count = T1.auto_count + 1,T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n");
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
				//curl request to get local_inst_id for feedback
				MYSQL_ROW row4;
				db.mysqlQuery("SELECT local_instance_id FROM Climate_Status WHERE id=(SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006');");
				string locInstId{};
				while(((row4 = mysql_fetch_row(db.result))))
				{
					locInstId = row4[0];
				}
				curlRequest("http://localhost:6161/Milan/Drivers/MILAN_EXE/CLIMATE/climate_feed.cgi?l_instid="+locInstId,"GET");
			}
			else
			{
				#ifdef DEBUG
					cout<<"CRC check failed.\n\n";
				#endif
			}
		}
	}
}

//friend function
size_t writeCallback(void *contents, size_t size, size_t nmemb, string *s)
{
    size_t newLength = size*nmemb;
    size_t oldLength = s->size();
    try
    {
        s->resize(oldLength + newLength);
    }
    catch(bad_alloc &e)
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
void samsungSerial::curlRequest(string webQuery,string type)
{
	#ifdef DEBUG
		cout << "webQuery:" << webQuery << ",\n";
		cout << "Query type:" << type << ",\n";
	#endif
	CURL *curl;
    CURLcode res;
	string curlResponse{};
    curl_global_init(CURL_GLOBAL_DEFAULT);
	
    curl = curl_easy_init();
    
    if(curl)
    {
		curl_easy_setopt(curl, CURLOPT_URL, webQuery.c_str());
		if(type == "POST")
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlResponse);
        
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
        {
			#ifdef DEBUG
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			#endif
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    #ifdef DEBUG
		cout << "curlResponse:" << curlResponse << ".\n\n";
	#endif
}

//control command sent
void samsungSerial::indoorUnitCtrl(int localFd,unsigned char ctrlParams[])
{
	unsigned char myBuf[100];
		
	myBuf[0] = 0x32;
	myBuf[1] = 0x00;
	myBuf[2] = 0x32;
	
	//Third party address 6A EE FF
	myBuf[3] = 0x6A;
	myBuf[4] = 0xEE;
	myBuf[5] = 0xFF;
	
	//All devices F1,F2 broadcasting
	myBuf[6] = 0x20;
	myBuf[7] = ctrlParams[0];//out door id
	myBuf[8] = ctrlParams[1];//indoor id
	
	//Command
	myBuf[9] = 0xC0;
	myBuf[10] = 0x13;
	myBuf[11] = 0x00;
	
	//No of Data sets
	myBuf[12] = 0x0A;
	
	//Dataset 1
	myBuf[13] = 0x40;
	myBuf[14] = 0x50;
	myBuf[15] = ctrlParams[2];//0x00;//buzzer sound ON
	
	//Dataset 2
	myBuf[16] = 0x40;
	myBuf[17] = 0x00;
	myBuf[18] = ctrlParams[3];//0x00;//operation ON/OFF
	
	//Dataset 3
	myBuf[19] = 0x40;
	myBuf[20] = 0x01;
	switch(ctrlParams[4])
	{
		case 50://auto
			{
				myBuf[21] = 0x00;
			}
			break;
		
		case 47://cool
			{
				myBuf[21] = 0x01;
			}
			break;
			
		case 48://dry/dehumidification
			{
				myBuf[21] = 0x02;
			}
			break;
			
		case 78://fan/ventillation
			{
				myBuf[21] = 0x03;
			}
			break;
			
		case 49://heat
			{
				myBuf[21] = 0x04;
			}
			break;
	}
				
	//Dataset 4
	myBuf[22] = 0x40;
	myBuf[23] = 0x06;
	
	//air flow
	switch(ctrlParams[5])
	{
		case 83://auto
			{
				myBuf[24] = 0x00;//0x00;//air flow auto
			}
			break;
			
		case 85://low
			{
				myBuf[24] = 0x01;//0x00;//air flow low
			}
			break;
			
		case 86://medium
			{
				myBuf[24] = 0x02;//0x00;//air flow medium
			}
			break;
			
		case 87://high
			{
				myBuf[24] = 0x03;//0x00;//air flow high
			}
	}
				
	//Dataset 5
	myBuf[25] = 0x40;
	myBuf[26] = 0x11;
	
	//swing type
	switch(ctrlParams[6])
	{
		case 45://swing off
			{
				myBuf[27] = 0x00;
			}
			break;
			
					
		case 46://swing on
			{
				myBuf[27] = 0x01;
			}
			break;
	}
		
	//Dataset 6
	myBuf[28] = 0x40;
	myBuf[29] = 0x25;
	myBuf[30] = ctrlParams[7];//0x00;//FILTER ALARM NO RESET
	
	//Dataset 7
	myBuf[31] = 0x42;
	myBuf[32] = 0x01;
	myBuf[33] = ctrlParams[8];
	myBuf[34] = ctrlParams[9];//0xB4;//desired temperature on 24C
	
	//Dataset 8
	myBuf[35] = 0x42;
	myBuf[36] = 0x2A;
	myBuf[37] = ctrlParams[10];//0x00;
	myBuf[38] = ctrlParams[11];
	
	//dataset 9
	myBuf[39] = 0x42;
	myBuf[40] = 0x2B;
	myBuf[41] = ctrlParams[12];//0x01;
	myBuf[42] = ctrlParams[13];//0x2C;
	
	//Dataset 10 
	myBuf[43] = 0x04;
	myBuf[44] = 0x09;
	myBuf[45] = 0xff;
	myBuf[46] = 0xff;
	myBuf[47] = ctrlParams[14];//0x00;//remote controller no restriction
	myBuf[48] = ctrlParams[14];//0x00;//remote controller no restriction
			
	//CRC
	unsigned short crc;
	crc = crc16_ccitt(myBuf+3, 46);//48-3+1
	
	myBuf[49] = crc >> 8;
	myBuf[50] = crc & 0xFF;
	myBuf[51] = 0x34;
	
	if(localFd >= 0)
	{
		writeSerialHex(localFd,myBuf,sizeof(myBuf));
		#ifdef DEBUG
			cout << "Ctrl frame sent:";
			for(size_t loopNo1=0; loopNo1 < 17;loopNo1++)
			{
				printf("%x ",myBuf[loopNo1]);
			}
			cout << "\n\n";
		#endif
	}
}

//db store function
void samsungSerial::dbStore(unsigned char ctrlParams[])
{	
	testMysql db("Milan_SP");
	string nodeId = ((ctrlParams[0]>9)?to_string(ctrlParams[0]):"0"+to_string(ctrlParams[0])) + ((ctrlParams[1]>9)?to_string(ctrlParams[1]):"0"+to_string(ctrlParams[1]));
		
	string temp = to_string(((ctrlParams[8])*255 + ctrlParams[9])/10);
	
	//property & propertyid
	//off -2, cool - 47, heat - 49, auto - 50, dry - 48 and dry air(ventilation) - 378
	//mode	
	if((ctrlParams[3] == 0) || (ctrlParams[4] == 2))
	{
		#ifdef DEBUG
			cout << "UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[5])+"',T1.swing_status = '"+to_string(ctrlParams[6])+"',T1.mode='2',T1.misc_status='2',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n";
		#endif
		db.mysqlQuery("UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[5])+"',T1.swing_status = '"+to_string(ctrlParams[6])+"',T1.mode='2',T1.misc_status='2',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;");
	}
	else
	{
		switch(ctrlParams[4])
		{
			case 50://auto
				{
					#ifdef DEBUG
						cout << "UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[5])+"',T1.swing_status = '"+to_string(ctrlParams[6])+"',T1.mode='50',auto_changeover_setpoint='"+temp+"',T1.misc_status='1',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n";
					#endif
					db.mysqlQuery("UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[5])+"',T1.swing_status = '"+to_string(ctrlParams[6])+"',T1.mode='50',auto_changeover_setpoint='"+temp+"',T1.misc_status='1',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;");
				}
				break;
			
			case 47://cool
				{
					#ifdef DEBUG
						cout << "UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[5])+"',T1.swing_status = '"+to_string(ctrlParams[6])+"',T1.mode='47',T1.cool_setpoint='"+temp+"',T1.misc_status='1',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n";
					#endif
					db.mysqlQuery("UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[5])+"',T1.swing_status = '"+to_string(ctrlParams[6])+"',T1.mode='47',T1.cool_setpoint='"+temp+"',T1.misc_status='1',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;");
				}
				break;
				
			case 48://dry/dehumidification
				{
					#ifdef DEBUG
						cout << "UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[5])+"',T1.swing_status = '"+to_string(ctrlParams[6])+"',T1.mode='48',T1.dry_setpoint='"+temp+"',T1.misc_status='1',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n";
					#endif
					db.mysqlQuery("UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[5])+"',T1.swing_status = '"+to_string(ctrlParams[6])+"',T1.mode='48',T1.dry_setpoint='"+temp+"',T1.misc_status='1',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;");
				}
				break;
				
			case 78://fan/ventillation
				{
					#ifdef DEBUG
						cout << "UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[5])+"',T1.swing_status = '"+to_string(ctrlParams[6])+"',T1.mode='378',T1.misc_status='1',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n";
					#endif
					db.mysqlQuery("UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[5])+"',T1.swing_status = '"+to_string(ctrlParams[6])+"',T1.mode='378',T1.misc_status='1',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;");
				}
				break;
				
			case 4://heat
				{
					#ifdef DEBUG
						cout << "UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[5])+"',T1.swing_status = '"+to_string(ctrlParams[6])+"',T1.mode='49',T1.heat_setpoint='"+temp+"',T1.misc_status='1',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n";
					#endif
					db.mysqlQuery("UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110006') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[5])+"',T1.swing_status = '"+to_string(ctrlParams[6])+"',T1.mode='49',T1.heat_setpoint='"+temp+"',T1.misc_status='1',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;");
				}
				break;
		}
	}
}

//monitor command sent
void samsungSerial::indoorUnitMtr(int localFd,unsigned char mtrParams[])
{
	unsigned char myBuf[100];
	myBuf[0] = 0x32;
	myBuf[1] = 0x00;
	myBuf[2] = 0x2F;
	
	//Third party address 6A EE FF
	myBuf[3] = 0x6A;
	myBuf[4] = 0xEE;
	myBuf[5] = 0xFF;
	
	//All devices F1,F2 broadcasting
	myBuf[6] = 0xB2;
	myBuf[7] = mtrParams[0];//out door id
	myBuf[8] = 0x20;
	
	//Command
	myBuf[9] = 0xC0;
	myBuf[10] = 0x11;
	myBuf[11] = 0x00;
	
	//No of Data sets
	myBuf[12] = 0x09;
	
	//Dataset 1
	myBuf[13] = 0x40;
	myBuf[14] = 0x00;
	myBuf[15] = 0xff;//0xff;//operation ON/OFF param
	
	//Dataset 2
	myBuf[16] = 0x40;
	myBuf[17] = 0x01;
	myBuf[18] = 0xff;//0xff;//operation mode
	
	//Dataset 3
	myBuf[19] = 0x40;
	myBuf[20] = 0x06;
	myBuf[21] = 0xff;//0xff;//air flow
	
	//Dataset 4
	myBuf[22] = 0x40;
	myBuf[23] = 0x11;
	myBuf[24] = 0xff;//0xff;//up/down swing
	
	//Dataset 5
	myBuf[25] = 0x40;
	myBuf[26] = 0x27;
	myBuf[27] = 0xff;//0xff;//filter alarm status
	
	//Dataset 6
	myBuf[28] = 0x42;
	myBuf[29] = 0x01;
	myBuf[30] = 0xff;//0xff;//desired temperature
	myBuf[31] = 0xff;//0xff;desired temperature
	
	//Dataset 7
	myBuf[32] = 0x42;
	myBuf[33] = 0x03;
	myBuf[34] = 0xff;//0xB4;Room Temperature
	myBuf[35] = 0xff;//0xff;Room Temperature
	
	//Dataset 8
	myBuf[36] = 0x02;
	myBuf[37] = 0x02;
	myBuf[38] = 0xff;//0xff;error code
	myBuf[39] = 0xff;//0xff;error code
	
	//dataset 9
	myBuf[40] = 0x04;
	myBuf[41] = 0x09;
	myBuf[42] = 0xff;//0xff;wired remote controller restrict status
	myBuf[43] = 0xff;//0xff;wired remote controller restrict status
	myBuf[44] = 0xff;//0xff;wired remote controller restrict status
	myBuf[45] = 0xff;//0xff;wired remote controller restrict status
		
	//CRC
	unsigned short crc;
	crc = crc16_ccitt(myBuf+3, 43);//45-3+1
	
	myBuf[46] = crc >> 8;
	myBuf[47] = crc & 0xFF;
	myBuf[48] = 0x34;
	
	if(localFd >= 0)
	{
		writeSerialHex(localFd,myBuf,sizeof(myBuf));
		#ifdef DEBUG
			cout << "Monitor frame sent:";
			for(size_t loopNo1=0; loopNo1 < 49;loopNo1++)
			{
				printf("%x ",myBuf[loopNo1]);
			}
			cout << "\n\n";
		#endif
	}
}

bool samsungSerial::hex2Binary(unsigned char byte,int k)
{
	/* Pass in char* array of bytes, get binary representation as string in bitStr */
	//size_t byte = 15;// Read from file
	size_t mask = 1; // Bit mask
	size_t bits[8]; //for 1 byte data (8 bits), array size is 8

	// Extract the bits
	for (size_t i = 0; i < 8; i++) //for 1 byte data (8 bits), array size is 8
	{
		// Mask each bit in the byte and store it
		bits[i] = (byte >> i) & mask;
	}
	
	for (size_t i = 0; i < 8; i++) //for 1 byte data (8 bits), array size is 8
	{
		//printf("%d",bits[7-i]);	
	}
	//printf("%d",bits[k-1]);
	return bits[k-1];	
}
