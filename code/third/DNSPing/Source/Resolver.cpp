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


#include "Resolver.h"

//Print response hexs
void __fastcall PrintResponseHex(
	const char *Buffer, 
	const size_t Length)
{
//Initialization
	size_t Index = 0;

//Start.
	fwprintf_s(stderr, L"------------------------------ Response Hex ------------------------------\n");

//Print hexs.
	for (Index = 0;Index < Length;++Index)
	{
		if (Index == 0)
		{
			fwprintf_s(stderr, L"0000  %02x ", (UCHAR)Buffer[Index]);
		}
		else if (Index % NUM_HEX + 1U == NUM_HEX)
		{
			fwprintf_s(stderr, L"%02x   ", (UCHAR)Buffer[Index]);
			for (size_t InnerIndex = Index - (NUM_HEX - 1U);InnerIndex < Index + 1U;++InnerIndex)
			{
				if (InnerIndex != Index - (NUM_HEX - 1U) && InnerIndex % (NUM_HEX / 2U) == 0)
					fwprintf_s(stderr, L" ");
				if ((UCHAR)Buffer[InnerIndex] >= ASCII_SPACE && (UCHAR)Buffer[InnerIndex] <= ASCII_TILDE)
					fwprintf_s(stderr, L"%c", (UCHAR)Buffer[InnerIndex]);
				else 
					fwprintf_s(stderr, L".");
			}
			if (Index + 1U < Length)
			{
				fwprintf_s(stderr, L"\n%04x  ", (UINT)(Index + 1U));
			}
		}
		else {
			if (Index % (NUM_HEX / 2U) == 0 && Index % NUM_HEX != 0)
				fwprintf_s(stderr, L" ");
			fwprintf_s(stderr, L"%02x ", (UCHAR)Buffer[Index]);
		}
	}
	if (NUM_HEX - Length % NUM_HEX < NUM_HEX)
	{
		for (Index = 0;Index < NUM_HEX - Length % NUM_HEX;++Index)
			fwprintf_s(stderr, L"   ");
	}
	if (Length % NUM_HEX > 0)
	{
		fwprintf_s(stderr, L"   ");
		for (Index = Length - Length % NUM_HEX;Index < Length;++Index)
		{
			if ((UCHAR)Buffer[Index] >= ASCII_SPACE && (UCHAR)Buffer[Index] <= ASCII_TILDE)
				fwprintf_s(stderr, L"%c", (UCHAR)Buffer[Index]);
			else
				fwprintf_s(stderr, L".");
		}
	}
	fwprintf_s(stderr, L"\n");

//End.
	fwprintf_s(stderr, L"------------------------------ Response Hex ------------------------------\n");
	return;
}

//Print response hexs to file
void __fastcall PrintResponseHex(
	const char *Buffer, 
	const size_t Length, 
	FILE *OutputFile)
{
//Initialization
	size_t Index = 0;

//Start.
	fwprintf_s(OutputFile, L"------------------------------ Response Hex ------------------------------\n");

//Print hexs.
	for (Index = 0;Index < Length;++Index)
	{
		if (Index == 0)
		{
			fwprintf_s(OutputFile, L"0000  %02x ", (UCHAR)Buffer[Index]);
		}
		else if (Index % NUM_HEX + 1U == NUM_HEX)
		{
			fwprintf_s(OutputFile, L"%02x   ", (UCHAR)Buffer[Index]);
			for (size_t InnerIndex = Index - (NUM_HEX - 1U);InnerIndex < Index + 1U;++InnerIndex)
			{
				if (InnerIndex != Index - (NUM_HEX - 1U) && InnerIndex % (NUM_HEX / 2U) == 0)
					fwprintf_s(OutputFile, L" ");
				if ((UCHAR)Buffer[InnerIndex] >= ASCII_SPACE && (UCHAR)Buffer[InnerIndex] <= ASCII_TILDE)
					fwprintf_s(OutputFile, L"%c", (UCHAR)Buffer[InnerIndex]);
				else
					fwprintf_s(OutputFile, L".");
			}
			if (Index + 1U < Length)
			{
				fwprintf_s(OutputFile, L"\n%04x  ", (UINT)(Index + 1U));
			}
		}
		else {
			if (Index % (NUM_HEX / 2U) == 0 && Index % NUM_HEX != 0)
				fwprintf_s(OutputFile, L" ");
			fwprintf_s(OutputFile, L"%02x ", (UCHAR)Buffer[Index]);
		}
	}
	if (NUM_HEX - Length % NUM_HEX < NUM_HEX)
	{
		for (Index = 0;Index < NUM_HEX - Length % NUM_HEX;++Index)
			fwprintf_s(OutputFile, L"   ");
	}
	if (Length % NUM_HEX > 0)
	{
		fwprintf_s(OutputFile, L"   ");
		for (Index = Length - Length % NUM_HEX;Index < Length;++Index)
		{
			if ((UCHAR)Buffer[Index] >= ASCII_SPACE && (UCHAR)Buffer[Index] <= ASCII_TILDE)
				fwprintf_s(OutputFile, L"%c", (UCHAR)Buffer[Index]);
			else
				fwprintf_s(OutputFile, L".");
		}
	}
	fwprintf_s(OutputFile, L"\n");

//End.
	fwprintf_s(OutputFile, L"------------------------------ Response Hex ------------------------------\n");
	return;
}

//Print response result or data
void __fastcall PrintResponse(
	const char *Buffer, 
	const size_t Length)
{
//Initialization
	size_t Index = 0, CurrentLength = sizeof(dns_hdr);
	auto pdns_hdr = (dns_hdr *)Buffer;

//Start.
	fwprintf_s(stderr, L"-------------------------------- Response --------------------------------\n");

//Print DNS header.
	fwprintf_s(stderr, L"ID: 0x%04x\n", ntohs(pdns_hdr->ID));
	fwprintf_s(stderr, L"Flags: 0x%04x", ntohs(pdns_hdr->Flags));
	PrintFlags(pdns_hdr->Flags);
	fwprintf_s(stderr, L"Questions RR Count: %u\n", ntohs(pdns_hdr->Questions));
	fwprintf_s(stderr, L"Answer RR Count: %u\n", ntohs(pdns_hdr->Answer));
	fwprintf_s(stderr, L"Authority RR Count: %u\n", ntohs(pdns_hdr->Authority));
	fwprintf_s(stderr, L"Additional RR Count: %u\n", ntohs(pdns_hdr->Additional));

//Print Questions RR.
	if (ntohs(pdns_hdr->Questions) > 0)
	{
		fwprintf_s(stderr, L"Questions RR:\n   Name: ");
		dns_qry *pdns_qry = nullptr;
		for (Index = 0;Index < ntohs(pdns_hdr->Questions);++Index)
		{
		//Print Name.
			PrintDomainName(Buffer, CurrentLength);
			fwprintf_s(stderr, L"\n");
			CurrentLength += strnlen_s(Buffer + CurrentLength, Length - CurrentLength) + 1U;

		//Print Type and Classes.
			pdns_qry = (dns_qry *)(Buffer + CurrentLength);
			fwprintf_s(stderr, L"   Type: 0x%04x", ntohs(pdns_qry->Type));
			PrintTypeClassesName(pdns_qry->Type, 0);
			fwprintf_s(stderr, L"   Classes: 0x%04x", ntohs(pdns_qry->Classes));
			PrintTypeClassesName(0, pdns_qry->Classes);
			CurrentLength += sizeof(dns_qry);
		}
	}

	dns_standard_record *pdns_standard_record = nullptr;
//Print Answer RR.
	if (ntohs(pdns_hdr->Answer) > 0)
	{
		fwprintf_s(stderr, L"Answer RR:\n");
		for (Index = 0;Index < ntohs(pdns_hdr->Answer);++Index)
		{
		//Print Name.
			fwprintf_s(stderr, L" RR(%u)\n   Name: ", (UINT)(Index + 1U));
			CurrentLength += PrintDomainName(Buffer, CurrentLength);
			fwprintf_s(stderr, L"\n");

		//Print Type, Classes, TTL and Length.
			pdns_standard_record = (dns_standard_record *)(Buffer + CurrentLength);
			fwprintf_s(stderr, L"   Type: 0x%04x", ntohs(pdns_standard_record->Type));
			PrintTypeClassesName(pdns_standard_record->Type, 0);
			fwprintf_s(stderr, L"   Classes: 0x%04x", ntohs(pdns_standard_record->Classes));
			PrintTypeClassesName(0, pdns_standard_record->Classes);
			fwprintf_s(stderr, L"   TTL: %u", ntohl(pdns_standard_record->TTL));
			PrintSecondsInDateTime(ntohl(pdns_standard_record->TTL));
			fwprintf_s(stderr, L"\n");
			fwprintf_s(stderr, L"   Length: %u", ntohs(pdns_standard_record->Length));
			CurrentLength += sizeof(dns_standard_record);
			PrintResourseData(Buffer, CurrentLength, ntohs(pdns_standard_record->Length), pdns_standard_record->Type, pdns_standard_record->Classes);
			CurrentLength += ntohs(pdns_standard_record->Length);
		}
	}

//Print Authority RR.
	if (ntohs(pdns_hdr->Authority) > 0)
	{
		fwprintf_s(stderr, L"Authority RR:\n");
		for (Index = 0;Index < ntohs(pdns_hdr->Authority);++Index)
		{
		//Print Name.
			fwprintf_s(stderr, L" RR(%u)\n   Name: ", (UINT)(Index + 1U));
			CurrentLength += PrintDomainName(Buffer, CurrentLength);
			fwprintf_s(stderr, L"\n");

		//Print Type, Classes, TTL and Length.
			pdns_standard_record = (dns_standard_record *)(Buffer + CurrentLength);
			fwprintf_s(stderr, L"   Type: 0x%04x", ntohs(pdns_standard_record->Type));
			PrintTypeClassesName(pdns_standard_record->Type, 0);
			fwprintf_s(stderr, L"   Classes: 0x%04x", ntohs(pdns_standard_record->Classes));
			PrintTypeClassesName(0, pdns_standard_record->Classes);
			fwprintf_s(stderr, L"   TTL: %u", ntohl(pdns_standard_record->TTL));
			PrintSecondsInDateTime(ntohl(pdns_standard_record->TTL));
			fwprintf_s(stderr, L"\n");
			fwprintf_s(stderr, L"   Length: %u", ntohs(pdns_standard_record->Length));
			CurrentLength += sizeof(dns_standard_record);
			PrintResourseData(Buffer, CurrentLength, ntohs(pdns_standard_record->Length), pdns_standard_record->Type, pdns_standard_record->Classes);
			CurrentLength += ntohs(pdns_standard_record->Length);
		}
	}

//Print Additional RR.
	if (ntohs(pdns_hdr->Additional) > 0)
	{
		fwprintf_s(stderr, L"Additional RR:\n");
		for (Index = 0;Index < ntohs(pdns_hdr->Additional);++Index)
		{
		//Print Name.
			fwprintf_s(stderr, L" RR(%u)\n   Name: ", (UINT)(Index + 1U));
			CurrentLength += PrintDomainName(Buffer, CurrentLength);
			fwprintf_s(stderr, L"\n");

		//Print Type, Classes, TTL and Length.
			pdns_standard_record = (dns_standard_record *)(Buffer + CurrentLength);
			fwprintf_s(stderr, L"   Type: 0x%04x", ntohs(pdns_standard_record->Type));
			PrintTypeClassesName(pdns_standard_record->Type, 0);
			if (pdns_standard_record->Type == htons(DNS_RECORD_OPT)) //EDNS Label
			{
				PrintResourseData(Buffer, CurrentLength - 1U, ntohs(pdns_standard_record->Length), pdns_standard_record->Type, pdns_standard_record->Classes);
				CurrentLength += sizeof(dns_standard_record) + ntohs(pdns_standard_record->Length);
			}
			else {
				fwprintf_s(stderr, L"   Classes: 0x%04x", ntohs(pdns_standard_record->Classes));
				PrintTypeClassesName(0, pdns_standard_record->Classes);
				fwprintf_s(stderr, L"   TTL: %u", ntohl(pdns_standard_record->TTL));
				PrintSecondsInDateTime(ntohl(pdns_standard_record->TTL));
				fwprintf_s(stderr, L"\n");
				fwprintf_s(stderr, L"   Length: %u", ntohs(pdns_standard_record->Length));
				CurrentLength += sizeof(dns_standard_record);
				PrintResourseData(Buffer, CurrentLength, ntohs(pdns_standard_record->Length), pdns_standard_record->Type, pdns_standard_record->Classes);
				CurrentLength += ntohs(pdns_standard_record->Length);
			}
		}
	}

//End.
	fwprintf_s(stderr, L"-------------------------------- Response --------------------------------\n");
	return;
}

