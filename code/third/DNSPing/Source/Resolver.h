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

void __fastcall PrintFlags(
	const uint16_t Flags);
void __fastcall PrintFlags(
	const uint16_t Flags, 
	FILE *OutputFile);
void __fastcall PrintTypeClassesName(
	const uint16_t Type, 
	const uint16_t Classes);
void __fastcall PrintTypeClassesName(
	const uint16_t Type, 
	const uint16_t Classes, 
	FILE *OutputFile);
size_t __fastcall PrintDomainName(
	const char *Buffer, 
	const size_t Location);
size_t __fastcall PrintDomainName(
	const char *Buffer, 
	const size_t Location, 
	FILE *OutputFile);
void __fastcall PrintResourseData(
	const char *Buffer, 
	const size_t Location, 
	const uint16_t Length, 
	const uint16_t Type, 
	const uint16_t Classes);
void __fastcall PrintResourseData(
	const char *Buffer, 
	const size_t Location, 
	const uint16_t Length, 
	const uint16_t Type, 
	const uint16_t Classes, 
	FILE *OutputFile);
