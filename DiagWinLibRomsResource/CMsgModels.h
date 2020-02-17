/***********************************************************************
* Filename: CMsgModels.h
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Define the CT message model.
*
* Copyright Information ?FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
**********************************************************************/

#pragma once
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

class CLogin {
public:
	CLogin(vector<string> info) {
		Set(info);
	}
	CLogin() {
	}
	void Set(vector<string> info) {
		for (int i = 0; i < info.size(); i += 2)
		{
			if (!strcmp(info[i].c_str(), "Id") && info[i + 1].c_str() != "")	user = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "Psd") && info[i + 1].c_str() != "")	password = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "Header") && info[i + 1].c_str() != "")	header = info[i + 1].c_str();

		}
	}
	string header;
	string user;
	string password;
};

class Machine
{
public:
	Machine() {
		last_com_time = "0001-01-01 00:00:00";
	}

	Machine(vector<string> info) {
		Set(info);
	}

	void Set(vector<string> info) {
		//device_state = "On";
		for (int i = 0; i < info.size(); i+=2)
		{
			if (!strcmp(info[i].c_str(), "SystemError"))
			{
				// replace ''' to '\'' for inserting into problem.
				boost::algorithm::replace_all(info[i + 1], "\'", "\'\'");
				problem = info[i + 1];
				if (problem != "")
					device_state = "Broken";
				else
					device_state = "On";
			}

			if (!strcmp(info[i].c_str(), "SystemError_Occure_Epoch") && info[i + 1] != "")	systemerror_occure_epoch = info[i + 1];	
			if (!strcmp(info[i].c_str(), "DeviceState") && info[i + 1] != "")				device_state = info[i + 1];
			if (!strcmp(info[i].c_str(), "TUBE_HEAT") && info[i + 1] != "")					tubeheat = info[i + 1];
			if (!strcmp(info[i].c_str(), "DiskContract_Image") && info[i + 1] != "")		diskcontract_image = info[i + 1];	
			if (!strcmp(info[i].c_str(), "DiskContract_RawData") && info[i + 1] != "")		diskcontract_rawdata = info[i + 1];
			if (!strcmp(info[i].c_str(), "BootTime_Lifetime") && info[i + 1] != "")			total_boottime = info[i + 1];
			if (!strcmp(info[i].c_str(), "BootTime_AllDays") && info[i + 1] != "")			bootTime_allDays = info[i + 1];
			if (!strcmp(info[i].c_str(), "ScanNumber_Lifetime") && info[i + 1] != "")		total_scannumber = info[i + 1];
			if (!strcmp(info[i].c_str(), "SN") && info[i + 1] != "")						SN = info[i + 1];
			if (!strcmp(info[i].c_str(), "SOFTWARE_VER"))									software_ver = info[i + 1];
			if (!strcmp(info[i].c_str(), "Header") && info[i + 1] != "")					header = info[i + 1];
			if (!strcmp(info[i].c_str(), "DetTempModel") && info[i + 1] != "")				dettempmodel_ = info[i + 1];
			if (!strcmp(info[i].c_str(), "RailTemperature") && info[i + 1] != "")
			{
				railtemperature = info[i + 1];
				is_railtemperature_insert_ = false;
			}
			if (!strcmp(info[i].c_str(), "Temperature") && info[i + 1] != "") {
				tempC = info[i + 1];
				is_temperature_insert_ = false;
			}
			if (!strcmp(info[i].c_str(), "Humidity") && info[i + 1] != "") {
				humidity = info[i + 1];
				is_humidty_insert_ = false;
			}
		}
	}
	
	string SN;
	string last_com_time;
	string problem;
	string systemerror_occure_epoch;
	string device_state="On";
	string tubeheat;
	string railtemperature;				// Detector temperature
	string diskcontract_image;
	string diskcontract_rawdata;
	string total_boottime;
	string total_scannumber;
	string bootTime_allDays;			
	string software_ver;
	string tempC;						// Bay temperature
	string humidity;
	string header;
	string dettempmodel_;
	bool is_humidty_insert_;
	bool is_temperature_insert_;
	bool is_railtemperature_insert_;
};

class Tube
{
public:
	Tube() {}

	void Set(vector<string> info) {
		for (int i = 0; i < info.size(); i += 2)
		{
			if (!strcmp(info[i].c_str(), "tube_model")) tube_model = info[i + 1].c_str();
		}
	}

	string tube_model;
};

class MachineConfiguration
{
public:

	MachineConfiguration() {
		file_upload_epoch = "0001-01-01 00:00:00";
	}

	MachineConfiguration(vector<string> info) {
		file_upload_epoch = "0001-01-01 00:00:00";
		Set(info);
	}