//Print response result or data to file
void __fastcall PrintResponse(
	const char *Buffer, 
	const size_t Length, 
	FILE *OutputFile)
{
//Initialization
	size_t Index = 0, CurrentLength = sizeof(dns_hdr);
	auto pdns_hdr = (dns_hdr *)Buffer;

//Start.
	fwprintf_s(OutputFile, L"-------------------------------- Response --------------------------------\n");

//Print DNS header.
	fwprintf_s(OutputFile, L"ID: 0x%04x\n", ntohs(pdns_hdr->ID));
	fwprintf_s(OutputFile, L"Flags: 0x%04x", ntohs(pdns_hdr->Flags));
	PrintFlags(pdns_hdr->Flags, OutputFile);
	fwprintf_s(OutputFile, L"Questions RR Count: %u\n", ntohs(pdns_hdr->Questions));
	fwprintf_s(OutputFile, L"Answer RR Count: %u\n", ntohs(pdns_hdr->Answer));
	fwprintf_s(OutputFile, L"Authority RR Count: %u\n", ntohs(pdns_hdr->Authority));
	fwprintf_s(OutputFile, L"Additional RR Count: %u\n", ntohs(pdns_hdr->Additional));

//Print Questions RR.
	if (ntohs(pdns_hdr->Questions) > 0)
	{
		fwprintf_s(OutputFile, L"Questions RR:\n   Name: ");
		dns_qry *pdns_qry = nullptr;
		for (Index = 0;Index < ntohs(pdns_hdr->Questions);++Index)
		{
		//Print Name.
			PrintDomainName(Buffer, CurrentLength, OutputFile);
			fwprintf_s(OutputFile, L"\n");
			CurrentLength += strnlen_s(Buffer + CurrentLength, Length - CurrentLength) + 1U;

		//Print Type and Classes.
			pdns_qry = (dns_qry *)(Buffer + CurrentLength);
			fwprintf_s(OutputFile, L"   Type: 0x%04x", ntohs(pdns_qry->Type));
			PrintTypeClassesName(pdns_qry->Type, 0, OutputFile);
			fwprintf_s(OutputFile, L"   Classes: 0x%04x", ntohs(pdns_qry->Classes));
			PrintTypeClassesName(0, pdns_qry->Classes, OutputFile);
			CurrentLength += sizeof(dns_qry);
		}
	}

	dns_standard_record *pdns_standard_record = nullptr;
//Print Answer RR.
	if (ntohs(pdns_hdr->Answer) > 0)
	{
		fwprintf_s(OutputFile, L"Answer RR:\n");
		for (Index = 0;Index < ntohs(pdns_hdr->Answer);++Index)
		{
		//Print Name.
			fwprintf_s(OutputFile, L" RR(%u)\n   Name: ", (UINT)(Index + 1U));
			CurrentLength += PrintDomainName(Buffer, CurrentLength, OutputFile);
			fwprintf_s(OutputFile, L"\n");

		//Print Type, Classes, TTL and Length.
			pdns_standard_record = (dns_standard_record *)(Buffer + CurrentLength);
			fwprintf_s(OutputFile, L"   Type: 0x%04x", ntohs(pdns_standard_record->Type));
			PrintTypeClassesName(pdns_standard_record->Type, 0, OutputFile);
			fwprintf_s(OutputFile, L"   Classes: 0x%04x", ntohs(pdns_standard_record->Classes));
			PrintTypeClassesName(0, pdns_standard_record->Classes, OutputFile);
			fwprintf_s(OutputFile, L"   TTL: %u", ntohl(pdns_standard_record->TTL));
			PrintSecondsInDateTime(ntohl(pdns_standard_record->TTL));
			fwprintf_s(OutputFile, L"\n");
			fwprintf_s(OutputFile, L"   Length: %u", ntohs(pdns_standard_record->Length));
			CurrentLength += sizeof(dns_standard_record);
			PrintResourseData(Buffer, CurrentLength, ntohs(pdns_standard_record->Length), pdns_standard_record->Type, pdns_standard_record->Classes, OutputFile);
			CurrentLength += ntohs(pdns_standard_record->Length);
		}
	}

//Print Authority RR.
	if (ntohs(pdns_hdr->Authority) > 0)
	{
		fwprintf_s(OutputFile, L"Authority RR:\n");
		for (Index = 0;Index < ntohs(pdns_hdr->Authority);++Index)
		{
		//Print Name.
			fwprintf_s(OutputFile, L" RR(%u)\n   Name: ", (UINT)(Index + 1U));
			CurrentLength += PrintDomainName(Buffer, CurrentLength, OutputFile);
			fwprintf_s(OutputFile, L"\n");

		//Print Type, Classes, TTL and Length.
			pdns_standard_record = (dns_standard_record *)(Buffer + CurrentLength);
			fwprintf_s(OutputFile, L"   Type: 0x%04x", ntohs(pdns_standard_record->Type));
			PrintTypeClassesName(pdns_standard_record->Type, 0, OutputFile);
			fwprintf_s(OutputFile, L"   Classes: 0x%04x", ntohs(pdns_standard_record->Classes));
			PrintTypeClassesName(0, pdns_standard_record->Classes, OutputFile);
			fwprintf_s(OutputFile, L"   TTL: %u", ntohl(pdns_standard_record->TTL));
			PrintSecondsInDateTime(ntohl(pdns_standard_record->TTL));
			fwprintf_s(OutputFile, L"\n");
			fwprintf_s(OutputFile, L"   Length: %u", ntohs(pdns_standard_record->Length));
			CurrentLength += sizeof(dns_standard_record);
			PrintResourseData(Buffer, CurrentLength, ntohs(pdns_standard_record->Length), pdns_standard_record->Type, pdns_standard_record->Classes, OutputFile);
			CurrentLength += ntohs(pdns_standard_record->Length);
		}
	}

//Print Additional RR.
	if (ntohs(pdns_hdr->Additional) > 0)
	{
		fwprintf_s(OutputFile, L"Additional RR:\n");
		for (Index = 0;Index < ntohs(pdns_hdr->Additional);++Index)
		{
		//Print Name.
			fwprintf_s(OutputFile, L" RR(%u)\n   Name: ", (UINT)(Index + 1U));
			CurrentLength += PrintDomainName(Buffer, CurrentLength, OutputFile);
			fwprintf_s(OutputFile, L"\n");

		//Print Type, Classes, TTL and Length.
			pdns_standard_record = (dns_standard_record *)(Buffer + CurrentLength);
			fwprintf_s(OutputFile, L"   Type: 0x%04x", ntohs(pdns_standard_record->Type));
			PrintTypeClassesName(pdns_standard_record->Type, 0, OutputFile);
			if (pdns_standard_record->Type == htons(DNS_RECORD_OPT)) //EDNS Label
			{
				PrintResourseData(Buffer, CurrentLength - 1U, ntohs(pdns_standard_record->Length), pdns_standard_record->Type, pdns_standard_record->Classes, OutputFile);
				CurrentLength += sizeof(dns_standard_record) + ntohs(pdns_standard_record->Length);
			}
			else {
				fwprintf_s(OutputFile, L"   Classes: 0x%04x", ntohs(pdns_standard_record->Classes));
				PrintTypeClassesName(0, pdns_standard_record->Classes, OutputFile);
				fwprintf_s(OutputFile, L"   TTL: %u", ntohl(pdns_standard_record->TTL));
				PrintSecondsInDateTime(ntohl(pdns_standard_record->TTL));
				fwprintf_s(OutputFile, L"\n");
				fwprintf_s(OutputFile, L"   Length: %u", ntohs(pdns_standard_record->Length));
				CurrentLength += sizeof(dns_standard_record);
				PrintResourseData(Buffer, CurrentLength, ntohs(pdns_standard_record->Length), pdns_standard_record->Type, pdns_standard_record->Classes, OutputFile);
				CurrentLength += ntohs(pdns_standard_record->Length);
			}
		}
	}

//End.
	fwprintf_s(OutputFile, L"-------------------------------- Response --------------------------------\n");
	return;
}

