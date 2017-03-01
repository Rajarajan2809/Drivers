/*******************************************************************************
 *	Purpose		:	T connect to Mysql database.							   *
 * 	Description	:	This program is the header for obtaining mysql connection  *
 * 					to the database.			   	   						   *
 *	Author		:	Rajarajan.U												   *
 *	DOC 		:	24-Mar-16												   *
 * 	DOLM		:	20-Oct-16												   *
 *******************************************************************************/
 
#ifndef _mysqldb_hpp
#define _mysqldb_hpp

/*-----Mysql library-----*/
#include <mysql/mysql.h>

using namespace std;

/*******************************************************************************
 * Declaration of Class which perform Mysql operations.                        *
 * *****************************************************************************/
class testMysql
{
	private:
		void startAll(void);
		void finishAll(void);
		void finishWithError(MYSQL *con);
		MYSQL *con;
		
		/*-----Variables with mysql parameters-----*/		
		string DB,USER,PASSWORD,SERVER;
		
		/*-----mysqlFreeResult required-----*/
		bool freeResultReq;
				
	public:
		/*-----Variables for device's mysql operations-----*/
		MYSQL_RES *result;
				
		/*-----Function to perform mysql operations on IP2IR,IP2RELAY and RGB-----*/
		void mysqlQuery(string query);
		
		/*-----Constructor-----*/
		testMysql(string dbParam);
		
		/*-----Destructor-----*/
		~testMysql();
};

/*******************************************************************************
 * Constrctor for mysql class.			                                       *
 *******************************************************************************/
testMysql::testMysql(string dbParam)
{
	/*-----Mysql parameters-----*/
	DB=dbParam;
	USER ="cecotsmilan";
	PASSWORD="Eloka43@Cecots12";
	SERVER="localhost";
	freeResultReq =1;
}

/*******************************************************************************
 * Destructor for mysql class.			                                       *
 *******************************************************************************/
testMysql::~testMysql()
{
	/*-----Mysql parameters-----*/
	DB.clear();
	USER.clear();
	PASSWORD.clear();
	SERVER.clear();
	
	/*-----data variables-----*/
	if(freeResultReq == 0)		//if 0 then last query is 'select', then only 
		mysql_free_result(result);
}

/*******************************************************************************
 * Mysql Port or mysql service problem.                                        *
 * database name are shown here.                                               *
 *******************************************************************************/
void testMysql::finishWithError(MYSQL *con)
{
	fprintf(stderr, "%s\n", mysql_error(con));
	mysql_close(con);
	/*-----exit(1); Incase to terminate the program after mysql error.-----*/
}

/*-----Source:http://zetcode.com/db/mysqlc/-----*/

/*******************************************************************************
 * Function to start/open  mysql connection.                                   *
 *******************************************************************************/
void testMysql::startAll()
{
	/*-----start Mysql connection----*/	
	con = mysql_init(NULL);
	if (con == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		
		/*-----exit(1); Incase to terminate the program after mysql error.-----*/
	}
	if( mysql_real_connect(con, SERVER.c_str(), USER.c_str(), PASSWORD.c_str(), DB.c_str(), 0, NULL, 0) == NULL)
	{
		finishWithError(con);
	}
	
	/*----- Start other requirements ----*/
}

/*******************************************************************************
 * function to end/close mysql connection.                                     *
 *******************************************************************************/
void testMysql::finishAll()
{
	/*-----Finish Mysql connection 	----*/
	/*-----mysql_free_result(result); Release memory used to store results and close connection -----*/
	mysql_close(con);
	/*-----Finish other requirements ----*/
}

/*******************************************************************************
 * Function to perform required mysql operation for Z-wave devices. Only 	   *
 * function accessible for the object.                                         *
 *******************************************************************************/
void testMysql::mysqlQuery(string query)
{
	startAll();
	
	/*------Enter the code to insert into mysqldb-------*/
	
	/*------query to get Z-wave nodes Devices integrated-------*/
	//cout<<"qry : "<<mysqlQuery<<endl;
	if (mysql_query(con, query.c_str()))
	{
		finishWithError(con);
	}
	if(query.substr(0,6) == "SELECT")
	{
		freeResultReq =0; 		//It is Select query
		result = mysql_store_result(con);
		if (result == NULL)
		{
			finishWithError(con);
		}
	}
	else
		freeResultReq =1;
	finishAll();
}

#endif

