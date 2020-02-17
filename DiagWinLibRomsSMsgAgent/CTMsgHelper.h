/***********************************************************************
* Filename: CTHelper.h
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Defines the CT bussiness for server.
* It will deal the message from CT.
*
* Copyright Information ?FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
**********************************************************************/

#pragma once
#include "..\DiagWinLibRomsResource\CMsgModels.h"
#include "..\DiagWinLibRomsSShareAgent\ShareArea.h"
#include "..\DiagWinLibRomsSDBAgent\DBhelper.h"
#include "..\DiagWinLibRomsSLogAgent\LoggerManager.h"
#include "SessionRoom.h"
#include "MsgHelperBase.h"

class CTMsgHelper : public MsgHelperBase
{
public:
	CTMsgHelper() {
	};


	void InitHelper();

	/***********************************************************************
	* Module Name: DealInfo
	*
	* Description: Dealing message from machine.
	*
	* Calling Arguments:
	* Name		Input/Output		Type		Description
	* content	read buffer			string		message read buffer
	*
	* Return/Output codes:
	* 0				Login failed or message head doesn't exit
	* 1				insert or update succeccfully
	* 2				target doesn't exist
	***********************************************************************/
	int DealInfo(const string& content);


	/***********************************************************************
	* Module Name: DealOnlineCTInfo
	*
	* Description: Dealing online message from machine.
	*
	* Calling Arguments:
	* Name		Input/Output		Type		Description
	* ctinfo	ctinfo				string		after spliting reader buffer
	*
	* Return/Output codes:
	* 0				Login failed or message head doesn't exit
	* 1				insert or update succeccfully
	* 2				target doesn't exist
	***********************************************************************/
	int DealOnlineCTInfo(const string& ctinfo);

	void Initialize();

	void SetID(const string& id);

	~CTMsgHelper() {};

private:

	string			SN_;

	std::unique_ptr<IDBHelper>		i_DbHelper_ptr;
	std::unique_ptr<IHistoryHelper> i_HistoryHelper_ptr;
	std::unique_ptr<IStateHelper>	i_State_ptr;
	std::unique_ptr<ILoginHelper>	i_Login_ptr;

	vector<string> v_msg_;
	vector<string> ct_info_;

	CLoginPtr				login_ptr;
	MachinePtr				ma_ptr_;
	MachineConfigurationPtr ma_config_ptr_;
	MachineTubePtr			ma_tube_ptr_;
	MachineTubeHistoryPtr	ma_tube_history_ptr_;
	MachineRotatePtr		ma_rotate_ptr_;
	MachineRotateHistoryPtr	ma_rotate_history_ptr_;

	enum Table {
		c_machine_telemetry, c_machine_version, c_machine_configeditor,
		c_machine_guiconfig, c_machine_database, c_machine_protocol_gui, c_machine_protocol_service,
		c_machine_logger, c_machine, c_login, c_machine_other_info,
		c_rd_login, c_machine_tube_history, c_machine_rotate_history,
		c_rd_machine, c_rd_machine_telemetry, c_rd_machine_version, c_rd_machine_configuration,
		c_rd_machine_logger, c_rd_machine_tube_history, c_rd_machine_rotate_history, c_feedback
	} header_;
};