//Print Header Flags
void __fastcall PrintFlags(
	const uint16_t Flags)
{
//Print Flags
	if (Flags > 0)
	{
		auto FlagsBits = ntohs(Flags);

	//Print OPCode
		fwprintf_s(stderr, L"(OPCode: ");
		FlagsBits = FlagsBits & HIGHEST_BIT_U16;
		FlagsBits = FlagsBits >> 11U;
		if (FlagsBits == DNS_OPCODE_QUERY)
			fwprintf_s(stderr, L"Query");
		else if (FlagsBits == DNS_OPCODE_IQUERY)
			fwprintf_s(stderr, L"Inverse Query");
		else if (FlagsBits == DNS_OPCODE_STATUS)
			fwprintf_s(stderr, L"Status");
		else if (FlagsBits == DNS_OPCODE_NOTIFY)
			fwprintf_s(stderr, L"Notify");
		else if (FlagsBits == DNS_OPCODE_UPDATE)
			fwprintf_s(stderr, L"Update");
		else
			fwprintf_s(stderr, L"%x", FlagsBits);

	//Print RCode.
		fwprintf_s(stderr, L"/RCode: ");
		FlagsBits = ntohs(Flags);
		FlagsBits = FlagsBits & UINT4_MAX;
		if (FlagsBits == DNS_RCODE_NOERROR)
			fwprintf_s(stderr, L"No Error");
		else if (FlagsBits == DNS_RCODE_FORMERR)
			fwprintf_s(stderr, L"Format Error");
		else if (FlagsBits == DNS_RCODE_SERVFAIL)
			fwprintf_s(stderr, L"Server Failure");
		else if (FlagsBits == DNS_RCODE_NXDOMAIN)
			fwprintf_s(stderr, L"Non-Existent Domain");
		else if (FlagsBits == DNS_RCODE_NOTIMP)
			fwprintf_s(stderr, L"Not Implemented");
		else if (FlagsBits == DNS_RCODE_REFUSED)
			fwprintf_s(stderr, L"Query Refused");
		else if (FlagsBits == DNS_RCODE_YXDOMAIN)
			fwprintf_s(stderr, L"Name Exists when it should not");
		else if (FlagsBits == DNS_RCODE_YXRRSET)
			fwprintf_s(stderr, L"RR Set Exists when it should not");
		else if (FlagsBits == DNS_RCODE_NXRRSET)
			fwprintf_s(stderr, L"RR Set that should exist does not");
		else if (FlagsBits == DNS_RCODE_NOTAUTH)
			fwprintf_s(stderr, L"Server Not Authoritative for zone/Not Authorized");
		else if (FlagsBits == DNS_RCODE_NOTZONE)
			fwprintf_s(stderr, L"Name not contained in zone");
		else if (FlagsBits == DNS_RCODE_BADVERS)
			fwprintf_s(stderr, L"Bad OPT Version/TSIG Signature Failure");
		else if (FlagsBits == DNS_RCODE_BADKEY)
			fwprintf_s(stderr, L"Key not recognized");
		else if (FlagsBits == DNS_RCODE_BADTIME)
			fwprintf_s(stderr, L"Signature out of time window");
		else if (FlagsBits == DNS_RCODE_BADMODE)
			fwprintf_s(stderr, L"Bad TKEY Mode");
		else if (FlagsBits == DNS_RCODE_BADNAME)
			fwprintf_s(stderr, L"Duplicate key name");
		else if (FlagsBits == DNS_RCODE_BADALG)
			fwprintf_s(stderr, L"Algorithm not supported");
		else if (FlagsBits == DNS_RCODE_BADTRUNC)
			fwprintf_s(stderr, L"Bad Truncation");
		else if (FlagsBits >= DNS_RCODE_PRIVATE_A && FlagsBits <= DNS_RCODE_PRIVATE_B)
			fwprintf_s(stderr, L"Reserved Private use");
		else if (FlagsBits == DNS_OPCODE_RESERVED)
			fwprintf_s(stderr, L"Reserved");
		else
			fwprintf_s(stderr, L"%x", FlagsBits);
		fwprintf_s(stderr, L"))");
	}

	fwprintf_s(stderr, L"\n");
	return;
}

//Print Header Flags to file
void __fastcall PrintFlags(
	const uint16_t Flags, 
	FILE *OutputFile)
{
//Print Flags
	if (Flags > 0)
	{
		auto FlagsBits = ntohs(Flags);

	//Print OPCode
		fwprintf_s(OutputFile, L"(OPCode: ");
		FlagsBits = FlagsBits & HIGHEST_BIT_U16;
		FlagsBits = FlagsBits >> 11U;
		if (FlagsBits == DNS_OPCODE_QUERY)
			fwprintf_s(OutputFile, L"Query");
		else if (FlagsBits == DNS_OPCODE_IQUERY)
			fwprintf_s(OutputFile, L"Inverse Query");
		else if (FlagsBits == DNS_OPCODE_STATUS)
			fwprintf_s(OutputFile, L"Status");
		else if (FlagsBits == DNS_OPCODE_NOTIFY)
			fwprintf_s(OutputFile, L"Notify");
		else if (FlagsBits == DNS_OPCODE_UPDATE)
			fwprintf_s(OutputFile, L"Update");
		else
			fwprintf_s(OutputFile, L"%x", FlagsBits);

	//Print RCode.
		fwprintf_s(OutputFile, L"/RCode: ");
		FlagsBits = ntohs(Flags);
		FlagsBits = FlagsBits & UINT4_MAX;
		if (FlagsBits == DNS_RCODE_NOERROR)
			fwprintf_s(OutputFile, L"No Error");
		else if (FlagsBits == DNS_RCODE_FORMERR)
			fwprintf_s(OutputFile, L"Format Error");
		else if (FlagsBits == DNS_RCODE_SERVFAIL)
			fwprintf_s(OutputFile, L"Server Failure");
		else if (FlagsBits == DNS_RCODE_NXDOMAIN)
			fwprintf_s(OutputFile, L"Non-Existent Domain");
		else if (FlagsBits == DNS_RCODE_NOTIMP)
			fwprintf_s(OutputFile, L"Not Implemented");
		else if (FlagsBits == DNS_RCODE_REFUSED)
			fwprintf_s(OutputFile, L"Query Refused");
		else if (FlagsBits == DNS_RCODE_YXDOMAIN)
			fwprintf_s(OutputFile, L"Name Exists when it should not");
		else if (FlagsBits == DNS_RCODE_YXRRSET)
			fwprintf_s(OutputFile, L"RR Set Exists when it should not");
		else if (FlagsBits == DNS_RCODE_NXRRSET)
			fwprintf_s(OutputFile, L"RR Set that should exist does not");
		else if (FlagsBits == DNS_RCODE_NOTAUTH)
			fwprintf_s(OutputFile, L"Server Not Authoritative for zone/Not Authorized");
		else if (FlagsBits == DNS_RCODE_NOTZONE)
			fwprintf_s(OutputFile, L"Name not contained in zone");
		else if (FlagsBits == DNS_RCODE_BADVERS)
			fwprintf_s(OutputFile, L"Bad OPT Version/TSIG Signature Failure");
		else if (FlagsBits == DNS_RCODE_BADKEY)
			fwprintf_s(OutputFile, L"Key not recognized");
		else if (FlagsBits == DNS_RCODE_BADTIME)
			fwprintf_s(OutputFile, L"Signature out of time window");
		else if (FlagsBits == DNS_RCODE_BADMODE)
			fwprintf_s(OutputFile, L"Bad TKEY Mode");
		else if (FlagsBits == DNS_RCODE_BADNAME)
			fwprintf_s(OutputFile, L"Duplicate key name");
		else if (FlagsBits == DNS_RCODE_BADALG)
			fwprintf_s(OutputFile, L"Algorithm not supported");
		else if (FlagsBits == DNS_RCODE_BADTRUNC)
			fwprintf_s(OutputFile, L"Bad Truncation");
		else if (FlagsBits >= DNS_RCODE_PRIVATE_A && FlagsBits <= DNS_RCODE_PRIVATE_B)
			fwprintf_s(OutputFile, L"Reserved Private use");
		else if (FlagsBits == DNS_OPCODE_RESERVED)
			fwprintf_s(OutputFile, L"Reserved");
		else
			fwprintf_s(OutputFile, L"%x", FlagsBits);
		fwprintf_s(OutputFile, L"))");
	}

	fwprintf_s(OutputFile, L"\n");
	return;
}

