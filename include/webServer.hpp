#include <iostream>

using namespace std;

class webServer
{
	string port;
	
	public:
		webServer(string port);
		~webServer();
		int socketFd;
		int servInit();
};

webServer::webServer(string port)
{
	this->port = port;
}

webServer::~webServer(string port)
{
	this->port = port;
}

int webServer::servInit(string port)
{
	/*-----Tcp Server listening for Web query in port 5556.-----*/
	int socketStatus,setOptStatus,bindStatus,listenStatus,acceptFd,setOptVal = 1;/*-----socketFd -> socket descriptor for the socket connection; acceptFd -> socket descriptor for accepting socket connection; responseFd -> socket descriptor for sending data through socket connection.-----*/
	struct addrinfo host_info;
	struct addrinfo *host_info_list;
	struct sockaddr_storage their_addr;
	ssize_t byteReceived,byteSent;
	char webQuery[1000];
	socklen_t addr_size = sizeof(their_addr);
		
	/*-----Server specifications----*/
	memset(&host_info, 0, sizeof host_info);
	host_info.ai_family = AF_UNSPEC;     /*-----IP version not specified, Can be both IPv4 and IPv6.-----*/
	host_info.ai_socktype = SOCK_STREAM; /*-----Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.-----*/
	host_info.ai_flags = AI_PASSIVE;     /*-----IP Wildcard-----*/
	
	/*-----getting the status of the port on the ip.-----*/
	socketStatus = getaddrinfo(NULL, port.c_str(), &host_info, &host_info_list);
	if (socketStatus != 0)
	{
		/*-----Could not get address info.-----*/
		#ifdef DEBUG
			cout << "Could not get address info about the ip.\n";
		#endif
		throw "socket error.\n";
	}
	else
	{	
		/*-----Opening the socket.-----*/
		socketFd = socket(host_info_list->ai_family, host_info_list->ai_socktype,host_info_list->ai_protocol);
		if (socketFd == -1)  
		{
			/*-----socket error-----*/
			#ifdef DEBUG
				cout << "socket error:socket could not be opened.\n";
			#endif
			throw "socket error.\n";
		}
		else
		{
			/*-----Setting Options for the socket.-----*/
			setOptStatus = setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &setOptVal, sizeof(int));
			if(setOptStatus != 0)
			{
				/*-----Options not set-----*/
				#ifdef DEBUG
					cout << "Options not set.\n";
				#endif
				throw "socket error.\n";
			}
			else
			{
				/*-----Binding to the socket.-----*/
				bindStatus = ::bind(socketFd, host_info_list->ai_addr, host_info_list->ai_addrlen);/*-----error part in cpp not in c, error solved by adding "::"-----*/
				if (bindStatus == -1)  
				{
					/*----not binded for----*/
					#ifdef DEBUG
						cout<<"Port(1104) is already used by another Service, Please stop that service and proceed.\n";
					#endif
					throw "Port(1104) is already used by another Service, Please stop that service and proceed.\n";
				}
				else
				{
					/*-----While loop running infinite times-----*/
					while(1)
					{
						string webQueryStr{},formData{},response{},request{},ip{};
						vector<string> param{},paramVal{};
						listenStatus =  listen(socketFd, 5);
						if (listenStatus == -1)  
						{
							/*-----not listening-----*/
							#ifdef DEBUG
								cout << "not listening in port 1104.\n";
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
						response = "lonWorks";
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
						for (size_t loopNo1=0; ((loopNo1 < param.size()) && (loopNo1 < paramVal.size())); loopNo1++)
						{
							#ifdef DEBUG
								cout << param[loopNo1] << ':' << paramVal[loopNo1] << '\n';
							#endif
						}

						#ifdef DEBUG
							cout << "loopback ip:" << getMyIp("lo") << '\n';
						#endif

						if((ip == "127.0.0.1") || (ip == "localhost") || (ip == getMyIp("lo")))
						{
							if(intId == "110004")
							{
								if((param.size() > 0) && (paramVal.size() > 0) && (!param[0].empty()) && (!paramVal[0].empty()) && (param[0] == "local_instance_id") && (isInt(paramVal[0],10)) && (stoi(paramVal[0]) >= 0))
								{
									/*  sample query
										http://localhost:1104/110004?local_instance_id=xxx&mode=cool
										http://localhost:1104/110004?local_instance_id=xxx&mode=heat
										http://localhost:1104/110004?local_instance_id=xxx&mode=auto
										http://localhost:1104/110004?local_instance_id=xxx&mode=off
									*/
									if((param.size() > 1) && (paramVal.size() > 1) && (!param[1].empty()) && (!paramVal[1].empty()) && (param[1] == "mode") && ((paramVal[1] == "cool") || (paramVal[1] == "heat") || (paramVal[1] == "auto") || (paramVal[1] == "off")))
									{
										modeSet(paramVal[0],paramVal[1]);//local instance id,mode
									}
									/*  sample query
										http://localhost:1104/110004?local_instance_id=xxx&temp=yyy
									*/
									else if((param.size() > 1) && (paramVal.size() > 1) && (!param[1].empty()) && (!paramVal[1].empty()) && (param[1] == "temp")  && (isInt(paramVal[1],10)) && (stoi(paramVal[1]) >= 0))
									{
										tempSet(paramVal[0],paramVal[1]);//local instance id,temp
									}
									/*	sample query
										http://localhost:1104/110004?local_instance_id=xxx&fan=auto
										http://localhost:1104/110004?local_instance_id=xxx&fan=on
									*/
									else if((param.size() > 1) && (paramVal.size() > 1) && (!param[1].empty()) && (!paramVal[1].empty()) && (param[1] == "fan") && ((paramVal[1] == "auto") || (paramVal[1] == "on")))
									{
										fanSet(paramVal[0],paramVal[1]);//local instance id,fanmode
									}
									/*	sample query
										http://localhost:1104/110004?local_instance_id=xxx&cmd=hold
										http://localhost:1104/110004?local_instance_id=xxx&cmd=feedback
									*/
									else if((param.size() > 1) && (paramVal.size() > 1) && (!param[1].empty()) && (!paramVal[1].empty()) && (param[1] == "cmd") && ((paramVal[1] == "hold") || (paramVal[1] == "feedback")))
									{
										if(paramVal[1] == "feedback")//local instance id,feedback#FFFFFF
										{
											fb(paramVal[0]);//local instance id
										}
										else if(paramVal[1] == "hold")
										{
											//left intentionally
										}
									}
								}
								else
								{
									#ifdef DEBUG
										cout << "local_instance_id key is not present (keep it as first key, if present).\n\n";
									#endif
								}
							}
						}
						else
						{
							#ifdef DEBUG
								cout << "Oops..! Sorry No Access.\n\n";
							#endif
						}
						//erasing vector along with address
						if(param.size() > 0)
							param.clear();
						
						//erasing vector along with address
						if(paramVal.size() > 0)
							paramVal.clear();
						usleep(1000);
					}
				}
			}
		}
		/*-----cout << "Socket Closed."<<endl;-----*/
		freeaddrinfo(host_info_list);
		close(socketFd);
	}
}
