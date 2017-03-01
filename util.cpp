/*******************************************************************************
 *	Purpose		:	Utility.									  			   *
 * 	Description	:	Clears log and monitors devices.						   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	2-Jun-16												   *
 * 	DOLM		:	27-Oct-16												   *
 *******************************************************************************/

/*-----CPP library-----*/
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

/*-----C library-----*/
#include <cassert>

/*-----Local Macros-----*/
#define USED_MAIN_MEMORY_THRESHOLD 90
#define USED_PERSISTANT_MEMORY_THRESHOLD 98
#define IND_MAIN_MEMORY_THRESHOLD 30
#define IND_CPU_THRESHOLD 60

using namespace std;

string exec(const char* cmd) 
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

void strSpaceOp(string &s1,string &s2)
{
	if((s1.find(" ") != string::npos) && (s1.substr(s1.find(" "),1) == " "))
	{
		if((s1.find(" ") != string::npos) && (s1.substr(s1.find(" "),2) == "  "))
		{
			if(((s1.find(" ") != string::npos) && s1.substr(s1.find(" "),3) == "   "))
			{
				//3 spaces success
				s2 = s1.substr(0,s1.find(" "));
				s1.erase(0,s1.find(" ")+3);
			}
			else
			{
				//2 spaces success
				s2 = s1.substr(0,s1.find(" "));
				s1.erase(0,s1.find(" ")+2);
			}
		}
		else
		{
			s2 = s1.substr(0,s1.find(" "));
			s1.erase(0,s1.find(" ")+1);
		}
	}
	//return s2;
}