//Print Type and Classes name
void __fastcall PrintTypeClassesName(
	const uint16_t Type, 
	const uint16_t Classes)
{
//Print Classes.
	if (Classes > 0)
	{
	//Cache flush check
		auto HighBitSet = false;
		auto ClassesTemp = ntohs(Classes);
		if (ClassesTemp >> HIGHEST_MOVE_BIT_U16 > 0)
		{
			HighBitSet = true;
			ClassesTemp = ntohs(Classes);
			ClassesTemp = ClassesTemp & HIGHEST_BIT_U16;
		}

	//Classes check
		if (ClassesTemp == DNS_CLASS_IN)
		{
			fwprintf_s(stderr, L"(Class IN");
			if (HighBitSet)
				fwprintf_s(stderr, L"/Unicast Queries or Cache Flush");
			fwprintf_s(stderr, L"))");
		}
		else if (ClassesTemp == DNS_CLASS_CSNET)
		{
			fwprintf_s(stderr, L"(Class CSNET");
			if (HighBitSet)
				fwprintf_s(stderr, L"/Unicast Queries or Cache Flush");
			fwprintf_s(stderr, L"))");
		}
		else if (ClassesTemp == DNS_CLASS_CHAOS)
		{
			fwprintf_s(stderr, L"(Class CHAOS");
			if (HighBitSet)
				fwprintf_s(stderr, L"/Unicast Queries or Cache Flush");
			fwprintf_s(stderr, L"))");
		}
		else if (ClassesTemp == DNS_CLASS_HESIOD)
		{
			fwprintf_s(stderr, L"(Class HESIOD");
			if (HighBitSet)
				fwprintf_s(stderr, L"/Unicast Queries or Cache Flush");
			fwprintf_s(stderr, L"))");
		}
		else if (ClassesTemp == DNS_CLASS_NONE)
		{
			fwprintf_s(stderr, L"(Class NONE");
			if (HighBitSet)
				fwprintf_s(stderr, L"/Unicast Queries or Cache Flush");
			fwprintf_s(stderr, L"))");
		}
		else if (ClassesTemp == DNS_CLASS_ALL)
		{
			fwprintf_s(stderr, L"(Class ALL");
			if (HighBitSet)
				fwprintf_s(stderr, L"/Unicast Queries or Cache Flush");
			fwprintf_s(stderr, L"))");
		}
		else if (ClassesTemp == DNS_CLASS_ANY)
		{
			fwprintf_s(stderr, L"(Class ANY");
			if (HighBitSet)
				fwprintf_s(stderr, L"/Unicast Queries or Cache Flush");
			fwprintf_s(stderr, L"))");
		}
	}
//Print Type.
	else {
		if (Type == htons(DNS_RECORD_A))
			fwprintf_s(stderr, L"(A Record)");
		else if (Type == htons(DNS_RECORD_NS))
			fwprintf_s(stderr, L"(NS Record)");
		else if (Type == htons(DNS_RECORD_MD))
			fwprintf_s(stderr, L"(MD Record)");
		else if (Type == htons(DNS_RECORD_MF))
			fwprintf_s(stderr, L"(MF Record)");
		else if (Type == htons(DNS_RECORD_CNAME))
			fwprintf_s(stderr, L"(CNAME Record)");
		else if (Type == htons(DNS_RECORD_SOA))
			fwprintf_s(stderr, L"(SOA Record)");
		else if (Type == htons(DNS_RECORD_MB))
			fwprintf_s(stderr, L"(MB Record)");
		else if (Type == htons(DNS_RECORD_MG))
			fwprintf_s(stderr, L"(MG Record)");
		else if (Type == htons(DNS_RECORD_MR))
			fwprintf_s(stderr, L"(MR Record)");
		else if (Type == htons(DNS_RECORD_NULL))
			fwprintf_s(stderr, L"(NULL Record)");
		else if (Type == htons(DNS_RECORD_WKS))
			fwprintf_s(stderr, L"(WKS Record)");
		else if (Type == htons(DNS_RECORD_PTR))
			fwprintf_s(stderr, L"(PTR Record)");
		else if (Type == htons(DNS_RECORD_HINFO))
			fwprintf_s(stderr, L"(HINFO Record)");
		else if (Type == htons(DNS_RECORD_MINFO))
			fwprintf_s(stderr, L"(MINFO Record)");
		else if (Type == htons(DNS_RECORD_MX))
			fwprintf_s(stderr, L"(MX Record)");
		else if (Type == htons(DNS_RECORD_TXT))
			fwprintf_s(stderr, L"(TXT Record)");
		else if (Type == htons(DNS_RECORD_RP))
			fwprintf_s(stderr, L"(RP Record)");
		else if (Type == htons(DNS_RECORD_RP))
			fwprintf_s(stderr, L"(RP Record)");
		else if (Type == htons(DNS_RECORD_AFSDB))
			fwprintf_s(stderr, L"(AFSDB Record)");
		else if (Type == htons(DNS_RECORD_X25))
			fwprintf_s(stderr, L"(X25 Record)");
		else if (Type == htons(DNS_RECORD_ISDN))
			fwprintf_s(stderr, L"(ISDN Record)");
		else if (Type == htons(DNS_RECORD_RT))
			fwprintf_s(stderr, L"(RT Record)");
		else if (Type == htons(DNS_RECORD_NSAP))
			fwprintf_s(stderr, L"(NSAP Record)");
		else if (Type == htons(DNS_RECORD_NSAP_PTR))
			fwprintf_s(stderr, L"(NSAP PTR Record)");
		else if (Type == htons(DNS_RECORD_SIG))
			fwprintf_s(stderr, L"(SIG Record)");
		else if (Type == htons(DNS_RECORD_KEY))
			fwprintf_s(stderr, L"(KEY Record)");
		else if (Type == htons(DNS_RECORD_PX))
			fwprintf_s(stderr, L"(PX Record)");
		else if (Type == htons(DNS_RECORD_GPOS))
			fwprintf_s(stderr, L"(GPOS Record)");
		else if (Type == htons(DNS_RECORD_AAAA))
			fwprintf_s(stderr, L"(AAAA Record)");
		else if (Type == htons(DNS_RECORD_LOC))
			fwprintf_s(stderr, L"(LOC Record)");
		else if (Type == htons(DNS_RECORD_NXT))
			fwprintf_s(stderr, L"(NXT Record)");
		else if (Type == htons(DNS_RECORD_EID))
			fwprintf_s(stderr, L"(EID Record)");
		else if (Type == htons(DNS_RECORD_NIMLOC))
			fwprintf_s(stderr, L"(NIMLOC Record)");
		else if (Type == htons(DNS_RECORD_SRV))
			fwprintf_s(stderr, L"(SRV Record)");
		else if (Type == htons(DNS_RECORD_ATMA))
			fwprintf_s(stderr, L"(ATMA Record)");
		else if (Type == htons(DNS_RECORD_NAPTR))
			fwprintf_s(stderr, L"(NAPTR Record)");
		else if (Type == htons(DNS_RECORD_KX))
			fwprintf_s(stderr, L"(KX Record)");
		else if (Type == htons(DNS_RECORD_CERT))
			fwprintf_s(stderr, L"(CERT Record)");
		else if (Type == htons(DNS_RECORD_DNAME))
			fwprintf_s(stderr, L"(DNAME Record)");
		else if (Type == htons(DNS_RECORD_SINK))
			fwprintf_s(stderr, L"(SINK Record)");
		else if (Type == htons(DNS_RECORD_OPT))
			fwprintf_s(stderr, L"(OPT/EDNS Record)");
		else if (Type == htons(DNS_RECORD_APL))
			fwprintf_s(stderr, L"(APL Record)");
		else if (Type == htons(DNS_RECORD_DS))
			fwprintf_s(stderr, L"(DS Record)");
		else if (Type == htons(DNS_RECORD_SSHFP))
			fwprintf_s(stderr, L"(SSHFP Record)");
		else if (Type == htons(DNS_RECORD_IPSECKEY))
			fwprintf_s(stderr, L"(IPSECKEY Record)");
		else if (Type == htons(DNS_RECORD_RRSIG))
			fwprintf_s(stderr, L"(RRSIG Record)");
		else if (Type == htons(DNS_RECORD_NSEC))
			fwprintf_s(stderr, L"(NSEC Record)");
		else if (Type == htons(DNS_RECORD_DNSKEY))
			fwprintf_s(stderr, L"(DNSKEY Record)");
		else if (Type == htons(DNS_RECORD_DHCID))
			fwprintf_s(stderr, L"(DHCID Record)");
		else if (Type == htons(DNS_RECORD_NSEC3))
			fwprintf_s(stderr, L"(NSEC3 Record)");
		else if (Type == htons(DNS_RECORD_NSEC3PARAM))
			fwprintf_s(stderr, L"(NSEC3PARAM Record)");
		else if (Type == htons(DNS_RECORD_TLSA))
			fwprintf_s(stderr, L"(TLSA Record)");
		else if (Type == htons(DNS_RECORD_HIP))
			fwprintf_s(stderr, L"(HIP Record)");
		else if (Type == htons(DNS_RECORD_NINFO))
			fwprintf_s(stderr, L"(NINFO Record)");
		else if (Type == htons(DNS_RECORD_RKEY))
			fwprintf_s(stderr, L"(RKEY Record)");
		else if (Type == htons(DNS_RECORD_TALINK))
			fwprintf_s(stderr, L"(TALINK Record)");
		else if (Type == htons(DNS_RECORD_CDS))
			fwprintf_s(stderr, L"(CDS Record)");
		else if (Type == htons(DNS_RECORD_CDNSKEY))
			fwprintf_s(stderr, L"(CDNSKEY Record)");
		else if (Type == htons(DNS_RECORD_OPENPGPKEY))
			fwprintf_s(stderr, L"(OPENPGPKEY Record)");
		else if (Type == htons(DNS_RECORD_SPF))
			fwprintf_s(stderr, L"(SPF Record)");
		else if (Type == htons(DNS_RECORD_UID))
			fwprintf_s(stderr, L"(UID Record)");
		else if (Type == htons(DNS_RECORD_GID))
			fwprintf_s(stderr, L"(GID Record)");
		else if (Type == htons(DNS_RECORD_UNSPEC))
			fwprintf_s(stderr, L"(UNSPEC Record)");
		else if (Type == htons(DNS_RECORD_NID))
			fwprintf_s(stderr, L"(NID Record)");
		else if (Type == htons(DNS_RECORD_L32))
			fwprintf_s(stderr, L"(L32 Record)");
		else if (Type == htons(DNS_RECORD_L64))
			fwprintf_s(stderr, L"(L64 Record)");
		else if (Type == htons(DNS_RECORD_LP))
			fwprintf_s(stderr, L"(LP Record)");
		else if (Type == htons(DNS_RECORD_EUI48))
			fwprintf_s(stderr, L"(EUI48 Record)");
		else if (Type == htons(DNS_RECORD_EUI64))
			fwprintf_s(stderr, L"(EUI64 Record)");
		else if (Type == htons(DNS_RECORD_TKEY))
			fwprintf_s(stderr, L"(TKEY Record)");
		else if (Type == htons(DNS_RECORD_TSIG))
			fwprintf_s(stderr, L"(TSIG Record)");
		else if (Type == htons(DNS_RECORD_IXFR))
			fwprintf_s(stderr, L"(IXFR Record)");
		else if (Type == htons(DNS_RECORD_AXFR))
			fwprintf_s(stderr, L"(AXFR Record)");
		else if (Type == htons(DNS_RECORD_MAILB))
			fwprintf_s(stderr, L"(MAILB Record)");
		else if (Type == htons(DNS_RECORD_MAILA))
			fwprintf_s(stderr, L"(MAILA Record)");
		else if (Type == htons(DNS_RECORD_ANY))
			fwprintf_s(stderr, L"(ANY Record)");
		else if (Type == htons(DNS_RECORD_URI))
			fwprintf_s(stderr, L"(URI Record)");
		else if (Type == htons(DNS_RECORD_CAA))
			fwprintf_s(stderr, L"(CAA Record)");
		else if (Type == htons(DNS_RECORD_TA))
			fwprintf_s(stderr, L"(TA Record)");
		else if (Type == htons(DNS_RECORD_DLV))
			fwprintf_s(stderr, L"(DLV Record)");
		else if (ntohs(Type) >= DNS_RECORD_PRIVATE_A && ntohs(Type) <= DNS_RECORD_PRIVATE_B)
			fwprintf_s(stderr, L"(Reserved Private use Record)");
		else if (Type == htons(DNS_RECORD_RESERVED))
			fwprintf_s(stderr, L"(Reserved Record)");
	}

	fwprintf_s(stderr, L"\n");
	return;
}

