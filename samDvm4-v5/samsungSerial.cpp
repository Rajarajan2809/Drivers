/*******************************************************************************
 *	Purpose		:	Samsung DVM 4 AC Integration.							   *
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
		db.mysqlQuery("SELECT port FROM Serial_Port WHERE device_id='110007'");
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
		
		portterm.c_iflag = IGNPAR | IGNBRK ;
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
		portterm.c_cflag &= ~PARENB;		//no parity
		portterm.c_cflag |= CSTOPB;			//stop bit 2
		portterm.c_cflag &= ~CSIZE;
		portterm.c_cflag |= CS8;			//8E1
		portterm.c_oflag &= ~(OPOST);		//  When the OPOST option is disabled, all other option bits in c_oflags are ignored.
		portterm.c_oflag &= ~(ONLCR);
		portterm.c_lflag &= ~(IEXTEN);		// Disable extended functions
		portterm.c_lflag &= ~(ECHOE | ECHOK | ECHOCTL |ECHOKE);
			
		tcflush(fd, TCIFLUSH);
  		tcsetattr(fd, TCSANOW, &portterm);
  		
  		/*-----Thread to listen in the serial port independantly-----*/
		serialRead();
	}
}

void samsungSerial::webQueryThread()
{
	/*-----Tcp Server listening for Web query in port 1107.-----*/
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
	socketStatus = getaddrinfo(NULL, "1107", &host_info, &host_info_list);
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
			cout<<"Port(1107) is already used by another Service, Please stop that service and proceed.\n";
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
				cout<<"receive error.\n";
			#endif
		}
		webQuery[byteReceived +1] = '\0';
		webQueryStr = webQuery;
		
		/*-----Response to client.-----*/
		
		response = "Samsung NASA DVM 4";
		byteSent = send(acceptFd, response.c_str(), response.length(),0);
		if(byteSent == -1)
		{
			/*-----Response not sent-----*/
			#ifdef DEBUG
				cout << "Response not sent, pls check for errors.\n";
			#endif
		}
		close(acceptFd);
		
		/*-----KeyCheck="integrationId" and paramValCheck="20000"-----*/ 
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
			if(intId == "110007")
			{
				//http://localhost:1107/110007?query=control&outid=00&inid=01&swing=00&cur_temp=26&air=83&set_temp=20&filter=0&mode=47&op=1&lev=2
				if((param.size() > 0) && (paramVal.size() > 0) && (!param[0].empty()) && (!paramVal[0].empty()) && (param[0] == "query") && (paramVal[0] == "control"))
				{
					//out door id limit check,refer protocol.
					if((param.size() > 1) && (paramVal.size() > 1) && (!param[1].empty()) && (!paramVal[1].empty()) && (param[1] == "outid") && (!paramVal[1].empty()) && (stoi(paramVal[1]) >=0) && (stoi(paramVal[1]) < 16))
					{
						//in door id limit check,refer protocol.
						if((param.size() > 2) && (paramVal.size() > 2) && (!param[2].empty()) && (!paramVal[2].empty()) && (param[2] == "inid") && (!paramVal[2].empty()) && (stoi(paramVal[2]) >=0) && (stoi(paramVal[2]) < 64))
						{
							//swing up/down left/right check,refer property id.
							if((param.size() > 3) && (paramVal.size() > 3) && (!param[3].empty()) && (!paramVal[3].empty()) && (param[3] == "swing") && (!paramVal[3].empty()) && ((paramVal[3] == "45") || (paramVal[3] == "46")))
							{
								//current temp check,refer protocol.
								if((param.size() > 4) && (paramVal.size() > 4) && (!param[4].empty()) && (!paramVal[4].empty()) && (param[4] == "cur_temp") && (!paramVal[4].empty()) && (stoi(paramVal[4]) >=0) && (stoi(paramVal[4]) <= 0x3f))
								{
									//air flow check,refer property id.
									if((param.size() > 5) && (paramVal.size() > 5) && (!param[5].empty()) && (!paramVal[5].empty()) && (param[5] == "air") && (!paramVal[5].empty()) && ((paramVal[5] == "83")||(paramVal[5] == "85")||(paramVal[5] == "86")||(paramVal[5] == "87")))
									{
										//set temperature check,refer protocol.
										if((param.size() > 6) && (paramVal.size() > 6) && (!param[6].empty()) && (!paramVal[6].empty()) && (param[6] == "set_temp") && (!paramVal[6].empty()) && (stoi(paramVal[6]) >=0) && (stoi(paramVal[6]) <= 0x1f))
										{
											//filter reset check,refer protocol.
											if((param.size() > 7) && (paramVal.size() > 7) && (!param[7].empty()) && (!paramVal[7].empty()) && (param[7] == "filter") && (!paramVal[7].empty()) && (stoi(paramVal[7]) >=0) && (stoi(paramVal[7]) <= 1))
											{
												//mode check,refer property id.
												if((param.size() > 8) && (paramVal.size() > 8) && (!param[8].empty()) && (!paramVal[8].empty()) && (param[8] == "mode") && (!paramVal[8].empty()) && ((paramVal[8] == "47") || (paramVal[8] == "48") || (paramVal[8] == "49") || (paramVal[8] == "50") || (paramVal[8] == "378") || (paramVal[8] == "2")))
												{
													//operation check,refer property id.
													if((param.size() > 9) && (paramVal.size() > 9) && (!param[9].empty()) && (!paramVal[9].empty()) && (param[9] == "op") && (!paramVal[9].empty()) && ((paramVal[9] == "0") || (paramVal[9] == "1" )))
													{
														//level,refer protocol.
														if((param.size() > 10) && (paramVal.size() > 10) && (!param[10].empty()) && (!paramVal[10].empty()) && (param[10] == "lev") && (!paramVal[10].empty()) && (stoi(paramVal[10]) >= 0) && (stoi(paramVal[10]) <= 2))
														{
															unsigned char ctrlParams[10];
															
															ctrlParams[0] = stoi(paramVal[1]);//outdoor id
															ctrlParams[1] = stoi(paramVal[2]);//indoor id
															ctrlParams[2] = stoi(paramVal[3]);//swing
															ctrlParams[3] = stoi(paramVal[4]);//current temperature
															ctrlParams[4] = stoi(paramVal[5]);//air flow 
															ctrlParams[5] = stoi(paramVal[6]);//set temperature
															ctrlParams[6] = stoi(paramVal[7]);//filter reset
															ctrlParams[7] = (paramVal[8] == "378")? 78 : stoi(paramVal[8]);//mode
															ctrlParams[8] = stoi(paramVal[9]);//operation on/off
															ctrlParams[9] = stoi(paramVal[10]);//level
																																												
															size_t cmdCount=0;
															
															do
															{
																#ifdef DEBUG
																	cout << "control command sent before acknowledgement:" << cmdCount << "\n";
																#endif
																if((cmdCount < 15) || (ctrlAckRcvd != 1))//ctrl command sent 15 times max (In protocol given 10 times max)
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
															while(cmdCount < 15); //for three loops cmdCount <3
															
															#ifdef DEBUG
																cout << "ctrlAckRcvd:" << ctrlAckRcvd << "(flag 0-NOACK, 1-ACKRCVD)\n";
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
																cout<<"check for param=lev and its paramVal.\n";
															#endif
														}
													}
													else
													{
														#ifdef DEBUG
															cout<<"check for param=op and its paramVal.\n";
														#endif
													}
												}
												else
												{
													#ifdef DEBUG
														cout<<"check for param=mode and its paramVal.\n";
													#endif
												}
											}
											else
											{
												#ifdef DEBUG
													cout<<"check for param=filter and its paramVal.\n";
												#endif
											}
										}
										else
										{
											#ifdef DEBUG
												cout<<"check for param=set_temp and its paramVal.\n";
											#endif
										}
									}
									else
									{
										#ifdef DEBUG
											cout<<"check for param=air and its paramVal.\n";
										#endif
									}
								}
								else
								{
									#ifdef DEBUG
										cout<<"check for param=cur_temp and its paramVal.\n";
									#endif
								}
							}
							else
							{
								#ifdef DEBUG
									cout<<"check for param=swing and its paramVal.\n";
								#endif
							}
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
				
				//http://192.168.1.151:1107/110007?query=update&outid=00&inid=00
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
						cout<<"Invalid Query.\n\n";
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
