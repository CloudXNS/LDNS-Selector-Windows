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

extern ConfigurationTable ConfigurationParameter;

#if (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
//Handle the system signal.
void SIG_Handler(
	const int Signal)
{
	fwprintf_s(stderr, L"Get closing signal.\n");
	PrintProcess(true, true);

//Close file handle.
	if (ConfigurationParameter.OutputFile != nullptr)
		fclose(ConfigurationParameter.OutputFile);
	
	exit(EXIT_SUCCESS);
	return;
}
#endif
