/*******************************************************************************
 *	Purpose		:	Media notification.							   			   *
 * 	Description	:	This is the Main Program to send notifications to media.   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	03-Jan-17												   *
 * 	DOLM		:	09-Jan-17												   *
 *******************************************************************************/
/*-----CPP library-----*/
#include "mediaNotify.hpp"

using namespace std;
using namespace cgicc;

/*******************************************************************************
 * Main program to perofrm Media notification.								   *
 *******************************************************************************/
int main()
{
	try
	{
		string locInsId{},catId{},devStatus{};
		/*-----Declaration-----*/
		timeval start;
		gettimeofday(&start, NULL);
		/*-----Declaration-----*/
		Cgicc cgi;
		const CgiEnvironment& env = cgi.getEnvironment();
		cout << HTTPHTMLHeader();
        // Output the HTML 4.0 DTD info
		cout << HTMLDoctype(HTMLDoctype::eStrict) << br();
		cout << html().set("lang", "en").set("dir", "ltr");
		// Set up the page's header and title.
		// I will put in lfs to ease reading of the produced HTML. 
		cout << head() << endl;

		// Output the style sheet portion of the header
		cout << title() << "Media Notification." << title();
		cout << head();
		
		// Start the HTML body
		cout << body();
		
		#ifdef DEBUG
			cout << "Method:" << env.getRequestMethod() << br();
			cout << "Postdata:" << env.getPostData() << br();
			cout << "Pathinfo:" << env.getPathInfo() << br();
			cout << "QueryString:" << env.getQueryString() << br();
			cout << "ContentLength:" << env.getContentLength() << br();
			cout << "Remote Address:" << env.getRemoteAddr() << br();
		#endif
		
		
		string queryName{},queryValue{};
		/*const_form_iterator i1{};
		for(i1 =cgi.getElements().begin(); i1 != cgi.getElements().end(); i1++)
		{
			//iter->getName();
			queryName = i1->getName();
			queryValue = i1->getValue();
			cout <<"queryName:" << queryName << br() << "queryValue:" << queryValue << br();
		}*/

		/*-----getting the query params-----*/
		form_iterator i1 = cgi.getElement("local_instance_id");
		if(i1 != cgi.getElements().end())
		{
			locInsId = i1->getValue();
		}
		
		form_iterator i2 = cgi.getElement("cat_id");
		if(i2 != cgi.getElements().end())
		{
			catId = i2->getValue();
		}
		
		form_iterator i3 = cgi.getElement("device_status");
		if(i3 != cgi.getElements().end())
		{
			devStatus = i3->getValue();
		}
		
		//devId = "1";
		//devStatus = "1";
		//Example query:http://localhost/Milan/Drivers/MILAN_EXE/PUSH/medNot.cgi?local_instance_id=123&device_id=1&device_status=1
		if(!locInsId.empty() && !catId.empty()  && !devStatus.empty())
		{
			#ifdef DEBUG
				cout << "local instance id:" << locInsId << br();
				cout << "category id:" << catId << br();
				cout << "Device status:" << devStatus << br() << br();
			#endif
			mediaNotify N(locInsId,catId,devStatus);
			#ifdef DEBUG
				N.notify();
			#else
				if((N.getMyIp("wlan0") == env.getRemoteAddr()) || (N.getMyIp("eth0") == env.getRemoteAddr()) || (N.getMyIp("lo") == env.getRemoteAddr()))
					N.notify();
			#endif
		}
		else
		{
			if(locInsId.empty())
			{
				#ifdef DEBUG
					cout << "local instance Id is empty." << br();
				#endif
			}

			if(catId.empty())
			{
				#ifdef DEBUG
					cout << "category Id is empty." << br();
				#endif
			}
			
			if(devStatus.empty())
			{
				#ifdef DEBUG
					cout << "Device Status is empty." << br();
				#endif
			}
			throw 1;
		}
		#ifdef DEBUG
			cout << "Media triggered successfully." << br();
		#endif
		timeval end;
		gettimeofday(&end, NULL);
		
		#ifdef DEBUG
			long us = ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec);
			cout << "Total time for request :" << us << "us" << " (" << (double) (us/1000000.0) << "s)";
		#endif
		cout << body() << html();
	}
	catch(exception& e) 
	{
		/*-----handle any errors - omitted for brevity-----*/
		#ifdef DEBUG
			cout<<"Exception handled, Media notification process terminated, Please Check for errors.";
		#endif
	}
}
