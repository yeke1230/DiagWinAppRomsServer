/***********************************************************************
* Filename: DBConnectPool.h
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Define the database connect pool.
*
* Copyright Information ?FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
* Distributed under the MySQL Connector Software License, Version 1.1.9
* Download from https://dev.mysql.com/downloads/connector/cpp/
**********************************************************************/

#pragma once
#include <functional>
#include <mutex>
#include <boost/shared_ptr.hpp>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <exception>

using namespace sql;
using delFunc = std::function<void(Connection*)>;

typedef std::shared_ptr<Connection> ConnectionPtr;

class ConnectionPool {
public:
	static ConnectionPool* GetInstance();

	/***********************************************************************
	* Module Name: GetConnect
	*
	* Description: Get a database connection.
	*			   It is syschronized way by using mutex.
	***********************************************************************/
	ConnectionPtr GetConnect();

	/***********************************************************************
	* Module Name: RecConnect
	*
	* Description: put back a database connection.
	*			   It is syschronized way by using mutex.
	***********************************************************************/
	void RecConnect(ConnectionPtr &ret);

	/***********************************************************************
	* Module Name: InitPool
	*
	* Description: Initialize a database thread pool.
	*
	* Calling Arguments:
	* Name		Input/Output		Type		Description
	* name		username			string		database username
	* pwd		password			string		database password
	* nurl		url					string		database ipaddress
	* db		database			string		database name
	* maxsize	maxsize				int			connection Number
	***********************************************************************/
	void InitPool(std::string name, std::string pwd, std::string nurl, std::string db, int maxsize);


	/***********************************************************************
	* Module Name: GetPoolSize
	*
	* Description: Get database connection poolsize.
	***********************************************************************/
	int GetPoolSize();
	~ConnectionPool() {};
private:
	ConnectionPool() {	}

	/***********************************************************************
	* Module Name: InitConnectPool
	*
	* Description: Initialize connection pool.
	***********************************************************************/
	void InitConnectPool(int initialSize);

	/***********************************************************************
	* Module Name: GetPoolSize
	*
	* Description: DestoryPool connection pool.
	***********************************************************************/
	void DestoryPool();

	/***********************************************************************
	* Module Name: GetPoolSize
	*
	* Description: DestoryPool one connection.
	***********************************************************************/
	void DestoryOneConn();

	/***********************************************************************
	* Module Name: GetPoolSize
	*
	* Description: Expand connection pool.
	***********************************************************************/
	void ExpandPool(int size);

	/***********************************************************************
	* Module Name: GetPoolSize
	*
	* Description: Reduce connection pool.
	***********************************************************************/
	void ReducePool(int size);

	/***********************************************************************
	* Module Name: AddConn
	*
	* Description: Add connections to pool.
	***********************************************************************/
	void AddConn(int size);

	std::string username;
	std::string password;
	std::string url;
	std::string database;
	int poolSize;

	std::list<ConnectionPtr> conList;
	
	static ConnectionPool *pool;
	std::mutex lock;
	Driver *driver;
};

