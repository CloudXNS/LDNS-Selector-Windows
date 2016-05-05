// This code is part of DNSPing
// Ping with DNS requesting.
// Copyright (C) 2014-2016 Chengr28
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#include "Base.h"

//Global variables
extern ConfigurationTable ConfigurationParameter;

//Functions
size_t __fastcall ConfigurationInitialization(
	void);
#if defined(PLATFORM_WIN)
size_t __fastcall ReadCommands(
	int argc, 
	wchar_t* argv[]);
#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
size_t __fastcall ReadCommands(
	int argc, 
	char *argv[]);
#endif
size_t __fastcall OutputResultToFile(
	void);
void __fastcall PrintHeaderToScreen(
	const std::wstring wTargetAddressString,
	const std::wstring wTestDomain);

int dnsping_main(
    int argc,
    wchar_t* argv[]);