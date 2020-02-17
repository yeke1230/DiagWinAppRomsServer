/***********************************************************************
* Filename: DBHelper.cpp
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Realize the helper function.
*
* Copyright Information © FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
* Distributed under the MySQL Connector Software License, Version 1.1.9
* Download from https://dev.mysql.com/downloads/connector/cpp/
**********************************************************************/

#include "DBhelper.h"
#include <iostream>
using namespace std;

ConnectionPool *pool = ConnectionPool::GetInstance();

void MachineHelper::Update()
{
	try
	{
		ConnectionPtr con = pool->GetConnect();
		PreparedStatementPtr pstmt;
		string str_pstmt;
		// Caution state check if state is warning
		// There are 7 condition we need to judge,
		// 1. when tube_usage > 90, we have to set problem as "tube usage more than ?"
		// 2. when rack_usage > 90, we have to set problem as "rack usage more than ?"
		// 3. when disk_image_usage > 90, we have to set problem as "image usage more than ?"
		// 4. when disk_rawdata_usage > 90, we have to set problem as "rawdata usage more than ?"
		// 5. when temperature > temp_upper and temperature < temp_lower, we have to set problem as "bay temperature..."
		// 6. when humitidy > humitidy_upper and  humitidy < humitidy_lower , we have to set problem as "humidity..."
		// 7. when raitemperature > dec_temp_warn_upper and raitemperature < high_dec_temp_standard, we have to set problem as "detector temperature..."
#pragma region Caution State JudgeMent
		if (machine_->device_state != "Broken")
		{
			machine_->device_state = "On";
			machine_->problem = "";
			machine_->systemerror_occure_epoch = "";

			float tube_usage=0.0, rack_usage=0.0;
			float disk_image_usage = 0.0, disk_rawdata_usage = 0.0;
			double humitidy_upper = 0.0, humitidy_lower = 0.0;
			double temp_upper = 0.0, temp_lower = 0.0;
			double dec_temp_warn_upper = 0.0, dec_temp_warn_lower = 0.0, dec_temp_standard = 0.0, dec_temp_err = 0.0;
			//double high_dec_temp_warn_upper, high_dec_temp_warn_lower, high_dec_temp_standard, high_dec_temp_err;

			string my_caution;
			// Select tube usage from machine_tube
			if (machine_->header == "c_machine")
				str_pstmt = "Select TUBE_USAGE from c_machine_tube where SN = ?";
			if (machine_->header == "c_rd_machine")
				str_pstmt = "Select TUBE_USAGE from c_rd_machine_tube where SN = ?";
			pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));
			pstmt->setString(1, machine_->SN.c_str());
			ResultSetPtr res = ResultSetPtr(pstmt->executeQuery());
			while (res->next())
				tube_usage = res->getDouble("TUBE_USAGE");
			res->close();
			pstmt->close();

			// Select rack usage from machine rotate
			if (machine_->header == "c_machine")
				str_pstmt = "Select RACK_USAGE from c_machine_rotate where SN = ?";
			if (machine_->header == "c_rd_machine")
				str_pstmt = "Select RACK_USAGE from c_rd_machine_rotate where SN = ?";
			pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));
			pstmt->setString(1, machine_->SN.c_str());
			res = ResultSetPtr(pstmt->executeQuery());
			while (res->next())
				rack_usage = res->getDouble("RACK_USAGE");
			res->close();
			pstmt->close();

			// Calculate rawdata and image disk usage more than 90%.
			vector<string> image_list;
			vector<string> rawdata_list;
			boost::algorithm::split(image_list, machine_->diskcontract_image, boost::is_any_of("/"));
			boost::algorithm::split(rawdata_list, machine_->diskcontract_rawdata, boost::is_any_of("/"));
			if (image_list.size() > 1)
				disk_image_usage = stod(image_list[0]) / stod(image_list[1]) * 100.00;
			if (rawdata_list.size() > 1)
				disk_rawdata_usage = stod(rawdata_list[0]) / stod(rawdata_list[1]) * 100.00;

			// Check whether temperature is in the law range.
			str_pstmt = "Select * from p_machine_environment";
			pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));
			res = ResultSetPtr(pstmt->executeQuery());
			while (res->next())
			{
				humitidy_upper = stod(res->getString("humidity_upper").c_str());
				humitidy_lower = stod(res->getString("humidity_lower").c_str());
				temp_upper = stod(res->getString("temperature_upper").c_str());
				temp_lower = stod(res->getString("temperature_lower").c_str());

				if (machine_->dettempmodel_ == "High")
				{
					dec_temp_warn_upper = stod(res->getString("High_TempEnvDetectorHignTemp_Warning").c_str());
					dec_temp_warn_lower = stod(res->getString("High_TempEnvDetector_Warning").c_str());
					dec_temp_standard	= stod(res->getString("High_TempEnvDetector_Standard").c_str());
					dec_temp_err		= stod(res->getString("High_TempEnvDetectorHigh_TempErr").c_str());
				}
				else
				{
					dec_temp_warn_upper = stod(res->getString("Normal_TempEnvDetectorHign_Temp_Warning").c_str());
					dec_temp_warn_lower = stod(res->getString("Normal_TempEnvDetectorLow_Temp_Warning").c_str());
					dec_temp_standard = stod(res->getString("Normal_TempEnvDetector_Standard").c_str());
					dec_temp_err = stod(res->getString("Normal_TempEnvDetectorHigh_TempErr").c_str());
				}
			}
			res->close();
			pstmt->close();

			// Tube usage
			if (tube_usage > LIMITE) {
				machine_->device_state = "Caution";
				my_caution += "tube usage more than " + std::to_string(tube_usage).substr(0, 4) + "%;";
				machine_->problem = my_caution;
			}
			// Rack usage
			if (rack_usage > LIMITE) {
				machine_->device_state = "Caution";
				my_caution += "rack usage more than " + std::to_string(rack_usage).substr(0, 4) + "%;";
				machine_->problem = my_caution;
			}
			// Disk image usage
			if (disk_image_usage > LIMITE) {
				machine_->device_state = "Caution";
				my_caution += "disk image usage more than " + std::to_string(disk_image_usage).substr(0, 4) + "%;";
				machine_->problem = my_caution;
			}
			// Disk rawdata usage
			if (disk_rawdata_usage > LIMITE) {
				machine_->device_state = "Caution";
				my_caution += "disk rawdata usage more than " + std::to_string(disk_rawdata_usage).substr(0, 4) + "%;";
				machine_->problem = my_caution;
			}
			// Bay humidity
			if (machine_->humidity != "" && humitidy_lower > stod(machine_->humidity.c_str()))
			{
				machine_->device_state = "Caution";
				my_caution += "bay humidity lower than " + std::to_string(humitidy_lower).substr(0, 4) + ";";
				machine_->problem = my_caution;
			}
			if (machine_->humidity != "" && humitidy_upper < stod(machine_->humidity.c_str()))
			{
				machine_->device_state = "Caution";
				my_caution += "bay humidity higher than " + std::to_string(humitidy_upper).substr(0, 4) + ";";
				machine_->problem = my_caution;
			}
			// Bay temperature
			if (machine_->tempC != "" && temp_lower > stod(machine_->tempC.c_str()))
			{
				machine_->device_state = "Caution";
				my_caution += "Bay temperature lower than " + std::to_string(temp_lower).substr(0, 4) + ";";
				machine_->problem = my_caution;
			}
			if (machine_->tempC != "" && temp_upper < stod(machine_->tempC.c_str()))
			{
				machine_->device_state = "Caution";
				my_caution += "Bay temperature higher than " + std::to_string(temp_upper).substr(0, 4) + ";";
				machine_->problem = my_caution;
			}
			// Detector temperature
			if (machine_->railtemperature != "" && dec_temp_standard > stod(machine_->railtemperature.c_str()))
			{
				machine_->device_state = "Caution";
				my_caution += "detector temperature lower than " + std::to_string(dec_temp_standard).substr(0, 4) + ";";
				machine_->problem = my_caution;
			}
			if (machine_->railtemperature != "" && dec_temp_warn_upper < stod(machine_->railtemperature.c_str()))
			{
				machine_->device_state = "Caution";
				my_caution += "detector temperature higher than " + std::to_string(dec_temp_warn_upper).substr(0, 4) + ";";
				machine_->problem = my_caution;
			}
		}
