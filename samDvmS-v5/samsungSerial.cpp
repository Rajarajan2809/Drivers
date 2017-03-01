/*******************************************************************************
 *	Purpose		:	Samsung DVM S AC Integration.							   *
 * 	Description	:	This program is the Source for Driver Program.			   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	01-Jun-16												   *
 * 	DOLM		:	17-Dec-16												   *
 *******************************************************************************/

#include "samsungSerial.hpp"

using namespace std;

void samsungSerial::serialThread()
{
	#ifdef DEBUG
		cout<<"<----------Serial configuration initiated---------->\n";
	#endif
	
	//serStart:
	string MODEMDEVICE ="/dev/tty",dbSerial;
	{
		MYSQL_ROW row1;
		testMysql db("Milan_SP");
		db.mysqlQuery("SELECT port FROM Serial_Port WHERE device_id='110006'");
		while ((row1 = mysql_fetch_row(db.result)))
		{
			dbSerial = row1[0];
		}
	}
	if(dbSerial.empty())
		MODEMDEVICE = MODEMDEVICE +"USB0";
	else
		MODEMDEVICE = MODEMDEVICE + dbSerial;
	
	#ifdef DEBUG	
		cout << "Serial port used:" << MODEMDEVICE << ".\n\n";
	#endif
	
	struct termios portterm;
	unsigned long int BAUDRATE = 9600;
	fd = open(MODEMDEVICE.c_str(),O_RDWR| O_NOCTTY | O_NONBLOCK);
	//int fd = open(MODEMDEVICE.c_str(),O_RDWR| O_NOCTTY | O_NONBLOCK);

	if (fd<0) 
	{
		#ifdef DEBUG
			cout << "Port:" << MODEMDEVICE << " is busy or not detected.\n";
		#endif
		//sleep(5);
		//goto serStart;
	}
	else
	{
		#ifdef DEBUG
			cout << "Port:" << MODEMDEVICE << " is working fine.\n";
		#endif
		tcgetattr(fd,&portterm); /* save current serial port settings */
	  	bzero(&portterm, sizeof(portterm)); /* clear struct for new port settings */
		tcgetattr(fd,&portterm);
		
		//portterm.c_iflag = IGNPAR | IGNBRK ;
		portterm.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
        portterm.c_cc[VMIN] = 77;
		portterm.c_cc[VTIME] = 0;
		portterm.c_cflag |= CREAD;		 	// Enable receiver
 		portterm.c_cflag |= CS8;			// 8 data bit
		portterm.c_iflag |= IGNPAR;		 	// Ignore framing errors and parity errors.
	  	portterm.c_lflag &= ~(ICANON); 		// DISABLE canonical mode.
											//Disables the special unsigned characters EOF, EOL, EOL2, 										// ERASE, KILL, LNEXT, REPRINT, STATUS, and WERASE, and buffers by lines.
		portterm.c_lflag &= ~(ECHO);		// DISABLE this: Echo input unsigned characters.
		portterm.c_lflag &= ~(ECHOE);		// DISABLE this: If ICANON is also set, the ERASE unsigned character erases the preceding input
  									
		portterm.c_iflag &= ~(ICRNL | BRKINT | IMAXBEL);			//input flag unsigned charage return to new line.
		portterm.c_lflag &= ~(ISIG);		// DISABLE this: When any of the unsigned characters INTR, QUIT, SUSP,
											// or DSUSP are received, generate the corresponding signal.
		portterm.c_cc[VMIN]=1;				// Minimum number of unsigned characters for non-canonical read.
		portterm.c_cc[VTIME]=0;				// Timeout in deciseconds for non-canonical read.

		cfsetispeed(&portterm, BAUDRATE);
        cfsetospeed(&portterm, BAUDRATE);
		
		/* Setting other Port Stuff */
		portterm.c_cflag |= PARENB;			//Even parity
		portterm.c_cflag &= ~PARODD;		//Even parity
		portterm.c_cflag &= ~CSTOPB;		//stop bit 1
		portterm.c_cflag &= ~CSIZE;
		portterm.c_cflag |= CS8;			//8E1
		portterm.c_oflag &= ~(OPOST);		//  When the OPOST option is disabled, all other option bits in c_oflags are ignored.
		portterm.c_oflag &= ~(ONLCR);
		portterm.c_lflag &= ~(IEXTEN);		// Disable extended functions
		portterm.c_lflag &= ~(ECHOE | ECHOK | ECHOCTL |ECHOKE);
			
		tcflush(fd, TCIFLUSH);
  		tcsetattr(fd, TCSANOW, &portterm);
  		serialRead();
	}
}

