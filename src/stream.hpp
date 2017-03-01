/*******************************************************************************
 *	Purpose		:	Stream Music from/by Milan.						   		   *
 * 	Description	:	This is the header for Main Program to stream music.	   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	22-Nov-16												   *
 * 	DOLM		:	22-Nov-16												   *
 *******************************************************************************/
 
#ifndef _sun_exec_hpp
#define _sun_exec_hpp

/*-----CPP library-----*/
#include <iostream>
#include <chrono>		/*------Used to perform time operations.-------*/
#include <vector>		/*------Replacement for arrays (vectors are dynamically allocated).-------*/
#include <memory>		/*------for sharedptr-----*/
#include <stdexcept>	/*------for runtime_error-----*/
#include <sstream>	

/*-----Curl Library-----*/
#include <curl/curl.h>

/*-----CGI Headers-----*/
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

/*-----local header files-----*/
#include "../include/mysql.hpp"
#include "../include/cJSON.hpp"

using namespace std;

size_t CurlWrite_CallbackFunc_StdString(void *contents, size_t size, size_t nmemb, string *s);
string curlRequests(string webAddress,string type);

/*******************************************************************************
 * Class which sets sunrise / sunset time.	                                   *
 *******************************************************************************/
class stream
{
	public:
			/*-----Public Member Declaration-----*/
			stream();
			~stream();
			testMysql db;
			string speakerCmd(string cmd);
			
	private:
			string urlEncode2(string s);
			string exec(const char* cmd);
			size_t hex2ascii(const char *pSrc, unsigned char *pDst, size_t nSrcLength, size_t nDstLength);
			size_t ascii2hex(char* ascii_in, char* hex_out, size_t ascii_len, size_t hex_len);
			string ascii2hex(string text);
			string hex2ascii(string text);

};

//constructor
stream::stream():db("Milan_SP")
{
}

//destructor
stream::~stream()
{
	#ifdef DEBUG
		cout << "cmd executed successfully.";
	#endif
}

//sample query
string stream::speakerCmd(string cmd)
{
	string curlResponse{},speakerIp{},localInstanceId{},propertyId{};
	db.mysqlQuery("SELECT UAI.instance_id, UAI.addon_id, UA.addon_id FROM C_User_Addon_Instances AS UAI LEFT JOIN C_User_Addons AS UA ON UA.id = UAI.addon_id LEFT JOIN C_Addons_Properties AS AP ON AP.addon_id = UA.addon_id AND AP.property_id = '"+localInstanceId+"' WHERE UAI.local_instance_id = '"+propertyId+"' and UAI.instance_id IS NOT NULL and UAI.instance_id IS NOT NULL and UAI.addon_id IS NOT NULL AND UA.addon_id IS NOT NULL;");
	
	speakerIp = "35.65.2.165";	
	
	cmd = speakerIp+"/httpapi.asp?command="+cmd;
	
	#ifdef DEBUG
		cout << "speaker ip\t\t:" << speakerIp <<",\n";
		cout << "cmd\t\t\t:" << cmd <<".\n\n";
	#endif
	
	curlResponse = curlRequests(cmd,"GET");
	cout << "curl response:" << curlResponse << ".\n\n";
	
	return curlResponse;
}

//shell command returns string
string stream::exec(const char* cmd) 
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

//URL encoding using curl easy escape
string stream::urlEncode2(string encodeData)
{
	CURL *curl = curl_easy_init();
	string encodedData{};
	if(curl) 
	{
		encodedData = curl_easy_escape(curl, encodeData.c_str(), encodeData.length());
		if(!encodedData.empty()) 
		{
			//cout<<"encodedData->"<<encodedData<<endl;
		}
		//curl_free(output);
	}
	return encodedData;
}

string stream::ascii2hex(string text)
{
	stringbuf buffer;             // empty buffer
	ostream os (&buffer);      // associate stream buffer to stream
	
	for(size_t i = 0 ; i < text.length() ; i++)
	{
		os<<hex<<(int)text[i];
	}
	return buffer.str();
}

string stream::hex2ascii(string text)
{
	int len = text.length();
	string newString{};
	for(int i=0; i< len; i+=2)
	{
		string byte = text.substr(i,2);
		char chr = (char) (int)strtol(byte.c_str(), NULL, 16);
		newString.push_back(chr);
	}
	return newString;
}

//hexadecimal to ascii
size_t stream::hex2ascii(const char *pSrc, unsigned char *pDst, size_t nSrcLength, size_t nDstLength)
{
	size_t i, j = 0;
	memset(pDst, 0, nDstLength);
	for (i = 0; i<nSrcLength; i+=2 )
	{
		char val1 = pSrc[i];
		char val2 = pSrc[i+1];
		if( val1 > 0x60) val1 -= 0x57;
		else if(val1 > 0x40) val1 -= 0x37;
		else val1 -= 0x30;
		if( val2 > 0x60) val2 -= 0x57;
		else if(val2 > 0x40) val2 -= 0x37;
		else val2 -= 0x30;
		if(val1 > 15 || val2 > 15 || val1 < 0 || val2 < 0)
		return 0;
		pDst[j] = val1*16 + val2;
		j++;
	}
	return j;
}

//ascii to hexadecimal
size_t stream::ascii2hex(char* ascii_in, char* hex_out, size_t ascii_len, size_t hex_len)
{
	const char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8','9', 'A', 'B', 'C', 'D', 'E', 'F'};
	size_t i = 0, ret = 0;
	memset(hex_out, 0, hex_len);
	while( i < ascii_len )
	{
		int b= ascii_in[i] & 0x000000ff;
		hex_out[i*2] = hex[b/16] ;
		hex_out[i*2+1] = hex[b%16] ;
		++i;
		ret += 2;
	}
	return ret;
}
//IP:	35.65.2.165:80 MAC Vendor:	Linkspri

#endif
