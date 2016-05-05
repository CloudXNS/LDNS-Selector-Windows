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

//Minimum supported system of Windows Version Helpers is Windows Vista.
#if (defined(PLATFORM_WIN32) && !defined(PLATFORM_WIN64))
//Check operation system which higher than Windows 7.
bool __fastcall IsLowerThanWin8(
	void)
{
	OSVERSIONINFOEX OSVI = {0};
	OSVI.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	BOOL bOsVersionInfoEx = GetVersionExW((OSVERSIONINFO *)&OSVI);

	if (bOsVersionInfoEx && OSVI.dwPlatformId == VER_PLATFORM_WIN32_NT && 
		(OSVI.dwMajorVersion < 6U || OSVI.dwMajorVersion == 6U && OSVI.dwMinorVersion < 2U))
			return true;

	return false;
}
#endif

//Check empty buffer
bool __fastcall CheckEmptyBuffer(
	const void *Buffer, 
	const size_t Length)
{
	if (Buffer == nullptr)
		return true;

	for (size_t Index = 0;Index < Length;++Index)
	{
		if (((uint8_t *)Buffer)[Index] != 0)
			return false;
	}

	return true;
}

#if (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
//Convert multiple bytes to wide char string
void MBSToWCSString(
	std::wstring &Target, 
	const char *Buffer)
{
	std::shared_ptr<wchar_t> TargetPTR(new wchar_t[strnlen(Buffer, LARGE_PACKET_MAXSIZE) + 1U]());
	wmemset(TargetPTR.get(), 0, strnlen(Buffer, LARGE_PACKET_MAXSIZE) + 1U);
	mbstowcs(TargetPTR.get(), Buffer, strnlen(Buffer, LARGE_PACKET_MAXSIZE));
	Target = TargetPTR.get();

	return;
}
#endif

//Convert lowercase/uppercase word(s) to uppercase/lowercase word(s).
size_t __fastcall CaseConvert(
	const bool IsLowerUpper, 
	char *Buffer, 
	const size_t Length)
{
	for (size_t Index = 0;Index < Length;++Index)
	{
		if (IsLowerUpper) //Lowercase to uppercase
		{
			if (Buffer[Index] > ASCII_ACCENT && Buffer[Index] < ASCII_BRACES_LEAD)
				Buffer[Index] -= ASCII_LOWER_TO_UPPER;
		}
		else { //Uppercase to lowercase
			if (Buffer[Index] > ASCII_AT && Buffer[Index] < ASCII_BRACKETS_LEFT)
				Buffer[Index] += ASCII_UPPER_TO_LOWER;
		}
	}

	return EXIT_SUCCESS;
}

//Convert address strings to binary.
size_t __fastcall AddressStringToBinary(
	const char *AddrString, 
	void *pAddr, 
	const uint16_t Protocol, 
	SSIZE_T &ErrCode)
{
	SSIZE_T Result = 0;

//inet_ntop() and inet_pton() was only support in Windows Vista and newer system. [Roy Tam]
#if (defined(PLATFORM_WIN32) && !defined(PLATFORM_WIN64))
	sockaddr_storage SockAddr = {0};
	int SockLength = 0;
#endif

//IPv6
	if (Protocol == AF_INET6)
	{
	//Check IPv6 addresses
		for (Result = 0;Result < (SSIZE_T)strlen(AddrString);++Result)
		{
			if (AddrString[Result] < ASCII_ZERO || AddrString[Result] > ASCII_COLON && AddrString[Result] < ASCII_UPPERCASE_A || AddrString[Result] > ASCII_UPPERCASE_F && AddrString[Result] < ASCII_LOWERCASE_A || AddrString[Result] > ASCII_LOWERCASE_F)
				break;
		}

		std::string sAddrString(AddrString);
	//Check abbreviation format.
		if (sAddrString.find(ASCII_COLON) == std::string::npos)
		{
			sAddrString.clear();
			sAddrString.append("::");
			sAddrString.append(AddrString);
		}
		else if (sAddrString.find(ASCII_COLON) == sAddrString.rfind(ASCII_COLON))
		{
			sAddrString.replace(sAddrString.find(ASCII_COLON), 1U, ("::"));
		}

	//Convert to binary.
	#if (defined(PLATFORM_WIN32) && !defined(PLATFORM_WIN64))
		SockLength = sizeof(sockaddr_in6);
		if (WSAStringToAddressA((char *)sAddrString.c_str(), AF_INET6, nullptr, (PSOCKADDR)&SockAddr, &SockLength) == SOCKET_ERROR)
	#else 
		Result = inet_pton(AF_INET6, sAddrString.c_str(), pAddr);
		if (Result == SOCKET_ERROR || Result == FALSE)
	#endif
		{
			ErrCode = WSAGetLastError();
			return EXIT_FAILURE;
		}
	#if (defined(PLATFORM_WIN32) && !defined(PLATFORM_WIN64))
		memcpy_s(pAddr, sizeof(in6_addr), &((PSOCKADDR_IN6)&SockAddr)->sin6_addr, sizeof(in6_addr));
	#endif
	}
//IPv4
	else {
		size_t CommaNum = 0;
		for (Result = 0;Result < (SSIZE_T)strlen(AddrString);++Result)
		{
			if (AddrString[Result] != ASCII_PERIOD && AddrString[Result] < ASCII_ZERO || AddrString[Result] > ASCII_NINE)
				return EXIT_FAILURE;
			else if (AddrString[Result] == ASCII_PERIOD)
				++CommaNum;
		}

		std::string sAddrString(AddrString);
	//Delete zero(s) before whole data.
		while (sAddrString.length() > 1U && sAddrString[0] == ASCII_ZERO && sAddrString[1U] != ASCII_PERIOD)
			sAddrString.erase(0, 1U);

	//Check abbreviation format.
		if (CommaNum == 0)
		{
			sAddrString.clear();
			sAddrString.append("0.0.0.");
			sAddrString.append(AddrString);
		}
		else if (CommaNum == 1U)
		{
			sAddrString.replace(sAddrString.find(ASCII_PERIOD), 1U, (".0.0."));
		}
		else if (CommaNum == 2U)
		{
			sAddrString.replace(sAddrString.find(ASCII_PERIOD), 1U, (".0."));
		}

	//Delete zero(s) before data.
		while (sAddrString.find(".00") != std::string::npos)
			sAddrString.replace(sAddrString.find(".00"), 3U, ("."));
		while (sAddrString.find(".0") != std::string::npos)
			sAddrString.replace(sAddrString.find(".0"), 2U, ("."));
		while (sAddrString.find("..") != std::string::npos)
			sAddrString.replace(sAddrString.find(".."), 2U, (".0."));
		if (sAddrString[sAddrString.length() - 1U] == ASCII_PERIOD)
			sAddrString.append("0");

	//Convert to binary.
	#if (defined(PLATFORM_WIN32) && !defined(PLATFORM_WIN64))
		SockLength = sizeof(sockaddr_in);
		if (WSAStringToAddressA((char *)sAddrString.c_str(), AF_INET, nullptr, (PSOCKADDR)&SockAddr, &SockLength) == SOCKET_ERROR)
	#else 
		Result = inet_pton(AF_INET, sAddrString.c_str(), pAddr);
		if (Result == SOCKET_ERROR || Result == FALSE)
	#endif
		{
			ErrCode = WSAGetLastError();
			return EXIT_FAILURE;
		}
	#if (defined(PLATFORM_WIN32) && !defined(PLATFORM_WIN64))
		memcpy_s(pAddr, sizeof(in_addr), &((PSOCKADDR_IN)&SockAddr)->sin_addr, sizeof(in_addr));
	#endif
	}

	return EXIT_SUCCESS;
}