void samsungSerial::webQueryThread()
{
	/*-----Tcp Server listening for Web query in port 5556.-----*/
	int socketStatus,setOptStatus,bindStatus,listenStatus,socketFd,acceptFd,setOptVal = 1;/*-----socketFd -> socket descriptor for the socket connection; acceptFd -> socket descriptor for accepting socket connection; responseFd -> socket descriptor for sending data through socket connection.-----*/
	struct addrinfo host_info;      
    struct addrinfo *host_info_list;
    struct sockaddr_storage their_addr;
    ssize_t byteReceived,byteSent;
    char webQuery[1000];
	string webQueryStr{},formData{},response{},request{},ip{};
	socklen_t addr_size = sizeof(their_addr);
	vector<string> param,paramVal;
		
	/*-----Server specifications----*/
	memset(&host_info, 0, sizeof host_info);
	host_info.ai_family = AF_UNSPEC;     /*-----IP version not specified, Can be both IPv4 and IPv6.-----*/
    host_info.ai_socktype = SOCK_STREAM; /*-----Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.-----*/
    host_info.ai_flags = AI_PASSIVE;     /*-----IP Wildcard-----*/
	
	/*-----getting the status of the port on the ip.-----*/
	socketStatus = getaddrinfo(NULL, "1106", &host_info, &host_info_list);
    if (socketStatus != 0)
    {
		/*-----Could not get address info.-----*/
		#ifdef DEBUG
			cout << "Could not get address info about the ip.\n";
		#endif
	}
		
    /*-----Opening the socket.-----*/
    socketFd = socket(host_info_list->ai_family, host_info_list->ai_socktype,host_info_list->ai_protocol);
    if (socketFd == -1)  
	{
		/*-----socket error-----*/
		#ifdef DEBUG
			cout << "socket error:socket could not be opened.\n";
		#endif
	}
		
	/*-----Setting Options for the socket.-----*/
    setOptStatus = setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &setOptVal, sizeof(int));
    if(setOptStatus != 0)
    {
		/*-----Options not set-----*/
		#ifdef DEBUG
			cout << "Options not set.\n";
		#endif
	}
	
    /*-----Binding to the socket.-----*/
    bindStatus = ::bind(socketFd, host_info_list->ai_addr, host_info_list->ai_addrlen);/*-----error part in cpp not in c, error solved by adding "::"-----*/
    if (bindStatus == -1)  
	{
		/*----not binded for----*/
		#ifdef DEBUG
			cout<<"Port(1106) is already used by another Service, Please stop that service and proceed.\n";
		#endif
	}
	
	/*-----While loop running infinite times-----*/
	while(1)
	{
		usleep(1000);
		listenStatus =  listen(socketFd, 5);
		if (listenStatus == -1)  
		{
			/*-----not listening-----*/
			#ifdef DEBUG
				cout << "not listening in port 1107.\n";
			#endif
		}
		
		/*-----accepting connections.-----*/	
		acceptFd = accept(socketFd, (struct sockaddr *)&their_addr, &addr_size);
		if (acceptFd == -1)
		{
			/*-----connection not accepted-----*/
			#ifdef DEBUG
				cout << "connection could not be accepted.\n";
			#endif
		}

		/*-----receiving Data-----*/
		byteReceived = recv(acceptFd, webQuery,1000, 0);
		if (byteReceived == -1)
		{
			/*-----receive error!-----*/
			#ifdef DEBUG
				cout<<"receive error"<<endl;
			#endif
		}
		
		webQuery[byteReceived +1] = '\0';
		webQueryStr = webQuery;
		
		/*-----Response to client.-----*/
		
		response = "Samsung NASA DVM S";
		byteSent = send(acceptFd, response.c_str(), response.length(),0);
		if(byteSent == -1)
		{
			/*-----Response not sent-----*/
			#ifdef DEBUG
				cout << "Response not sent, pls check for errors.\n";
			#endif
		}
		close(acceptFd);
		
		/*-----KeyCheck="integrationId" and keyValueCheck="20000"-----*/ 
		Uri uriObj(webQueryStr);
		formData = uriObj.getFormData(webQueryStr,request,ip);
		string intId{};
		intId = formData.substr(1,formData.find_first_of("?")-1);
		
		#ifdef DEBUG
			cout << "The webquery:" << formData << ",\n";
			cout << "intId:" << intId << ",\n";
			cout << "ip:" << ip << ",\n";
			cout << "request:" << request << ".\n\n";
		#endif
		
		formData.erase(0,formData.find_first_of("?")+1);
		uriObj.paramBuilder(formData,param,paramVal);
		for (size_t loopNo1=0; loopNo1 < param.size() && loopNo1 < paramVal.size(); loopNo1++)
		{
			#ifdef DEBUG
				cout << param[loopNo1] << ':' << paramVal[loopNo1] << '\n';
			#endif
		}
		
		if((ip == "127.0.0.1")||(ip == "localhost"))
		{
			if(intId == "110006")
			{
				//http://localhost:1106/110006?query=control&outid=0&inid=0&buzz=00&op=00&mode=01&air=00&swing=01&alarm=00&temp=18&rem=00
				if((param.size() > 0) && (paramVal.size() > 0) && (!param[0].empty())&&(!paramVal[0].empty())&&(param[0] == "query")&&(paramVal[0] == "control"))
				{
					//out door id limit check
					if((param.size() > 1) && (paramVal.size() > 1) && (!param[1].empty()) && (!paramVal[1].empty()) && (param[1] == "outid") && (!paramVal[1].empty()) && (stoi(paramVal[1]) >=0) && (stoi(paramVal[1]) < 16))
					{
						//indoor id limit check
						if((param.size() > 2) && (paramVal.size() > 2) && (!param[2].empty()) && (!paramVal[2].empty()) && (param[2] == "inid") && (!paramVal[2].empty()) && (stoi(paramVal[2]) >=0) && (stoi(paramVal[2]) < 64))
						{
							//buzzzer sound on/off check
							if((param.size() > 3) && (paramVal.size() > 3) && (!param[3].empty()) && (!paramVal.at(3).empty()) && (param.at(3) == "buzz") && ((paramVal.at(3) == "0") || (paramVal.at(3) == "1")))
							{
								//operation on/off check
								if((param.size() > 4) && (paramVal.size() > 4) && (!paramVal.at(4).empty()) && (param.at(4) == "op") && ((paramVal.at(4) == "0") || (paramVal.at(4) == "1")))
								{
									//mode check check
									if((param.size() > 5) && (paramVal.size() > 5) && (!param[5].empty()) && (!paramVal.at(5).empty()) && (param.at(5) == "mode") && ((paramVal[5] == "47") || (paramVal[5] == "48") || (paramVal[5] == "49") || (paramVal[5] == "50") || (paramVal[5] == "378")|| (paramVal[8] == "2")))
									{
										//air flow check
										if((param.size() > 6) && (paramVal.size() > 6) && (!param[6].empty()) && (!paramVal.at(6).empty()) && (param.at(6) == "air") && ((paramVal[6] == "83")||(paramVal[6] == "85")||(paramVal[6] == "86")||(paramVal[6] == "87")))
										{
											//up/down swing check
											if((param.size() > 7) && (paramVal.size() > 7) && (!param[7].empty()) && (!paramVal.at(7).empty()) &&(param.at(7) == "swing") && ((paramVal[7] == "45") || (paramVal[7] == "46")))
											{
												//filter alarm reset check
												if((param.size() > 8) && (paramVal.size() > 8) && (!param[8].empty()) && (!paramVal.at(8).empty()) && (param.at(8) == "alarm") && ((paramVal.at(8) == "0") || (paramVal.at(8) == "1")))
												{
													//temperature check
													if((param.size() > 9) && (paramVal.size() > 9) && (!param[9].empty()) && (!paramVal.at(9).empty()) && (param.at(9) == "temp") && ((stoi(paramVal.at(9)) >= 16) && (stoi(paramVal.at(9)) < 31)))
													{
														//remote restriction check
														if((param.size() > 10) && (paramVal.size() > 10) && (!param[10].empty()) && (!paramVal.at(10).empty()) && (param.at(10) == "rem") && ((stoi(paramVal.at(10)) == 0)||(stoi(paramVal.at(10)) == 106)))
														{
															unsigned char ctrlParams[1024];
															
															ctrlParams[0] = stoi(paramVal.at(1));//outdoor id
															ctrlParams[1] = stoi(paramVal.at(2));//indoor id
															ctrlParams[2] = stoi(paramVal.at(3));//buzzer sound ON/OFF
															ctrlParams[3] = stoi(paramVal.at(4));//operation ON/OFF
															ctrlParams[4] = (paramVal[5] == "378")? 78 : stoi(paramVal[5]);//mode
															ctrlParams[5] = stoi(paramVal.at(6));//air flow auto
															ctrlParams[6] = stoi(paramVal.at(7));//up down swing ON
															ctrlParams[7] = stoi(paramVal.at(8));//FILTER ALARM NO RESET / RESET
															if(stoi(paramVal.at(10)) <= 25)
															{
																ctrlParams[8] = 0;//desired temperature on 24'C
																ctrlParams[9] = stoi(paramVal.at(9))*10;
															}
															else
															{
																ctrlParams[8] = 1;//desired temperature on 24'C
																ctrlParams[9] = stoi(paramVal.at(9))*10 - 255;
															}
															//p1=00&p2=160&p3=01&p4=44&
															ctrlParams[10] = 0;//unknown parameter 1
															ctrlParams[11] = 160;//unknown parameter 2
															ctrlParams[12] = 1;//unknown parameter 3
															ctrlParams[13] = 44;//unknown parameter 4
															ctrlParams[14] = stoi(paramVal.at(10));//remote controller restriction or no restriction
															
															size_t cmdCount = 0;
															do
															{
																#ifdef DEBUG
																	cout << "control command sent before acknowledgement:" << cmdCount << "\n";
																#endif
																if((cmdCount < 15) || (ctrlAckRcvd != 1)) //ctrl command sent 5 times max (In protocol given 10 times max)
																{
																	indoorUnitCtrl(fd,ctrlParams);
																	sleep(1);
																	cmdCount++;
																}
																else
																{
																	break;
																}
															}
															while(ctrlAckRcvd == 0);
															
															#ifdef DEBUG
																cout << "ackRcvd:" << ctrlAckRcvd << "(flag 0-NOACK, 1-ACKRCVD)\n";
															#endif
																														
															if((cmdCount >= 15) || (ctrlAckRcvd != 1))	//ctrl command sent 5 times max (In protocol given 10 times max)
															{
																#ifdef DEBUG
																	cout << "Acknowledgement not received / No response from Device.\n\n";
																#endif
															}
															else
															{
																dbStore(ctrlParams);
															}
															ctrlAckRcvd = 0;
														}
														else
														{
															#ifdef DEBUG
																cout << "check for key=rem and its keyValue.\n";
															#endif
														}	
													}
													else
													{
														#ifdef DEBUG
															cout << "check for key=temp and its keyValue.\n";
														#endif
													}
												}
												else
												{
													#ifdef DEBUG
														cout << "check for key=alarm and its keyValue.\n";
													#endif
												}
											}
											else
											{
												#ifdef DEBUG
													cout << "check for key=swing and its keyValue.\n";
												#endif
											}
										}
										else
										{
											#ifdef DEBUG
												cout << "check for key=air and its keyValue.\n";
											#endif
										}
									}
									else
									{
										#ifdef DEBUG
											cout << "check for key=mode and its keyValue.\n";
										#endif
									}
								}
								else
								{
									#ifdef DEBUG
										cout << "check for key=op and its keyValue.\n";
									#endif
								}
							}
							else
							{
								#ifdef DEBUG
									cout << "check for key=buzz and its keyValue.\n";
								#endif
							}
						}
						else
						{
							#ifdef DEBUG
								cout<<"check for key=inid and its keyValue.\n";
							#endif
						}
					}
					else
					{
						#ifdef DEBUG
							cout << "check for key=outid and its keyValue.\n";
						#endif
					}
				}
				
				//http://192.168.1.151:1106/110006?query=update&outid=00&inid=00
				else if((param.size() > 0) && (paramVal.size() > 0) && (!param.at(0).empty()) && (!paramVal.at(0).empty()) && (param.at(0) == "query") && (paramVal.at(0) == "update"))
				{
					if((param.size() > 1) && (paramVal.size() > 1) && (!param[1].empty()) && (!paramVal[1].empty()) && (param[1] == "outid") && (stoi(paramVal[1]) >= 0) && (stoi(paramVal[1]) < 16))//out door id limit check
					{
						if((param.size() > 2) && (paramVal.size() > 2) && (!param[2].empty()) && (!paramVal[2].empty()) && (param[2] == "inid") && (stoi(paramVal[2]) >=0) && (stoi(paramVal[2]) < 64))//in door id limit check
						{
							unsigned char mtrParams[2];
							mtrParams[0] = stoi(paramVal[1]);//outdoor id
							mtrParams[1] = stoi(paramVal[2]);//indoor id
														
							size_t cmdCount=0;
							do
							{
								#ifdef DEBUG
									cout << "feedback command sent before acknowledgement:" << cmdCount << "\n";
								#endif
								if((cmdCount < 15) || (fbAckRcvd != 1))//ctrl command sent 15 times max (In protocol given 10 times max)
								{
									indoorUnitMtr(fd,mtrParams);
									sleep(1);
									cmdCount++;
								}
								else
								{
									break;
								}
							}
							while(cmdCount < 15); //for three loops cmdCount <3
							#ifdef DEBUG
								cout << "fbAckRcvd:" << fbAckRcvd << "(flag-0-NOACK, 1-ACKRCVD)\n";
							#endif
															
							if((cmdCount >= 15)|| (fbAckRcvd != 1))	//ctrl command sent 5 times max (In protocol given 10 times max)
							{
								#ifdef DEBUG
									cout << "Acknowledgement not received / No response from Device.\n";
								#endif
							}
							fbAckRcvd =0;
						}
						else
						{
							#ifdef DEBUG
								cout<<"check for param=inid and its paramVal.\n";
							#endif
						}
					}
					else
					{
						#ifdef DEBUG
							cout<<"check for param=outid and its paramVal.\n";
						#endif
					}
				}
				else
				{
					#ifdef DEBUG
						cout<<"Invalid Query"<<endl;
					#endif
				}
			}
		}
		#ifdef DEBUG
			cout << '\n';
		#endif
	
		//erasing vector along with address
		param.clear();
		
		//erasing vector along with address
		paramVal.clear();
		usleep(1000);
	}	
	/*-----cout << "Socket Closed."<<endl;-----*/
	freeaddrinfo(host_info_list);
    close(socketFd);
}

int main()
{
	try
	{
		//one thread to listen the web query and other to initiate Full duplex serial communication
		thread th1(&samsungSerial::webQueryThread,samsungSerial());
		thread th2(&samsungSerial::serialThread,samsungSerial());
				
		//joining the thread
		th1.join();
		th2.join();
    }
	catch(exception& e) //Takes a reference to an 'exception' object
    {
		#ifdef DEBUG
			cout << "Error allocating memory: " << e.what() << '\n';
		#endif
    }
	return 0;
}