#pragma endregion

		string head = "update " + machine_->header + " set ";
		string body;
		if (machine_->last_com_time != "")				body += "LAST_COMMUNICATION_EPOCH = '" + machine_->last_com_time + "',";
		if (machine_->problem != "")					body += "LAST_KNOWN_PROBLEM = '" + machine_->problem + "',";
		else if (machine_->device_state == "On")		body += "LAST_KNOWN_PROBLEM = NULL,";
		if (machine_->systemerror_occure_epoch != "")	body += "CURRENT_PROBLEM_OCCURE_EPOCH = '" + machine_->systemerror_occure_epoch + "',";
		else if (machine_->device_state != "Broken" )	body += "CURRENT_PROBLEM_OCCURE_EPOCH = NULL,";
		if (machine_->device_state != "")				body += "LAST_KNOWN_STATE = '" + machine_->device_state + "',";
		if (machine_->tubeheat != "")					body += "TUBE_HEAT = '" + machine_->tubeheat + "',";
		if (machine_->railtemperature != "")			body += "RAIL_TEMPERATURE = '" + machine_->railtemperature + "',";
		if (machine_->diskcontract_image != "")			body += "DISK_CONTRACT_IMAGE = '" + machine_->diskcontract_image + "',";
		if (machine_->diskcontract_rawdata != "")		body += "DISK_CONTRACT_RAWDATA = '" + machine_->diskcontract_rawdata + "',";
		if (machine_->total_boottime != "")				body += "BOOT_TIME_ALL_DAYS = '" + machine_->total_boottime + "',";
		if (machine_->total_scannumber != "")			body += "TOTAL_SCAN_NUMBER = '" + machine_->total_scannumber + "',";
		if (machine_->bootTime_allDays != "")			body += "CT_DAYS = '" + machine_->bootTime_allDays + "',";
		if (machine_->software_ver != "")				body += "SOFTWARE_VER = '" + machine_->software_ver + "',";
		if (machine_->tempC != "")						body += "TEMPERATURE = '" + machine_->tempC + "',";
		if (machine_->humidity != "")					body += "HUMIDITY = '" + machine_->humidity + "',";
		if (machine_->dettempmodel_ != "")              body += "RAIL_ENVIRONMENT = '" + machine_->dettempmodel_ + "',";

		body[body.length() - 1] = ' ';
		string end = "Where MANUFACTURE_NO1 = '" + machine_->SN + "'";

		str_pstmt = head + body + end;

		StatementPtr stmt = StatementPtr(con->createStatement());
		stmt->execute(str_pstmt.c_str());
		stmt->close();