//Convert protocol name to hex
uint16_t __fastcall InternetProtocolNameToPort(
	const std::wstring &Buffer)
{
//Internet Protocol Number(Part 1)
	if (Buffer == L"HOPOPTS" || Buffer == L"hopopts")
		return IPPROTO_HOPOPTS;
	else if (Buffer == L"ICMP" || Buffer == L"icmp")
		return IPPROTO_ICMP;
	else if (Buffer == L"IGMP" || Buffer == L"igmp")
		return IPPROTO_IGMP;
	else if (Buffer == L"GGP" || Buffer == L"ggp")
		return IPPROTO_GGP;
	else if (Buffer == L"IPV4" || Buffer == L"ipv4")
		return IPPROTO_IPV4;
	else if (Buffer == L"ST" || Buffer == L"st")
		return IPPROTO_ST;
	else if (Buffer == L"TCP" || Buffer == L"tcp")
		return IPPROTO_TCP;
	else if (Buffer == L"CBT" || Buffer == L"cbt")
		return IPPROTO_CBT;
	else if (Buffer == L"EGP" || Buffer == L"egp")
		return IPPROTO_EGP;
	else if (Buffer == L"IGP" || Buffer == L"igp")
		return IPPROTO_IGP;
	else if (Buffer == L"BBNRCCMON" || Buffer == L"bbnrccmon")
		return IPPROTO_BBN_RCC_MON;
	else if (Buffer == L"NVPII" || Buffer == L"nvpii")
		return IPPROTO_NVP_II;
	else if (Buffer == L"PUP" || Buffer == L"pup")
		return IPPROTO_PUP;
	else if (Buffer == L"ARGUS" || Buffer == L"argus")
		return IPPROTO_ARGUS;
	else if (Buffer == L"EMCON" || Buffer == L"emcon")
		return IPPROTO_EMCON;
	else if (Buffer == L"XNET" || Buffer == L"xnet")
		return IPPROTO_XNET;
	else if (Buffer == L"CHAOS" || Buffer == L"chaos")
		return IPPROTO_CHAOS;
	else if (Buffer == L"UDP" || Buffer == L"udp")
		return IPPROTO_UDP;
	else if (Buffer == L"MUX" || Buffer == L"mux")
		return IPPROTO_MUX;
	else if (Buffer == L"DCN" || Buffer == L"dcn")
		return IPPROTO_DCN;
	else if (Buffer == L"HMP" || Buffer == L"hmp")
		return IPPROTO_HMP;
	else if (Buffer == L"PRM" || Buffer == L"prm")
		return IPPROTO_PRM;
	else if (Buffer == L"IDP" || Buffer == L"idp")
		return IPPROTO_IDP;
	else if (Buffer == L"TRUNK-1" || Buffer == L"trunk-1")
		return IPPROTO_TRUNK_1;
	else if (Buffer == L"TRUNK-2" || Buffer == L"trunk-2")
		return IPPROTO_TRUNK_2;
	else if (Buffer == L"LEAF-1" || Buffer == L"leaf-1")
		return IPPROTO_LEAF_1;
	else if (Buffer == L"LEAF" || Buffer == L"leaf-2")
		return IPPROTO_LEAF_2;
	else if (Buffer == L"RDP" || Buffer == L"rdp")
		return IPPROTO_RDP;
	else if (Buffer == L"IRTP" || Buffer == L"irtp")
		return IPPROTO_IRTP;
	else if (Buffer == L"ISOTP4" || Buffer == L"isotp4")
		return IPPROTO_ISO_TP4;
	else if (Buffer == L"NETBLT" || Buffer == L"netblt")
		return IPPROTO_NETBLT;
	else if (Buffer == L"MFE" || Buffer == L"mfe")
		return IPPROTO_MFE;
	else if (Buffer == L"MERIT" || Buffer == L"merit")
		return IPPROTO_MERIT;
	else if (Buffer == L"DCCP" || Buffer == L"dccp")
		return IPPROTO_DCCP;
	else if (Buffer == L"3PC" || Buffer == L"3pc")
		return IPPROTO_3PC;
	else if (Buffer == L"IDPR" || Buffer == L"idpr")
		return IPPROTO_IDPR;
	else if (Buffer == L"XTP" || Buffer == L"xtp")
		return IPPROTO_XTP;
	else if (Buffer == L"DDP" || Buffer == L"ddp")
		return IPPROTO_DDP;
	else if (Buffer == L"IDPRCMTP" || Buffer == L"idrpcmtp")
		return IPPROTO_IDPR_CMTP;
	else if (Buffer == L"TP++" || Buffer == L"tp++")
		return IPPROTO_TPPLUS;
	else if (Buffer == L"IL" || Buffer == L"il")
		return IPPROTO_IL;
	else if (Buffer == L"IPV6" || Buffer == L"ipv6")
		return IPPROTO_IPV6;
	else if (Buffer == L"SDRP" || Buffer == L"sdrp")
		return IPPROTO_SDRP;
	else if (Buffer == L"ROUTING" || Buffer == L"routing")
		return IPPROTO_ROUTING;
	else if (Buffer == L"FRAGMENT" || Buffer == L"fragment")
		return IPPROTO_FRAGMENT;
	else if (Buffer == L"IDRP" || Buffer == L"idrp")
		return IPPROTO_IDRP;
	else if (Buffer == L"RSVP" || Buffer == L"rsvp")
		return IPPROTO_RSVP;
	else if (Buffer == L"GRE" || Buffer == L"gre")
		return IPPROTO_GRE;
	else if (Buffer == L"DSR" || Buffer == L"dsr")
		return IPPROTO_DSR;
	else if (Buffer == L"BNA" || Buffer == L"bna")
		return IPPROTO_BNA;
	else if (Buffer == L"ESP" || Buffer == L"esp")
		return IPPROTO_ESP;
	else if (Buffer == L"AH" || Buffer == L"ah")
		return IPPROTO_AH;
	else if (Buffer == L"NLSP" || Buffer == L"nlsp")
		return IPPROTO_NLSP;
	else if (Buffer == L"SWIPE" || Buffer == L"swipe")
		return IPPROTO_SWIPE;
	else if (Buffer == L"NARP" || Buffer == L"narp")
		return IPPROTO_NARP;
	else if (Buffer == L"MOBILE" || Buffer == L"mobile")
		return IPPROTO_MOBILE;
	else if (Buffer == L"TLSP" || Buffer == L"tlsp")
		return IPPROTO_TLSP;
	else if (Buffer == L"SKIP" || Buffer == L"skip")
		return IPPROTO_SKIP;
	else if (Buffer == L"ICMPV6" || Buffer == L"icmpv6")
		return IPPROTO_ICMPV6;
	else if (Buffer == L"NONE" || Buffer == L"none")
		return IPPROTO_NONE;
	else if (Buffer == L"DSTOPTS" || Buffer == L"dstopts")
		return IPPROTO_DSTOPTS;
	else if (Buffer == L"AHI" || Buffer == L"ahi")
		return IPPROTO_AHI;
	else if (Buffer == L"CFTP" || Buffer == L"cftp")
		return IPPROTO_CFTP;
	else if (Buffer == L"ALN" || Buffer == L"aln")
		return IPPROTO_ALN;
	else if (Buffer == L"SAT" || Buffer == L"sat")
		return IPPROTO_SAT;
	else if (Buffer == L"KRYPTOLAN" || Buffer == L"kryptolan")
		return IPPROTO_KRYPTOLAN;
	else if (Buffer == L"RVD" || Buffer == L"rvd")
		return IPPROTO_RVD;
	else if (Buffer == L"IPPC" || Buffer == L"ippc")
		return IPPROTO_IPPC;
	else if (Buffer == L"ADF" || Buffer == L"adf")
		return IPPROTO_ADF;
	else if (Buffer == L"SATMON" || Buffer == L"satmon")
		return IPPROTO_SAT_MON;
	else if (Buffer == L"VISA" || Buffer == L"visa")
		return IPPROTO_VISA;
	else if (Buffer == L"IPCV" || Buffer == L"ipcv")
		return IPPROTO_IPCV;
	else if (Buffer == L"CPNX" || Buffer == L"cpnx")
		return IPPROTO_CPNX;
	else if (Buffer == L"CPHB" || Buffer == L"cphb")
		return IPPROTO_CPHB;
	else if (Buffer == L"WSN" || Buffer == L"wsn")
		return IPPROTO_WSN;
	else if (Buffer == L"PVP" || Buffer == L"pvp")
		return IPPROTO_PVP;
	else if (Buffer == L"BR" || Buffer == L"br")
		return IPPROTO_BR;
	else if (Buffer == L"ND" || Buffer == L"nd")
		return IPPROTO_ND;
	else if (Buffer == L"ICLFXBM" || Buffer == L"iclfxbm")
		return IPPROTO_ICLFXBM;
	else if (Buffer == L"WBEXPAK" || Buffer == L"wbexpak")
		return IPPROTO_WBEXPAK;
	else if (Buffer == L"ISO" || Buffer == L"iso")
		return IPPROTO_ISO;
	else if (Buffer == L"VMTP" || Buffer == L"vmtp")
		return IPPROTO_VMTP;
	else if (Buffer == L"SVMTP" || Buffer == L"svmtp")
		return IPPROTO_SVMTP;
	else if (Buffer == L"VINES" || Buffer == L"vines")
		return IPPROTO_VINES;
	else if (Buffer == L"TTP" || Buffer == L"ttp")
		return IPPROTO_TTP;
	else if (Buffer == L"IPTM" || Buffer == L"iptm")
		return IPPROTO_IPTM;
	else if (Buffer == L"NSFNET" || Buffer == L"nsfnet")
		return IPPROTO_NSFNET;
	else if (Buffer == L"DGP" || Buffer == L"dgp")
		return IPPROTO_DGP;
	else if (Buffer == L"TCF" || Buffer == L"tcf")
		return IPPROTO_TCF;
	else if (Buffer == L"EIGRP" || Buffer == L"eigrp")
		return IPPROTO_EIGRP;
	else if (Buffer == L"SPRITE" || Buffer == L"sprite")
		return IPPROTO_SPRITE;
	else if (Buffer == L"LARP" || Buffer == L"larp")
		return IPPROTO_LARP;
	else if (Buffer == L"MTP" || Buffer == L"mtp")
		return IPPROTO_MTP;
	else if (Buffer == L"AX25" || Buffer == L"ax25")
		return IPPROTO_AX25;
	else if (Buffer == L"IPIP" || Buffer == L"ipip")
		return IPPROTO_IPIP;
	else if (Buffer == L"MICP" || Buffer == L"micp")
		return IPPROTO_MICP;
	else if (Buffer == L"SCC" || Buffer == L"scc")
		return IPPROTO_SCC;
	else if (Buffer == L"ETHERIP" || Buffer == L"etherip")
		return IPPROTO_ETHERIP;
	else if (Buffer == L"ENCAP" || Buffer == L"encap")
		return IPPROTO_ENCAP;
	else if (Buffer == L"APES" || Buffer == L"apes")
		return IPPROTO_APES;
	else if (Buffer == L"GMTP" || Buffer == L"gmtp")
		return IPPROTO_GMTP;
	else if (Buffer == L"IFMP" || Buffer == L"ifmp")
		return IPPROTO_IFMP;
	else if (Buffer == L"PIM" || Buffer == L"pim")
		return IPPROTO_PIM;
	else if (Buffer == L"PNNI" || Buffer == L"pnni")
		return IPPROTO_PNNI;
	else if (Buffer == L"ARIS" || Buffer == L"aris")
		return IPPROTO_ARIS;
	else if (Buffer == L"SCPS" || Buffer == L"scps")
		return IPPROTO_SCPS;
	else if (Buffer == L"QNX" || Buffer == L"qnx")
		return IPPROTO_QNX;
	else if (Buffer == L"AN" || Buffer == L"an")
		return IPPROTO_AN;
	else if (Buffer == L"IPCOMP" || Buffer == L"ipcomp")
		return IPPROTO_IPCOMP;
	else if (Buffer == L"SNP" || Buffer == L"snp")
		return IPPROTO_SNP;
	else if (Buffer == L"COMPAQ" || Buffer == L"compaq")
		return IPPROTO_COMPAQ;
	else if (Buffer == L"IPX" || Buffer == L"ipx")
		return IPPROTO_IPX;
	else if (Buffer == L"PGM" || Buffer == L"pgm")
		return IPPROTO_PGM;
	else if (Buffer == L"0HOP" || Buffer == L"0hop")
		return IPPROTO_0HOP;
	else if (Buffer == L"L2TP" || Buffer == L"l2tp")
		return IPPROTO_L2TP;
	else if (Buffer == L"DDX" || Buffer == L"ddx")
		return IPPROTO_DDX;
	else if (Buffer == L"IATP" || Buffer == L"iatp")
		return IPPROTO_IATP;
	else if (Buffer == L"STP" || Buffer == L"stp")
		return IPPROTO_STP;
	else if (Buffer == L"SRP" || Buffer == L"srp")
		return IPPROTO_SRP;
	else if (Buffer == L"UTI" || Buffer == L"uti")
		return IPPROTO_UTI;
	else if (Buffer == L"SMP" || Buffer == L"smp")
		return IPPROTO_SMP;
	else if (Buffer == L"SM" || Buffer == L"sm")
		return IPPROTO_SM;
	else if (Buffer == L"PTP" || Buffer == L"ptp")
		return IPPROTO_PTP;

//Internet Protocol Number(Part 2)
	if (Buffer == L"ISIS" || Buffer == L"isis")
		return IPPROTO_ISIS;
	else if (Buffer == L"FIRE" || Buffer == L"fire")
		return IPPROTO_FIRE;
	else if (Buffer == L"CRTP" || Buffer == L"crtp")
		return IPPROTO_CRTP;
	else if (Buffer == L"CRUDP" || Buffer == L"crudp")
		return IPPROTO_CRUDP;
	else if (Buffer == L"SSCOPMCE" || Buffer == L"sscopmce")
		return IPPROTO_SSCOPMCE;
	else if (Buffer == L"IPLT" || Buffer == L"iplt")
		return IPPROTO_IPLT;
	else if (Buffer == L"SPS" || Buffer == L"sps")
		return IPPROTO_SPS;
	else if (Buffer == L"PIPE" || Buffer == L"pipe")
		return IPPROTO_PIPE;
	else if (Buffer == L"SCTP" || Buffer == L"sctp")
		return IPPROTO_SCTP;
	else if (Buffer == L"FC" || Buffer == L"fc")
		return IPPROTO_FC;
	else if (Buffer == L"RSVPE2E" || Buffer == L"rsvpe2e")
		return IPPROTO_RSVP_E2E;
	else if (Buffer == L"MOBILITY" || Buffer == L"mobility")
		return IPPROTO_MOBILITY;
	else if (Buffer == L"UDPLITE" || Buffer == L"udplite")
		return IPPROTO_UDPLITE;
	else if (Buffer == L"MPLS" || Buffer == L"mpls")
		return IPPROTO_MPLS;
	else if (Buffer == L"MANET" || Buffer == L"manet")
		return IPPROTO_MANET;
	else if (Buffer == L"HIP" || Buffer == L"hip")
		return IPPROTO_HIP;
	else if (Buffer == L"SHIM6" || Buffer == L"shim6")
		return IPPROTO_SHIM6;
	else if (Buffer == L"WESP" || Buffer == L"wesp")
		return IPPROTO_WESP;
	else if (Buffer == L"ROHC" || Buffer == L"rohc")
		return IPPROTO_ROHC;
	else if (Buffer == L"TEST-1" || Buffer == L"test-1")
		return IPPROTO_TEST_1;
	else if (Buffer == L"TEST-2" || Buffer == L"test-2")
		return IPPROTO_TEST_2;
	else if (Buffer == L"RAW" || Buffer == L"raw")
		return IPPROTO_RAW;

//No match.
	return 0;
}

