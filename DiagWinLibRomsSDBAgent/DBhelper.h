/***********************************************************************
* Filename: DBHelper.h
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Define the database helper.
*
* Copyright Information © FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
* Distributed under the MySQL Connector Software License, Version 1.1.9
* Download from https://dev.mysql.com/downloads/connector/cpp/
**********************************************************************/

#pragma once
#include <stdlib.h>
#include <iostream>

#include "mysql_connection.h"
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/algorithm/string.hpp>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <boost/shared_ptr.hpp>
#include "../DiagWinLibRomsResource/CMsgModels.h"
#include "../DiagWinLibRomsSConfigAgent/Configuration.h"
#include "../DiagWinLibRomsSShareAgent/ShareArea.h"

#include "DBConnectPool.h"

typedef std::shared_ptr<sql::PreparedStatement> PreparedStatementPtr;
typedef std::shared_ptr<sql::Statement> StatementPtr;
typedef std::shared_ptr<sql::ResultSet> ResultSetPtr;

#define LIMITE 90.0

class IDBHelper
{
public:
	/***********************************************************************
	* Module Name: Insert
	*
	* Description: Interface insert function. Inherited by data base helper.
	***********************************************************************/
	virtual void Insert() = 0;

	/***********************************************************************
	* Module Name: Update
	*
	* Description: Interface update function. Inherited by data base helper
	***********************************************************************/
	virtual void Update() = 0;

	virtual ~IDBHelper() {}

private:

};

class IHistoryHelper
{
public:
	/***********************************************************************
	* Module Name: IsSameEpoch
	*
	* Description: This function is overrided by history message helper.
	*			   It is used to examine if the history message is exist
	*			   in history record table, c_tube_history etc.. If exist,
	*              then we exucte update, else excute insert.
	***********************************************************************/
	virtual bool IsSameEpoch() = 0;

	virtual ~IHistoryHelper() {}
private:

};

class IStateHelper
{
public:
	/***********************************************************************
	* Module Name: IStateHelper
	*
	* Description: This function is overrided by machine  helper.
	*			   It is used to get final communication time.
	***********************************************************************/
	virtual string GetFinalCom() = 0;

	virtual ~IStateHelper() {}
private:

};

class ILoginHelper
{
public:
	/***********************************************************************
	* Module Name: ILoginHelper
	*
	* Description: This function is check the log message.			   
	***********************************************************************/
	virtual bool LoginCheck() = 0;

	virtual ~ILoginHelper() {}
private:

};

// The following class is c_tablename helper                  
// c_tablename is used to record the machine in hospital       

class CLoginHelper:public ILoginHelper
{
public:
	
	CLoginHelper(CLoginPtr login) : login_(login) {};

	bool LoginCheck();

	virtual ~CLoginHelper() {};

private:
	CLoginPtr login_;
};

class CRDLoginHelper :public ILoginHelper
{
public:

	CRDLoginHelper(CLoginPtr login) : login_(login) {};

	bool LoginCheck();

	virtual ~CRDLoginHelper() {};

private:
	CLoginPtr login_;
};


class MachineHelper: public IDBHelper , public IStateHelper
{
public:
	MachineHelper() {}
	explicit MachineHelper(MachinePtr machine) : machine_(machine) {};

	void Insert();
	void Update();
	string GetFinalCom();

	virtual ~MachineHelper() {}
private:
	MachinePtr machine_;
};

class MachineTelemetryHelper : public IDBHelper
{
public:
	MachineTelemetryHelper() {}
	explicit MachineTelemetryHelper(MachineConfigurationPtr tele) :machine_tele_(tele) {}
	void Insert();
	void Update();

	virtual ~MachineTelemetryHelper() {}

private:
	MachineConfigurationPtr machine_tele_;
};

class MachineLoggerHelper : public IDBHelper
{
public:
	MachineLoggerHelper() {}
	explicit MachineLoggerHelper(MachineConfigurationPtr logger) :machine_logger_(logger) {}

	void Insert();
	void Update();

	virtual ~MachineLoggerHelper() {}

private:
	MachineConfigurationPtr machine_logger_;
};

class MachineVersionHelper : public IDBHelper
{
public:
	MachineVersionHelper() {}
	explicit MachineVersionHelper(MachineConfigurationPtr version) :machine_version_(version) {}

	void Insert();
	void Update();

