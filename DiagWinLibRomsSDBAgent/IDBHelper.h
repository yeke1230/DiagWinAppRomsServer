/***********************************************************************
* Filename: IDBHelper.h
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Define define the interface of database operation.
*
* Copyright Information © FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
* Distributed under the MySQL Connector Software License, Version 1.1.9
* Download from https://dev.mysql.com/downloads/connector/cpp/
**********************************************************************/

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