//Convert service name to port
uint16_t __fastcall ServiceNameToPort(
	const std::wstring &Buffer)
{
//Server name
	if (Buffer == L"TCPMUX" || Buffer == L"tcpmux")
		return htons(IPPORT_TCPMUX);
	else if (Buffer == L"ECHO" || Buffer == L"echo")
		return htons(IPPORT_ECHO);
	else if (Buffer == L"DISCARD" || Buffer == L"discard")
		return htons(IPPORT_DISCARD);
	else if (Buffer == L"SYSTAT" || Buffer == L"systat")
		return htons(IPPORT_SYSTAT);
	else if (Buffer == L"DAYTIME" || Buffer == L"daytime")
		return htons(IPPORT_DAYTIME);
	else if (Buffer == L"NETSTAT" || Buffer == L"netstat")
		return htons(IPPORT_NETSTAT);
	else if (Buffer == L"QOTD" || Buffer == L"qotd")
		return htons(IPPORT_QOTD);
	else if (Buffer == L"MSP" || Buffer == L"msp")
		return htons(IPPORT_MSP);
	else if (Buffer == L"CHARGEN" || Buffer == L"chargen")
		return htons(IPPORT_CHARGEN);
	else if (Buffer == L"FTPDATA" || Buffer == L"ftpdata")
		return htons(IPPORT_FTP_DATA);
	else if (Buffer == L"FTP" || Buffer == L"ftp")
		return htons(IPPORT_FTP);
	else if (Buffer == L"SSH" || Buffer == L"ssh")
		return htons(IPPORT_SSH);
	else if (Buffer == L"TELNET" || Buffer == L"telnet")
		return htons(IPPORT_TELNET);
	else if (Buffer == L"SMTP" || Buffer == L"smtp")
		return htons(IPPORT_SMTP);
	else if (Buffer == L"TIME" || Buffer == L"time")
		return htons(IPPORT_TIMESERVER);
	else if (Buffer == L"RAP" || Buffer == L"rap")
		return htons(IPPORT_RAP);
	else if (Buffer == L"RLP" || Buffer == L"rlp")
		return htons(IPPORT_RLP);
	else if (Buffer == L"NAME" || Buffer == L"name")
		return htons(IPPORT_NAMESERVER);
	else if (Buffer == L"WHOIS" || Buffer == L"whois")
		return htons(IPPORT_WHOIS);
	else if (Buffer == L"TACACS" || Buffer == L"tacacs")
		return htons(IPPORT_TACACS);
	else if (Buffer == L"DNS" || Buffer == L"dns")
		return htons(IPPORT_DNS);
	else if (Buffer == L"XNSAUTH" || Buffer == L"xnsauth")
		return htons(IPPORT_XNSAUTH);
	else if (Buffer == L"MTP" || Buffer == L"mtp")
		return htons(IPPORT_MTP);
	else if (Buffer == L"BOOTPS" || Buffer == L"bootps")
		return htons(IPPORT_BOOTPS);
	else if (Buffer == L"BOOTPC" || Buffer == L"bootpc")
		return htons(IPPORT_BOOTPC);
	else if (Buffer == L"TFTP" || Buffer == L"tftp")
		return htons(IPPORT_TFTP);
	else if (Buffer == L"RJE" || Buffer == L"rje")
		return htons(IPPORT_RJE);
	else if (Buffer == L"FINGER" || Buffer == L"finger")
		return htons(IPPORT_FINGER);
	else if (Buffer == L"HTTP" || Buffer == L"http")
		return htons(IPPORT_HTTP);
	else if (Buffer == L"HTTPBACKUP" || Buffer == L"httpbackup")
		return htons(IPPORT_HTTPBACKUP);
	else if (Buffer == L"TTYLINK" || Buffer == L"ttylink")
		return htons(IPPORT_TTYLINK);
	else if (Buffer == L"SUPDUP" || Buffer == L"supdup")
		return htons(IPPORT_SUPDUP);
	else if (Buffer == L"POP3" || Buffer == L"pop3")
		return htons(IPPORT_POP3);
	else if (Buffer == L"SUNRPC" || Buffer == L"sunrpc")
		return htons(IPPORT_SUNRPC);
	else if (Buffer == L"SQL" || Buffer == L"sql")
		return htons(IPPORT_SQL);
	else if (Buffer == L"NTP" || Buffer == L"ntp")
		return htons(IPPORT_NTP);
	else if (Buffer == L"EPMAP" || Buffer == L"epmap")
		return htons(IPPORT_EPMAP);
	else if (Buffer == L"NETBIOSNS" || Buffer == L"netbiosns")
		return htons(IPPORT_NETBIOS_NS);
	else if (Buffer == L"NETBIOSDGM" || Buffer == L"netbiosdgm")
		return htons(IPPORT_NETBIOS_DGM);
	else if (Buffer == L"NETBIOSSSN" || Buffer == L"netbiosssn")
		return htons(IPPORT_NETBIOS_SSN);
	else if (Buffer == L"IMAP" || Buffer == L"imap")
		return htons(IPPORT_IMAP);
	else if (Buffer == L"BFTP" || Buffer == L"bftp")
		return htons(IPPORT_BFTP);
	else if (Buffer == L"SGMP" || Buffer == L"sgmp")
		return htons(IPPORT_SGMP);
	else if (Buffer == L"SQLSRV" || Buffer == L"sqlsrv")
		return htons(IPPORT_SQLSRV);
	else if (Buffer == L"DMSP" || Buffer == L"dmsp")
		return htons(IPPORT_DMSP);
	else if (Buffer == L"SNMP" || Buffer == L"snmp")
		return htons(IPPORT_SNMP);
	else if (Buffer == L"SNMPTRAP" || Buffer == L"snmptrap")
		return htons(IPPORT_SNMP_TRAP);
	else if (Buffer == L"ATRTMP" || Buffer == L"atrtmp")
		return htons(IPPORT_ATRTMP);
	else if (Buffer == L"ATHBP" || Buffer == L"athbp")
		return htons(IPPORT_ATHBP);
	else if (Buffer == L"QMTP" || Buffer == L"qmtp")
		return htons(IPPORT_QMTP);
	else if (Buffer == L"IPX" || Buffer == L"ipx")
		return htons(IPPORT_IPX);
	else if (Buffer == L"IMAP3" || Buffer == L"imap3")
		return htons(IPPORT_IMAP3);
	else if (Buffer == L"BGMP" || Buffer == L"bgmp")
		return htons(IPPORT_BGMP);
	else if (Buffer == L"TSP" || Buffer == L"tsp")
		return htons(IPPORT_TSP);
	else if (Buffer == L"IMMP" || Buffer == L"immp")
		return htons(IPPORT_IMMP);
	else if (Buffer == L"ODMR" || Buffer == L"odmr")
		return htons(IPPORT_ODMR);
	else if (Buffer == L"RPC2PORTMAP" || Buffer == L"rpc2portmap")
		return htons(IPPORT_RPC2PORTMAP);
	else if (Buffer == L"CLEARCASE" || Buffer == L"clearcase")
		return htons(IPPORT_CLEARCASE);
	else if (Buffer == L"HPALARMMGR" || Buffer == L"hpalarmmgr")
		return htons(IPPORT_HPALARMMGR);
	else if (Buffer == L"ARNS" || Buffer == L"arns")
		return htons(IPPORT_ARNS);
	else if (Buffer == L"AURP" || Buffer == L"aurp")
		return htons(IPPORT_AURP);
	else if (Buffer == L"LDAP" || Buffer == L"ldap")
		return htons(IPPORT_LDAP);
	else if (Buffer == L"UPS" || Buffer == L"ups")
		return htons(IPPORT_UPS);
	else if (Buffer == L"SLP" || Buffer == L"slp")
		return htons(IPPORT_SLP);
	else if (Buffer == L"HTTPS" || Buffer == L"https")
		return htons(IPPORT_HTTPS);
	else if (Buffer == L"SNPP" || Buffer == L"snpp")
		return htons(IPPORT_SNPP);
	else if (Buffer == L"MICROSOFTDS" || Buffer == L"microsoftds")
		return htons(IPPORT_MICROSOFT_DS);
	else if (Buffer == L"KPASSWD" || Buffer == L"kpasswd")
		return htons(IPPORT_KPASSWD);
	else if (Buffer == L"TCPNETHASPSRV" || Buffer == L"tcpnethaspsrv")
		return htons(IPPORT_TCPNETHASPSRV);
	else if (Buffer == L"RETROSPECT" || Buffer == L"retrospect")
		return htons(IPPORT_RETROSPECT);
	else if (Buffer == L"ISAKMP" || Buffer == L"isakmp")
		return htons(IPPORT_ISAKMP);
	else if (Buffer == L"BIFFUDP" || Buffer == L"biffudp")
		return htons(IPPORT_BIFFUDP);
	else if (Buffer == L"WHOSERVER" || Buffer == L"whoserver")
		return htons(IPPORT_WHOSERVER);
	else if (Buffer == L"SYSLOG" || Buffer == L"syslog")
		return htons(IPPORT_SYSLOG);
	else if (Buffer == L"ROUTERSERVER" || Buffer == L"routerserver")
		return htons(IPPORT_ROUTESERVER);
	else if (Buffer == L"NCP" || Buffer == L"ncp")
		return htons(IPPORT_NCP);
	else if (Buffer == L"COURIER" || Buffer == L"courier")
		return htons(IPPORT_COURIER);
	else if (Buffer == L"COMMERCE" || Buffer == L"commerce")
		return htons(IPPORT_COMMERCE);
	else if (Buffer == L"RTSP" || Buffer == L"rtsp")
		return htons(IPPORT_RTSP);
	else if (Buffer == L"NNTP" || Buffer == L"nntp")
		return htons(IPPORT_NNTP);
	else if (Buffer == L"HTTPRPCEPMAP" || Buffer == L"httprpcepmap")
		return htons(IPPORT_HTTPRPCEPMAP);
	else if (Buffer == L"IPP" || Buffer == L"ipp")
		return htons(IPPORT_IPP);
	else if (Buffer == L"LDAPS" || Buffer == L"ldaps")
		return htons(IPPORT_LDAPS);
	else if (Buffer == L"MSDP" || Buffer == L"msdp")
		return htons(IPPORT_MSDP);
	else if (Buffer == L"AODV" || Buffer == L"aodv")
		return htons(IPPORT_AODV);
	else if (Buffer == L"FTPSDATA" || Buffer == L"ftpsdata")
		return htons(IPPORT_FTPSDATA);
	else if (Buffer == L"FTPS" || Buffer == L"ftps")
		return htons(IPPORT_FTPS);
	else if (Buffer == L"NAS" || Buffer == L"nas")
		return htons(IPPORT_NAS);
	else if (Buffer == L"TELNETS" || Buffer == L"telnets")
		return htons(IPPORT_TELNETS);
//No match.
	return 0;
}

