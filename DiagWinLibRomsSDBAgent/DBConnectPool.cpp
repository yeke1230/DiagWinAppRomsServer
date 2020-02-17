/***********************************************************************
* Filename: DBConnnectPool.cpp
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Realize the database connect pool.
*
* Copyright Information © FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
* Distributed under the MySQL Connector Software License, Version 1.1.9
* Download from https://dev.mysql.com/downloads/connector/cpp/
**********************************************************************/

#include "DBConnectPool.h"

ConnectionPool* ConnectionPool::pool = nullptr;

//ConnectionPool::ConnectionPool(std::string name, std::string pwd, std::string nurl, int maxSize) :
//	username(name), password(pwd), url(nurl), poolSize(maxSize)
//{
//	driver = get_driver_instance();
//	initConnectPool(poolSize / 2);
//}

ConnectionPool * ConnectionPool::GetInstance()
{
	if (pool == nullptr)
	{
		//pool = new ConnectionPool("root", "fmi-drooga", "tcp://10.10.20.89:3306", 40);
		pool = new ConnectionPool();
	}
	return pool;
}

ConnectionPtr ConnectionPool::GetConnect()
{
	try
	{
		std::lock_guard<std::mutex> locker(lock);
		if (conList.size() <= 0)
			AddConn(5);

		std::shared_ptr<Connection> sp = conList.front();
		conList.pop_front();
		return sp;
	}
	catch (const std::exception& err)
	{
		ExpandPool(5);
		throw err;
	}
	catch (const sql::SQLException& err) {
		throw err;
	}
}

void ConnectionPool::RecConnect(ConnectionPtr& ret)
{
	std::lock_guard<std::mutex> locker(lock);
	conList.push_back(ret);
}

void ConnectionPool::InitPool(std::string name, std::string pwd, std::string nurl, std::string db, int maxsize)
{
	try
	{
		username = name;
		password = pwd;
		url = nurl;
		poolSize = maxsize;
		database = db;

		driver = get_driver_instance();
		InitConnectPool(poolSize);
	}
	catch (const sql::SQLException& err)
	{
		throw err;
	}

}

int ConnectionPool::GetPoolSize()
{
	return 0;
}

void ConnectionPool::InitConnectPool(int initialSize)
{
	try
	{
		std::lock_guard<std::mutex> locker(lock);
		AddConn(initialSize);
	}
	catch (const sql::SQLException& err)
	{
		throw err;
	}

}

void ConnectionPool::DestoryPool()
{
	for (auto &conn : conList)
	{
		std::shared_ptr<Connection> &&sp = std::move(conList.front());
		sp->close();
	}
}

void ConnectionPool::DestoryOneConn()
{
	std::shared_ptr<Connection> &&sp = std::move(conList.front());
	sp->close();
	--poolSize;
}

void ConnectionPool::ExpandPool(int size)
{
	std::lock_guard<std::mutex> locker(lock);
	AddConn(size);
	poolSize += size;
}

void ConnectionPool::ReducePool(int size)
{
	std::lock_guard<std::mutex> locker(lock);
	if (size > poolSize)
	{
		return;
	}
	for (int i = 0; i < size; i++)
	{
		DestoryOneConn();
	}
	poolSize -= size;
}

void ConnectionPool::AddConn(int size)
{
	try
	{
		for (int i = 0; i < size; ++i)
		{
			if (driver != NULL)
			{

					Connection *conn = driver->connect(url.c_str(), username.c_str(), password.c_str());
					conn->setSchema(database.c_str());
					//std::shared_ptr<Connection> sp(conn,
					//	[](Connection *conn) {
					//	delete conn;
					//});

					std::shared_ptr<Connection> sp(conn);
					conList.push_back(std::move(sp));
				}

			}

		}
	catch (const sql::SQLException& err)
	{
		std::cout << err.what() << std::endl;
		throw err;
	}

}