#pragma region Insert humidy, detector temperature, and bay temperature
		// insert into history value into c_machine_temperature_history, but only once.
		if (!machine_->is_temperature_insert_ && machine_->tempC != "")
		{
			boost::uuids::uuid uuid = boost::uuids::random_generator()();
			string id = boost::uuids::to_string(uuid);
			boost::algorithm::replace_all(id, "-", "");

			if (machine_->header == "c_machine")
				str_pstmt = " Insert into c_machine_temperature_history (ID, ROOM_TEMPERATURE, TIME, MACHINE_SERIAL_NUMBER) values (?, ?, ?, ?)";
			if (machine_->header == "c_rd_machine")
				str_pstmt = " Insert into c_rd_machine_temperature_history (ID, ROOM_TEMPERATURE, TIME, MACHINE_SERIAL_NUMBER) values (?, ?, ?, ?)";
			pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));
			pstmt->setString(1, id.c_str());
			pstmt->setString(2, machine_->tempC.c_str());
			pstmt->setDateTime(3, machine_->last_com_time.c_str());
			pstmt->setString(4, machine_->SN.c_str());
			pstmt->executeUpdate();
			pstmt->close();
			machine_->is_temperature_insert_ = true;
		}
		// insert into history value into c_machine_detector_temperature_history, but only once.
		if (!machine_->is_railtemperature_insert_ && machine_->railtemperature != "")
		{
			if (machine_->header == "c_machine")
				str_pstmt = " Insert into c_machine_detector_temperature_history (ID, DETECTOR_TEMPERATURE, TIME, MACHINE_SERIAL_NUMBER) values (?, ?, ?, ?)";
			if (machine_->header == "c_rd_machine")
				str_pstmt = " Insert into c_rd_machine_detector_temperature_history (ID, DETECTOR_TEMPERATURE, TIME, MACHINE_SERIAL_NUMBER) values (?, ?, ?, ?)";
			boost::uuids::uuid uuid = boost::uuids::random_generator()();
			string id = boost::uuids::to_string(uuid);
			boost::algorithm::replace_all(id, "-", "");

			pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));
			pstmt->setString(1, id.c_str());
			pstmt->setString(2, machine_->railtemperature.c_str());
			pstmt->setDateTime(3, machine_->last_com_time.c_str());
			pstmt->setString(4, machine_->SN.c_str());
			pstmt->executeUpdate();
			pstmt->close();
			machine_->is_railtemperature_insert_ = true;
		}
		// insert into history value into c_machine_humidity_history, but only once.
		if (!machine_->is_humidty_insert_ && machine_->humidity != "")
		{
			if (machine_->header == "c_machine")
				str_pstmt = " Insert into c_machine_humidity_history (ID, ROOM_HUMIDITY, TIME, MACHINE_SERIAL_NUMBER) values (?, ?, ?, ?)";
			if (machine_->header == "c_rd_machine")
				str_pstmt = " Insert into c_rd_machine_humidity_history (ID, ROOM_HUMIDITY, TIME, MACHINE_SERIAL_NUMBER) values (?, ?, ?, ?)";
			boost::uuids::uuid uuid = boost::uuids::random_generator()();
			string id = boost::uuids::to_string(uuid);
			boost::algorithm::replace_all(id, "-", "");
			pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));
			pstmt->setString(1, id.c_str());
			pstmt->setString(2, machine_->humidity.c_str());
			pstmt->setDateTime(3, machine_->last_com_time.c_str());
			pstmt->setString(4, machine_->SN.c_str());
			pstmt->executeUpdate();
			pstmt->close();
			machine_->is_humidty_insert_ = true;
		}
#pragma endregion

		pool->RecConnect(con);
	}
	catch (const sql::SQLException& err)
	{
		if (!strcmp(err.what(), "Lost connection to MySQL server during query"))
		{
			Update();
			return;
		}
		throw err;
	}
	catch (const std::exception & err) {
		throw err;
	}

}

string MachineHelper::GetFinalCom()
{
	if (machine_->SN != "")
	{
		ConnectionPtr con = pool->GetConnect();
		string final_com;
		string str_pstmt = "select LAST_COMMUNICATION_EPOCH from " + machine_->header + " where MANUFACTURE_NO1 = ?";
		PreparedStatementPtr pstmt1 = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));
		pstmt1->setString(1, machine_->SN.c_str());
		ResultSetPtr res = ResultSetPtr(pstmt1->executeQuery());
		while (res->next())
		{
			final_com = res->getString("LAST_COMMUNICATION_EPOCH");
			if (final_com != "")
			{
				pool->RecConnect(con);
				return final_com;
			}
		}
		pstmt1->close();
		res->close();
		pool->RecConnect(con);
		return string();
	}

	return string();
}

void MachineHelper::Insert() {
}