//Convert DNS classes name to hex
uint16_t __fastcall DNSClassesNameToHex(
	const std::wstring &Buffer)
{
//DNS classes name
	if (Buffer == L"INTERNET" || Buffer == L"internet" || Buffer == L"IN" || Buffer == L"in")
		return htons(DNS_CLASS_IN);
	else if (Buffer == L"CSNET" || Buffer == L"csnet")
		return htons(DNS_CLASS_CSNET);
	else if (Buffer == L"CHAOS" || Buffer == L"chaos")
		return htons(DNS_CLASS_CHAOS);
	else if (Buffer == L"HESIOD" || Buffer == L"hesiod")
		return htons(DNS_CLASS_HESIOD);
	else if (Buffer == L"NONE" || Buffer == L"none")
		return htons(DNS_CLASS_NONE);
	else if (Buffer == L"ALL" || Buffer == L"all")
		return htons(DNS_CLASS_ALL);
	else if (Buffer == L"ANY" || Buffer == L"any")
		return htons(DNS_CLASS_ANY);
//No match.
	return 0;
}

//Convert DNS type name to hex
uint16_t __fastcall DNSTypeNameToHex(
	const std::wstring &Buffer)
{
//DNS type name
	if (Buffer == L"A" || Buffer == L"a")
		return htons(DNS_RECORD_A);
	else if (Buffer == L"NS" || Buffer == L"ns")
		return htons(DNS_RECORD_NS);
	else if (Buffer == L"MD" || Buffer == L"md")
		return htons(DNS_RECORD_MD);
	else if (Buffer == L"MF" || Buffer == L"mf")
		return htons(DNS_RECORD_MF);
	else if (Buffer == L"CNAME" || Buffer == L"cname")
		return htons(DNS_RECORD_CNAME);
	else if (Buffer == L"SOA" || Buffer == L"soa")
		return htons(DNS_RECORD_SOA);
	else if (Buffer == L"MB" || Buffer == L"mb")
		return htons(DNS_RECORD_MB);
	else if (Buffer == L"MG" || Buffer == L"mg")
		return htons(DNS_RECORD_MG);
	else if (Buffer == L"MR" || Buffer == L"mr")
		return htons(DNS_RECORD_MR);
	else if (Buffer == L"PTR" || Buffer == L"ptr")
		return htons(DNS_RECORD_PTR);
	else if (Buffer == L"NULL" || Buffer == L"null")
		return htons(DNS_RECORD_NULL);
	else if (Buffer == L"WKS" || Buffer == L"wks")
		return htons(DNS_RECORD_WKS);
	else if (Buffer == L"HINFO" || Buffer == L"hinfo")
		return htons(DNS_RECORD_HINFO);
	else if (Buffer == L"MINFO" || Buffer == L"minfo")
		return htons(DNS_RECORD_MINFO);
	else if (Buffer == L"MX" || Buffer == L"mx")
		return htons(DNS_RECORD_MX);
	else if (Buffer == L"TXT" || Buffer == L"txt")
		return htons(DNS_RECORD_TXT);
	else if (Buffer == L"RP" || Buffer == L"rp")
		return htons(DNS_RECORD_RP);
	else if (Buffer == L"SIG" || Buffer == L"sig")
		return htons(DNS_RECORD_SIG);
	else if (Buffer == L"AFSDB" || Buffer == L"afsdb")
		return htons(DNS_RECORD_AFSDB);
	else if (Buffer == L"X25" || Buffer == L"x25")
		return htons(DNS_RECORD_X25);
	else if (Buffer == L"ISDN" || Buffer == L"isdn")
		return htons(DNS_RECORD_ISDN);
	else if (Buffer == L"RT" || Buffer == L"rt")
		return htons(DNS_RECORD_RT);
	else if (Buffer == L"NSAP" || Buffer == L"nsap")
		return htons(DNS_RECORD_NSAP);
	else if (Buffer == L"NSAPPTR" || Buffer == L"nsapptr")
		return htons(DNS_RECORD_NSAP_PTR);
	else if (Buffer == L"SIG" || Buffer == L"sig")
		return htons(DNS_RECORD_SIG);
	else if (Buffer == L"KEY" || Buffer == L"key")
		return htons(DNS_RECORD_KEY);
	else if (Buffer == L"AAAA" || Buffer == L"aaaa")
		return htons(DNS_RECORD_AAAA);
	else if (Buffer == L"PX" || Buffer == L"px")
		return htons(DNS_RECORD_PX);
	else if (Buffer == L"GPOS" || Buffer == L"gpos")
		return htons(DNS_RECORD_GPOS);
	else if (Buffer == L"LOC" || Buffer == L"loc")
		return htons(DNS_RECORD_LOC);
	else if (Buffer == L"NXT" || Buffer == L"nxt")
		return htons(DNS_RECORD_NXT);
	else if (Buffer == L"EID" || Buffer == L"eid")
		return htons(DNS_RECORD_EID);
	else if (Buffer == L"NIMLOC" || Buffer == L"nimloc")
		return htons(DNS_RECORD_NIMLOC);
	else if (Buffer == L"SRV" || Buffer == L"srv")
		return htons(DNS_RECORD_SRV);
	else if (Buffer == L"ATMA" || Buffer == L"atma")
		return htons(DNS_RECORD_ATMA);
	else if (Buffer == L"NAPTR" || Buffer == L"naptr")
		return htons(DNS_RECORD_NAPTR);
	else if (Buffer == L"KX" || Buffer == L"kx")
		return htons(DNS_RECORD_KX);
	else if (Buffer == L"CERT" || Buffer == L"cert")
		return htons(DNS_RECORD_CERT);
	else if (Buffer == L"A6" || Buffer == L"a6")
		return htons(DNS_RECORD_A6);
	else if (Buffer == L"DNAME" || Buffer == L"dname")
		return htons(DNS_RECORD_DNAME);
	else if (Buffer == L"SINK" || Buffer == L"sink")
		return htons(DNS_RECORD_SINK);
	else if (Buffer == L"OPT" || Buffer == L"opt")
		return htons(DNS_RECORD_OPT);
	else if (Buffer == L"APL" || Buffer == L"apl")
		return htons(DNS_RECORD_APL);
	else if (Buffer == L"DS" || Buffer == L"ds")
		return htons(DNS_RECORD_DS);
	else if (Buffer == L"SSHFP" || Buffer == L"sshfp")
		return htons(DNS_RECORD_SSHFP);
	else if (Buffer == L"IPSECKEY" || Buffer == L"ipseckey")
		return htons(DNS_RECORD_IPSECKEY);
	else if (Buffer == L"RRSIG" || Buffer == L"rrsig")
		return htons(DNS_RECORD_RRSIG);
	else if (Buffer == L"NSEC" || Buffer == L"nsec")
		return htons(DNS_RECORD_NSEC);
	else if (Buffer == L"DNSKEY" || Buffer == L"dnskey")
		return htons(DNS_RECORD_DNSKEY);
	else if (Buffer == L"DHCID" || Buffer == L"dhcid")
		return htons(DNS_RECORD_DHCID);
	else if (Buffer == L"NSEC3" || Buffer == L"nsec3")
		return htons(DNS_RECORD_NSEC3);
	else if (Buffer == L"NSEC3PARAM" || Buffer == L"nsec3param")
		return htons(DNS_RECORD_NSEC3PARAM);
	else if (Buffer == L"TLSA" || Buffer == L"tlsa")
		return htons(DNS_RECORD_TLSA);
	else if (Buffer == L"HIP" || Buffer == L"hip")
		return htons(DNS_RECORD_HIP);
	else if (Buffer == L"HINFO" || Buffer == L"hinfo")
		return htons(DNS_RECORD_HINFO);
	else if (Buffer == L"RKEY" || Buffer == L"rkey")
		return htons(DNS_RECORD_RKEY);
	else if (Buffer == L"TALINK" || Buffer == L"talink")
		return htons(DNS_RECORD_TALINK);
	else if (Buffer == L"CDS" || Buffer == L"cds")
		return htons(DNS_RECORD_CDS);
	else if (Buffer == L"CDNSKEY" || Buffer == L"cdnskey")
		return htons(DNS_RECORD_CDNSKEY);
	else if (Buffer == L"OPENPGPKEY" || Buffer == L"openpgpkey")
		return htons(DNS_RECORD_OPENPGPKEY);
	else if (Buffer == L"SPF" || Buffer == L"spf")
		return htons(DNS_RECORD_SPF);
	else if (Buffer == L"UINFO" || Buffer == L"uinfo")
		return htons(DNS_RECORD_UINFO);
	else if (Buffer == L"UID" || Buffer == L"uid")
		return htons(DNS_RECORD_UID);
	else if (Buffer == L"GID" || Buffer == L"gid")
		return htons(DNS_RECORD_GID);
	else if (Buffer == L"UNSPEC" || Buffer == L"unspec")
		return htons(DNS_RECORD_UNSPEC);
	else if (Buffer == L"NID" || Buffer == L"nid")
		return htons(DNS_RECORD_NID);
	else if (Buffer == L"L32" || Buffer == L"l32")
		return htons(DNS_RECORD_L32);
	else if (Buffer == L"L64" || Buffer == L"l64")
		return htons(DNS_RECORD_L64);
	else if (Buffer == L"LP" || Buffer == L"lp")
		return htons(DNS_RECORD_LP);
	else if (Buffer == L"EUI48" || Buffer == L"eui48")
		return htons(DNS_RECORD_EUI48);
	else if (Buffer == L"EUI64" || Buffer == L"eui64")
		return htons(DNS_RECORD_EUI64);
	else if (Buffer == L"TKEY" || Buffer == L"tkey")
		return htons(DNS_RECORD_TKEY);
	else if (Buffer == L"TSIG" || Buffer == L"tsig")
		return htons(DNS_RECORD_TSIG);
	else if (Buffer == L"IXFR" || Buffer == L"ixfr")
		return htons(DNS_RECORD_IXFR);
	else if (Buffer == L"AXFR" || Buffer == L"axfr")
		return htons(DNS_RECORD_AXFR);
	else if (Buffer == L"MAILB" || Buffer == L"mailb")
		return htons(DNS_RECORD_MAILB);
	else if (Buffer == L"MAILA" || Buffer == L"maila")
		return htons(DNS_RECORD_MAILA);
	else if (Buffer == L"ANY" || Buffer == L"any")
		return htons(DNS_RECORD_ANY);
	else if (Buffer == L"URI" || Buffer == L"uri")
		return htons(DNS_RECORD_URI);
	else if (Buffer == L"CAA" || Buffer == L"caa")
		return htons(DNS_RECORD_CAA);
	else if (Buffer == L"TA" || Buffer == L"ta")
		return htons(DNS_RECORD_TA);
	else if (Buffer == L"DLV" || Buffer == L"dlv")
		return htons(DNS_RECORD_DLV);
	else if (Buffer == L"RESERVED" || Buffer == L"reserved")
		return htons(DNS_RECORD_RESERVED);
//No match.
	return 0;
}