//Print Type and Classes name to file
void __fastcall PrintTypeClassesName(
	const uint16_t Type, 
	const uint16_t Classes, 
	FILE *OutputFile)
{
//Print Classes.
	if (Classes > 0)
	{
	//Cache flush check
		auto HighBitSet = false;
		auto ClassesTemp = ntohs(Classes);
		if (ClassesTemp >> HIGHEST_MOVE_BIT_U16 > 0)
		{
			HighBitSet = true;
			ClassesTemp = ntohs(Classes);
			ClassesTemp = ClassesTemp & HIGHEST_BIT_U16;
		}

	//Classes check
		if (ClassesTemp == DNS_CLASS_IN)
		{
			fwprintf_s(OutputFile, L"(Class IN");
			if (HighBitSet)
				fwprintf_s(OutputFile, L"/Unicast Queries or Cache Flush");
			fwprintf_s(OutputFile, L"))");
		}
		else if (ClassesTemp == DNS_CLASS_CSNET)
		{
			fwprintf_s(OutputFile, L"(Class CSNET");
			if (HighBitSet)
				fwprintf_s(OutputFile, L"/Unicast Queries or Cache Flush");
			fwprintf_s(OutputFile, L"))");
		}
		else if (ClassesTemp == DNS_CLASS_CHAOS)
		{
			fwprintf_s(OutputFile, L"(Class CHAOS");
			if (HighBitSet)
				fwprintf_s(OutputFile, L"/Unicast Queries or Cache Flush");
			fwprintf_s(OutputFile, L"))");
		}
		else if (ClassesTemp == DNS_CLASS_HESIOD)
		{
			fwprintf_s(OutputFile, L"(Class HESIOD");
			if (HighBitSet)
				fwprintf_s(OutputFile, L"/Unicast Queries or Cache Flush");
			fwprintf_s(OutputFile, L"))");
		}
		else if (ClassesTemp == DNS_CLASS_NONE)
		{
			fwprintf_s(OutputFile, L"(Class NONE");
			if (HighBitSet)
				fwprintf_s(OutputFile, L"/Unicast Queries or Cache Flush");
			fwprintf_s(OutputFile, L"))");
		}
		else if (ClassesTemp == DNS_CLASS_ALL)
		{
			fwprintf_s(OutputFile, L"(Class ALL");
			if (HighBitSet)
				fwprintf_s(OutputFile, L"/Unicast Queries or Cache Flush");
			fwprintf_s(OutputFile, L"))");
		}
		else if (ClassesTemp == DNS_CLASS_ANY)
		{
			fwprintf_s(OutputFile, L"(Class ANY");
			if (HighBitSet)
				fwprintf_s(OutputFile, L"/Unicast Queries or Cache Flush");
			fwprintf_s(OutputFile, L"))");
		}
	}
//Print Type.
	else {
		if (Type == htons(DNS_RECORD_A))
			fwprintf_s(OutputFile, L"(A Record)");
		else if (Type == htons(DNS_RECORD_NS))
			fwprintf_s(OutputFile, L"(NS Record)");
		else if (Type == htons(DNS_RECORD_MD))
			fwprintf_s(OutputFile, L"(MD Record)");
		else if (Type == htons(DNS_RECORD_MF))
			fwprintf_s(OutputFile, L"(MF Record)");
		else if (Type == htons(DNS_RECORD_CNAME))
			fwprintf_s(OutputFile, L"(CNAME Record)");
		else if (Type == htons(DNS_RECORD_SOA))
			fwprintf_s(OutputFile, L"(SOA Record)");
		else if (Type == htons(DNS_RECORD_MB))
			fwprintf_s(OutputFile, L"(MB Record)");
		else if (Type == htons(DNS_RECORD_MG))
			fwprintf_s(OutputFile, L"(MG Record)");
		else if (Type == htons(DNS_RECORD_MR))
			fwprintf_s(OutputFile, L"(MR Record)");
		else if (Type == htons(DNS_RECORD_NULL))
			fwprintf_s(OutputFile, L"(NULL Record)");
		else if (Type == htons(DNS_RECORD_WKS))
			fwprintf_s(OutputFile, L"(WKS Record)");
		else if (Type == htons(DNS_RECORD_PTR))
			fwprintf_s(OutputFile, L"(PTR Record)");
		else if (Type == htons(DNS_RECORD_HINFO))
			fwprintf_s(OutputFile, L"(HINFO Record)");
		else if (Type == htons(DNS_RECORD_MINFO))
			fwprintf_s(OutputFile, L"(MINFO Record)");
		else if (Type == htons(DNS_RECORD_MX))
			fwprintf_s(OutputFile, L"(MX Record)");
		else if (Type == htons(DNS_RECORD_TXT))
			fwprintf_s(OutputFile, L"(TXT Record)");
		else if (Type == htons(DNS_RECORD_RP))
			fwprintf_s(OutputFile, L"(RP Record)");
		else if (Type == htons(DNS_RECORD_RP))
			fwprintf_s(OutputFile, L"(RP Record)");
		else if (Type == htons(DNS_RECORD_AFSDB))
			fwprintf_s(OutputFile, L"(AFSDB Record)");
		else if (Type == htons(DNS_RECORD_X25))
			fwprintf_s(OutputFile, L"(X25 Record)");
		else if (Type == htons(DNS_RECORD_ISDN))
			fwprintf_s(OutputFile, L"(ISDN Record)");
		else if (Type == htons(DNS_RECORD_RT))
			fwprintf_s(OutputFile, L"(RT Record)");
		else if (Type == htons(DNS_RECORD_NSAP))
			fwprintf_s(OutputFile, L"(NSAP Record)");
		else if (Type == htons(DNS_RECORD_NSAP_PTR))
			fwprintf_s(OutputFile, L"(NSAP PTR Record)");
		else if (Type == htons(DNS_RECORD_SIG))
			fwprintf_s(OutputFile, L"(SIG Record)");
		else if (Type == htons(DNS_RECORD_KEY))
			fwprintf_s(OutputFile, L"(KEY Record)");
		else if (Type == htons(DNS_RECORD_PX))
			fwprintf_s(OutputFile, L"(PX Record)");
		else if (Type == htons(DNS_RECORD_GPOS))
			fwprintf_s(OutputFile, L"(GPOS Record)");
		else if (Type == htons(DNS_RECORD_AAAA))
			fwprintf_s(OutputFile, L"(AAAA Record)");
		else if (Type == htons(DNS_RECORD_LOC))
			fwprintf_s(OutputFile, L"(LOC Record)");
		else if (Type == htons(DNS_RECORD_NXT))
			fwprintf_s(OutputFile, L"(NXT Record)");
		else if (Type == htons(DNS_RECORD_EID))
			fwprintf_s(OutputFile, L"(EID Record)");
		else if (Type == htons(DNS_RECORD_NIMLOC))
			fwprintf_s(OutputFile, L"(NIMLOC Record)");
		else if (Type == htons(DNS_RECORD_SRV))
			fwprintf_s(OutputFile, L"(SRV Record)");
		else if (Type == htons(DNS_RECORD_ATMA))
			fwprintf_s(OutputFile, L"(ATMA Record)");
		else if (Type == htons(DNS_RECORD_NAPTR))
			fwprintf_s(OutputFile, L"(NAPTR Record)");
		else if (Type == htons(DNS_RECORD_KX))
			fwprintf_s(OutputFile, L"(KX Record)");
		else if (Type == htons(DNS_RECORD_CERT))
			fwprintf_s(OutputFile, L"(CERT Record)");
		else if (Type == htons(DNS_RECORD_DNAME))
			fwprintf_s(OutputFile, L"(DNAME Record)");
		else if (Type == htons(DNS_RECORD_SINK))
			fwprintf_s(OutputFile, L"(SINK Record)");
		else if (Type == htons(DNS_RECORD_OPT))
			fwprintf_s(OutputFile, L"(OPT/EDNS Record)");
		else if (Type == htons(DNS_RECORD_APL))
			fwprintf_s(OutputFile, L"(APL Record)");
		else if (Type == htons(DNS_RECORD_DS))
			fwprintf_s(OutputFile, L"(DS Record)");
		else if (Type == htons(DNS_RECORD_SSHFP))
			fwprintf_s(OutputFile, L"(SSHFP Record)");
		else if (Type == htons(DNS_RECORD_IPSECKEY))
			fwprintf_s(OutputFile, L"(IPSECKEY Record)");
		else if (Type == htons(DNS_RECORD_RRSIG))
			fwprintf_s(OutputFile, L"(RRSIG Record)");
		else if (Type == htons(DNS_RECORD_NSEC))
			fwprintf_s(OutputFile, L"(NSEC Record)");
		else if (Type == htons(DNS_RECORD_DNSKEY))
			fwprintf_s(OutputFile, L"(DNSKEY Record)");
		else if (Type == htons(DNS_RECORD_DHCID))
			fwprintf_s(OutputFile, L"(DHCID Record)");
		else if (Type == htons(DNS_RECORD_NSEC3))
			fwprintf_s(OutputFile, L"(NSEC3 Record)");
		else if (Type == htons(DNS_RECORD_NSEC3PARAM))
			fwprintf_s(OutputFile, L"(NSEC3PARAM Record)");
		else if (Type == htons(DNS_RECORD_TLSA))
			fwprintf_s(OutputFile, L"(TLSA Record)");
		else if (Type == htons(DNS_RECORD_HIP))
			fwprintf_s(OutputFile, L"(HIP Record)");
		else if (Type == htons(DNS_RECORD_NINFO))
			fwprintf_s(OutputFile, L"(NINFO Record)");
		else if (Type == htons(DNS_RECORD_RKEY))
			fwprintf_s(OutputFile, L"(RKEY Record)");
		else if (Type == htons(DNS_RECORD_TALINK))
			fwprintf_s(OutputFile, L"(TALINK Record)");
		else if (Type == htons(DNS_RECORD_CDS))
			fwprintf_s(OutputFile, L"(CDS Record)");
		else if (Type == htons(DNS_RECORD_CDNSKEY))
			fwprintf_s(OutputFile, L"(CDNSKEY Record)");
		else if (Type == htons(DNS_RECORD_OPENPGPKEY))
			fwprintf_s(OutputFile, L"(OPENPGPKEY Record)");
		else if (Type == htons(DNS_RECORD_SPF))
			fwprintf_s(OutputFile, L"(SPF Record)");
		else if (Type == htons(DNS_RECORD_UID))
			fwprintf_s(OutputFile, L"(UID Record)");
		else if (Type == htons(DNS_RECORD_GID))
			fwprintf_s(OutputFile, L"(GID Record)");
		else if (Type == htons(DNS_RECORD_UNSPEC))
			fwprintf_s(OutputFile, L"(UNSPEC Record)");
		else if (Type == htons(DNS_RECORD_NID))
			fwprintf_s(OutputFile, L"(NID Record)");
		else if (Type == htons(DNS_RECORD_L32))
			fwprintf_s(OutputFile, L"(L32 Record)");
		else if (Type == htons(DNS_RECORD_L64))
			fwprintf_s(OutputFile, L"(L64 Record)");
		else if (Type == htons(DNS_RECORD_LP))
			fwprintf_s(OutputFile, L"(LP Record)");
		else if (Type == htons(DNS_RECORD_EUI48))
			fwprintf_s(OutputFile, L"(EUI48 Record)");
		else if (Type == htons(DNS_RECORD_EUI64))
			fwprintf_s(OutputFile, L"(EUI64 Record)");
		else if (Type == htons(DNS_RECORD_TKEY))
			fwprintf_s(OutputFile, L"(TKEY Record)");
		else if (Type == htons(DNS_RECORD_TSIG))
			fwprintf_s(OutputFile, L"(TSIG Record)");
		else if (Type == htons(DNS_RECORD_IXFR))
			fwprintf_s(OutputFile, L"(IXFR Record)");
		else if (Type == htons(DNS_RECORD_AXFR))
			fwprintf_s(OutputFile, L"(AXFR Record)");
		else if (Type == htons(DNS_RECORD_MAILB))
			fwprintf_s(OutputFile, L"(MAILB Record)");
		else if (Type == htons(DNS_RECORD_MAILA))
			fwprintf_s(OutputFile, L"(MAILA Record)");
		else if (Type == htons(DNS_RECORD_ANY))
			fwprintf_s(OutputFile, L"(ANY Record)");
		else if (Type == htons(DNS_RECORD_URI))
			fwprintf_s(OutputFile, L"(URI Record)");
		else if (Type == htons(DNS_RECORD_CAA))
			fwprintf_s(OutputFile, L"(CAA Record)");
		else if (Type == htons(DNS_RECORD_TA))
			fwprintf_s(OutputFile, L"(TA Record)");
		else if (Type == htons(DNS_RECORD_DLV))
			fwprintf_s(OutputFile, L"(DLV Record)");
		else if (ntohs(Type) >= DNS_RECORD_PRIVATE_A && ntohs(Type) <= DNS_RECORD_PRIVATE_B)
			fwprintf_s(OutputFile, L"(Reserved Private use Record)");
		else if (Type == htons(DNS_RECORD_RESERVED))
			fwprintf_s(OutputFile, L"(Reserved Record)");
	}

	fwprintf_s(OutputFile, L"\n");
	return;
}

