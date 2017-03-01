/*******************************************************************************
 *	Purpose		:	Stream Music from/by Milan.						   		   *
 * 	Description	:	This is the Main Program to stream music.				   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	22-Nov-16												   *
 * 	DOLM		:	22-Nov-16												   *
 *******************************************************************************/
 
 /*-----local files-----*/
#include "stream.hpp"

using namespace std;
using namespace cgicc;

/****************************************************************************************
 * Function to store curl response in DB.												*
 ****************************************************************************************/
size_t CurlWrite_CallbackFunc_StdString(void *contents, size_t size, size_t nmemb, string *s)
{
    size_t newLength = size*nmemb;
    size_t oldLength = s->size();
    try
    {
        s->resize(oldLength + newLength);
    }
    catch(std::bad_alloc &e)
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
string curlRequests(string webAddress,string type)
{
	CURL *curl;
    CURLcode res;
	string curlResponse;
    curl_global_init(CURL_GLOBAL_DEFAULT);
	
    curl = curl_easy_init();
    
    if(curl)
    {
		curl_easy_setopt(curl, CURLOPT_URL, webAddress.c_str());
		if(type == "POST")
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlResponse);
        

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
	return curlResponse;
}

int main()
{
	try
	{
		/*-----Declaration-----*/
		Cgicc cgi;
		const CgiEnvironment& env = cgi.getEnvironment();
		
		#ifdef DEBUG
			cout << "\n";
			cout << "Remotehost\t\t:" << env.getRemoteHost() << ",\n";
			cout << "Remote Address\t\t:" << env.getRemoteAddr() << ",\n";
			cout << "Server name\t\t:" << env.getServerName() << ".\n\n";
		#endif
		
		//if((env.getRemoteAddr() == env.getServerName()) || (env.getRemoteHost() == "127.0.0.1"))
		//{
			/*-----getting the User id-----*/
			string cmd{};
			for(int loopNo1=1;loopNo1 <= 6;loopNo1++)
			{
				switch(loopNo1)
				{
					case 1:
						{
							form_iterator i1 = cgi.getElement("cmd");
							if(i1 != cgi.getElements().end())
							{
								cmd = i1->getValue();
							}
						}
						break;					
				}
			}
			
			#ifdef DEBUG		
				cout << "Cmd\t\t\t:" << cmd << ",\n";
			#endif		
			
			//cmd = XXXXXXXXXXXXXXXX;
			//Example query:http://localhost:6161/Milan/Drivers/MILAN_EXE/STREAM/stream.cgi?cmd=TS0010
			if(!cmd.empty())
			{
				stream s1;
				s1.speakerCmd(cmd);
			}
			else
			{
				if(cmd.empty())
				{
					#ifdef DEBUG
						cout << "cmd is empty.\n\n";
					#endif
					throw 1;
				}
			}
			
			/*-----Command to Close the HTML document------*/
			
		//}
		//else
		//{
			//#ifdef DEBUG
				//cout << "Requests from foreign systems prohibited.";
			//#endif
		//}
	}
	catch(exception& e) 
	{
		/*-----handle any errors - omitted for brevity-----*/
		#ifdef DEBUG
			cout << "Exception handled at stream, Program terminated, please check for errors.";
		#endif
	}
}