	void Set(vector<string> info) {
		for (int i = 0; i < info.size(); i += 2)
		{
			if (!strcmp(info[i].c_str(), "File_Path")) {
				file_path = info[i + 1].c_str();
				vector<string> strs;
				boost::algorithm::split(strs, file_path, boost::algorithm::is_any_of("/"));
				file_name = strs[strs.size() - 1].c_str();
				boost::algorithm::replace_all(file_path, "/", "\\");
				//file_path = "C:\\RDC\\RDC_Files" + file_path;
			}
			if (!strcmp(info[i].c_str(), "File_Upload_Epoch"))					file_upload_epoch = info[i + 1];
			if (!strcmp(info[i].c_str(), "File_Size"))							file_size = info[i + 1];
			if (!strcmp(info[i].c_str(), "SN"))									SN = info[i + 1];
			if (!strcmp(info[i].c_str(), "Header") && info[i + 1] != "")		header = info[i + 1];
			if (!strcmp(info[i].c_str(), "Tag") && info[i + 1] != "")			tag_ = info[i + 1];
			if (!strcmp(info[i].c_str(), "Source") && info[i + 1] != "")		source_ = info[i + 1];
			if (!strcmp(info[i].c_str(), "Identify") && info[i + 1] != "")		identify_ = info[i + 1];
			if (!strcmp(info[i].c_str(), "SystemError") && info[i + 1] != "")	description_ = info[i + 1];
		}
	}

	string id;
	string file_path;
	string file_name;
	string file_upload_epoch;
	string file_size;
	string SN;
	string header;
	string tag_;
	string source_;
	string identify_;
	string description_;
};


class MachineTube
{
public:

	MachineTube() {
	}

	void Set(vector<string> info) {
		for (int i = 0; i < info.size(); i += 2)
		{
			//if (!strcmp(info[i].c_str(), "Tube_Model")) tube_model = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "Exposure_Lifetime")) lifetime_exposure = info[i + 1].c_str();
			//if (!strcmp(info[i].c_str(), "Tube_Creation_Date")) tube_creation_date = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "SN")) SN = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "Header") && info[i + 1].c_str() != "")	header = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "TubeSN") && info[i + 1].c_str() != "")	tubesn_ = info[i + 1].c_str();
		}
	}

	string tube_model;
	string tube_serialNumber;
	string lifetime_exposure;
	string tube_creation_date;
	string SN;
	string header;
	string tubesn_;
};

class MachineTubeHistory
{
public:

	MachineTubeHistory() {
	}

	void Set(vector<string> info) {
		for (int i = 0; i < info.size(); i += 2)
		{
			if (!strcmp(info[i].c_str(), "Tube_Model")) tube_model = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "TubeSN")) tube_serialNumber = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "Exposure_Today")) today_exposure = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "Exposure_Lifetime")) today_lifetime_exposure = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "Exposure_Epoch")) exposure_epoch = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "SN")) SN = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "Header") && info[i + 1].c_str() != "")	header = info[i + 1].c_str();
		}
	}

	string id;
	string tube_model;
	string tube_serialNumber;
	string today_exposure;
	string today_lifetime_exposure;
	string exposure_epoch;
	string SN;
	string header;
};

class MachineRotate
{
public:

	MachineRotate() {
	}

	void Set(vector<string> info) {
		for (int i = 0; i < info.size(); i += 2)
		{
			if (!strcmp(info[i].c_str(), "Rotation_Lifetime")) lifetime_rotation = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "SN")) SN = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "Header") && info[i + 1].c_str() != "")	header = info[i + 1].c_str();

		}
	}

	string lifetime_rotation;
	string SN;
	string header;

};

class MachineRotateHistory
{
public:

	MachineRotateHistory() {
	}

	void Set(vector<string> info) {
		for (int i = 0; i < info.size(); i += 2)
		{
			if (!strcmp(info[i].c_str(), "Rotation_Today")) today_rotation = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "Rotation_Lifetime")) today_lifetime_rotation = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "Rotation_Epoch")) rotation_epoch = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "SN")) SN = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "Header") && info[i + 1].c_str() != "")	header = info[i + 1].c_str();

		}
	}

	string id;
	string rack_model;
	string today_rotation;
	string today_lifetime_rotation;
	string rotation_epoch;
	string SN;
	string header;

};


typedef std::shared_ptr<Machine> MachinePtr;
typedef std::shared_ptr<Tube> TubePtr;
typedef std::shared_ptr<MachineConfiguration> MachineConfigurationPtr;
typedef std::shared_ptr<MachineTube> MachineTubePtr;
typedef std::shared_ptr<MachineTubeHistory> MachineTubeHistoryPtr;
typedef std::shared_ptr<MachineRotate> MachineRotatePtr;
typedef std::shared_ptr<MachineRotateHistory> MachineRotateHistoryPtr;
typedef std::shared_ptr<CLogin> CLoginPtr;




