/*******************************************************************************
 *	Purpose		:	Trigger Execution in Slave Devices.			  			   *
 * 	Description	:	This is the Source for Trigger Execution in slave devices. *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	2-Jun-16												   *
 * 	DOLM		:	9-Nov-16												   *
 *******************************************************************************/
 
#include "trigger.hpp"

using namespace std;
using namespace cgicc;

vector<string> dumpList(const Cgicc& formData,int variableChosen) 
{
	vector<string> localVector;
	// Iterate through the vector, and print out each value
	const_form_iterator iter;
	for(iter = formData.getElements().begin();iter != formData.getElements().end();++iter)
	{
		switch(variableChosen)
		{
			case 1:
				if(iter->getName() == "instanceId")
				{
					localVector.push_back(iter->getValue());
				}
				break;
			
			case 2:
				if(iter->getName() == "propertyId")
				{
					localVector.push_back(iter->getValue());
				}
				break;
				
			case 3:
				if(iter->getName() == "propertyValue")
				{
					localVector.push_back(iter->getValue());
				}
				break;
				
			case 4:
				if(iter->getName() == "userId")
				{
					localVector.push_back(iter->getValue());
				}
				break;
				
			case 5:
				if(iter->getName() == "delay")
				{
					localVector.push_back(iter->getValue());
				}
				break;
			
			case 6:
				if(iter->getName() == "type")
				{
					localVector.push_back(iter->getValue());
				}
				break;
				
			case 7:
				if(iter->getName() == "ref1")
				{
					localVector.push_back(iter->getValue());
				}
				break;		
		}		
	}
	return localVector;
}

int main()
{
	try
	{
		Cgicc cgi;
		#ifdef DEBUG
			cout << "\t\t\t Trigger Slave Execution Starts\t\t\t\n\n";
		#endif
		vector<string> key1,key2,key3,key4,key5,key6,key7;
				
		//sample query -> http://localhost/Milan/MultipleValu.cgi?instanceId=11&propertyId=12&propertyValue=13&userId=14&ref_1=15&instanceId=21&propertyId=22&propertyValue=23&userId=24&ref_1=25
			
		key1 = dumpList(cgi,1);
		key2 = dumpList(cgi,2);
		key3 = dumpList(cgi,3);
		key4 = dumpList(cgi,4);
		key5 = dumpList(cgi,5);
		key6 = dumpList(cgi,6);
		key7 = dumpList(cgi,7);
		
		for(size_t loopNo1=0;loopNo1<key1.size();loopNo1++)
		{
			if(!key1.at(loopNo1).empty() && !key2.at(loopNo1).empty() && !key3.at(loopNo1).empty() && !key4.at(loopNo1).empty() && !key5.at(loopNo1).empty() && !key6.at(loopNo1).empty() && !key7.at(loopNo1).empty())
			{
				#ifdef DEBUG
					cout << "key1.at("<<loopNo1<<")\t\t\t:" << key1.at(loopNo1) << ",\n";
					cout << "key2.at("<<loopNo1<<")\t\t\t:" << key2.at(loopNo1) << ",\n";
					cout << "key3.at("<<loopNo1<<")\t\t\t:" << key3.at(loopNo1) << ",\n";
					cout << "key4.at("<<loopNo1<<")\t\t\t:" << key4.at(loopNo1) << ",\n";
					cout << "key5.at("<<loopNo1<<")\t\t\t:" << key5.at(loopNo1) << ",\n";
					cout << "key6.at("<<loopNo1<<")\t\t\t:" << key6.at(loopNo1) << ",\n";
					cout << "key7.at("<<loopNo1<<")\t\t\t:" << key7.at(loopNo1) << ".\n\n";
				#endif
			}
			else
			{
				if(key1.at(loopNo1).empty())
				{
					#ifdef DEBUG
						cout << "Instance id is empty.\n\n";
					#endif
					throw 1;
				}
				if(key2.at(loopNo1).empty())
				{
					#ifdef DEBUG
						cout << "Property id is empty.\n\n";
					#endif
					throw 1;
				}
				if(key3.at(loopNo1).empty())
				{
					#ifdef DEBUG
						cout << "Property value is empty.\n\n";
					#endif
					throw 1;
				}
				if(key4.at(loopNo1).empty())
				{
					#ifdef DEBUG
						cout << "User id is empty.\n\n";
					#endif
					throw 1;
				}
				if(key5.at(loopNo1).empty())
				{
					#ifdef DEBUG
						cout << "Type id is empty.\n\n";
					#endif
					throw 1;
				}
				if(key6.at(loopNo1).empty())
				{
					#ifdef DEBUG
						cout << "Delay id is empty.\n\n";
					#endif
					throw 1;
				}
				if(key7.at(loopNo1).empty())
				{
					#ifdef DEBUG
						cout << "ref1 is empty.\n\n";
					#endif
					throw 1;
				}
			}
		}
					
		//key.push_back("75B7418A-3E4D-4249-B129-2FAF82F61C6A");
		//keyValue.push_back("100");		
		if(!key1.empty() && !key2.empty() &&!key3.empty() && !key4.empty() && !key5.empty() && !key6.empty() && !key7.empty())
		{
			trigger t1;
			t1.triggerExecution(key1,key2,key3,key4,key5,key6,key7);
		}
				
		/*-----Command to Close the HTML document------*/
		#ifdef DEBUG
			cout<<"Trigger Slave Execution finished successfully";
		#endif
	}
	catch(exception& e)
	{
		#ifdef DEBUG
			cout<<"Exception handled, Trigger Slave Execution terminated, please check for errors.";
		#endif
	}
	return EXIT_SUCCESS;
}			
