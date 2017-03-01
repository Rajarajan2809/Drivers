#include <iostream>
#include <cstdlib>

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include <sys/utsname.h>
#include <sys/time.h>

using namespace std;
using namespace cgicc;

// Main Street, USA
int main()
{
	try 
	{
		timeval start;
		gettimeofday(&start, NULL);

		// Create a new Cgicc object containing all the CGI data
		Cgicc cgi;
		
		// Get a pointer to the environment
		const CgiEnvironment& env = cgi.getEnvironment();

		// Output the headers for an HTML document with the cookie only
		// if the cookie is not empty
		
		// Start the HTML body
		
		cout << "GNU cgicc version\t:" << cgi.getVersion() << ",\n";
		
		// Generic thank you message
		cout << "Method\t\t\t:" << env.getRequestMethod() << ",\n";
		cout << "Postdata\t\t:" << env.getPostData() << ",\n";
		cout << "Pathinfo\t\t:" << env.getPathInfo() << ",\n";
		cout << "QueryString\t\t:" << env.getQueryString() << ",\n";
		cout << "ContentLength\t\t:" << env.getContentLength() << ",\n";
		cout << "ContentType\t\t:" << env.getContentType() << ",\n";
		cout << "ServerSoftware\t\t:" << env.getServerSoftware() << ",\n";
		cout << "Remotehost\t\t:" << env.getRemoteHost() << ",\n";
		cout << "Remote Address\t\t:" << env.getRemoteAddr() << ",\n";
		cout << "Host\t\t\t:" << cgi.getHost() << ",\n";
		cout << "Server name\t\t:" << env.getServerName() << ",\n"; 
		cout << "Server Port\t\t:" << env.getServerPort() << ".\n";
		struct utsname info;
		if(uname(&info) != -1) 
		{
		  cout << "Host OS\t\t\t:" << info.sysname << ",\n" << "Host OS Version\t\t:" << info.release << ",\n" << "Host name\t\t:" << info.nodename << ",\n";
		}
		string queryName{},queryValue{};
		const_form_iterator i1{};
		for(i1 =cgi.getElements().begin(); i1 != cgi.getElements().end(); i1++)
		{
			//iter->getName();
			queryName = i1->getName();
			queryValue = i1->getValue();
			cout <<"queryName\t\t:" << queryName << ",\nqueryValue\t\t:" << queryValue << ".\n";
		}
		
		// Information on this query
		timeval end;
		gettimeofday(&end, NULL);
		long us = ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec);
		cout << "Total time for request :" << us << "us" << " (" << (double) (us/1000000.0) << "s)" << ".\n";

		// End of document

		// No chance for failure in this example
		return EXIT_SUCCESS;
	}

	// Did any errors occur?
	catch(exception& e) 
	{
		cout << "GNU cgi caught an exception."; 

		return EXIT_FAILURE;
	}
}
