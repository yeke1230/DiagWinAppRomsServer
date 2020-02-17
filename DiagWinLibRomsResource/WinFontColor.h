/***********************************************************************
* Filename: WinFontColor.h
* Original Author: Ke Ye
* File Creation Date: 2018.5.29
* Subsystem:
* Diagnosis
* description:
* Set console font color.
*
* Copyright Information © FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
**********************************************************************/

#pragma once

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <WINDOWS.H>
#include <STRING.H>

static void SetConsoleColor(WORD wAttribute)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, wAttribute);
}


inline std::ostream&  defcolor(std::ostream& ostr)
{
	SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	return ostr;
}

inline std::ostream&  greencolor(std::ostream& ostr)
{
	SetConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	return ostr;
}

inline std::ostream&  bluecolor(std::ostream& ostr)
{
	SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	return ostr;
}

inline std::ostream&  redcolor(std::ostream& ostr)
{
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
	return ostr;
}

inline std::ostream&  lredcolor(std::ostream& ostr)
{
	SetConsoleColor(FOREGROUND_RED);
	return ostr;
}