//Convert data from char(s) to DNS query
size_t __fastcall CharToDNSQuery(
	const char *FName, 
	char *TName)
{
	int Index[] = {(int)strnlen_s(FName, DOMAIN_MAXSIZE) - 1, 0, 0};
	Index[2U] = Index[0] + 1;
	TName[Index[0] + 2] = 0;

	for (;Index[0] >= 0;Index[0]--,Index[2U]--)
	{
		if (FName[Index[0]] == ASCII_PERIOD)
		{
			TName[Index[2U]] = (char)Index[1U];
			Index[1U] = 0;
		}
		else
		{
			TName[Index[2U]] = FName[Index[0]];
			++Index[1U];
		}
	}

	TName[Index[2U]] = (char)Index[1U];
	return strnlen_s(TName, DOMAIN_MAXSIZE - 1U) + 1U;
}

//Convert data from DNS query to char(s)
size_t __fastcall DNSQueryToChar(
	const char *TName, 
	char *FName, 
	uint16_t &Truncated)
{
//Initialization
	size_t uIndex = 0;
	int Index[] = {0, 0};

//Convert domain.
	for (uIndex = 0;uIndex < DOMAIN_MAXSIZE;++uIndex)
	{
	//Pointer
		if ((UCHAR)TName[uIndex] >= 0xC0)
		{
			Truncated = (UCHAR)(TName[uIndex] & 0x3F);
			Truncated = Truncated << sizeof(char) * BYTES_TO_BITS;
			Truncated += (UCHAR)TName[uIndex + 1U];
			return uIndex + sizeof(uint16_t);
		}
		else if (uIndex == 0)
		{
			Index[0] = TName[uIndex];
		}
		else if (uIndex == Index[0] + Index[1U] + 1U)
		{
			Index[0] = TName[uIndex];
			if (Index[0] == 0)
				break;
			Index[1U] = (int)uIndex;

			FName[uIndex - 1U] = ASCII_PERIOD;
		}
		else {
			FName[uIndex - 1U] = TName[uIndex];
		}
	}

	Truncated = 0;
	return uIndex;
}

