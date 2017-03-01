/*******************************************************************************
 *	Purpose		:	Samsung DVM 4 AC Integration.							   *
 * 	Description	:	This program is the URI Parser Program.					   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	01-Jun-16												   *
 * 	DOLM		:	2-Dec-16												   *
 *******************************************************************************/
#ifndef _UriParser_hpp
#define _UriParser_hpp

#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <algorithm> 

using namespace std;

class Uri
{
	public:
			Uri(string q1);
			~Uri();
			string getFormData(string webQuery,string &request,string &ip);
			void paramBuilder(string webQuery,vector<string> &param,vector<string> &paramVal);

	private:
	
	protected:
			string webQuery;
			
};

/*******************************************************************************
 * Constructor for parseUri					 							   	   *
 *******************************************************************************/
 
Uri::Uri(string webQuery)
{
	this -> webQuery = webQuery;
}

/*******************************************************************************
 * Destructor for testGateway.					 							   *
 *******************************************************************************/
Uri::~Uri()
{
	
}

string Uri::getFormData(string webQuery,string &request,string &ip)
{
	vector<string> webQueryParams{};/*-----vector to store the Web query string -----*/
	
	while((webQuery.find(' ') != string::npos) || !webQuery.empty())
	{
		string temp1 = webQuery.substr(0,webQuery.find(' '));
		if(temp1.find('\n') != string::npos)
		{
			while((temp1.find('\n') != string::npos) || !temp1.empty())
			{
				string temp2 = temp1.substr(0,temp1.find('\n'));
				webQueryParams.push_back(temp2);
				if(temp1.find('\n') != string::npos)
					temp1.erase(0,webQuery.find('\n')+1);
				else
					temp1.erase(0,webQuery.size());
			}
		}
		else
		{
			webQueryParams.push_back(temp1);
		}
		
		if(webQuery.find(' ') != string::npos)
			webQuery.erase(0,webQuery.find(' ')+1);
		else
			webQuery.erase(0,webQuery.size());
	}
	/*for (size_t loopNo1 = 0; loopNo1 != webQueryParams.size(); loopNo1++)
	{
		cout << loopNo1 << "->" << webQueryParams.at(loopNo1) << '\n';
	}
	for (auto it1 = begin(webQueryParams); it1!=end(webQueryParams); ++it1)
	{
		cout << *it1 << '\n';
	}*/
	request = webQueryParams.at(0);
	
	vector<string>::iterator it2;
	it2 = find (webQueryParams.begin(), webQueryParams.end(), "Host:");
	if (it2 != webQueryParams.end())
	{
		ip = *(it2+1);
		ip = ip.substr(0,ip.find(':'));
	}
	return webQueryParams.at(1);/*-----Formdata-----*/	
}

void Uri::paramBuilder(string webQuery,vector<string> &param,vector<string> &paramVal)
{
	vector<string> eqn{};
	while((webQuery.find('&') != string::npos) || !webQuery.empty())
	{
		eqn.push_back(webQuery.substr(0,webQuery.find('&')));
		for (auto it1 = begin(eqn); it1!=end(eqn); ++it1)
		{
			//cout << *it1 << '\n';
		}
		while(eqn.back().find('=') != string::npos)
		{
			param.push_back(eqn.back().substr(0,eqn.back().find('=')));
			paramVal.push_back(eqn.back().substr(eqn.back().find('=')+1,eqn.back().back()));
			eqn.back().clear();
		}
		if(webQuery.find('&') != string::npos)
			webQuery.erase(0,webQuery.find('&')+1);
		else
			webQuery.erase(0,webQuery.size());
	}
}

#endif
