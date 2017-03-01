/*******************************************************************************
 *	Purpose		:	Samsung DVM 4 AC Integration.							   *
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
		int readSerialHex(int localFd,unsigned char recvBuf[],size_t bufLen, long timeout);
		int writeSerialHex(int localFd,unsigned char sendBuf[],size_t bufLen);
		void feedbackProcessing(unsigned char myBuf[1024]);
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
	
	while(1)
	{	
		size_t serialDataLength=0;
		// read the first byte from the serial port
		serialDataLength = readSerialHex(fd,serialBuffer, 1, 100);
		if (serialDataLength == 1) 
		{
			if(serialBuffer[0] == 0x32) 
			{
				//SOF found
				#ifdef DEBUG
					printf("SOF Found=%x\n",serialBuffer[0]);
				#endif
				
				//find the mode
				readSerialHex(fd,serialBuffer+1, 3, 100);
				
				//Destination address of third party device
				if(serialBuffer[2] == 0xf0)
				{
					//for crc calc
					unsigned short crc;unsigned char crc1,crc2;
					switch(serialBuffer[3])
					{
						case 0xb2://tracking -> 32 D0 F0 B2 28 40 00 02 FE 05 01 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 67 48 00 39 34
							#ifdef DEBUG
								cout<<"Tracking feedback received:\n";
							#endif
							readSerialHex(fd,serialBuffer+4, 55, 100);
							for(int i=0;i<59;i++)
							{
								#ifdef DEBUG	
									printf("Buffer[%d]=%x\n",i,serialBuffer[i]);
								#endif
							}
								
							//CRC
							crc = crc16_ccitt(serialBuffer+1, 53);//
							crc1 = crc >> 8;
							crc2 = crc & 0xFF;
							
							#ifdef DEBUG
								printf("crc1=%x\n",crc1);
								printf("crc2=%x\n",crc2);
							#endif
							if((crc1 ==serialBuffer[54])&&(crc2 ==serialBuffer[55]))
							{
								#ifdef DEBUG
									cout<<"crc check success.\n";
								#endif
							}
							else
							{
								#ifdef DEBUG
									cout<<"crc check failed.\n";
								#endif
							}
							#ifdef DEBUG	
								printf("EOF Found=%x\n",serialBuffer[58]);
							#endif
							break;
							
						case 0xb5://status -> 32 D0 F0 B5 00 03 00 4F 55 3D 32 00 00 00 43 00 ED 01 4F 4F 48 45 06 00 01 03 00 ED 02 4F 55 44 47 00 00 02 13 00 ED 01 00 10 00 00 00 45 62 6D 2D 1D 00 49 10 11 20 00 00 01 00 3b 41 00 38 34
							{
								#ifdef DEBUG
									cout<<"feedback received.\n";
								#endif
							
								//extract no of indoor devices
								readSerialHex(fd,serialBuffer+4, 2, 100);
								int i{},j{};
								#ifdef DEBUG
									printf("No of Indoor units : %x\n",serialBuffer[5]);
								#endif
								
								readSerialHex(fd,serialBuffer+6, serialBuffer[5]*11, 100);
								//extract data from indoor unit
								//readSerialHex(fd,serialBuffer+6, serialBuffer[5]*11 -1, 200);
								//for(i=0;i < ((serialBuffer[5]*11)+6);i++)
								//{	
									//printf("Buffer[%d]=%x\n",i,serialBuffer[i]);
								//}
								
								//ODU data1 extraction
								readSerialHex(fd,serialBuffer+i,1,100);							
								#ifdef DEBUG
									printf("ODU data :%x\n",serialBuffer[i]);
								#endif	
																								
								readSerialHex(fd,serialBuffer+i+1,24,100);
								for(j=0;j < ((serialBuffer[5]*11)+31);j++)
								{
									#ifdef DEBUG
										printf("%x ",serialBuffer[j]);
									#endif
								}
								//printf("error info=%x\n",serialBuffer[i+18]);
								
								//CRC
								crc = crc16_ccitt(serialBuffer+1, j-5);
								crc1 = crc >> 8;
								crc2 = crc & 0xFF;
								
								#ifdef DEBUG
									printf("crc1=%x\n",crc1);
									printf("crc2=%x\n",crc2);
								#endif
								if((crc1 ==serialBuffer[((serialBuffer[5]*11)+6)+21])&&(crc2 ==serialBuffer[((serialBuffer[5]*11)+6)+22]))
								{
									#ifdef DEBUG
										cout<<"crc check success.\n";
									#endif
									fbAckRcvd =1;
									feedbackProcessing(serialBuffer);
								}
								else
								{
									#ifdef DEBUG
										cout<<"crc check failed.\n";
									#endif
									fbAckRcvd =1;
									feedbackProcessing(serialBuffer);
								}
								#ifdef DEBUG
									printf("EOF Found=%x\n",serialBuffer[j-1]);
								#endif
							}
							break;
							
						case 0xb0://control -> 32 d0 f0 b0 00 01 46 71 00 0a 34 //length of frame -> 11
							#ifdef DEBUG
								cout<<"control acknowledgement received.\n";
							#endif
							readSerialHex(fd,serialBuffer+4,7,100);
							
							//CRC
							crc = crc16_ccitt(serialBuffer+1, 5);//15-3+1
							crc1 = crc >> 8;
							crc2 = crc & 0xFF;
							
							#ifdef DEBUG
								printf("crc1=%x\n",crc1);
								printf("crc2=%x\n",crc2);
							#endif
							if((crc1 ==serialBuffer[6])&&(crc2 ==serialBuffer[7]))
							{
								ctrlAckRcvd =1;
								#ifdef DEBUG
									cout<<"crc check success.\n";
								#endif
							}
							else
							{
								#ifdef DEBUG
									cout<<"crc check failed.\n";
								#endif
							}
							#ifdef DEBUG
								printf("EOF Found=%x\n",serialBuffer[10]);
							#endif
							break;
					}
				}
				tcdrain(fd);
			}
			else
			{
				#ifdef DEBUG
					printf("SOF not Found and data received is =%x\n",serialBuffer[0]);
				#endif
			}
		}
		usleep(1000);
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
	//cout<<"write_fd -> "<<write_fd<<endl;
	tcdrain(localFd);
	return write_fd;
}

//feedback processing part
void samsungSerial::feedbackProcessing(unsigned char serialBuffer[1024])
{
	if((serialBuffer[3] == 0xB5)&&(serialBuffer[4] == 0x00))
	{
		thread *thread1 = new thread[serialBuffer[5]];
		for(int i=0; i < serialBuffer[5]; i++)
		{
			testMysql db("Milan_SP");
			//data 1
			bool b5[]={0,0,0,0,0,0,0,0};
			b5[7] = hex2Binary(serialBuffer[(i*11)+6],8);
			b5[6] = hex2Binary(serialBuffer[(i*11)+6],7);
			b5[5] = hex2Binary(serialBuffer[(i*11)+6],6);
			b5[4] = hex2Binary(serialBuffer[(i*11)+6],5);
			b5[3] = hex2Binary(serialBuffer[(i*11)+6],4);
			b5[2] = hex2Binary(serialBuffer[(i*11)+6],3);
			b5[1] = hex2Binary(serialBuffer[(i*11)+6],2);
			b5[0] = hex2Binary(serialBuffer[(i*11)+6],1);
			
			unsigned char indoorId1 = (b5[0]*1)+(b5[1]*2)+(b5[2]*4)+(b5[3]*8)+(b5[4]*16)+(b5[5]*32)+(b5[6]*64);
			
			#ifdef DEBUG
				printf("IDU:%d\n",indoorId1);
				printf("ODU:%x\n",(serialBuffer[1] - 0xd0));
			#endif
			string nodeId = ((serialBuffer[4]>9)?to_string(serialBuffer[4]):"0"+to_string(serialBuffer[4])) + ((indoorId1>9)?to_string(indoorId1):"0"+to_string(indoorId1));
			
			//setting temperature data 2
			#ifdef DEBUG
				bool b3[]={0,0,0,0,0,0,0,0};
				b3[7] = hex2Binary(serialBuffer[(i*11)+6+1],8);
				b3[6] = hex2Binary(serialBuffer[(i*11)+6+1],7);
				b3[5] = hex2Binary(serialBuffer[(i*11)+6+1],6);
				b3[4] = hex2Binary(serialBuffer[(i*11)+6+1],5);
				b3[3] = hex2Binary(serialBuffer[(i*11)+6+1],4);
				b3[2] = hex2Binary(serialBuffer[(i*11)+6+1],3);
				b3[1] = hex2Binary(serialBuffer[(i*11)+6+1],2);
				b3[0] = hex2Binary(serialBuffer[(i*11)+6+1],1);
				unsigned int desiredTemp = (b3[0]*1)+(b3[1]*2)+(b3[2]*4)+(b3[3]*8)+(b3[4]*16)+(b3[5]*32)+(b3[6]*64) - 55;
				cout << "desired temperature:" << desiredTemp << "\n";
			#endif
			
			if((serialBuffer[(i*11)+6+1] >= 0x00)&&(serialBuffer[(i*11)+6+1] <= 0x7F))
			{
				//setting temperature data 2
				bool b4[]={0,0,0,0,0,0,0,0};
				b4[7] = hex2Binary(serialBuffer[(i*11)+6+2],8);
				b4[6] = hex2Binary(serialBuffer[(i*11)+6+2],7);
				b4[5] = hex2Binary(serialBuffer[(i*11)+6+2],6);
				b4[4] = hex2Binary(serialBuffer[(i*11)+6+2],5);
				b4[3] = hex2Binary(serialBuffer[(i*11)+6+2],4);
				b4[2] = hex2Binary(serialBuffer[(i*11)+6+2],3);
				b4[1] = hex2Binary(serialBuffer[(i*11)+6+2],2);
				b4[0] = hex2Binary(serialBuffer[(i*11)+6+2],1);
				
				unsigned int currentTemp = (b4[0]*1)+(b4[1]*2)+(b4[2]*4)+(b4[3]*8)+(b4[4]*16)+(b4[5]*32)+(b4[6]*64) - 55;
				
				//data 3
				if((serialBuffer[(i*11)+6+2] >= 0x00)&&(serialBuffer[(i*11)+6+2] <= 0x7F))
				{
					#ifdef DEBUG
						cout << "current temperature :" << currentTemp << "\n";
					#endif
					
					//data 6
					bool b1[]={0,0,0,0,0,0,0,0};
					b1[7] = hex2Binary(serialBuffer[(i*11)+6+5],8);
					b1[6] = hex2Binary(serialBuffer[(i*11)+6+5],7);
					b1[5] = hex2Binary(serialBuffer[(i*11)+6+5],6);
					b1[4] = hex2Binary(serialBuffer[(i*11)+6+5],5);
					b1[3] = hex2Binary(serialBuffer[(i*11)+6+5],4);
					b1[2] = hex2Binary(serialBuffer[(i*11)+6+5],3);
					b1[1] = hex2Binary(serialBuffer[(i*11)+6+5],2);
					b1[0] = hex2Binary(serialBuffer[(i*11)+6+5],1);
					
					string swing{},fanMode{};
					//swing
					if((b1[7] == 1)&&(b1[6] == 1)&&(b1[5] == 0)&&(b1[4] == 1)&&(b1[3] == 0))
					{
						swing = "46";
						#ifdef DEBUG
							cout << "swing:up/down\n";
							cout << "swing=" << swing << '\n';
						#endif
					}
					else if((b1[7] == 1)&&(b1[6] == 1)&&(b1[5] == 0)&&(b1[4] == 1)&&(b1[3] == 1))
					{
						swing = "46";
						#ifdef DEBUG
							cout << "swing:left/right\n";
							cout << "swing=" << swing << '\n';
						#endif
					}
					else if((b1[7] == 1)&&(b1[6] == 1)&&(b1[5] == 1)&&(b1[4] == 0)&&(b1[3] == 0))
					{
						swing = "46";
						#ifdef DEBUG
							cout << "swing:up/down and left/right\n";
							cout << "swing=" << swing << '\n';
						#endif
					}
					else if((b1[7] == 1)&&(b1[6] == 1)&&(b1[5] == 1)&&(b1[4] == 1)&&(b1[3] == 1))
					{
						swing = "45";
						#ifdef DEBUG
							cout << "swing:stop\n";
							cout << "swing=" << swing << '\n';
						#endif	
					}							
					
					//airflow
					if((b1[2] == 0)&&(b1[1] == 0)&&(b1[0] == 0))
					{
						fanMode="86";
						#ifdef DEBUG
							cout << "wind speed:auto\n";
							cout << "fan_mode=" << fanMode << '\n';
						#endif
					}
					else if((b1[2] == 0)&&(b1[1] == 1)&&(b1[0] == 0))
					{
						fanMode="83";
						#ifdef DEBUG
							cout << "wind speed:low\n";
							cout << "fan_mode=" << fanMode << '\n';
						#endif
					}
					else if((b1[2] == 1)&&(b1[1] == 0)&&(b1[0] == 0))
					{
						fanMode="87";
						#ifdef DEBUG
							cout << "wind speed:medium\n";
							cout << "fan_mode=" << fanMode << '\n';
						#endif
					}
					else if((b1[2] == 1)&&(b1[1] == 0)&&(b1[0] == 1))
					{
						fanMode="85";
						#ifdef DEBUG
							cout << "wind speed:high\n";
							cout << "fan_mode=" << fanMode << '\n';
						#endif
					}
					
					//data 7
					bool b2[]={0,0,0,0,0,0,0,0};
					b2[7] = hex2Binary(serialBuffer[(i*11)+6+6],8);
					b2[6] = hex2Binary(serialBuffer[(i*11)+6+6],7);
					b2[5] = hex2Binary(serialBuffer[(i*11)+6+6],6);
					b2[4] = hex2Binary(serialBuffer[(i*11)+6+6],5);
					b2[3] = hex2Binary(serialBuffer[(i*11)+6+6],4);
					b2[2] = hex2Binary(serialBuffer[(i*11)+6+6],3);
					b2[1] = hex2Binary(serialBuffer[(i*11)+6+6],2);
					b2[0] = hex2Binary(serialBuffer[(i*11)+6+6],1);
															
					//property & propertyid
					//off -2, cool - 47, heat - 49, auto - 50, dry - 48 and dry air(ventilation) - 378
					//mode
					if((b2[6] == 0)&&(b2[5] == 0))
					{
						#ifdef DEBUG
							cout << "mode:off\n";
							cout << "UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110007') AS T2 SET T1.fan_mode='"+fanMode+"',T1.swing_status='"+swing+"',T1.mode='2',T1.misc_status='2',T1.current_temp='"+to_string(currentTemp)+"',T1.auto_count = T1.auto_count + 1,T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n";
						#endif
						db.mysqlQuery("UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110007') AS T2 SET T1.fan_mode='"+fanMode+"',T1.swing_status='"+swing+"',T1.mode='2',T1.current_temp='"+to_string(currentTemp)+"',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n");
					}
					else
					{
						string mode{};
						if((b2[4] == 0)&&(b2[3] == 0)&&(b2[2] == 0))
						{
							mode="50";
							#ifdef DEBUG
								cout << "mode:auto\n";
								cout << "mode=" << mode << '\n';
							#endif
						}
						else if((b2[4] == 0)&&(b2[3] == 0)&&(b2[2] == 1))
						{
							mode="47";
							#ifdef DEBUG
								cout << "mode:cool\n";
								cout << "mode=" << mode << '\n';
							#endif
						}
						else if((b2[4] == 0)&&(b2[3] == 1)&&(b2[2] == 0))
						{
							mode="48";
							#ifdef DEBUG
								cout << "mode:dry";
								cout << "mode=" << mode << '\n';
							#endif
						}
						else if((b2[4] == 0)&&(b2[3] == 1)&&(b2[2] == 1))
						{
							mode="378";
							#ifdef DEBUG
								cout << "mode:fan(ventillation)";
								cout << "mode=" << mode << '\n';
							#endif
						}
						else if((b2[4] == 1)&&(b2[3] == 0)&&(b2[2] == 0))
						{
							mode="49";
							#ifdef DEBUG				
								cout << "mode:heat\n";
								cout << "mode=" << mode << '\n';
							#endif
						}
						#ifdef DEBUG				
							cout << "UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110007') AS T2 SET T1.fan_mode='"+fanMode+"',T1.swing_status='"+swing+"',T1.mode='"+mode+"',T1.current_temp='"+to_string(currentTemp)+"',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n";
						#endif
						db.mysqlQuery("UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110007') AS T2 SET T1.fan_mode='"+fanMode+"',T1.swing_status='"+swing+"',T1.mode='"+mode+"',T1.current_temp='"+to_string(currentTemp)+"', T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n");
					}
					
					//level control
					if((b2[4] == 0)&&(b2[3] == 0)&&(b2[2] == 0))
					{
						#ifdef DEBUG
							cout<<"level 1\n";
						#endif
					}
					else if((b2[4] == 0)&&(b2[3] == 0)&&(b2[2] == 1))
					{
						#ifdef DEBUG
							cout<<"level 2.\n";
						#endif
					}
					else if((b2[4] == 0)&&(b2[3] == 1)&&(b2[2] == 0))
					{
						#ifdef DEBUG
							cout<<"level 3.\n";
						#endif
					}
				}
			}
			//curl request to get local_inst_id for feedback
			MYSQL_ROW row4;
			db.mysqlQuery("SELECT local_instance_id FROM Climate_Status WHERE id=(SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110007');");
			string locInstId{};
			while(((row4 = mysql_fetch_row(db.result))))
			{
				locInstId = row4[0];
			}
			thread1[i] = thread(&samsungSerial::curlRequest,samsungSerial(),"http://localhost:6161/Milan/Drivers/MILAN_EXE/CLIMATE/climate_feed.cgi?l_instid="+locInstId,"GET");
		}
		
		for(int i=0; i < serialBuffer[5]; i++)
		{
			thread1[i].join();
			usleep(1000);
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
	//32 F0 D0 B0 00 01 01 00 18 12 01 F4 CS CS 00 0F 34
	//32 f0 d0 b5 b2 ff ff ff ff ff 20 10 03 71 00 0f 34
	unsigned char myBuf[100];
	myBuf[0] = 0x32;
	
	//3rd party address f0
	myBuf[1] = 0xf0;
	
	//out door unit id
	myBuf[2] = 0xd0+ctrlParams[0];//out door id
		
	//mode for control
	myBuf[3] = 0xb0;
	
	//preamble
	myBuf[4] = 0x00;
	
	//unit type indoor/outdoor
	myBuf[5] = 01; //If frame is to be sent to outdoor unit -> 0x01 and If frame is to be sent to indoor unit -> 0x02
	
	//IDU control by	te
	//IDU number ranges from 00 - 63 or 0x00 0x3f
	myBuf[6] = ctrlParams[1];//indoor id
	
	//swing type
	switch(ctrlParams[2])
	{
		case 46://up/down and left/right
			{
				myBuf[7] = 0x1c;
			}
			break;
			
		case 45://stop
			{
				myBuf[7] = 0x1f;
			}
			break;
			
	}
	
	//current temperature
	if((ctrlParams[3] >=0) && (ctrlParams[3] <= 0x3f))
		myBuf[8] = ctrlParams[3];
	
	bool b1[] = {0,0,0,0,0,0,0,0};// for air flow and temperature
	
	//airflow
	switch(ctrlParams[4])
	{
		//auto
		case 0:
			{
				b1[7] = 0;
				b1[6] = 0;
				b1[5] = 0;
			}
			break;
		
		//weak wind	
		case 1:
			{
				b1[7] = 0;
				b1[6] = 1;
				b1[5] = 0;
			}
			break;
		
		//middle wind	
		case 5:
			{
				b1[7] = 1;
				b1[6] = 0;
				b1[5] = 0;
			}
			break;
		
		//strong wind	
		case 3:
			{
				b1[7] = 1;
				b1[6] = 0;
				b1[5] = 1;
			}
			break;
		
		//default auto	
		default:
			{
				b1[7] = 0;
				b1[6] = 0;
				b1[5] = 0;
			}
			break;	
	}
	
	//set temperature
	if((ctrlParams[5] >= 0) && (ctrlParams[5] <= 0x1f))
	{
		b1[4] = hex2Binary(ctrlParams[5],5);
		b1[3] = hex2Binary(ctrlParams[5],4);
		b1[2] = hex2Binary(ctrlParams[5],3);
		b1[1] = hex2Binary(ctrlParams[5],2);
		b1[0] = hex2Binary(ctrlParams[5],1);
	}
	
	myBuf[9] = (b1[0]*1 + b1[1]*2 + b1[2]*4 + b1[3]*8 + b1[4]*16 + b1[5]*32 + b1[6]*64 + b1[7]*128);
	
	bool b2[] = {0,0,0,0,0,0,0,0};// for filter reset and mode
	
	//filter reset
	switch(ctrlParams[6])
	{
		case 0://reset not required
			{
				b2[7] = 0;
				b2[6] = 0;
				b2[5] = 0;
				b2[4] = 0;
				b2[3] = 0;	
			}
			break;
			
		case 1://reset required
			{
				b2[7] = 0;
				b2[6] = 0;
				b2[5] = 1;
				b2[4] = 0;
				b2[3] = 0;
			}
			break;	
		
	}
	
	//mode
	//property & propertyid
	//off -2, cool - 47, heat - 49, auto - 50, dry - 48 and dry air(ventilation) - 378
	
	switch(ctrlParams[7])
	{
		case 50://auto
			{
				b2[2] = 0;
				b2[1] = 0;
				b2[0] = 0;
			}
			break;
			
		case 47://cool
			{
				b2[2] = 0;
				b2[1] = 0;
				b2[0] = 1;
			}
			break;
			
		case 48://dehumidification/dry
			{
				b2[2] = 0;
				b2[1] = 1;
				b2[0] = 0;
			}
			break;
			
		case 78://ventilation/dry air
			{
				b2[2] = 0;
				b2[1] = 1;
				b2[0] = 1;
			}
			break;			
		
		case 49://heating
			{
				b2[2] = 1;
				b2[1] = 0;
				b2[0] = 0;
			}
			break;
	}
	
	myBuf[10] = (b2[0]*1 + b2[1]*2 + b2[2]*4 + b2[3]*8 + b2[4]*16 + b2[5]*32 + b2[6]*64 + b2[7]*128);
	
	bool b3[] = {0,0,0,0,0,0,0,0};// for operation on/off and level
	
	//operation on/off
	switch(ctrlParams[8])
	{
		case 0://stop / off
			{
				b3[7] = 1;
				b3[6] = 1;
				b3[5] = 0;
				b3[4] = 0;
				b3[3] = 0;
				b3[2] = 1;
			}
			break;
			
		case 1://run / on
			{
				b3[7] = 1;
				b3[6] = 1;
				b3[5] = 1;
				b3[4] = 1;
				b3[3] = 0;
				b3[2] = 1;
			}
			break;	
	}
	
	//level
	switch(ctrlParams[9])
	{
		case 1://level 1
			{
				b3[1] = 0;
				b3[0] = 0;
			}
			break;
			
		case 2://level 2
			{
				b3[1] = 0;
				b3[0] = 1;
			}
			break;
			
		case 3://level 3
			{
				b3[1] = 1;
				b3[0] = 0;
			}
			break;
		
		default://level 2
			{
				b3[1] = 0;
				b3[0] = 1;
			}
			break; 
	}
	
	myBuf[11] = (b3[0]*1 + b3[1]*2 + b3[2]*4 + b3[3]*8 + b3[4]*16 + b3[5]*32 + b3[6]*64 + b3[7]*128);
		
	//CRC
	unsigned short crc;
	crc = crc16_ccitt(myBuf+1, 11);//48-3+1
	
	myBuf[12] = crc >> 8;
	myBuf[13] = crc & 0xFF;
	myBuf[14] = 0x00;
	myBuf[15] = 0x0f;
	myBuf[16] = 0x34;
	
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
				
	//property & propertyid
	//off -2, cool - 47, heat - 49, auto - 50, dry - 48 and dry air(ventilation) - 378
	//mode
	if((ctrlParams[8] == 0) || (ctrlParams[7] == 2))
	{
		#ifdef DEBUG
			cout << "UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110007') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[4])+"',T1.swing_status = '"+to_string(ctrlParams[2])+"',T1.mode='2',T1.misc_status=2,T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n";
		#endif
		db.mysqlQuery("UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110007') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[4])+"',T1.swing_status = '"+to_string(ctrlParams[2])+"',T1.mode='2',T1.misc_status=2,T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;");
	}
	else
	{
		switch(ctrlParams[7])
		{
			case 50://auto
				{
					#ifdef DEBUG
						cout << "UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110007') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[4])+"',T1.swing_status = '"+to_string(ctrlParams[2])+"',T1.mode='50',auto_changeover_setpoint='"+to_string(ctrlParams[5])+"',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n";
					#endif
					db.mysqlQuery("UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110007') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[4])+"',T1.swing_status = '"+to_string(ctrlParams[2])+"',T1.mode='50',auto_changeover_setpoint='"+to_string(ctrlParams[5])+"',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;");
				}
				break;
				
			case 47://cool
				{
					#ifdef DEBUG
						cout << "UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110007') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[4])+"',T1.swing_status = '"+to_string(ctrlParams[2])+"',T1.mode='47',T1.cool_setpoint='"+to_string(ctrlParams[5])+"',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n";
					#endif
					db.mysqlQuery("UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110007') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[4])+"',T1.swing_status = '"+to_string(ctrlParams[2])+"',T1.mode='47',T1.cool_setpoint='"+to_string(ctrlParams[5])+"',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;");
				}
				break;
				
			case 48://dehumidification/dry
				{
					#ifdef DEBUG
						cout << "UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110007') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[4])+"',T1.swing_status = '"+to_string(ctrlParams[2])+"',T1.mode='48',T1.dry_setpoint='"+to_string(ctrlParams[5])+"',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n";
					#endif
					db.mysqlQuery("UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110007') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[4])+"',T1.swing_status = '"+to_string(ctrlParams[2])+"',T1.mode='48',T1.dry_setpoint='"+to_string(ctrlParams[5])+"',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;");
				}
				break;
				
			case 78://ventilation
				{
					#ifdef DEBUG
						cout << "UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110007') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[4])+"',T1.swing_status = '"+to_string(ctrlParams[2])+"',T1.mode='378',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n";
					#endif
					db.mysqlQuery("UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110007') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[4])+"',T1.swing_status = '"+to_string(ctrlParams[2])+"',T1.mode='378',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;");
				}
				break;			
			
			case 49://heating
				{
					#ifdef DEBUG
						cout << "UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110007') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[4])+"',T1.swing_status = '"+to_string(ctrlParams[2])+"',T1.mode='49',T1.heat_setpoint='"+to_string(ctrlParams[5])+"',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;\n";
					#endif
					db.mysqlQuery("UPDATE Climate_Status AS T1, (SELECT id FROM Climate_Status WHERE device_id = '"+nodeId+"' AND integration_id = '110007') AS T2 SET T1.fan_mode='"+to_string(ctrlParams[4])+"',T1.swing_status = '"+to_string(ctrlParams[2])+"',T1.mode='49',T1.heat_setpoint='"+to_string(ctrlParams[5])+"',T1.auto_count = T1.auto_count + 1, T1.timestamp = CURRENT_TIMESTAMP WHERE T1.id = T2.id;");
				}
				break;
		}
	}
}

//monitor command sent
void samsungSerial::indoorUnitMtr(int localFd,unsigned char mtrParams[])
{
	//32 F0 D0 B5 B2 FF FF FF FF FF 20 10 16 58 00 0F 34
	//32 f0 d0 b5 b2 ff ff ff ff ff 20 10 3 71 0 f 34
	unsigned char myBuf[100];
	myBuf[0] = 0x32;
	myBuf[1] = 0xF0;
	myBuf[2] = 0xd0+mtrParams[0];
	myBuf[3] = 0xb5;
	myBuf[4] = 0xb2;
	myBuf[5] = 0xFF;
	myBuf[6] = 0xFF;
	myBuf[7] = 0xFF;
	myBuf[8] = 0xFF;
	myBuf[9] = 0xFF;
	myBuf[10] = 0x20;
	myBuf[11] = 0x10;
			
	//CRC
	unsigned short crc;
	crc = crc16_ccitt(myBuf+1, 11);//13-3+1
	
	myBuf[12] = crc >> 8;
	myBuf[13] = crc & 0xFF;
	myBuf[14] = 0x00;
	myBuf[15] = 0x0f;
	myBuf[16] = 0x34;
		
	if(localFd >= 0)
	{
		writeSerialHex(localFd,myBuf,sizeof(myBuf));
		#ifdef DEBUG
			cout << "Monitor frame sent:";
			for(size_t loopNo1=0; loopNo1 < 17;loopNo1++)
			{
				printf("%x ",myBuf[loopNo1]);
			}
			cout << "\n\n";
		#endif
	}
}

bool samsungSerial::hex2Binary(unsigned char byte,int k)
{
	//8 bit data has bit[7] to bit[0] so if u want bit[7] give k value as 8 and for bit[0] give k value as 1
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