//Print Domain Name in response
size_t __fastcall PrintDomainName(
	const char *Buffer, 
	const size_t Location)
{
//Root check
	if (Buffer[Location] == 0)
	{
		fwprintf_s(stderr, L"<Root>");
		return sizeof(char);
	}

//Initialization
	std::shared_ptr<char> BufferTemp(new char[PACKET_MAXSIZE]());
	memset(BufferTemp.get(), 0, PACKET_MAXSIZE);
	size_t Index = 0, Result = 0;
	uint16_t Truncated = 0;
	auto MultiplePTR = false;

//Convert.
	Result = DNSQueryToChar(Buffer + Location, BufferTemp.get(), Truncated);
	if (Truncated > 0)
	{
	//Print once when pointer is not at first.
		if (Result > sizeof(uint16_t))
		{
			for (Index = 0;Index < strnlen_s(BufferTemp.get(), PACKET_MAXSIZE);++Index)
				fwprintf_s(stderr, L"%c", BufferTemp.get()[Index]);
			memset(BufferTemp.get(), 0, PACKET_MAXSIZE);
			fwprintf_s(stderr, L".");
		}

	//Get pointer.
		while (Truncated > 0)
		{
			if (MultiplePTR)
				fwprintf_s(stderr, L".");
			DNSQueryToChar(Buffer + Truncated, BufferTemp.get(), Truncated);
			for (Index = 0;Index < strnlen_s(BufferTemp.get(), PACKET_MAXSIZE);++Index)
				fwprintf_s(stderr, L"%c", BufferTemp.get()[Index]);
			memset(BufferTemp.get(), 0, PACKET_MAXSIZE);
			MultiplePTR = true;
		}
	}
	else {
		++Result;
	}

//Print last.
	for (Index = 0;Index < strnlen_s(BufferTemp.get(), PACKET_MAXSIZE);++Index)
		fwprintf_s(stderr, L"%c", BufferTemp.get()[Index]);
	return Result;
}

//Print Domain Name in response to file
size_t __fastcall PrintDomainName(
	const char *Buffer, 
	const size_t Location, 
	FILE *OutputFile)
{
//Root check
	if (Buffer[Location] == 0)
	{
		fwprintf_s(OutputFile, L"<Root>");
		return sizeof(char);
	}

//Initialization
	std::shared_ptr<char> BufferTemp(new char[PACKET_MAXSIZE]());
	memset(BufferTemp.get(), 0, PACKET_MAXSIZE);
	size_t Index = 0, Result = 0;
	uint16_t Truncated = 0;
	auto MultiplePTR = false;

//Convert.
	Result = DNSQueryToChar(Buffer + Location, BufferTemp.get(), Truncated);
	if (Truncated > 0)
	{
	//Print once when pointer is not at first.
		if (Result > sizeof(uint16_t))
		{
			for (Index = 0;Index < strnlen_s(BufferTemp.get(), PACKET_MAXSIZE);++Index)
				fwprintf_s(OutputFile, L"%c", BufferTemp.get()[Index]);
			memset(BufferTemp.get(), 0, PACKET_MAXSIZE);
			fwprintf_s(OutputFile, L".");
		}

	//Get pointer.
		while (Truncated > 0)
		{
			if (MultiplePTR)
				fwprintf_s(OutputFile, L".");
			DNSQueryToChar(Buffer + Truncated, BufferTemp.get(), Truncated);
			for (Index = 0;Index < strnlen_s(BufferTemp.get(), PACKET_MAXSIZE);++Index)
				fwprintf_s(OutputFile, L"%c", BufferTemp.get()[Index]);
			memset(BufferTemp.get(), 0, PACKET_MAXSIZE);
			MultiplePTR = true;
		}
	}
	else {
		++Result;
	}

//Print last.
	for (Index = 0;Index < strnlen_s(BufferTemp.get(), PACKET_MAXSIZE);++Index)
		fwprintf_s(OutputFile, L"%c", BufferTemp.get()[Index]);
	return Result;
}

//Print Resourse data
void __fastcall PrintResourseData(
	const char *Buffer, 
	const size_t Location, 
	const uint16_t Length, 
	const uint16_t Type, 
	const uint16_t Classes)
{
//Length and Type check
	if (Length == 0 && Type != htons(DNS_RECORD_OPT) || Classes == 0)
		return;

//Initialization
	size_t Index = 0, CurrentLength = 0;

//A Record(IPv4 address)
	if (Type == htons(DNS_RECORD_A) && Length == sizeof(in_addr))
	{
		fwprintf_s(stderr, L"\n   Data: ");

		auto Addr = (in_addr *)(Buffer + Location);
		fwprintf_s(stderr, L"%u.%u.%u.%u", Addr->s_net, Addr->s_host, Addr->s_lh, Addr->s_impno);
	}
//NS Record(Authoritative Name Server)
	else if (Type == htons(DNS_RECORD_NS))
	{
		fwprintf_s(stderr, L"\n   Data: ");

		PrintDomainName(Buffer, Location);
	}
//CNAME Record(Canonical Name)
	else if (Type == htons(DNS_RECORD_CNAME))
	{
		fwprintf_s(stderr, L"\n   Data: ");

		PrintDomainName(Buffer, Location);
	}
//SOA Record(Start Of a zone of Authority)
	else if (Type == htons(DNS_RECORD_SOA))
	{
		fwprintf_s(stderr, L"\n   Data: ");

		fwprintf_s(stderr, L"Primary Name Server: ");
		CurrentLength = PrintDomainName(Buffer, Location);
		fwprintf_s(stderr, L"\n         Responsible authority's mailbox: ");
		CurrentLength += PrintDomainName(Buffer, Location + CurrentLength);
		auto pdns_soa_record = (dns_soa_record *)(Buffer + Location + CurrentLength);
		fwprintf_s(stderr, L"\n         Serial Number: %u", ntohl(pdns_soa_record->Serial));
		fwprintf_s(stderr, L"\n         Refresh Interval: %u", ntohl(pdns_soa_record->RefreshInterval));
		PrintSecondsInDateTime(ntohl(pdns_soa_record->RefreshInterval));
		fwprintf_s(stderr, L"\n         Retry Interval: %u", ntohl(pdns_soa_record->RetryInterval));
		PrintSecondsInDateTime(ntohl(pdns_soa_record->RetryInterval));
		fwprintf_s(stderr, L"\n         Expire Limit: %u", ntohl(pdns_soa_record->ExpireLimit));
		PrintSecondsInDateTime(ntohl(pdns_soa_record->ExpireLimit));
		fwprintf_s(stderr, L"\n         Minimum TTL: %u", ntohl(pdns_soa_record->MinimumTTL));
		PrintSecondsInDateTime(ntohl(pdns_soa_record->MinimumTTL));
	}
//PTR Record(domain name PoinTeR)
	else if (Type == htons(DNS_RECORD_PTR))
	{
		fwprintf_s(stderr, L"\n   Data: ");

		PrintDomainName(Buffer, Location);
	}
//MX Record(Mail eXchange)
	else if (Type == htons(DNS_RECORD_MX))
	{
		fwprintf_s(stderr, L"\n   Data: ");

		auto pdns_mx_record = (dns_mx_record *)(Buffer + Location);
		fwprintf_s(stderr, L"Preference: %u", ntohs(pdns_mx_record->Preference));
		fwprintf_s(stderr, L"\n         Mail Exchange: ");
		PrintDomainName(Buffer, Location + sizeof(dns_mx_record));
	}
//TXT Record(Text strings)
	else if (Type == htons(DNS_RECORD_TXT))
	{
		fwprintf_s(stderr, L"\n   Data: ");

		auto pdns_txt_record = (dns_txt_record *)(Buffer + Location);
		fwprintf_s(stderr, L"Length: %u", pdns_txt_record->Length);
		fwprintf_s(stderr, L"\n         TXT: \"");
		for (Index = Location + sizeof(dns_txt_record);Index < Location + Length;++Index)
			fwprintf_s(stderr, L"%c", Buffer[Index]);
		fwprintf_s(stderr, L"\"");
	}
//AAAA Record(IPv6 address)
	else if (Type == htons(DNS_RECORD_AAAA) && Length == sizeof(in6_addr))
	{
		fwprintf_s(stderr, L"\n   Data: ");

		char BufferTemp[ADDR_STRING_MAXSIZE] = {0};
	#if (defined(PLATFORM_WIN32) && !defined(PLATFORM_WIN64))
		DWORD BufferLength = ADDR_STRING_MAXSIZE;
		sockaddr_storage SockAddr = {0};
		SockAddr.ss_family = AF_INET6;
		((PSOCKADDR_IN6)&SockAddr)->sin6_addr = *(in6_addr *)(Buffer + Location);
		WSAAddressToStringA((PSOCKADDR)&SockAddr, sizeof(sockaddr_in6), nullptr, BufferTemp, &BufferLength);
	#else
		inet_ntop(AF_INET6, (char *)(Buffer + Location), BufferTemp, ADDR_STRING_MAXSIZE);
	#endif
		CaseConvert(true, BufferTemp, strnlen_s(BufferTemp, PACKET_MAXSIZE));

		for (Index = 0;Index < strnlen_s(BufferTemp, PACKET_MAXSIZE);++Index)
			fwprintf_s(stderr, L"%c", BufferTemp[Index]);
	}
//SRV Record(Server Selection)
	else if (Type == htons(DNS_RECORD_SRV))
	{
		fwprintf_s(stderr, L"\n   Data: ");

		auto pdns_srv_record = (dns_srv_record *)(Buffer + Location);
		fwprintf_s(stderr, L"Priority: %x", ntohs(pdns_srv_record->Priority));
		fwprintf_s(stderr, L"\n         Weight: %u", ntohs(pdns_srv_record->Weight));
		fwprintf_s(stderr, L"\n         Port: %u", ntohs(pdns_srv_record->Port));
		fwprintf_s(stderr, L"\n         Target: ");
		PrintDomainName(Buffer, Location + sizeof(dns_srv_record));
	}
//OPT/EDNS Record(Extension Mechanisms for Domain Name System)
	else if (Type == htons(DNS_RECORD_OPT))
	{
		fwprintf_s(stderr, L"   Data: ");

		auto pdns_opt_record = (dns_opt_record *)(Buffer + Location);
		fwprintf_s(stderr, L"UDP Playload Size: %u", ntohs(pdns_opt_record->UDPPayloadSize));
		fwprintf_s(stderr, L"\n         Extended RCode: %x", pdns_opt_record->Extended_RCode);
		fwprintf_s(stderr, L"\n         EDNS Version: %u", pdns_opt_record->Version);
		if (ntohs(pdns_opt_record->Z_Field) >> HIGHEST_MOVE_BIT_U16 == 0)
			fwprintf_s(stderr, L"\n         Server cannot handle DNSSEC security RRs.");
		else 
			fwprintf_s(stderr, L"\n         Server can handle DNSSEC security RRs.");

	//EDNS Option
		if (Length >= sizeof(dns_edns0_option))
		{
			auto pdns_edns0_option = (dns_edns0_option *)(Buffer + Location + sizeof(dns_opt_record));
			fwprintf_s(stderr, L"\n         EDNS Option:\n                         Code: ");
			if (pdns_edns0_option->Code == htons(EDNS0_CODE_LLQ))
				fwprintf_s(stderr, L"LLQ");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_UL))
				fwprintf_s(stderr, L"LLQ");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_NSID))
				fwprintf_s(stderr, L"");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_OWNER))
				fwprintf_s(stderr, L"OWNER");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_DAU))
				fwprintf_s(stderr, L"DAU");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_DHU))
				fwprintf_s(stderr, L"DHU");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_N3U))
				fwprintf_s(stderr, L"N3U");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_CLIENT_SUBNET))
				fwprintf_s(stderr, L"CLIENT_SUBNET");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_EDNS_EXPIRE))
				fwprintf_s(stderr, L"EDNS_EXPIRE");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_CLIENT_SUBNET_EXP))
				fwprintf_s(stderr, L"CLIENT_SUBNET_EXP");
			else 
				fwprintf_s(stderr, L"%x", ntohs(pdns_edns0_option->Code));
			fwprintf_s(stderr, L"\n                         Length: %x", ntohs(pdns_edns0_option->Length));
		}
	}