void MachineTelemetryHelper::Insert()
{
	try
	{
		ConnectionPtr con = pool->GetConnect();

		string str_pstmt = "insert into "+ machine_tele_ ->header + "(TELE_ID, TELE_PATH, TELE_FILE_NAME, \
						TELE_LAST_UPLOAD_EPOCH, TELE_FILE_SIZE, SN, COMMENTS, DATA_SOURCE) values (?, ?, ?, ?, ?, ?, ?, ?) ";

		PreparedStatementPtr pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));
		/* Create unit id*/
		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		machine_tele_->id = boost::uuids::to_string(uuid);
		boost::algorithm::replace_all(machine_tele_->id, "-", "");

		pstmt->setString(1, machine_tele_->id.c_str());
		pstmt->setString(2, machine_tele_->file_path.c_str());
		pstmt->setString(3, machine_tele_->file_name.c_str());
		pstmt->setDateTime(4, machine_tele_->file_upload_epoch.c_str());
		pstmt->setString(5, machine_tele_->file_size.c_str());
		pstmt->setString(6, machine_tele_->SN.c_str());
		pstmt->setString(7, machine_tele_->tag_.c_str());
		pstmt->setString(8, machine_tele_->source_.c_str());

		pstmt->executeUpdate();
		pstmt->close();
		pool->RecConnect(con);
	}
	catch (const sql::SQLException& err)
	{
		//pool->RecConnect(con);
		throw err;
	}


}

void MachineTelemetryHelper::Update()
{

}


void MachineLoggerHelper::Insert()
{
	try
	{
		ConnectionPtr con = pool->GetConnect();

		string str_pstmt = "insert into " + machine_logger_->header + " (LOG_ID, LOG_PATH, \
						LOG_LAST_UPLOAD_EPOCH, LOG_FILE_SIZE, LOG_FILE_NAME, SN, COMMENTS, DATA_SOURCE, LOG_PROBLEM_DESCRIPTION) values (?, ?, ?, ?, ?, ?, ?, ?, ?) ";

		PreparedStatementPtr pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));
		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		machine_logger_->id = boost::uuids::to_string(uuid);
		boost::algorithm::replace_all(machine_logger_->id, "-", "");

		pstmt->setString(1, machine_logger_->id.c_str());
		pstmt->setString(2, machine_logger_->file_path.c_str());
		pstmt->setDateTime(3, machine_logger_->file_upload_epoch.c_str());
		pstmt->setString(4, machine_logger_->file_size.c_str());
		pstmt->setString(5, machine_logger_->file_name.c_str());
		pstmt->setString(6, machine_logger_->SN.c_str());
		pstmt->setString(7, machine_logger_->tag_.c_str());
		pstmt->setString(8, machine_logger_->source_.c_str());
		pstmt->setString(9, machine_logger_->description_.c_str());
		pstmt->executeUpdate();
		pstmt->close();
		pool->RecConnect(con);
	}
	catch (const sql::SQLException& err)
	{
		if (!strcmp(err.what(), "Lost connection to MySQL server during query"))
		{
			Insert();
			return;
		}
		//else
		//{
		//	pool->RecConnect(con);
		//}
		throw err;
	}


}


void MachineLoggerHelper::Update()
{
}


void MachineVersionHelper::Insert()
{
	try
	{
		ConnectionPtr con = pool->GetConnect();

		string str_pstmt = "insert into " + machine_version_->header + "(VERS_ID, VERS_PATH, VERS_FILE_NAME, VERS_FILE_SIZE, \
						VERS_LAST_UPLOAD_EPOCH, SN, DATA_SOURCE, COMMENTS) values (?, ?, ?, ?, ?, ?, ?, ?) ";

		PreparedStatementPtr pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));

		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		machine_version_->id = boost::uuids::to_string(uuid);
		boost::algorithm::replace_all(machine_version_->id, "-", "");

		pstmt->setString(1, machine_version_->id.c_str());
		pstmt->setString(2, machine_version_->file_path.c_str());
		pstmt->setString(3, machine_version_->file_name.c_str());
		pstmt->setDateTime(4, machine_version_->file_size.c_str());
		pstmt->setDateTime(5, machine_version_->file_upload_epoch.c_str());
		pstmt->setString(6, machine_version_->SN.c_str());
		pstmt->setString(7, machine_version_->source_.c_str());
		pstmt->setString(8, machine_version_->tag_.c_str());

		pstmt->executeUpdate();
		pstmt->close();
		pool->RecConnect(con);
	}
	catch (const sql::SQLException& err)
	{
		if (!strcmp(err.what(), "Lost connection to MySQL server during query"))
		{
			Insert();
			return;
		}
		//else
		//{
		//	pool->RecConnect(con);
		//}
		throw err;

	}catch (const std::exception& e)
	{
		throw e;
	}

}

void MachineVersionHelper::Update()
{
}

void MachineTubeHelper::Insert()
{

}


void MachineTubeHelper::Update()
{
	try
	{
		ConnectionPtr con = pool->GetConnect();
		// Calculate the tube usage.
		string slt = "select TUBE_LIFE from " + machine_tube_ ->header+ " where SN = ? AND TUBE_SERIAL_NUMBER = ?";
		PreparedStatementPtr pstmt = PreparedStatementPtr(con->prepareStatement(slt.c_str()));
		pstmt->setString(1, machine_tube_->SN.c_str());
		pstmt->setString(2, machine_tube_->tubesn_.c_str());
		ResultSetPtr res = ResultSetPtr(pstmt->executeQuery());
		string usage;
		while (res->next())
			usage = std::to_string(stod(machine_tube_->lifetime_exposure) / stod(res->getString("TUBE_LIFE").c_str()) * 100.00);
		usage = usage.substr(0, 4);
		res->close();
		pstmt->close();

		string head = "update " + machine_tube_->header + " set ";
		string body;
		if (machine_tube_->lifetime_exposure != "") body += "TUBE_CURRENT_EXPOSE_TIMES = '" + machine_tube_->lifetime_exposure + "',";
			
		body += "TUBE_USAGE = '" + usage + "',";
		body[body.length() - 1] = ' ';
		string end = "Where SN = '" + machine_tube_->SN + "' and TUBE_SERIAL_NUMBER = '" + machine_tube_->tubesn_ + "'";
		string str_pstmt = head + body + end;

		StatementPtr stmt = StatementPtr(con->createStatement());
		stmt->execute(str_pstmt.c_str());

		stmt->close();
		pool->RecConnect(con);
	}
	catch (const sql::SQLException& err)
	{
		if (!strcmp(err.what(), "Lost connection to MySQL server during query"))
		{
			Update();
			return;
		}
		//else
		//{
		//	pool->RecConnect(con);
		//}
		throw err;
	}


}


