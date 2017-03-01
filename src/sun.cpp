/*******************************************************************************
 *	Purpose		:	Sun Rise / Sun set time.						   		   *
 * 	Description	:	This is the header for Main Program to calculate the Sun   *
 * 					rise and Sun set time.									   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	12-Sep-16												   *
 * 	DOLM		:	31-Oct-16												   *
 *******************************************************************************/

/*-----CPP Headers-----*/
#include "sun.hpp"

using namespace std;

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

/*******************************************************************************
 * Main program to perofrm scene Execution.									   *
 *******************************************************************************/
int main(int count, char* timeZone[])
{
	try
	{
		sun s1;
		if(count == 2)
			s1.timeZoneUpdate(timeZone[1]);
		else
		{
			switch(s1.riseSetDBUpdate())
			{
				case 0:
					cout<<"NTP Server is offline."<<endl;
					break;
				
				case 1:
					cout<<"No API key in Database."<<endl;
					break;
				
				case 2:
					cout<<"No Serial Number in Database."<<endl;
					break;
					
				case 3:
					cout<<"Encryption failed."<<endl;
					break;
					
				case 4:
					cout<<"{\"Update\":\"Success\"}"<<endl;
					break;
					
				case 5:
					cout<<"{\"Update\":\"failed\"}"<<endl;
					break;	
					
			}
		}
	}
	catch(exception& e) 
	{
		/*-----handle any errors - omitted for brevity-----*/
	}
}

//curl -s --head http://0.ubuntu.ntp.pool.org | grep ^Date: | sed 's/Date: //g'