int main()
{
	vector<string> diskU{},diskC{};
	string cpuUsage = exec("grep 'cpu ' /proc/stat | awk '{usage=($2+$4)*100/($2+$4+$5)} END {print usage \"%\"}'");
	string mainMemory = exec("echo 'Cecots!*52764^Eloka' | sudo -S free | awk 'FNR == 3 {print $3/($3+$4)*100 \"%\"}'");
	
	#ifdef DEBUG
		cout << "cpuUsage is : " << ((cpuUsage.find("\n") != string::npos)?cpuUsage.erase(cpuUsage.find("\n")):cpuUsage) << endl;
		cout << "mainMemory is : " << ((mainMemory.find("\n") != string::npos)?mainMemory.erase(mainMemory.find("\n")):mainMemory) << endl;
	#endif
	
	if(stoi(mainMemory) > USED_MAIN_MEMORY_THRESHOLD)
	{
		#ifdef DEBUG
			cout << "RAM MEMORY THRESHOLD REACHED[" << USED_MAIN_MEMORY_THRESHOLD << "]. INITIATING REBOOT..." << endl;
			cout << "cmd : sudo reboot" << endl;
		#endif
		exec("sudo reboot");
	}
	else
	{
		#ifdef DEBUG
			cout << "USED:" << mainMemory << " [Action threshold \"" << USED_MAIN_MEMORY_THRESHOLD<< "%\"]. No action required." << endl;
		#endif
	}	
	string diskUsage = exec("df -kh |grep -v 'Filesystem' | awk '{ print $5 }' | sed 's/%//g'");
	
	#ifdef DEBUG
		cout << "Total diskUsage :	" << diskUsage << endl;
	#endif
	for(;;)
	{
		if(!diskUsage.empty())
		{
			if(diskUsage.find("\n") != string::npos)
			{
				#ifdef DEBUG
					cout<<"diskUsage :	" << diskUsage <<endl;
				#endif
				diskU.push_back(diskUsage.substr(0,diskUsage.find("\n")));
				diskUsage.erase(0,diskUsage.find("\n")+1);
			}
			//break;
		}
		else
			break;
	}
		
	string diskCapacity = exec("df -k |grep -v 'Filesystem' | awk '{ print $2 }'");
	
	#ifdef DEBUG
		cout << "disk total Capacity :	" << diskCapacity << "\n\n";
	#endif
	
	for(;;)
	{
		if(!diskCapacity.empty())
		{
			diskC.push_back(diskCapacity.substr(0,diskCapacity.find("\n")));
			diskCapacity.erase(0,diskCapacity.find("\n")+1);
		}
		else
			break;
	}
	
	#ifdef DEBUG
		cout << "Disk Capacity\t\tDisk Usage" << endl;
	#endif
	
	for(size_t loopNo1 =0; (loopNo1 < diskC.size()); loopNo1++)
	{
		#ifdef DEBUG
			cout << diskC.at(loopNo1) << "\t\t\t" << 	diskU.at(loopNo1) << endl;
		#endif
		if(stol(diskC.at(loopNo1)) > stol(diskU.at(loopNo1)))
		{
			string logRemove = exec("sudo apt-get autoclean ; sudo apt-get clean ; sudo rm /var/mail/root ; sudo /etc/cron.daily/logrotate ; sudo find /var/log -type f -name '*.gz' -delete ; sudo find /var/log -type f -name '*.1' -delete ; sudo find /var/log -type f -name '*.0' -delete ; sudo truncate -s 0 /var/log/asterisk/* ; sudo truncate -s 0 /var/log/apt/* ; sudo truncate -s 0 /var/log/lighttpd/* ; sudo truncate -s 0 /var/log/mysql/* ; sudo truncate -s 0 /var/log/* ; sudo truncate -s 0 /var/log/fsck/* ; sudo truncate -s 0 /var/log/ntpstats/* ; sudo truncate -s 0 /var/log/news/* ; sudo truncate -s 0 /var/log/upstart/* ; sudo truncate -s 0 /freepbx-bounce_op.log /JsonServer.log /Z-Wave.log freepbx-bounce_op.log JsonServer.log Z-Wave.log /var/www/Milan/MILAN_API/JSON_SERVER/JsonServer.log");
			//string logRemove = exec("sudo apt-get autoclean || sudo apt-get clean || sudo rm /var/mail/root || sudo /etc/cron.daily/logrotate || sudo find /var/log -type f -name '*.gz' -delete || sudo find /var/log -type f -name '*.1' -delete || sudo find /var/log -type f -name '*.0' -delete || sudo truncate -s 0 /var/log/* || sudo truncate -s 0 /var/log/asterisk/* || sudo truncate -s 0 /var/log/apt/* || sudo truncate -s 0 /var/log/lighttpd/* || sudo truncate -s 0 /var/log/mysql/* || sudo truncate -s 0 /var/log/upstart/* || sudo truncate -s 0 /var/log/apache2/* || sudo truncate -s 0 /var/log/installer/*");
			#ifdef DEBUG
				cout << "logRemove : " << logRemove << endl;
			#endif
		}
		else
		{
			string logRemove = exec("sudo apt-get autoclean ; sudo apt-get clean ; sudo rm /var/mail/root ; sudo /etc/cron.daily/logrotate ; sudo find /var/log -type f -name '*.gz' -delete ; sudo find /var/log -type f -name '*.1' -delete ; sudo find /var/log -type f -name '*.0' -delete ; sudo truncate -s 0 /var/log/asterisk/* ; sudo truncate -s 0 /var/log/apt/* ; sudo truncate -s 0 /var/log/lighttpd/* ; sudo truncate -s 0 /var/log/mysql/* ; sudo truncate -s 0 /var/log/* ; sudo truncate -s 0 /var/log/fsck/* ; sudo truncate -s 0 /var/log/ntpstats/* ; sudo truncate -s 0 /var/log/news/* ; sudo truncate -s 0 /var/log/upstart/* ; sudo truncate -s 0 /freepbx-bounce_op.log /JsonServer.log /Z-Wave.log freepbx-bounce_op.log JsonServer.log Z-Wave.log /var/www/Milan/MILAN_API/JSON_SERVER/JsonServer.log");
			//string logRemove = exec("sudo apt-get autoclean || sudo apt-get clean || sudo rm /var/mail/root || sudo /etc/cron.daily/logrotate || sudo find /var/log -type f -name '*.gz' -delete || sudo find /var/log -type f -name '*.1' -delete || sudo find /var/log -type f -name '*.0' -delete || sudo truncate -s 0 /var/log/* || sudo truncate -s 0 /var/log/asterisk/* || sudo truncate -s 0 /var/log/apt/* || sudo truncate -s 0 /var/log/lighttpd/* || sudo truncate -s 0 /var/log/mysql/* || sudo truncate -s 0 /var/log/upstart/* || sudo truncate -s 0 /var/log/apache2/* || sudo truncate -s 0 /var/log/installer/*");
			#ifdef DEBUG
				cout << "logRemove : " << logRemove << endl;
			#endif
		}
	}
	
	#ifdef DEBUG
		cout << "\n\nCPU % Threshold check.\n\n" ;
	#endif
	
	string processStatus = exec("sudo ps eaxco pmem,pcpu,pid,cmd | sort -k 1 -nr | head -15");
	
	for(size_t loopNo1 =0; processStatus.find("\n") != string::npos; loopNo1++)
	{
		string lineString = processStatus.substr(0,processStatus.find("\n"));
				
		for(size_t loopNo2 =0; !lineString.empty(); loopNo2++)
		{
			if(loopNo1 != 0)
			{	
				lineString = lineString.erase(0,lineString.find(" ")+1);
			}
			string procId{},cpuPercent{},ramPercent{};
			
			strSpaceOp(lineString,ramPercent);
			strSpaceOp(lineString,cpuPercent);
			strSpaceOp(lineString,procId);
			
			#ifdef DEBUG			
				cout << "Process Name:" << lineString << ".\n"; //lineString contains process name only.
				cout << "RAM:" << stod(ramPercent) << "%.\n";
				cout << "CPU:" << stod(cpuPercent) << "%.\n";
				cout << "Process id:" << procId << ".\n\n";
			#endif
				
			if(stod(ramPercent) > IND_MAIN_MEMORY_THRESHOLD)//RAM threshold
			{
				string cmd = cmd+"sudo kill -HUP "+procId.c_str();
				#ifdef DEBUG
					cout << "cmd:" << cmd << "\n\n";
				#endif
				exec(cmd.c_str());
			}
			
			if(stod(cpuPercent) > IND_CPU_THRESHOLD)
			{
				string cmd = cmd+"sudo kill -HUP "+procId.c_str();
				#ifdef DEBUG
					cout << "cmd:" << cmd << "\n\n";
				#endif
				exec(cmd.c_str());
			}
			lineString.clear();
		}
		processStatus.erase(0,processStatus.find("\n")+1);
		//cout << "after loop processStatus->" << processStatus << endl;
	}
		
	#ifdef DEBUG
		cout << "Router check started." << "\n\n";
	#endif
	
	string routerStatus = exec("ip route show | grep -i 'default via'| awk '{print $3 }' | fping ");
	
	if(routerStatus.find("alive") != string::npos)
	{
		#ifdef DEBUG	
			cout << "routerStatus : " << routerStatus << "\n\n";
		#endif
	}
	
	return EXIT_SUCCESS;
}