void MachineRotateHelper::Insert()
{

}

void MachineRotateHelper::Update()
{
	try
	{
		ConnectionPtr con = pool->GetConnect();
		// Calculate the rack usage.
		string slt = "select RACK_LIFE from " + machine_rotate_->header + " where SN = ?";
		PreparedStatementPtr pstmt = PreparedStatementPtr(con->prepareStatement(slt.c_str()));
		pstmt->setString(1, machine_rotate_->SN.c_str());
		ResultSetPtr res = ResultSetPtr(pstmt->executeQuery());
		string usage;
		while (res->next())
			usage = std::to_string(stod(machine_rotate_->lifetime_rotation) / stod(res->getString("RACK_LIFE").c_str()) * 100.00);
		usage = usage.substr(0, 4);
		res->close();
		pstmt->close();


		string head = "update " + machine_rotate_->header + " set ";
		string body;
		if (machine_rotate_->lifetime_rotation != "") body += "CURRENT_ROTATION_ANGLE = '" + machine_rotate_->lifetime_rotation + "',";
		body += "RACK_USAGE = '" + usage + "',";
		body[body.length() - 1] = ' ';
		string end = "Where SN = '" + machine_rotate_->SN + "'";
		string str_pstmt = head + body + end;

		StatementPtr stmt = StatementPtr(con->createStatement());
		stmt->execute(str_pstmt.c_str());
		stmt->close();
		pool->RecConnect(con);
	}
	catch (const sql::SQLException& err)
	{
		if (!strcmp(err.what(), "Lost connection to MySQL server during query"))
		{
			Update();
			return;
		}
		//else
		//{
		//	pool->RecConnect(con);
		//}
		throw err;

	}


}

void MachineConfigEditorHelper::Insert()
{
	try
	{
		ConnectionPtr con = pool->GetConnect();

		string str_pstmt = "insert into " + machine_configuration_->header +  " (CONFEDITOR_ID, CONFEDITOR_PATH, \
						CONFEDITOR_LAST_UPLOAD_EPOCH, CONFEDITOR_FILE_SIZE, SN, COMMENTS, DATA_SOURCE, CONFEDITOR_FILE_NAME) values (?, ?, ?, ?, ?, ?, ?, ?) ";

		PreparedStatementPtr pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));
		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		machine_configuration_->id = boost::uuids::to_string(uuid);
		boost::algorithm::replace_all(machine_configuration_->id, "-", "");

		pstmt->setString(1, machine_configuration_->id.c_str());
		pstmt->setString(2, machine_configuration_->file_path.c_str());
		pstmt->setDateTime(3, machine_configuration_->file_upload_epoch.c_str());
		pstmt->setString(4, machine_configuration_->file_size.c_str());
		pstmt->setString(5, machine_configuration_->SN.c_str());
		pstmt->setString(6, machine_configuration_->tag_.c_str());
		pstmt->setString(7, machine_configuration_->source_.c_str());
		pstmt->setString(8, machine_configuration_->file_name.c_str());
		pstmt->executeUpdate();
		pstmt->close();
		pool->RecConnect(con);
	}
	catch (const sql::SQLException& err)
	{
		if (!strcmp(err.what(), "Lost connection to MySQL server during query"))
		{
			Update();
			return;
		}
		//else
		//{
		//	pool->RecConnect(con);
		//}
		throw err;

	}

}


void MachineConfigEditorHelper::Update()
{
}

void MachineDataBaseHelper::Insert()
{
	try
	{
		ConnectionPtr con = pool->GetConnect();

		string str_pstmt = "insert into "+ machine_configuration_ ->header+" (DB_ID, DB_PATH, \
						DB_LAST_UPLOAD_EPOCH, DB_FILE_SIZE, SN, COMMENTS, DATA_SOURCE, DB_FILE_NAME) values (?, ?, ?, ?, ?, ?, ?, ?) ";

		PreparedStatementPtr pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));
		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		machine_configuration_->id = boost::uuids::to_string(uuid);
		boost::algorithm::replace_all(machine_configuration_->id, "-", "");

		pstmt->setString(1, machine_configuration_->id.c_str());
		pstmt->setString(2, machine_configuration_->file_path.c_str());
		pstmt->setDateTime(3, machine_configuration_->file_upload_epoch.c_str());
		pstmt->setString(4, machine_configuration_->file_size.c_str());
		pstmt->setString(5, machine_configuration_->SN.c_str());
		pstmt->setString(6, machine_configuration_->tag_.c_str());
		pstmt->setString(7, machine_configuration_->source_.c_str());
		pstmt->setString(8, machine_configuration_->file_name.c_str());
		pstmt->executeUpdate();
		pstmt->close();
		pool->RecConnect(con);
	}
	catch (const sql::SQLException& err)
	{
		if (!strcmp(err.what(), "Lost connection to MySQL server during query"))
		{
			Insert();
			return;
		}
		//else
		//{
		//	pool->RecConnect(con);
		//}
		throw err;

	}

}

