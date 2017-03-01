#include <string>
#include "../MILAN_CLASSES/ABSTRACT/mysql.hpp"	
#include <regex>
#include "../MILAN_CLASSES/ABSTRACT/UriParser.hpp"
#include "../MILAN_CLASSES/ABSTRACT/Serial.hpp"
#include "../MILAN_CLASSES/socket.hpp"
#include <thread>

using namespace std;
class eloka
{
	private:
		int socketSend(string jsonData);
		int openPort(string port,unsigned long int baudrate);
		int readSerial(string &recvBuf);
		static int fd;
		
	public:
		string serDevice;
		int server_listen();
		int serial_initialize();
};

int eloka::fd=0;

int eloka::serial_initialize()
{
	int len{};
	string buffer{},id{},lev{},sql{},jsonUpdate{},lightId{},controlType{},integrationId{},category{},roomId{};
	
	serDevice = "/dev/ttyS2";
	cout << "Serial Port:" << serDevice << '\n';
	if(openPort(serDevice,9600))
	{
		while(1)
		{
			len = readSerial(buffer);

			if(len>1)
			{
				cout << "buffer:" << buffer << '\n';
				while(buffer.find('\n') !=string::npos)
				{
					string data = buffer.substr(0,buffer.find('\n'));
					
					cout << "data:" << data << '\n';
					
					id = buffer.substr(2,3);
					lev = buffer.substr(5,2);
					lev = (lev == "99")?"100":lev;
					
					cout << "light id:" << id << '\n';
					cout << "level:" << lev << "\n\n";
					
					//db fetch here
										
					lightId			="001";
					controlType		="2";
					integrationId	="60000";
					category		="Light1";//only for printing purpose
					roomId			="1";
					
					jsonUpdate = "{\"method\":\"update\",\"params\":[{\"category\":\""+category+"\",\"room_id\":\""+roomId+"\",\"model\":\""+integrationId+"\",\"args\":[\""+lightId+"\",\"\",\"\"\""+lev+"\",\""+controlType+"\",]}]}\n";
					cout << "json:" << jsonUpdate << '\n';
					buffer.erase(0,buffer.find('\n')+1);
					//socketSend(jsonUpdate);
				}
				buffer.clear();
			}
		}
		close(serialPort);
	}
	
	return 0;
}

int eloka::server_listen()
{
	string line{},formData{},result{},key{},keyValue{},intid{},lightid{},level{},userid{},light_id{},lightCmd{};
	socketuri socketuriObj{};
	int status{},new_sd{},socket_fd{};	
	socket_fd = socketuriObj.getFormsocket("6001");
	
	while(1)
	{
		status = listen(socket_fd, 5);
		if ( status == -1 )  
		{
			cout << "Could not be able to listen on this port.(6007)\n";
		}
		struct sockaddr_storage their_addr;
		socklen_t addr_size = sizeof(their_addr);
		new_sd = accept(socket_fd, (struct sockaddr *)&their_addr, &addr_size);
		if (new_sd 	== -1)
		{
			cout << "Could not be able to accept connections on this port.(6007)\n";
		}
		else
		{
			//cout << "Connection accepted. Using new socketfd : "  <<  new_sd << endl;
		
			ssize_t bytes_recieved;
			char incomming_data_buffer[1000];
			bytes_recieved = recv(new_sd, incomming_data_buffer,1000, 0);
			if (bytes_recieved != -1)
			{
				parseUri uri{}; //uri
				formData	= uri.getFormData(incomming_data_buffer);
				keyValue 	= uri.get(formData,"intid");
				intid		= keyValue;
				keyValue 	= uri.get(formData,"lightid");
				lightid		= keyValue;
				keyValue 	= uri.get(formData,"level");
				level		= keyValue;
				keyValue 	= uri.get(formData,"userid");
				userid		= keyValue;
				//keyValue 	= uri.get(formData,"l_instid");
				//l_instid	= keyValue;
				if(level=="0")
					level="00";
				close(new_sd);
				//serial serial_write;
				if(level=="stop")
				{
					lightCmd="^E"+lightid+"50"+"00";
				}
				else
				{
					lightCmd="^E"+lightid+level+"00";
				}
				
				if(write(fd,lightCmd.c_str(),lightCmd.length()))
				{
					cout << "sent command :" << lightCmd << "\n\n";
				}
				lightCmd.clear();
			}
		}
	}
	close(socket_fd);
	return 0;
}

