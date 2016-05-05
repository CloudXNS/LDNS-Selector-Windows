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

#if defined(PLATFORM_WIN)
//Catch Control-C exception from keyboard.
BOOL __fastcall CtrlHandler(
	const DWORD fdwCtrlType)
{
//Handle the CTRL-C signal.
	if (fdwCtrlType == CTRL_C_EVENT)
	{
		fwprintf_s(stderr, L"Get Control-C.\n");
	}
//Handle the CTRL-Break signal.
	else if (fdwCtrlType == CTRL_BREAK_EVENT)
	{
		fwprintf_s(stderr, L"Get Control-Break.\n");
		PrintProcess(true, true);
		
		return TRUE;
	}
//Handle other signals.
	else {
		fwprintf_s(stderr, L"Get closing signal.\n");
	}

//Print statistics and close all file handles.
	PrintProcess(true, true);
	_fcloseall();

	WSACleanup();
	return FALSE;
}
#endif