void MachineDataBaseHelper::Update()
{
}


void MachineProtocolHelper::Insert()
{
	try
	{
		ConnectionPtr con = pool->GetConnect();

		string str_pstmt = "insert into " + machine_configuration_->header + "(PROTO_ID, PROTO_PATH, \
						PROTO_LAST_UPLOAD_EPOCH, PROTO_FILE_SIZE, SN, COMMENTS, DATA_SOURCE, PROTO_FILE_NAME) values (?, ?, ?, ?, ?, ?, ?, ?) ";

		PreparedStatementPtr pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));
		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		machine_configuration_->id = boost::uuids::to_string(uuid);
		boost::algorithm::replace_all(machine_configuration_->id, "-", "");

		pstmt->setString(1, machine_configuration_->id.c_str());
		pstmt->setString(2, machine_configuration_->file_path.c_str());
		pstmt->setDateTime(3, machine_configuration_->file_upload_epoch.c_str());
		pstmt->setString(4, machine_configuration_->file_size.c_str());
		pstmt->setString(5, machine_configuration_->SN.c_str());
		pstmt->setString(6, machine_configuration_->tag_.c_str());
		pstmt->setString(7, machine_configuration_->source_.c_str());
		pstmt->setString(8, machine_configuration_->file_name.c_str());
		pstmt->executeUpdate();
		pstmt->close();
		pool->RecConnect(con);
	}
	catch (const sql::SQLException& err)
	{
		if (!strcmp(err.what(), "Lost connection to MySQL server during query"))
		{
			Insert();
			return;
		}
		//else
		//{
		//	pool->RecConnect(con);
		//}
		throw err;

	}
}

void MachineProtocolHelper::Update()
{
}

void MachineGUIconfigHelper::Insert()
{
	try
	{
		ConnectionPtr con = pool->GetConnect();

		string str_pstmt = "insert into " + machine_configuration_->header + " (GUICONF_ID, GUICONF_PATH, \
						GUICONF_LAST_UPLOAD_EPOCH, GUICONF_FILE_SIZE, SN, COMMENTS, DATA_SOURCE, GUICONF_FILE_NAME) values (?, ?, ?, ?, ?, ?, ?, ?) ";

		PreparedStatementPtr pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));
		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		machine_configuration_->id = boost::uuids::to_string(uuid);
		boost::algorithm::replace_all(machine_configuration_->id, "-", "");

		pstmt->setString(1, machine_configuration_->id.c_str());
		pstmt->setString(2, machine_configuration_->file_path.c_str());
		pstmt->setDateTime(3, machine_configuration_->file_upload_epoch.c_str());
		pstmt->setString(4, machine_configuration_->file_size.c_str());
		pstmt->setString(5, machine_configuration_->SN.c_str());
		pstmt->setString(6, machine_configuration_->tag_.c_str());
		pstmt->setString(7, machine_configuration_->source_.c_str());
		pstmt->setString(8, machine_configuration_->file_name.c_str());
		pstmt->executeUpdate();
		pstmt->close();
		pool->RecConnect(con);
	}
	catch (const sql::SQLException& err)
	{
		if (!strcmp(err.what(), "Lost connection to MySQL server during query"))
		{
			Insert();
			return;
		}
		//else
		//{
		//	pool->RecConnect(con);
		//}
		throw err;

	}
}

void MachineGUIconfigHelper::Update()
{
}


void MachineTubeHistoryHelper::Insert()
{
	try
	{
		ConnectionPtr con = pool->GetConnect();

		string str_pstmt = "insert into " + machine_tube_history_->header + " (ID, TUBE_SERIAL_NUMBER, TUBE_DAY_EXPOSE_TIMES, \
						TUBE_TOTAL_EXPOSE_TIMES, TUBE_UPDATE_EPOCH, SN) values (?, ?, ?, ?, ?, ?)";

		PreparedStatementPtr pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));

		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		machine_tube_history_->id = boost::uuids::to_string(uuid);
		boost::algorithm::replace_all(machine_tube_history_->id, "-", "");
		pstmt->setString(1, machine_tube_history_->id.c_str());

		pstmt->setString(2, machine_tube_history_->tube_serialNumber.c_str());
		pstmt->setString(3, machine_tube_history_->today_exposure.c_str());
		pstmt->setString(4, machine_tube_history_->today_lifetime_exposure.c_str());
		pstmt->setDateTime(5, machine_tube_history_->exposure_epoch.c_str());
		pstmt->setString(6, machine_tube_history_->SN.c_str());

		pstmt->executeUpdate();
		pstmt->close();
		pool->RecConnect(con);
	}
	catch (const sql::SQLException& err)
	{
		if (!strcmp(err.what(), "Lost connection to MySQL server during query"))
		{
			Insert();
			return;
		}
		//else
		//{
		//	pool->RecConnect(con);
		//}
		throw err;
	}
}