//Validate packets
bool __fastcall ValidatePacket(
	const char *Buffer, 
	const size_t Length, 
	const uint16_t DNS_ID)
{
	auto pdns_hdr = (dns_hdr *)Buffer;

//DNS ID and Questions check
	if (pdns_hdr->ID != DNS_ID || pdns_hdr->Questions == 0)
		return false;

//EDNS Label check
	if (ConfigurationParameter.EDNS)
	{
		if (pdns_hdr->Additional == 0)
		{
			return false;
		}
		else if (pdns_hdr->Additional == 1U)
		{
			if (Length > sizeof(dns_opt_record))
			{
				auto pdns_opt_record = (dns_opt_record *)(Buffer + Length - sizeof(dns_opt_record));

			//UDP Payload Size and Z Field of DNSSEC check
				if (pdns_opt_record->UDPPayloadSize == 0 || ConfigurationParameter.DNSSEC && pdns_opt_record->Z_Field == 0)
					return false;
			}
			else {
				return false;
			}
		}
	}

	return true;
}

//Print date from seconds
void __fastcall PrintSecondsInDateTime(
	const time_t Seconds)
{
//Less than 1 minute
	if (Seconds < SECONDS_IN_MINUTE)
		return;

//Initialization
	auto Before = false;
	auto DateTime = Seconds;
	fwprintf_s(stderr, L"(");

//Years
	if (DateTime / SECONDS_IN_YEAR > 0)
	{
		fwprintf_s(stderr, L"%u year", (UINT)(DateTime / SECONDS_IN_YEAR));
		if (DateTime / SECONDS_IN_YEAR > 1U)
			fwprintf_s(stderr, L"s");
		DateTime %= SECONDS_IN_YEAR;
		Before = true;
	}
//Months
	if (DateTime / SECONDS_IN_MONTH > 0)
	{
		if (Before)
			fwprintf_s(stderr, L" ");
		fwprintf_s(stderr, L"%u month", (UINT)(DateTime / SECONDS_IN_MONTH));
		if (DateTime / SECONDS_IN_MONTH > 1U)
			fwprintf_s(stderr, L"s");
		DateTime %= SECONDS_IN_MONTH;
		Before = true;
	}
//Days
	if (DateTime / SECONDS_IN_DAY > 0)
	{
		if (Before)
			fwprintf_s(stderr, L" ");
		fwprintf_s(stderr, L"%u day", (UINT)(DateTime / SECONDS_IN_DAY));
		if (DateTime / SECONDS_IN_DAY > 1U)
			fwprintf_s(stderr, L"s");
		DateTime %= SECONDS_IN_DAY;
		Before = true;
	}
//Hours
	if (DateTime / SECONDS_IN_HOUR > 0)
	{
		if (Before)
			fwprintf_s(stderr, L" ");
		fwprintf_s(stderr, L"%u hour", (UINT)(DateTime / SECONDS_IN_HOUR));
		if (DateTime / SECONDS_IN_HOUR > 1U)
			fwprintf_s(stderr, L"s");
		DateTime %= SECONDS_IN_HOUR;
		Before = true;
	}
//Minutes
	if (DateTime / SECONDS_IN_MINUTE > 0)
	{
		if (Before)
			fwprintf_s(stderr, L" ");
		fwprintf_s(stderr, L"%u minute", (UINT)(DateTime / SECONDS_IN_MINUTE));
		if (DateTime / SECONDS_IN_MINUTE > 1U)
			fwprintf_s(stderr, L"s");
		DateTime %= SECONDS_IN_MINUTE;
		Before = true;
	}
//Seconds
	if (DateTime > 0)
	{
		if (Before)
			fwprintf_s(stderr, L" ");
		fwprintf_s(stderr, L"%u second", (UINT)(DateTime));
		if (DateTime > 1U)
			fwprintf_s(stderr, L"s");
	}

	fwprintf_s(stderr, L")");
	return;
}