//DS Record(Delegation Signer)
	else if (Type == htons(DNS_RECORD_DS))
	{
		fwprintf_s(stderr, L"\n   Data: ");


	}
//RRSIG Record(Resource Record digital SIGnature)
	else if (Type == htons(DNS_RECORD_RRSIG))
	{
		fwprintf_s(stderr, L"\n   Data: ");

		auto pdns_rrsig_record = (dns_rrsig_record *)(Buffer + Location);
		fwprintf_s(stderr, L"Type Covered: 0x%04x", ntohs(pdns_rrsig_record->TypeCovered));
		PrintTypeClassesName(pdns_rrsig_record->TypeCovered, 0);
		fwprintf_s(stderr, L"         Algorithm: ");
		if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_RSA_MD5)
			fwprintf_s(stderr, L"RSA/MD5");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_DH)
			fwprintf_s(stderr, L"DH");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_DSA)
			fwprintf_s(stderr, L"DSA");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_ECC)
			fwprintf_s(stderr, L"ECC");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_RSA_SHA1)
			fwprintf_s(stderr, L"RSA/SHA-1");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_DSA_NSEC3_SHA1)
			fwprintf_s(stderr, L"DSA/NSEC3/SHA-1");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_RSA_SHA1_NSEC3_SHA1)
			fwprintf_s(stderr, L"RSA/SHA-1/NSEC3/SHA-1");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_RSA_SHA256)
			fwprintf_s(stderr, L"RSA/SHA-256");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_RSA_SHA512)
			fwprintf_s(stderr, L"RSA/SHA-512");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_ECC_GOST)
			fwprintf_s(stderr, L"ECC/GOST");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_ECDSA_P256_SHA256)
			fwprintf_s(stderr, L"ECDSA P256/SHA-256");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_ECDSA_P386_SHA386)
			fwprintf_s(stderr, L"ECDSA P386/SHA-386");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_HMAC_MD5)
			fwprintf_s(stderr, L"HMAC/MD5");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_INDIRECT)
			fwprintf_s(stderr, L"Indirect");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_PRIVATE_DNS)
			fwprintf_s(stderr, L"Private DNS");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_PRIVATE_OID)
			fwprintf_s(stderr, L"Private OID");
		else 
			fwprintf_s(stderr, L"%u", pdns_rrsig_record->Algorithm);
		fwprintf_s(stderr, L"\n         Labels: %u", pdns_rrsig_record->Labels);
		fwprintf_s(stderr, L"\n         Original TTL: %u", ntohl(pdns_rrsig_record->TTL));
		PrintSecondsInDateTime(ntohl(pdns_rrsig_record->TTL));
		fwprintf_s(stderr, L"\n         Signature Expiration: ");
		PrintDateTime(ntohl(pdns_rrsig_record->Expiration));
		fwprintf_s(stderr, L"\n         Signature Inception: ");
		PrintDateTime(ntohl(pdns_rrsig_record->Inception));
		fwprintf_s(stderr, L"\n         Key Tag: %u", ntohs(pdns_rrsig_record->KeyTag));
		fwprintf_s(stderr, L"\n         Signer's name: ");
		CurrentLength = PrintDomainName(Buffer, Location + sizeof(dns_rrsig_record)) + 1U;
		CurrentLength += sizeof(dns_rrsig_record);
		fwprintf_s(stderr, L"\n         Signature: ");
		for (Index = Location + CurrentLength;Index < Location + Length;++Index)
			fwprintf_s(stderr, L"%02x", (UCHAR)Buffer[Index]);
	}
//NSEC Record(Next-SECure)
	else if (Type == htons(DNS_RECORD_NSEC))
	{
		fwprintf_s(stderr, L"\n   Data: ");

		fwprintf_s(stderr, L"Next Domain Name: ");
		CurrentLength = PrintDomainName(Buffer, Location);
		fwprintf_s(stderr, L"\n         List of Type Bit Map: ");
		for (Index = Location + CurrentLength;Index < Location + Length;++Index)
			fwprintf_s(stderr, L"%x", (UCHAR)Buffer[Index]);
	}
//DNSKEY Record(DNS public KEY)
	else if (Type == htons(DNS_RECORD_DNSKEY))
	{
		fwprintf_s(stderr, L"\n   Data: ");


	}
//CAA Record(Certification Authority Authorization)
	else if (Type == htons(DNS_RECORD_CAA))
	{
		fwprintf_s(stderr, L"   Data: ");

		auto pdns_caa_record = (dns_caa_record *)(Buffer + Location);
		fwprintf_s(stderr, L"Flags: %x", pdns_caa_record->Flags);
		fwprintf_s(stderr, L"\n         Length: %u", pdns_caa_record->Length);
		fwprintf_s(stderr, L"\n         Tag: \"");
		for (Index = Location + sizeof(dns_caa_record);Index < Location + sizeof(dns_caa_record) + pdns_caa_record->Length;++Index)
			fwprintf_s(stderr, L"%c", Buffer[Index]);
		fwprintf_s(stderr, L"\"");
		fwprintf_s(stderr, L"\n         Value: \"");
		for (Index = Location + sizeof(dns_caa_record) + pdns_caa_record->Length;Index < Location + Length;++Index)
			fwprintf_s(stderr, L"%c", Buffer[Index]);
		fwprintf_s(stderr, L"\"");
	}

	fwprintf_s(stderr, L"\n");
	return;
}