void MachineTubeHistoryHelper::Update()
{
	try
	{
		ConnectionPtr con = pool->GetConnect();

		string head = "update " + machine_tube_history_->header + " set ";
		string body;
		if (machine_tube_history_->today_exposure != "") body += "TUBE_DAY_EXPOSE_TIMES = '" + machine_tube_history_->today_exposure + "',";
		if (machine_tube_history_->today_lifetime_exposure != "") body += "TUBE_TOTAL_EXPOSE_TIMES = '" + machine_tube_history_->today_lifetime_exposure + "',";
		if (machine_tube_history_->tube_serialNumber != "") body += "TUBE_SERIAL_NUMBER = '" + machine_tube_history_->tube_serialNumber + "',";
		//if (machine_tube_history_->exposure_epoch != "") body += "TUBE_UPDATE_EPOCH = '" + machine_tube_history_->exposure_epoch + "',";
		body[body.length() - 1] = ' ';
		string end = "Where SN = '" + machine_tube_history_->SN + "'" + " AND " + "TUBE_UPDATE_EPOCH = '" + machine_tube_history_->exposure_epoch + "'" +
					" AND " + "TUBE_SERIAL_NUMBER = '" + machine_tube_history_->tube_serialNumber + "'";
		string str_pstmt = head + body + end;

		StatementPtr stmt = StatementPtr(con->createStatement());
		stmt->execute(str_pstmt.c_str());
		stmt->close();
		pool->RecConnect(con);
	}
	catch (const sql::SQLException& err)
	{
		if (!strcmp(err.what(), "Lost connection to MySQL server during query"))
		{
			Update();
			return;
		}
		//else
		//{
		//	pool->RecConnect(con);
		//}
		throw err;

	}

}

bool MachineTubeHistoryHelper::IsSameEpoch()
{
	try
	{
		ConnectionPtr con = pool->GetConnect();

		vector<string> m_strs;
		boost::split(m_strs, machine_tube_history_->exposure_epoch, boost::is_any_of(" "));

		string str_pstmt = "select TUBE_UPDATE_EPOCH, TUBE_SERIAL_NUMBER from "+ machine_tube_history_ ->header+ " where SN = ?";

		PreparedStatementPtr pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));
		pstmt->setString(1, machine_tube_history_->SN.c_str());
		ResultSetPtr res = ResultSetPtr(pstmt->executeQuery());
		while (res->next())
		{
			vector<string> m_temp;
			string des = res->getString("TUBE_UPDATE_EPOCH").c_str();
			string mysn = res->getString("TUBE_SERIAL_NUMBER").c_str();
			boost::split(m_temp, des, boost::is_any_of(" "));

			if (m_strs[0] == m_temp[0] && mysn == machine_tube_history_->tube_serialNumber)
			{
				res->close();
				pstmt->close();
				pool->RecConnect(con);
				return true;
			}

		}
		res->close();
		pstmt->close();
		pool->RecConnect(con);
		return false;
	}
	catch (const sql::SQLException& err)
	{
		if (!strcmp(err.what(), "Lost connection to MySQL server during query"))
		{		
			return IsSameEpoch();
		}
		//else
		//{
		//	pool->RecConnect(con);
		//}
		throw err;

	}
}

void MachineRotateHistoryHelper::Insert()
{
	try
	{
		ConnectionPtr con = pool->GetConnect();

		string str_pstmt = "insert into " + machine_rotate_history_->header + " (ID, RACK_MODEL, RACK_DAY_ROTATION_TIMES, \
						RACK_TOTAL_ROTATION_TIMES, RACK_UPDATE_EPOCH, SN) values (?, ?, ?, ?, ?, ?)";

		PreparedStatementPtr pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));

		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		machine_rotate_history_->id = boost::uuids::to_string(uuid);
		boost::algorithm::replace_all(machine_rotate_history_->id, "-", "");
		pstmt->setString(1, machine_rotate_history_->id.c_str());

		pstmt->setString(2, machine_rotate_history_->rack_model.c_str());
		pstmt->setString(3, machine_rotate_history_->today_rotation.c_str());
		pstmt->setString(4, machine_rotate_history_->today_lifetime_rotation.c_str());
		pstmt->setDateTime(5, machine_rotate_history_->rotation_epoch.c_str());
		pstmt->setString(6, machine_rotate_history_->SN.c_str());

		pstmt->executeUpdate();
		pstmt->close();
		pool->RecConnect(con);
	}
	catch (const sql::SQLException& err)
	{
		if (!strcmp(err.what(), "Lost connection to MySQL server during query"))
		{
			Insert();
			return;
		}
		//else
		//{
		//	pool->RecConnect(con);
		//}
		throw err;

	}

}


void MachineRotateHistoryHelper::Update()
{
	try
	{
		ConnectionPtr con = pool->GetConnect();

		string head = "update " + machine_rotate_history_->header + " set ";
		string body;
		if (machine_rotate_history_->today_rotation != "") body += "RACK_DAY_ROTATION_TIMES = '" + machine_rotate_history_->today_rotation + "',";
		if (machine_rotate_history_->today_lifetime_rotation != "") body += "RACK_TOTAL_ROTATION_TIMES = '" + machine_rotate_history_->today_lifetime_rotation + "',";
		//if (machine_rotate_history_->rotation_epoch != "") body += "RACK_UPDATE_EPOCH = '" + machine_rotate_history_->rotation_epoch + "',";
		body[body.length() - 1] = ' ';
		string end = "Where SN = '" + machine_rotate_history_->SN + "'" + " AND " + "RACK_UPDATE_EPOCH = '" + machine_rotate_history_->rotation_epoch + "'";
		string str_pstmt = head + body + end;

		StatementPtr stmt = StatementPtr(con->createStatement());
		stmt->execute(str_pstmt.c_str());
		stmt->close();

		pool->RecConnect(con);
	}
	catch (const sql::SQLException& err )
	{
		if (!strcmp(err.what(), "Lost connection to MySQL server during query"))
		{
			Update();
			return;
		}
		//else
		//{
		//	pool->RecConnect(con);
		//}
		throw err;
	}

}