	virtual ~MachineVersionHelper() {}

private:
	MachineConfigurationPtr machine_version_;
};

class MachineConfigEditorHelper : public IDBHelper
{
public:
	MachineConfigEditorHelper() {}
	explicit MachineConfigEditorHelper(MachineConfigurationPtr mac_config) :machine_configuration_(mac_config){}

	void Insert();
	void Update();
	void Set(MachineConfigurationPtr ptr) {
		machine_configuration_ = ptr;
	}
	virtual ~MachineConfigEditorHelper() {}

private:
	MachineConfigurationPtr machine_configuration_;
};

class MachineDataBaseHelper : public IDBHelper
{
public:
	MachineDataBaseHelper() {}
	explicit MachineDataBaseHelper(MachineConfigurationPtr mac_config) :machine_configuration_(mac_config) {}

	void Insert();
	void Update();
	void Set(MachineConfigurationPtr ptr) {
		machine_configuration_ = ptr;
	}
	virtual ~MachineDataBaseHelper() {}

private:
	MachineConfigurationPtr machine_configuration_;
};

class MachineGUIconfigHelper : public IDBHelper
{
public:
	MachineGUIconfigHelper() {}
	explicit MachineGUIconfigHelper(MachineConfigurationPtr mac_config) :machine_configuration_(mac_config) {}

	void Insert();
	void Update();
	void Set(MachineConfigurationPtr ptr) {
		machine_configuration_ = ptr;
	}
	virtual ~MachineGUIconfigHelper() {}

private:
	MachineConfigurationPtr machine_configuration_;
};

class MachineProtocolHelper : public IDBHelper
{
public:
	MachineProtocolHelper() {}
	explicit MachineProtocolHelper(MachineConfigurationPtr mac_config) :machine_configuration_(mac_config) {}

	void Insert();
	void Update();
	void Set(MachineConfigurationPtr ptr) {
		machine_configuration_ = ptr;
	}
	virtual ~MachineProtocolHelper() {}

private:
	MachineConfigurationPtr machine_configuration_;
};

class MachineOtherInfoHelper : public IDBHelper
{
public:
	MachineOtherInfoHelper() {}
	explicit MachineOtherInfoHelper(MachineConfigurationPtr mac_config) :machine_configuration_(mac_config) {}

	void Insert();
	void Update();
	void Set(MachineConfigurationPtr ptr) {
		machine_configuration_ = ptr;
	}
	virtual ~MachineOtherInfoHelper() {}

private:
	MachineConfigurationPtr machine_configuration_;
};

class MachineTubeHelper : public IDBHelper
{
public:
	MachineTubeHelper() {}
	explicit MachineTubeHelper(MachineTubePtr tube) :machine_tube_(tube) {}

	void Insert();
	void Update();
	void Set(MachineTubePtr ptr) {
		machine_tube_ = ptr;
	}
	virtual ~MachineTubeHelper() {}

private:
	MachineTubePtr machine_tube_;
};

class MachineTubeHistoryHelper : public IDBHelper, public IHistoryHelper
{
public:
	MachineTubeHistoryHelper() {}
	explicit MachineTubeHistoryHelper(MachineTubeHistoryPtr rotate) :machine_tube_history_(rotate) {}

	void Insert();
	void Update();
	bool IsSameEpoch();
	void Set(MachineTubeHistoryPtr ptr) {
		machine_tube_history_ = ptr;
	}
	virtual ~MachineTubeHistoryHelper() {}

private:
	MachineTubeHistoryPtr machine_tube_history_;
};

class MachineRotateHelper : public IDBHelper
{
public:
	MachineRotateHelper() {}
	explicit MachineRotateHelper(MachineRotatePtr rotate) :machine_rotate_(rotate) {}

	void Insert();
	void Update();

	virtual ~MachineRotateHelper() {}

private:
	MachineRotatePtr machine_rotate_;
};

class MachineRotateHistoryHelper : public IDBHelper, public IHistoryHelper
{
public:
	MachineRotateHistoryHelper() {}
	explicit MachineRotateHistoryHelper(MachineRotateHistoryPtr rotatehistory) :machine_rotate_history_(rotatehistory) {}

	void Insert();
	void Update();
	bool IsSameEpoch();

	virtual ~MachineRotateHistoryHelper() {}

private:
	MachineRotateHistoryPtr machine_rotate_history_;
};