//Print Resourse data to file
void __fastcall PrintResourseData(
	const char *Buffer, 
	const size_t Location, 
	const uint16_t Length, 
	const uint16_t Type, 
	const uint16_t Classes, 
	FILE *OutputFile)
{
//Length and Type check
	if (Length == 0 && Type != htons(DNS_RECORD_OPT) || Classes == 0)
		return;
	size_t Index = 0, CurrentLength = 0;

//A Record(IPv4 address)
	if (Type == htons(DNS_RECORD_A) && Length == sizeof(in_addr))
	{
		fwprintf_s(OutputFile, L"\n   Data: ");
		auto Addr = (in_addr *)(Buffer + Location);
		fwprintf_s(OutputFile, L"%u.%u.%u.%u", Addr->s_net, Addr->s_host, Addr->s_lh, Addr->s_impno);
	}
//NS Record(Authoritative Name Server) and CNAME Record(Canonical Name)
	else if (Type == htons(DNS_RECORD_NS) || Type == htons(DNS_RECORD_CNAME))
	{
		fwprintf_s(OutputFile, L"\n   Data: ");
		PrintDomainName(Buffer, Location, OutputFile);
	}
//SOA Record(Start Of a zone of Authority)
	else if (Type == htons(DNS_RECORD_SOA))
	{
		fwprintf_s(OutputFile, L"\n   Data: ");

		fwprintf_s(OutputFile, L"Primary Name Server: ");
		CurrentLength = PrintDomainName(Buffer, Location, OutputFile);
		fwprintf_s(OutputFile, L"\n         Responsible authority's mailbox: ");
		CurrentLength += PrintDomainName(Buffer, Location + CurrentLength, OutputFile);
		auto pdns_soa_record = (dns_soa_record *)(Buffer + Location + CurrentLength);
		fwprintf_s(OutputFile, L"\n         Serial Number: %u", ntohl(pdns_soa_record->Serial));
		fwprintf_s(OutputFile, L"\n         Refresh Interval: %u", ntohl(pdns_soa_record->RefreshInterval));
		PrintSecondsInDateTime(ntohl(pdns_soa_record->RefreshInterval));
		fwprintf_s(OutputFile, L"\n         Retry Interval: %u", ntohl(pdns_soa_record->RetryInterval));
		PrintSecondsInDateTime(ntohl(pdns_soa_record->RetryInterval));
		fwprintf_s(OutputFile, L"\n         Expire Limit: %u", ntohl(pdns_soa_record->ExpireLimit));
		PrintSecondsInDateTime(ntohl(pdns_soa_record->ExpireLimit));
		fwprintf_s(OutputFile, L"\n         Minimum TTL: %u", ntohl(pdns_soa_record->MinimumTTL));
		PrintSecondsInDateTime(ntohl(pdns_soa_record->MinimumTTL));
	}
//PTR Record(domain name PoinTeR)
	else if (Type == htons(DNS_RECORD_PTR))
	{
		fwprintf_s(OutputFile, L"\n   Data: ");
		PrintDomainName(Buffer, Location, OutputFile);
	}
//MX Record(Mail eXchange)
	else if (Type == htons(DNS_RECORD_MX))
	{
		fwprintf_s(OutputFile, L"\n   Data: ");

		auto pdns_mx_record = (dns_mx_record *)(Buffer + Location);
		fwprintf_s(OutputFile, L"Preference: %u", ntohs(pdns_mx_record->Preference));
		fwprintf_s(OutputFile, L"\n         Mail Exchange: ");
		PrintDomainName(Buffer, Location + sizeof(dns_mx_record), OutputFile);
	}
//TXT Record(Text strings)
	else if (Type == htons(DNS_RECORD_TXT))
	{
		fwprintf_s(OutputFile, L"\n   Data: ");

		auto pdns_txt_record = (dns_txt_record *)(Buffer + Location);
		fwprintf_s(OutputFile, L"Length: %u", pdns_txt_record->Length);
		fwprintf_s(OutputFile, L"\n         TXT: \"");
		for (Index = Location + sizeof(dns_txt_record);Index < Location + Length;++Index)
			fwprintf_s(OutputFile, L"%c", Buffer[Index]);
		fwprintf_s(OutputFile, L"\"");
	}
//AAAA Record(IPv6 address)
	else if (Type == htons(DNS_RECORD_AAAA) && Length == sizeof(in6_addr))
	{
		fwprintf_s(OutputFile, L"\n   Data: ");

		char BufferTemp[ADDR_STRING_MAXSIZE] = {0};
	#if (defined(PLATFORM_WIN32) && !defined(PLATFORM_WIN64))
		DWORD BufferLength = ADDR_STRING_MAXSIZE;
		sockaddr_storage SockAddr = {0};
		SockAddr.ss_family = AF_INET6;
		((PSOCKADDR_IN6)&SockAddr)->sin6_addr = *(in6_addr *)(Buffer + Location);
		WSAAddressToStringA((PSOCKADDR)&SockAddr, sizeof(sockaddr_in6), nullptr, BufferTemp, &BufferLength);
	#else
		inet_ntop(AF_INET6, (char *)(Buffer + Location), BufferTemp, ADDR_STRING_MAXSIZE);
	#endif
		CaseConvert(true, BufferTemp, strnlen_s(BufferTemp, ADDR_STRING_MAXSIZE));

		for (Index = 0;Index < strnlen_s(BufferTemp, ADDR_STRING_MAXSIZE);++Index)
			fwprintf_s(OutputFile, L"%c", BufferTemp[Index]);
	}
//SRV Record(Server Selection)
	else if (Type == htons(DNS_RECORD_SRV))
	{
		fwprintf_s(OutputFile, L"\n   Data: ");

		auto pdns_srv_record = (dns_srv_record *)(Buffer + Location);
		fwprintf_s(OutputFile, L"Priority: %x", ntohs(pdns_srv_record->Priority));
		fwprintf_s(OutputFile, L"\n         Weight: %u", ntohs(pdns_srv_record->Weight));
		fwprintf_s(OutputFile, L"\n         Port: %u", ntohs(pdns_srv_record->Port));
		fwprintf_s(OutputFile, L"\n         Target: ");
		PrintDomainName(Buffer, Location + sizeof(dns_srv_record), OutputFile);
	}
//OPT/EDNS Record(Extension Mechanisms for Domain Name System)
	else if (Type == htons(DNS_RECORD_OPT))
	{
		fwprintf_s(OutputFile, L"   Data: ");

		auto pdns_opt_record = (dns_opt_record *)(Buffer + Location);
		fwprintf_s(OutputFile, L"UDP Playload Size: %u", ntohs(pdns_opt_record->UDPPayloadSize));
		fwprintf_s(OutputFile, L"\n         Extended RCode: %x", pdns_opt_record->Extended_RCode);
		fwprintf_s(OutputFile, L"\n         EDNS Version: %u", pdns_opt_record->Version);
		if (ntohs(pdns_opt_record->Z_Field) >> HIGHEST_MOVE_BIT_U16 == 0)
			fwprintf_s(OutputFile, L"\n         Server cannot handle DNSSEC security RRs.");
		else
			fwprintf_s(OutputFile, L"\n         Server can handle DNSSEC security RRs.");

	//EDNS Option
		if (Length >= sizeof(dns_edns0_option))
		{
			auto pdns_edns0_option = (dns_edns0_option *)(Buffer + Location + sizeof(dns_opt_record));
			fwprintf_s(OutputFile, L"\n         EDNS Option:\n                         Code: ");
			if (pdns_edns0_option->Code == htons(EDNS0_CODE_LLQ))
				fwprintf_s(OutputFile, L"LLQ");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_UL))
				fwprintf_s(OutputFile, L"LLQ");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_NSID))
				fwprintf_s(OutputFile, L"");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_OWNER))
				fwprintf_s(OutputFile, L"OWNER");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_DAU))
				fwprintf_s(OutputFile, L"DAU");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_DHU))
				fwprintf_s(OutputFile, L"DHU");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_N3U))
				fwprintf_s(OutputFile, L"N3U");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_CLIENT_SUBNET))
				fwprintf_s(OutputFile, L"CLIENT_SUBNET");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_EDNS_EXPIRE))
				fwprintf_s(OutputFile, L"EDNS_EXPIRE");
			else if (pdns_edns0_option->Code == htons(EDNS0_CODE_CLIENT_SUBNET_EXP))
				fwprintf_s(OutputFile, L"CLIENT_SUBNET_EXP");
			else
				fwprintf_s(OutputFile, L"%x", ntohs(pdns_edns0_option->Code));
			fwprintf_s(OutputFile, L"\n                         Length: %x", ntohs(pdns_edns0_option->Length));
		}
	}
//RRSIG Record(Resource Record digital SIGnature)
	else if (Type == htons(DNS_RECORD_RRSIG))
	{
		fwprintf_s(OutputFile, L"\n   Data: ");

		auto pdns_rrsig_record = (dns_rrsig_record *)(Buffer + Location);
		fwprintf_s(OutputFile, L"Type Covered: 0x%04x", ntohs(pdns_rrsig_record->TypeCovered));
		PrintTypeClassesName(pdns_rrsig_record->TypeCovered, 0);
		fwprintf_s(OutputFile, L"         Algorithm: ");
		if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_RSA_MD5)
			fwprintf_s(OutputFile, L"RSA/MD5");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_DH)
			fwprintf_s(OutputFile, L"DH");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_DSA)
			fwprintf_s(OutputFile, L"DSA");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_ECC)
			fwprintf_s(OutputFile, L"ECC");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_RSA_SHA1)
			fwprintf_s(OutputFile, L"RSA/SHA-1");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_DSA_NSEC3_SHA1)
			fwprintf_s(OutputFile, L"DSA/NSEC3/SHA-1");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_RSA_SHA1_NSEC3_SHA1)
			fwprintf_s(OutputFile, L"RSA/SHA-1/NSEC3/SHA-1");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_RSA_SHA256)
			fwprintf_s(OutputFile, L"RSA/SHA-256");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_RSA_SHA512)
			fwprintf_s(OutputFile, L"RSA/SHA-512");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_ECC_GOST)
			fwprintf_s(OutputFile, L"ECC/GOST");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_ECDSA_P256_SHA256)
			fwprintf_s(OutputFile, L"ECDSA P256/SHA-256");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_ECDSA_P386_SHA386)
			fwprintf_s(OutputFile, L"ECDSA P386/SHA-386");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_HMAC_MD5)
			fwprintf_s(OutputFile, L"HMAC/MD5");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_INDIRECT)
			fwprintf_s(OutputFile, L"Indirect");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_PRIVATE_DNS)
			fwprintf_s(OutputFile, L"Private DNS");
		else if (pdns_rrsig_record->Algorithm == DNSSEC_AlGORITHM_PRIVATE_OID)
			fwprintf_s(OutputFile, L"Private OID");
		else 
			fwprintf_s(OutputFile, L"%u", pdns_rrsig_record->Algorithm);
		fwprintf_s(OutputFile, L"\n         Labels: %u", pdns_rrsig_record->Labels);
		fwprintf_s(OutputFile, L"\n         Original TTL: %u", ntohl(pdns_rrsig_record->TTL));
		PrintSecondsInDateTime(ntohl(pdns_rrsig_record->TTL));
		fwprintf_s(OutputFile, L"\n         Signature Expiration: ");
		PrintDateTime(ntohl(pdns_rrsig_record->Expiration));
		fwprintf_s(OutputFile, L"\n         Signature Inception: ");
		PrintDateTime(ntohl(pdns_rrsig_record->Inception));
		fwprintf_s(OutputFile, L"\n         Key Tag: %u", ntohs(pdns_rrsig_record->KeyTag));
		fwprintf_s(OutputFile, L"\n         Signer's name: ");
		CurrentLength = PrintDomainName(Buffer, Location + sizeof(dns_rrsig_record), OutputFile) + 1U;
		CurrentLength += sizeof(dns_rrsig_record);
		fwprintf_s(OutputFile, L"\n         Signature: ");
		for (Index = Location + CurrentLength;Index < Location + Length;++Index)
			fwprintf_s(OutputFile, L"%02x", (UCHAR)Buffer[Index]);
	}
//NSEC Record(Next-SECure)
	else if (Type == htons(DNS_RECORD_NSEC))
	{
		fwprintf_s(OutputFile, L"\n   Data: ");

		fwprintf_s(OutputFile, L"Next Domain Name: ");
		CurrentLength = PrintDomainName(Buffer, Location, OutputFile);
		fwprintf_s(OutputFile, L"\n         List of Type Bit Map: ");
		for (Index = Location + CurrentLength;Index < Location + Length;++Index)
			fwprintf_s(OutputFile, L"%x", (UCHAR)Buffer[Index]);
	}
//CAA Record(Certification Authority Authorization)
	else if (Type == htons(DNS_RECORD_CAA))
	{
		fwprintf_s(OutputFile, L"   Data: ");

		auto pdns_caa_record = (dns_caa_record *)(Buffer + Location);
		fwprintf_s(OutputFile, L"Flags: %x", pdns_caa_record->Flags);
		fwprintf_s(OutputFile, L"\n         Length: %u", pdns_caa_record->Length);
		fwprintf_s(OutputFile, L"\n         Tag: \"");
		for (Index = Location + sizeof(dns_caa_record);Index < Location + sizeof(dns_caa_record) + pdns_caa_record->Length;++Index)
			fwprintf_s(OutputFile, L"%c", Buffer[Index]);
		fwprintf_s(OutputFile, L"\"");
		fwprintf_s(OutputFile, L"\n         Value: \"");
		for (Index = Location + sizeof(dns_caa_record) + pdns_caa_record->Length;Index < Location + Length;++Index)
			fwprintf_s(OutputFile, L"%c", Buffer[Index]);
		fwprintf_s(OutputFile, L"\"");
	}

	fwprintf_s(OutputFile, L"\n");
	return;
}