bool MachineRotateHistoryHelper::IsSameEpoch()
{
	try
	{
		ConnectionPtr con = pool->GetConnect();

		vector<string> m_strs;
		boost::split(m_strs, machine_rotate_history_->rotation_epoch, boost::is_any_of(" "));

		string str_pstmt = "select RACK_UPDATE_EPOCH from " + machine_rotate_history_->header + " where SN = ?";

		PreparedStatementPtr pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));
		pstmt->setString(1, machine_rotate_history_->SN.c_str());
		ResultSetPtr res = ResultSetPtr(pstmt->executeQuery());
		while (res->next())
		{
			vector<string> m_temp;
			string des = res->getString("RACK_UPDATE_EPOCH").c_str();
			boost::split(m_temp, des, boost::is_any_of(" "));

			if (m_strs[0] == m_temp[0])
			{
				res->close();
				pstmt->close();
				pool->RecConnect(con);
				return true;
			}

		}
		res->close();
		pstmt->close();
		pool->RecConnect(con);
		return false;
	}
	catch (const sql::SQLException& err)
	{
		if (!strcmp(err.what(), "Lost connection to MySQL server during query"))
		{			
			return IsSameEpoch();
		}
		//else
		//{
		//	pool->RecConnect(con);
		//}

		throw err;
	}

}


bool CLoginHelper::LoginCheck()
{
	try
	{
		/* Create a connection */
		ConnectionPtr con = pool->GetConnect();
		/* Create a query */
		string ppstmt;
		string table_name;
		if (login_->header == "c_login")
			table_name = "c_machine";
		if (login_->header == "c_rd_login")
			table_name = "c_rd_machine";

		ppstmt = "Select MANUFACTURE_NO1 from " + table_name + " where MANUFACTURE_NO1 = ?";

		PreparedStatementPtr pstmt = PreparedStatementPtr(con->prepareStatement(ppstmt.c_str()));

		pstmt->setString(1, login_->user.c_str());
		ResultSetPtr res = ResultSetPtr(pstmt->executeQuery());
		string sn;
		while (res->next()) {
			sn = res->getString("MANUFACTURE_NO1").c_str();
			if (sn != "")
				break;
		}
		res->close();
		pstmt->close();

		if (sn != "")
		{
			//switch (msg_type_)
			//{
			//case Session::machine:
			ppstmt = "update " + table_name + " set LAST_KNOWN_STATE = ? Where MANUFACTURE_NO1 = ?";
			pstmt = PreparedStatementPtr(con->prepareStatement(ppstmt.c_str()));
			pstmt->setString(1, "On");
			pstmt->setString(2, sn.c_str());
			pstmt->executeUpdate();
			pstmt->close();

			pool->RecConnect(con);
			return true;
		}

		pool->RecConnect(con);
		return false;
	}
	catch (const sql::SQLException& err)
	{
		if (!strcmp(err.what(), "Lost connection to MySQL server during query"))
		{
			return LoginCheck();
		}
		//else
		//{
		//	pool->RecConnect(con);
		//}
		std::cout << err.what() << endl;
		throw err;
	}
}

bool CRDLoginHelper::LoginCheck()
{
	return false;
}

void MachineOtherInfoHelper::Insert()
{
	try
	{
		ConnectionPtr con = pool->GetConnect();

		string str_pstmt = "insert into " + machine_configuration_->header + " (INFO_ID, INFO_PATH, \
						INFO_LAST_UPLOAD_EPOCH, INFO_FILE_SIZE, SN, COMMENTS, DATA_SOURCE, INFO_FILE_NAME) values (?, ?, ?, ?, ?, ?, ?, ?) ";

		PreparedStatementPtr pstmt = PreparedStatementPtr(con->prepareStatement(str_pstmt.c_str()));
		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		machine_configuration_->id = boost::uuids::to_string(uuid);
		boost::algorithm::replace_all(machine_configuration_->id, "-", "");

		pstmt->setString(1, machine_configuration_->id.c_str());
		pstmt->setString(2, machine_configuration_->file_path.c_str());
		pstmt->setDateTime(3, machine_configuration_->file_upload_epoch.c_str());
		pstmt->setString(4, machine_configuration_->file_size.c_str());
		pstmt->setString(5, machine_configuration_->SN.c_str());
		pstmt->setString(6, machine_configuration_->tag_.c_str());
		pstmt->setString(7, machine_configuration_->source_.c_str());
		pstmt->setString(8, machine_configuration_->file_name.c_str());
		pstmt->executeUpdate();
		pstmt->close();
		pool->RecConnect(con);
	}
	catch (const sql::SQLException& err)
	{
		if (!strcmp(err.what(), "Lost connection to MySQL server during query"))
		{
			Insert();
			return;
		}
		//else
		//{
		//	pool->RecConnect(con);
		//}
		throw err;

	}
}

void MachineOtherInfoHelper::Update()
{
}