//Print date from seconds to file
void __fastcall PrintSecondsInDateTime(
	const time_t Seconds, 
	FILE *OutputFile)
{
//Less than 1 minute
	if (Seconds < SECONDS_IN_MINUTE)
		return;

//Initialization
	auto Before = false;
	auto DateTime = Seconds;
	fwprintf_s(OutputFile, L"(");

//Years
	if (DateTime / SECONDS_IN_YEAR > 0)
	{
		fwprintf_s(OutputFile, L"%u year", (UINT)(DateTime / SECONDS_IN_YEAR));
		if (DateTime / SECONDS_IN_YEAR > 1U)
			fwprintf_s(OutputFile, L"s");
		DateTime %= SECONDS_IN_YEAR;
		Before = true;
	}
//Months
	if (DateTime / SECONDS_IN_MONTH > 0)
	{
		if (Before)
			fwprintf_s(OutputFile, L" ");
		fwprintf_s(OutputFile, L"%u month", (UINT)(DateTime / SECONDS_IN_MONTH));
		if (DateTime / SECONDS_IN_MONTH > 1U)
			fwprintf_s(OutputFile, L"s");
		DateTime %= SECONDS_IN_MONTH;
		Before = true;
	}
//Days
	if (DateTime / SECONDS_IN_DAY > 0)
	{
		if (Before)
			fwprintf_s(OutputFile, L" ");
		fwprintf_s(OutputFile, L"%u day", (UINT)(DateTime / SECONDS_IN_DAY));
		if (DateTime / SECONDS_IN_DAY > 1U)
			fwprintf_s(OutputFile, L"s");
		DateTime %= SECONDS_IN_DAY;
		Before = true;
	}
//Hours
	if (DateTime / SECONDS_IN_HOUR > 0)
	{
		if (Before)
			fwprintf_s(OutputFile, L" ");
		fwprintf_s(OutputFile, L"%u hour", (UINT)(DateTime / SECONDS_IN_HOUR));
		if (DateTime / SECONDS_IN_HOUR > 1U)
			fwprintf_s(OutputFile, L"s");
		DateTime %= SECONDS_IN_HOUR;
		Before = true;
	}
//Minutes
	if (DateTime / SECONDS_IN_MINUTE > 0)
	{
		if (Before)
			fwprintf_s(OutputFile, L" ");
		fwprintf_s(OutputFile, L"%u minute", (UINT)(DateTime / SECONDS_IN_MINUTE));
		if (DateTime / SECONDS_IN_MINUTE > 1U)
			fwprintf_s(OutputFile, L"s");
		DateTime %= SECONDS_IN_MINUTE;
		Before = true;
	}
//Seconds
	if (DateTime > 0)
	{
		if (Before)
			fwprintf_s(OutputFile, L" ");
		fwprintf_s(OutputFile, L"%u second", (UINT)(DateTime));
		if (DateTime > 1U)
			fwprintf_s(OutputFile, L"s");
	}

	fwprintf_s(OutputFile, L")");
	return;
}

//Print Date and Time with UNIX time
void __fastcall PrintDateTime(
	const time_t Time)
{
	tm TimeStructure = {0};
	localtime_s(&TimeStructure, &Time);
	fwprintf_s(stderr, L"%d-%02d-%02d %02d:%02d:%02d", TimeStructure.tm_year + 1900, TimeStructure.tm_mon + 1, TimeStructure.tm_mday, TimeStructure.tm_hour, TimeStructure.tm_min, TimeStructure.tm_sec);

	return;
}

//Print Date and Time with UNIX time to file
void __fastcall PrintDateTime(
	const time_t Time, 
	FILE *OutputFile)
{
	tm TimeStructure = {0};
	localtime_s(&TimeStructure, &Time);
	fwprintf_s(OutputFile, L"%d-%02d-%02d %02d:%02d:%02d", TimeStructure.tm_year + 1900, TimeStructure.tm_mon + 1, TimeStructure.tm_mday, TimeStructure.tm_hour, TimeStructure.tm_min, TimeStructure.tm_sec);

	return;
}