int eloka::socketSend(string jsonData)
{
	/*-----Tcp Client for sending IR commands.-----*/
	int socketStatus,connectStatus,socketFd;  /*-----socketFd -> socket descriptor for the socket connection.-----*/
	struct addrinfo host_info;       
	struct addrinfo *host_info_list; 
	
	memset(&host_info, 0, sizeof host_info);
	host_info.ai_family = AF_UNSPEC;     /*-----IP version not specified, Can be both IPv4 and IPv6.-----*/
	host_info.ai_socktype = SOCK_STREAM; /*-----Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.-----*/
	
	/*-----getting the status of the port on the ip.-----*/
	socketStatus = getaddrinfo("localhost", "64526", &host_info, &host_info_list);
	if (socketStatus != 0)
	{
		/*-----Could not get address info.-----*/
		cout<<"Could not get address info."<<endl;
		freeaddrinfo(host_info_list);
		/*-----address info not received return value-----*/
		return 1;
	}
	else
	{
		/*-----address info received-----*/
		/*-----Opening the socket.-----*/
		socketFd = socket(host_info_list->ai_family, host_info_list->ai_socktype,host_info_list->ai_protocol);
		if (socketFd == -1)  
		{
			/*-----socket error-----*/
			cout<<"socket error"<<endl;
			freeaddrinfo(host_info_list);
			/*-----socket not created return value-----*/
			return 2;
		}
		
		/*-----Connecting to the socket.-----*/
		connectStatus = connect(socketFd, host_info_list->ai_addr, host_info_list->ai_addrlen);
		if (connectStatus == -1) 
		{
			/*-----not connected-----*/
			cout<<"not connected"<<endl;
			close(socketFd);
			freeaddrinfo(host_info_list);
			/*-----socket not connected return value-----*/
			return 3;
		}
		else
		{
			/*-----connected-----*/
			send(socketFd, jsonData.c_str(), strlen(jsonData.c_str()), 0);
		}
	}
	freeaddrinfo(host_info_list);
	close(socketFd);
	return 4;
}

int eloka::openPort(string port,unsigned long int baudrate)
{
	fd = open(port.c_str(),O_RDWR| O_NOCTTY | O_NONBLOCK);
	struct termios portterm;
	if (fd<0) 
	{
		cout << "Port:" << port << " is busy.\n";
		//throw(fd);
	}
	else
	{
		cout << "Port:" << port << " is working fine.\n";
		tcgetattr(fd,&portterm); /* save current serial port settings */
	  	bzero(&portterm, sizeof(portterm)); /* clear struct for new port settings */
		tcgetattr(fd,&portterm);
				
		portterm.c_iflag = IGNPAR | IGNBRK ;
		portterm.c_cflag |= CREAD;
		portterm.c_cflag |= CS8;
		portterm.c_iflag |= IGNPAR;
		portterm.c_lflag &= ~(ICANON);
		portterm.c_lflag &= ~(ECHO);
		portterm.c_lflag &= ~(ECHOE);
		portterm.c_iflag &= ~(ICRNL | BRKINT | IMAXBEL);
		portterm.c_lflag &= ~(ISIG);
		portterm.c_cc[VMIN]=1;
		portterm.c_cc[VTIME]=0;
		cfsetispeed(&portterm, baudrate);
        cfsetospeed(&portterm, baudrate);
		
		// Setting other Port Stuff
		portterm.c_cflag     &=  ~PARENB;        // Make 8n1
		portterm.c_cflag     &=  ~CSTOPB;
		portterm.c_cflag     &=  ~CSIZE;
		portterm.c_cflag     |=  CS8;
		portterm.c_oflag &= ~(OPOST);		//  When the OPOST option is disabled, all other option bits in c_oflags are ignored.
		portterm.c_oflag &= ~(ONLCR);
		portterm.c_lflag &= ~(IEXTEN);			// Disable extended functions
		portterm.c_lflag &= ~(ECHOE | ECHOK | ECHOCTL |ECHOKE);
		tcflush(fd, TCIFLUSH);
		tcsetattr(fd, TCSANOW, &portterm);
		    		
	}	
	return fd;
}

int eloka::readSerial(string &recvBuf)
{
	ssize_t numRead;                    /* # of bytes fetched by last read() */
    size_t totRead;                     /* Total bytes read so far */
//char *buf;
    char ch;

    if (10 <= 0 || recvBuf.c_str() == NULL) 
    {
        errno = EINVAL;
        return -1;
    }

    recvBuf.c_str();                       /* No pointer arithmetic on "void *" */

    totRead = 0;
    for (;;) 
    {
        numRead = read(fd, &ch, 1);

        if (numRead == -1) 
        {
            if (errno == EINTR)         /* Interrupted --> restart read() */
                continue;
            else
                return -1;              /* Some other error */

        } 
        else if (numRead == 0) 
        {      /* EOF */
            if (totRead == 0)           /* No bytes read; return 0 */
                return -1;
            else                        /* Some bytes read; add '\0' */
                break;

        }
        else
        {                        /* 'numRead' must be 1 if we get here */
            if (totRead < 10 - 1) 
            {      /* Discard > (n - 1) bytes */
                totRead++;
                //*buf++ = ch;
                recvBuf.push_back(ch);
            }

            if (ch == '\n')
                break;
        }
    }

   // *buf = '\0';
    return totRead;
}


int main() 
{
	try
	{
		thread first(&eloka::server_listen,eloka());
		thread second(&eloka::serial_initialize,eloka());
		first.join();
		second.join();
	}
	catch (exception& e)
	{
		cerr << "exception caught: " << e.what() << '\n';
	}
	return 0;
}
