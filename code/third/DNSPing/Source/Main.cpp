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


#include "Main.h"

ConfigurationTable ConfigurationParameter;

//Main function of program
int dnsping_main(
	int argc, 
	wchar_t* argv[])
{
//Main process
	if (argc <= 2)
	{
		PrintDescription();
	}
	else {
	//Initialization and read commands.
		if (ConfigurationInitialization() == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
		else if (ReadCommands(argc, argv) == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

	//Check parameter reading.
		if (ConfigurationParameter.SockAddr_Normal.ss_family == AF_INET6) //IPv6
		{
			if (CheckEmptyBuffer(&((PSOCKADDR_IN6)&ConfigurationParameter.SockAddr_Normal)->sin6_addr, sizeof(in6_addr)))
			{
				fwprintf_s(stderr, L"\nTarget is empty.\n");

				return EXIT_FAILURE;
			}
			else {
			//Mark port.
				if (ConfigurationParameter.ServiceType == 0)
				{
					ConfigurationParameter.ServiceType = htons(IPPORT_DNS);
					((PSOCKADDR_IN6)&ConfigurationParameter.SockAddr_Normal)->sin6_port = htons(IPPORT_DNS);
				}
				else {
					((PSOCKADDR_IN6)&ConfigurationParameter.SockAddr_Normal)->sin6_port = ConfigurationParameter.ServiceType;
				}
			}
		}
		else if (ConfigurationParameter.SockAddr_Normal.ss_family == AF_INET) //IPv4
		{
			if (((PSOCKADDR_IN)&ConfigurationParameter.SockAddr_Normal)->sin_addr.s_addr == 0)
			{
				fwprintf_s(stderr, L"\nTarget is empty.\n");

				return EXIT_FAILURE;
			}
			else {
			//Mark port.
				if (ConfigurationParameter.ServiceType == 0)
				{
					ConfigurationParameter.ServiceType = htons(IPPORT_DNS);
					((PSOCKADDR_IN)&ConfigurationParameter.SockAddr_Normal)->sin_port = htons(IPPORT_DNS);
				}
				else {
					((PSOCKADDR_IN)&ConfigurationParameter.SockAddr_Normal)->sin_port = ConfigurationParameter.ServiceType;
				}
			}
		}
		else { //SOCKS mode
			if (ConfigurationParameter.SockAddr_SOCKS.ss_family == 0)
			{
				fwprintf_s(stderr, L"\nTarget is empty.\n");

				return EXIT_FAILURE;
			}
			else {
			//Mark port.
				if (ConfigurationParameter.ServiceType == 0)
				{
					ConfigurationParameter.ServiceType = htons(IPPORT_DNS);
/*					((PSOCKADDR_IN)&ConfigurationParameter.SockAddr_Normal)->sin_port = htons(IPPORT_DNS);
				}
				else {
					((PSOCKADDR_IN)&ConfigurationParameter.SockAddr_Normal)->sin_port = ConfigurationParameter.ServiceType;
*/				}
			}
		}

	//Check parameter.
	//Minimum supported system of Windows Version Helpers is Windows Vista.
	#if defined(PLATFORM_WIN)
		#if defined(PLATFORM_WIN64)
			if (!IsWindows8OrGreater())
		#elif defined(PLATFORM_WIN32)
			if (IsLowerThanWin8())
		#endif
			{
				if (ConfigurationParameter.SocketTimeout > TIME_OUT_MIN)
					ConfigurationParameter.SocketTimeout -= 500;
				else if (ConfigurationParameter.SocketTimeout == TIME_OUT_MIN)
					ConfigurationParameter.SocketTimeout = 1;
			}
		ConfigurationParameter.Statistics_MinTime = ConfigurationParameter.SocketTimeout;
	#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
		ConfigurationParameter.Statistics_MinTime = ConfigurationParameter.SocketTimeout.tv_sec * SECOND_TO_MILLISECOND + ConfigurationParameter.SocketTimeout.tv_usec / MICROSECOND_TO_MILLISECOND;
	#endif

	//Convert multiple byte string to wide char string.
		std::wstring wTestDomain, wTargetAddressString;
	#if defined(PLATFORM_WIN)
		std::shared_ptr<wchar_t> BufferTemp(new wchar_t[LARGE_PACKET_MAXSIZE]());
		wmemset(BufferTemp.get(), 0, LARGE_PACKET_MAXSIZE);
		MultiByteToWideChar(CP_ACP, 0, ConfigurationParameter.TargetString_Normal.c_str(), MBSTOWCS_NULLTERMINATE, BufferTemp.get(), (int)ConfigurationParameter.TargetString_Normal.length());
		ConfigurationParameter.wTargetString = BufferTemp.get();
		wmemset(BufferTemp.get(), 0, LARGE_PACKET_MAXSIZE);
		MultiByteToWideChar(CP_ACP, 0, ConfigurationParameter.TestDomain.c_str(), MBSTOWCS_NULLTERMINATE, BufferTemp.get(), (int)ConfigurationParameter.TestDomain.length());
		wTestDomain = BufferTemp.get();
		if (!ConfigurationParameter.TargetAddressString.empty())
		{
			wmemset(BufferTemp.get(), 0, LARGE_PACKET_MAXSIZE);
			MultiByteToWideChar(CP_ACP, 0, ConfigurationParameter.TargetAddressString.c_str(), MBSTOWCS_NULLTERMINATE, BufferTemp.get(), (int)ConfigurationParameter.TargetAddressString.length());
			wTargetAddressString = BufferTemp.get();
		}
		BufferTemp.reset();
	#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
		MBSToWCSString(ConfigurationParameter.wTargetString, ConfigurationParameter.TargetString_Normal.c_str());
		MBSToWCSString(wTestDomain, ConfigurationParameter.TestDomain.c_str());
		if (!ConfigurationParameter.TargetAddressString.empty())
			MBSToWCSString(wTargetAddressString, ConfigurationParameter.TargetAddressString.c_str());
	#endif

	//Check DNS header.
		if (ConfigurationParameter.Parameter_Header.Flags == 0)
			ConfigurationParameter.Parameter_Header.Flags = htons(DNS_STANDARD);
		if (ConfigurationParameter.Parameter_Header.Questions == 0)
			ConfigurationParameter.Parameter_Header.Questions = htons(U16_NUM_ONE);

	//Check DNS query.
		if (ConfigurationParameter.Parameter_Query.Classes == 0)
			ConfigurationParameter.Parameter_Query.Classes = htons(DNS_CLASS_IN);
		if (ConfigurationParameter.Parameter_Query.Type == 0)
		{
			if (ConfigurationParameter.SockAddr_Normal.ss_family == AF_INET6) //IPv6
				ConfigurationParameter.Parameter_Query.Type = htons(DNS_RECORD_AAAA);
			else //IPv4
				ConfigurationParameter.Parameter_Query.Type = htons(DNS_RECORD_A);
		}

	//Check EDNS Label.
		if (ConfigurationParameter.DNSSEC)
			ConfigurationParameter.EDNS = true;
		if (ConfigurationParameter.EDNS)
		{
			ConfigurationParameter.Parameter_Header.Additional = htons(U16_NUM_ONE);
			ConfigurationParameter.Parameter_EDNS.Type = htons(DNS_RECORD_OPT);
			if (ConfigurationParameter.EDNSPayloadSize == 0)
				ConfigurationParameter.Parameter_EDNS.UDPPayloadSize = htons(EDNS0_MINSIZE);
			else 
				ConfigurationParameter.Parameter_EDNS.UDPPayloadSize = htons((uint16_t)ConfigurationParameter.EDNSPayloadSize);
			if (ConfigurationParameter.DNSSEC)
			{
				ConfigurationParameter.Parameter_Header.FlagsBits.AD = ~ConfigurationParameter.Parameter_Header.FlagsBits.AD; //Local DNSSEC Server validate
				ConfigurationParameter.Parameter_Header.FlagsBits.CD = ~ConfigurationParameter.Parameter_Header.FlagsBits.CD; //Client validate
				ConfigurationParameter.Parameter_EDNS.Z_Bits.DO = ~ConfigurationParameter.Parameter_EDNS.Z_Bits.DO; //Accepts DNSSEC security RRs
			}
		}

	//Output result to file.
		if (!ConfigurationParameter.wOutputFileName.empty() && OutputResultToFile() == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

	//Print to screen before sending and send request.
		PrintHeaderToScreen(wTargetAddressString, wTestDomain);
		if (ConfigurationParameter.Statistics_Send == 0)
		{
			for (;;)
			{
				if (ConfigurationParameter.Statistics_RealSend <= UINT16_MAX)
				{
					++ConfigurationParameter.Statistics_RealSend;
					if (SendProcess(ConfigurationParameter.SockAddr_Normal, false) == EXIT_FAILURE)
					{
						return EXIT_FAILURE;
					}
				}
				else {
					fwprintf_s(stderr, L"\nStatistics is full.\n");
					if (ConfigurationParameter.OutputFile != nullptr)
						fwprintf_s(ConfigurationParameter.OutputFile, L"\nStatistics is full.\n");

					PrintProcess(true, true);
				//Close file handle.
					if (ConfigurationParameter.OutputFile != nullptr)
						fclose(ConfigurationParameter.OutputFile);

					return EXIT_SUCCESS;
				}
			}
		}
		else {
			auto LastSend = false;
			for (size_t Index = 0;Index < ConfigurationParameter.Statistics_Send;++Index)
			{
				++ConfigurationParameter.Statistics_RealSend;
				if (Index == ConfigurationParameter.Statistics_Send - 1U)
					LastSend = true;
				if (SendProcess(ConfigurationParameter.SockAddr_Normal, LastSend) == EXIT_FAILURE)
				{
				//Close file handle.
					if (ConfigurationParameter.OutputFile != nullptr)
						fclose(ConfigurationParameter.OutputFile);

					return EXIT_FAILURE;
				}
			}
		}

	//Print to screen before finished and close file handle.
		PrintProcess(true, true);
		if (ConfigurationParameter.OutputFile != nullptr)
			fclose(ConfigurationParameter.OutputFile);
	}

	return EXIT_SUCCESS;
}

//Configuration initialization process
size_t __fastcall ConfigurationInitialization(
	void)
{
//Initialization
#if defined(PLATFORM_WIN)
	memset(&ConfigurationParameter, 0, sizeof(ConfigurationTable) - (sizeof(std::string) * 5U + sizeof(std::wstring) * 3U + sizeof(std::shared_ptr<char>)));
#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
	memset(&ConfigurationParameter, 0, sizeof(ConfigurationTable) - (sizeof(std::string) * 6U + sizeof(std::wstring) * 3U + sizeof(std::shared_ptr<char>)));
#endif

	ConfigurationParameter.Statistics_Send = DEFAULT_SEND_TIMES;
	ConfigurationParameter.BufferSize = PACKET_MAXSIZE;
	ConfigurationParameter.Validate = true;
#if defined(PLATFORM_WIN)
	ConfigurationParameter.SocketTimeout = DEFAULT_TIME_OUT;
#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
	ConfigurationParameter.SocketTimeout.tv_sec = DEFAULT_TIME_OUT;
#endif

#if defined(PLATFORM_WIN)
//Handle the system signal.
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE) == false)
	{
		fwprintf_s(stderr, L"\nSet console ctrl handler error, error code is %lu.\n", GetLastError());
		return EXIT_FAILURE;
	}

//Winsock initialization
	WSAData WSAInitialization = {0};
	if (WSAStartup(MAKEWORD(2, 2), &WSAInitialization) != 0 || LOBYTE(WSAInitialization.wVersion) != 2 || HIBYTE(WSAInitialization.wVersion) != 2)
	{
		fwprintf_s(stderr, L"\nWinsock initialization error, error code is %d.\n", WSAGetLastError());

		WSACleanup();
		return EXIT_FAILURE;
	}
#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
//Handle the system signal.
	if (signal(SIGHUP, SIG_Handler) == SIG_ERR || signal(SIGINT, SIG_Handler) == SIG_ERR || signal(SIGQUIT, SIG_Handler) == SIG_ERR || signal(SIGTERM, SIG_Handler) == SIG_ERR)
	{
		fwprintf(stderr, L"Handle the system signal error, error code is %d.\n", errno);
		return EXIT_FAILURE;
	}
#endif

	return EXIT_SUCCESS;
}

//Read commands
#if defined(PLATFORM_WIN)
size_t __fastcall ReadCommands(
	int argc, 
	wchar_t* argv[])
#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
size_t __fastcall ReadCommands(
	int argc, 
	char *argv[])
#endif
{
//Initialization
	std::wstring Parameter;
	SSIZE_T Result = 0;

//Read parameter
	for (size_t Index = 1U;Index < (size_t)argc;++Index)
	{
	#if defined(PLATFORM_WIN)
		Parameter = argv[Index];
	#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
		MBSToWCSString(Parameter, argv[Index]);
	#endif
		Result = 0;

	//Description(Usage)
		if (Parameter.find(L"?") != std::string::npos || Parameter == L"-h" || Parameter == L"--help")
		{
			PrintDescription();
		}
	//Pings the specified host until stopped. To see statistics and continue type Control-Break. To stop type Control-C.
		else if (Parameter == L"-t" || Parameter == L"--until-break")
		{
			ConfigurationParameter.Statistics_Send = 0;
		}
	//Resolve addresses to host names.
		else if (Parameter == L"-a" || Parameter == L"--reverse-lookup")
		{
			ConfigurationParameter.ReverseLookup = true;
		}
	//Set number of echo requests to send.
		else if (Parameter == L"-n" || Parameter == L"--number")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

				Result = wcstoul(Parameter.c_str(), nullptr, 0);
				if (Result > 0 && Result <= UINT16_MAX)
				{
					ConfigurationParameter.Statistics_Send = Result;
				}
				else {
					fwprintf_s(stderr, L"\nParameter [-n count] error.\n");
					return EXIT_FAILURE;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Set the "Don't Fragment" flag in outgoing packets.
	//All Non-SOCK_STREAM will set "Don't Fragment" flag(Linux).
	#if defined(PLATFORM_WIN)
		else if (Parameter == L"-f" || Parameter == L"--do-not-fragment")
		{
			ConfigurationParameter.IPv4_DF = true;
		}
	#endif
	//Specifie a Time To Live for outgoing packets.
		else if (Parameter == L"-i" || Parameter == L"--hop-limits")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

				Result = wcstoul(Parameter.c_str(), nullptr, 0);
				if (Result > 0 && Result <= UINT8_MAX)
				{
					ConfigurationParameter.IP_HopLimits = (int)Result;
				}
				else {
					fwprintf_s(stderr, L"\nParameter [-i hoplimit/ttl] error.\n");
					return EXIT_FAILURE;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Set a long wait periods (in milliseconds) for a response.
		else if (Parameter == L"-w" || Parameter == L"--waiting-time")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

				Result = wcstoul(Parameter.c_str(), nullptr, 0);
				if (Result >= TIME_OUT_MIN && Result < UINT16_MAX)
				{
				#if defined(PLATFORM_WIN)
					ConfigurationParameter.SocketTimeout = (int)Result;
				#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
					ConfigurationParameter.SocketTimeout.tv_sec = (time_t)(Result / SECOND_TO_MILLISECOND);
					ConfigurationParameter.SocketTimeout.tv_usec = (suseconds_t)(Result % MICROSECOND_TO_MILLISECOND * MICROSECOND_TO_MILLISECOND);
				#endif
				}
				else {
					fwprintf_s(stderr, L"\nParameter [-w timeout] error.\n");
					return EXIT_FAILURE;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Specifie DNS header ID.
		else if (Parameter == L"-id" || Parameter == L"--dns-id")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

				Result = wcstoul(Parameter.c_str(), nullptr, 0);
				if (Result > 0 && Result <= UINT16_MAX)
				{
					ConfigurationParameter.Parameter_Header.ID = htons((uint16_t)Result);
				}
				else {
					fwprintf_s(stderr, L"\nParameter [-id dns_id] error.\n");
					return EXIT_FAILURE;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Set DNS header flag: QR
		else if (Parameter == L"-qr" || Parameter == L"--flags-qr")
		{
			ConfigurationParameter.Parameter_Header.FlagsBits.QR = ~ConfigurationParameter.Parameter_Header.FlagsBits.QR;
		}
	//Specifie DNS header OPCode.
		else if (Parameter == L"-opcode" || Parameter == L"--flags-opcode")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

				Result = wcstoul(Parameter.c_str(), nullptr, 0);
				if (Result > 0 && Result <= UINT4_MAX)
				{
				#if __BYTE_ORDER == __LITTLE_ENDIAN
					auto TempFlags = (uint16_t)Result;
					TempFlags = htons(TempFlags << 11U);
					ConfigurationParameter.Parameter_Header.Flags = ConfigurationParameter.Parameter_Header.Flags | TempFlags;
				#else //Big-Endian
					auto TempFlags = (uint8_t)Result;
					TempFlags = TempFlags & 15;//0x00001111
					ConfigurationParameter.Parameter_Header.FlagsBits.OPCode = TempFlags;
				#endif
				}
				else {
					fwprintf_s(stderr, L"\nParameter [-opcode opcode] error.\n");
					return EXIT_FAILURE;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Set DNS header flag: AA
		else if (Parameter == L"-aa" || Parameter == L"--flags-aa")
		{
			ConfigurationParameter.Parameter_Header.FlagsBits.AA = ~ConfigurationParameter.Parameter_Header.FlagsBits.AA;
		}
	//Set DNS header flag: TC
		else if (Parameter == L"-tc" || Parameter == L"--flags-tc")
		{
			ConfigurationParameter.Parameter_Header.FlagsBits.TC = ~ConfigurationParameter.Parameter_Header.FlagsBits.TC;
		}
	//Set DNS header flag: RD
		else if (Parameter == L"-rd" || Parameter == L"--flags-rd")
		{
			ConfigurationParameter.Parameter_Header.FlagsBits.RD = ~ConfigurationParameter.Parameter_Header.FlagsBits.RD;
		}
	//Set DNS header flag: RA
		else if (Parameter == L"-ra" || Parameter == L"--flags-ra")
		{
			ConfigurationParameter.Parameter_Header.FlagsBits.RA = ~ConfigurationParameter.Parameter_Header.FlagsBits.RA;
		}
	//Set DNS header flag: AD
		else if (Parameter == L"-ad" || Parameter == L"--flags-ad")
		{
			ConfigurationParameter.Parameter_Header.FlagsBits.AD = ~ConfigurationParameter.Parameter_Header.FlagsBits.AD;
		}
	//Set DNS header flag: CD
		else if (Parameter == L"-cd" || Parameter == L"--flags-cd")
		{
			ConfigurationParameter.Parameter_Header.FlagsBits.CD = ~ConfigurationParameter.Parameter_Header.FlagsBits.CD;
		}
	//Specifie DNS header RCode.
		else if (Parameter == L"-rcode" || Parameter == L"--flags-rcode")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

				Result = wcstoul(Parameter.c_str(), nullptr, 0);
				if (Result > 0 && Result <= UINT4_MAX)
				{
				#if __BYTE_ORDER == __LITTLE_ENDIAN
					auto TempFlags = (uint16_t)Result;
					TempFlags = htons(TempFlags);
					ConfigurationParameter.Parameter_Header.Flags = ConfigurationParameter.Parameter_Header.Flags | TempFlags;
				#else //Big-Endian
					auto TempFlags = (uint8_t)Result;
					TempFlags = TempFlags & 15; //0x00001111
					ConfigurationParameter.Parameter_Header.FlagsBits.RCode = TempFlags;
				#endif
				}
				else {
					fwprintf_s(stderr, L"\nParameter [-rcode rcode] error.\n");
					return EXIT_FAILURE;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Specifie DNS header question count.
		else if (Parameter == L"-qn" || Parameter == L"--dns-qn")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

				Result = wcstoul(Parameter.c_str(), nullptr, 0);
				if (Result > 0 && Result <= UINT16_MAX)
				{
					ConfigurationParameter.Parameter_Header.Questions = htons((uint16_t)Result);
				}
				else {
					fwprintf_s(stderr, L"\nParameter [-qn count] error.\n");
					return EXIT_FAILURE;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Specifie DNS header Answer count.
		else if (Parameter == L"-ann" || Parameter == L"--dns-ann")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

				Result = wcstoul(Parameter.c_str(), nullptr, 0);
				if (Result > 0 && Result <= UINT16_MAX)
				{
					ConfigurationParameter.Parameter_Header.Answer = htons((uint16_t)Result);
				}
				else {
					fwprintf_s(stderr, L"\nParameter [-ann count] error.\n");
					return EXIT_FAILURE;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Specifie DNS header Authority count.
		else if (Parameter == L"-aun" || Parameter == L"--dns-aun")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

				Result = wcstoul(Parameter.c_str(), nullptr, 0);
				if (Result > 0 && Result <= UINT16_MAX)
				{
					ConfigurationParameter.Parameter_Header.Authority = htons((uint16_t)Result);
				}
				else {
					fwprintf_s(stderr, L"\nParameter [-aun count] error.\n");
					return EXIT_FAILURE;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Specifie DNS header Additional count.
		else if (Parameter == L"-adn" || Parameter == L"--dns-adn")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

				Result = wcstoul(Parameter.c_str(), nullptr, 0);
				if (Result > 0 && Result <= UINT16_MAX)
				{
					ConfigurationParameter.Parameter_Header.Additional = htons((uint16_t)Result);
				}
				else {
					fwprintf_s(stderr, L"\nParameter [-adn count] error.\n");
					return EXIT_FAILURE;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Specifie transmission interval time(in milliseconds).
		else if (Parameter == L"-ti" || Parameter == L"--transmission-interval")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

				Result = wcstoul(Parameter.c_str(), nullptr, 0);
				if (Result >= 0)
				{
				#if defined(PLATFORM_WIN)
					ConfigurationParameter.TransmissionInterval = Result;
				#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
					ConfigurationParameter.TransmissionInterval = Result * MICROSECOND_TO_MILLISECOND;
				#endif
				}
				else {
					fwprintf_s(stderr, L"\nParameter [-ti interval_time] error.\n");
					return EXIT_FAILURE;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Send with EDNS Label.
		else if (Parameter == L"-edns")
		{
			ConfigurationParameter.EDNS = true;
		}
	//Specifie EDNS Label UDP Payload length.
		else if (Parameter == L"-payload")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

				Result = wcstoul(Parameter.c_str(), nullptr, 0);
				if (Result > OLD_DNS_MAXSIZE && Result <= UINT16_MAX)
				{
					ConfigurationParameter.EDNSPayloadSize = Result;
				}
				else {
					fwprintf_s(stderr, L"\nParameter [-payload length] error.\n");
					return EXIT_FAILURE;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}

			ConfigurationParameter.EDNS = true;
		}
	//Send with DNSSEC requesting.
		else if (Parameter == L"-dnssec")
		{
			ConfigurationParameter.EDNS = true;
			ConfigurationParameter.DNSSEC = true;
		}
	//Specifie Query Type.
		else if (Parameter == L"-qt" || Parameter == L"--query-type")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

			//Type name
				Result = DNSTypeNameToHex(Parameter);
				if (Result == 0)
				{
			//Type number
					Result = wcstoul(Parameter.c_str(), nullptr, 0);
					if (Result > 0 && Result <= UINT16_MAX)
					{
						ConfigurationParameter.Parameter_Query.Type = htons((uint16_t)Result);
					}
					else {
						fwprintf_s(stderr, L"\nParameter [-qt type] error.\n");
						return EXIT_FAILURE;
					}
				}
				else {
					ConfigurationParameter.Parameter_Query.Type = (uint16_t)Result;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Specifie Query Classes.
		else if (Parameter == L"-qc" || Parameter == L"--query-classes")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

			//Classes name
				Result = DNSClassesNameToHex(Parameter);
				if (Result == 0)
				{
			//Classes number
					Result = wcstoul(Parameter.c_str(), nullptr, 0);
					if (Result > 0 && Result <= UINT16_MAX)
					{
						ConfigurationParameter.Parameter_Query.Classes = htons((uint16_t)Result);
					}
					else {
						fwprintf_s(stderr, L"\nParameter [-qc classes] error.\n");
						return EXIT_FAILURE;
					}
				}
				else {
					ConfigurationParameter.Parameter_Query.Classes = (uint16_t)Result;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Specifie requesting server name or port.
		else if (Parameter == L"-p" || Parameter == L"--port")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

			//Server name
				Result = ServiceNameToPort(Parameter);
				if (Result == 0)
				{
				//Number port
					Result = wcstoul(Parameter.c_str(), nullptr, 0);
					if (Result > 0 && Result <= UINT16_MAX)
					{
						ConfigurationParameter.ServiceType = htons((uint16_t)Result);
					}
					else {
						fwprintf_s(stderr, L"\nParameter [-p service_type/protocol] error.\n");
						return EXIT_FAILURE;
					}
				}
				else {
					ConfigurationParameter.ServiceType = (uint16_t)Result;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Specifie Raw data to send.
		else if (Parameter == L"-rawdata")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

			//Initialization
				std::shared_ptr<char> RawDataStringPTR(new char[Parameter.length() + 1U]());
				memset(RawDataStringPTR.get(), 0, Parameter.length() + 1U);
			#if defined(PLATFORM_WIN)
				WideCharToMultiByte(CP_ACP, 0, Parameter.c_str(), (int)Parameter.length(), RawDataStringPTR.get(), (int)Parameter.length() + 1U, nullptr, nullptr);
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				wcstombs(RawDataStringPTR.get(), Parameter.c_str(), Parameter.length());
			#endif
				std::string RawDataString(RawDataStringPTR.get());
				RawDataStringPTR.reset();

				if (RawDataString.length() < PACKET_MINSIZE && RawDataString.length() > PACKET_MAXSIZE)
				{
					fwprintf_s(stderr, L"\nParameter [-rawdata raw_data] error.\n");
					return EXIT_FAILURE;
				}
				std::shared_ptr<char> TempRawData(new char[PACKET_MAXSIZE]());
				memset(TempRawData.get(), 0, PACKET_MAXSIZE);
				ConfigurationParameter.RawData.swap(TempRawData);
				TempRawData.reset();
				char TempString[5U] = {0};
				TempString[0] = ASCII_ZERO;
				TempString[1U] = 120; //"x"

			//Read raw data.
				for (size_t InnerIndex = 0;InnerIndex < RawDataString.length();++InnerIndex)
				{
					TempString[2U] = RawDataString[InnerIndex];
					++InnerIndex;
					TempString[3U] = RawDataString[InnerIndex];
					Result = (SSIZE_T)strtoul(TempString, nullptr, 0);
					if (Result > 0 && Result <= UINT8_MAX)
					{
						ConfigurationParameter.RawData.get()[ConfigurationParameter.RawDataLen] = (char)Result;
						++ConfigurationParameter.RawDataLen;
					}
					else {
						fwprintf_s(stderr, L"\nParameter [-rawdata raw_data] error.\n");
						return EXIT_FAILURE;
					}
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Send RAW data with Raw Socket.
		else if (Parameter == L"-raw")
		{
			if (Index + 1U < (size_t)argc)
			{
				ConfigurationParameter.RawSocket = true;
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

			//Protocol name
				Result = InternetProtocolNameToPort(Parameter);
				if (Result == 0)
				{
			//Protocol number
					Result = wcstoul(Parameter.c_str(), nullptr, 0);
					if (Result == IPPROTO_UDP)
					{
						ConfigurationParameter.RawSocket = false;
					}
					else if (Result > 0 && Result <= UINT4_MAX)
					{
						ConfigurationParameter.ServiceType = (uint8_t)Result;
					}
					else {
						fwprintf_s(stderr, L"\nParameter [-raw service_type] error.\n");
						return EXIT_FAILURE;
					}
				}
				else if (Result == IPPROTO_UDP)
				{
					ConfigurationParameter.RawSocket = false;
				}
				else {
					ConfigurationParameter.ServiceType = (uint8_t)Result;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Specifie SOCKS target.
		else if (Parameter == L"-socks")
		{
			if (Index + 1U < (size_t)argc)
			{
			//Initialization
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif
				std::shared_ptr<char> ParameterPTR(new char[Parameter.length() + 1U]());
				memset(ParameterPTR.get(), 0, Parameter.length() + 1U);
			#if defined(PLATFORM_WIN)
				WideCharToMultiByte(CP_ACP, 0, Parameter.c_str(), (int)Parameter.length(), ParameterPTR.get(), (int)(Parameter.length() + 1U), nullptr, nullptr);
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				wcstombs(ParameterPTR.get(), Parameter.c_str(), Parameter.length());
			#endif
				std::string ParameterString(ParameterPTR.get());
				ConfigurationParameter.TargetString_SOCKS = Parameter;
				ParameterPTR.reset();

			//IPv6 address
				if (ParameterString.find(ASCII_COLON) == std::string::npos)
				{
					fwprintf_s(stderr, L"\nParameter [-socks target] error.\n");
					return EXIT_FAILURE;
				}
				else if (ParameterString.find(ASCII_BRACKETS_LEFT) != std::string::npos || ParameterString.find(ASCII_BRACKETS_RIGHT) != std::string::npos)
				{
				//Check parameter.
					if (Parameter.length() < 6U || Parameter.length() > 47U || //IPv6 format
						ParameterString.find("]:") == std::string::npos || ParameterString.find(ASCII_BRACKETS_RIGHT) != ParameterString.find("]:"))
					{
						fwprintf_s(stderr, L"\nTarget length error.\n");
						return EXIT_FAILURE;
					}
					else {
						ConfigurationParameter.SockAddr_SOCKS.ss_family = AF_INET6;
					}

				//Mark port.
					if (Parameter.length() > Parameter.find(L"]:"))
					{
						std::wstring Port;
						Port.append(Parameter, Parameter.find(L"]:") + 2U, Parameter.length() - Parameter.find(L"]:"));

					//Server name
						Result = ServiceNameToPort(Port);
						if (Result == 0)
						{
						//Number port
							Result = wcstoul(Port.c_str(), nullptr, 0);
							if (Result > 0 && Result <= UINT16_MAX)
							{
								((sockaddr_in6 *)&ConfigurationParameter.SockAddr_SOCKS)->sin6_port = htons((uint16_t)Result);
							}
							else {
								fwprintf_s(stderr, L"\nParameter [-socks target] error.\n");
								return EXIT_FAILURE;
							}
						}
						else {
							((sockaddr_in6 *)&ConfigurationParameter.SockAddr_SOCKS)->sin6_port = htons((uint16_t)Result);
						}

						ParameterString.erase(0, 1U);
						ParameterString.erase(ParameterString.find("]:"), ParameterString.length() - ParameterString.find("]:"));
					}
					else {
						fwprintf_s(stderr, L"\nParameter [-socks target] error.\n");
						return EXIT_FAILURE;
					}

				//Mark address.
					if (AddressStringToBinary((char *)ParameterString.c_str(), &((PSOCKADDR_IN6)&ConfigurationParameter.SockAddr_SOCKS)->sin6_addr, AF_INET6, Result) == EXIT_FAILURE)
					{
						fwprintf_s(stderr, L"\nParameter [-socks target] error, error code is %d.\n", (int)Result);
						return EXIT_FAILURE;
					}
				}
				else {
				//Mark port.
					if (ParameterString.find(ASCII_COLON) != std::string::npos && Parameter.length() > Parameter.find(ASCII_COLON))
					{
						std::wstring Port;
						Port.append(Parameter, Parameter.find(ASCII_COLON) + 1U, Parameter.length() - Parameter.find(ASCII_COLON));

					//Server name
						Result = ServiceNameToPort(Port);
						if (Result == 0)
						{
						//Number port
							Result = wcstoul(Port.c_str(), nullptr, 0);
							if (Result <= 0 || Result > UINT16_MAX)
							{
								fwprintf_s(stderr, L"\nParameter [-socks target] error\n");
								return EXIT_FAILURE;
							}
						}

						ParameterString.erase(ParameterString.find(ASCII_COLON), ParameterString.length() - ParameterString.find(ASCII_COLON));
					}
					else {
						fwprintf_s(stderr, L"\nParameter [-socks target] error.\n");
						return EXIT_FAILURE;
					}

				//Mark target(Domain or IPv4 address)
					for (auto StringIter = ParameterString.begin();StringIter != ParameterString.end();++StringIter)
					{
					//Domain
						if (*StringIter < ASCII_PERIOD || *StringIter == ASCII_SLASH || *StringIter > ASCII_NINE)
						{
							if (Parameter.length() < DOMAIN_MINSIZE + 2U || Parameter.length() > DOMAIN_MAXSIZE + 6U)
							{
								fwprintf_s(stderr, L"\nParameter [-socks target] error.\n");
								return EXIT_FAILURE;
							}
							else {
							//Get address.
								ADDRINFOA AddrInfoHints = {0}, *AddrInfo = nullptr;
								AddrInfoHints.ai_family = ConfigurationParameter.Protocol;
								if (getaddrinfo(ParameterString.c_str(), nullptr, &AddrInfoHints, &AddrInfo) != 0)
								{
									fwprintf_s(stderr, L"\nResolve domain name error, error code is %d.\n", WSAGetLastError());
									return EXIT_FAILURE;
								}

							//Get address from result.
								if (AddrInfo != nullptr)
								{
									for (auto PTR = AddrInfo;PTR != nullptr;PTR = PTR->ai_next)
									{
									//IPv6
										if (PTR->ai_family == AF_INET6 && !IN6_IS_ADDR_LINKLOCAL((in6_addr *)(PTR->ai_addr)) && !(((PSOCKADDR_IN6)(PTR->ai_addr))->sin6_scope_id == 0)) //Get port from first(Main) IPv6 device
										{
											ConfigurationParameter.SockAddr_SOCKS.ss_family = AF_INET6;
											((PSOCKADDR_IN6)&ConfigurationParameter.SockAddr_SOCKS)->sin6_addr = ((PSOCKADDR_IN6)(PTR->ai_addr))->sin6_addr;
											((PSOCKADDR_IN6)&ConfigurationParameter.SockAddr_SOCKS)->sin6_port = htons((uint16_t)Result);

										//Get string of address.
											char Buffer[ADDR_STRING_MAXSIZE] = {0};

										//Minimum supported system of inet_ntop() and inet_pton() is Windows Vista. [Roy Tam]
										#if (defined(PLATFORM_WIN32) && !defined(PLATFORM_WIN64))
											DWORD BufferLength = ADDR_STRING_MAXSIZE;
											WSAAddressToStringA((PSOCKADDR)&ConfigurationParameter.SockAddr_SOCKS, sizeof(sockaddr_in6), nullptr, Buffer, &BufferLength);
										#else
											inet_ntop(AF_INET6, &((PSOCKADDR_IN6)&ConfigurationParameter.SockAddr_SOCKS)->sin6_addr, Buffer, ADDR_STRING_MAXSIZE);
										#endif

											break;
										}
									//IPv4
										else if (PTR->ai_family == AF_INET && ((PSOCKADDR_IN)(PTR->ai_addr))->sin_addr.s_addr != INADDR_LOOPBACK && ((PSOCKADDR_IN)(PTR->ai_addr))->sin_addr.s_addr != INADDR_BROADCAST)
										{
											ConfigurationParameter.SockAddr_SOCKS.ss_family = AF_INET;
											((PSOCKADDR_IN)&ConfigurationParameter.SockAddr_SOCKS)->sin_addr = ((PSOCKADDR_IN)(PTR->ai_addr))->sin_addr;
											((PSOCKADDR_IN)&ConfigurationParameter.SockAddr_SOCKS)->sin_port = htons((uint16_t)Result);

										//Get string of address.
											char Buffer[ADDR_STRING_MAXSIZE] = {0};

										//Minimum supported system of inet_ntop() and inet_pton() is Windows Vista. [Roy Tam]
										#if (defined(PLATFORM_WIN32) && !defined(PLATFORM_WIN64))
											DWORD BufferLength = ADDR_STRING_MAXSIZE;
											WSAAddressToStringA((PSOCKADDR)&ConfigurationParameter.SockAddr_SOCKS, sizeof(sockaddr_in), nullptr, Buffer, &BufferLength);
										#else
											inet_ntop(AF_INET, &((PSOCKADDR_IN)&ConfigurationParameter.SockAddr_SOCKS)->sin_addr, Buffer, ADDR_STRING_MAXSIZE);
										#endif

											break;
										}
									}

									freeaddrinfo(AddrInfo);
								}
								else {
									fwprintf_s(stderr, L"\nResolve domain name error, error code is %d.\n", WSAGetLastError());
									return EXIT_FAILURE;
								}
							}

							break;
						}

					//IPv4
						if (StringIter == ParameterString.end() - 1U)
						{
						//Check parameter.
							if (Parameter.length() < 9U || Parameter.length() > 21U) //IPv4 format
							{
								fwprintf_s(stderr, L"\nParameter [-socks target] error.\n");
								return EXIT_FAILURE;
							}
							else {
								((PSOCKADDR_IN)&ConfigurationParameter.SockAddr_SOCKS)->sin_port = htons((uint16_t)Result);
							}

						//Mark address.
							ConfigurationParameter.SockAddr_SOCKS.ss_family = AF_INET;
							if (AddressStringToBinary((char *)ParameterString.c_str(), &((PSOCKADDR_IN)&ConfigurationParameter.SockAddr_SOCKS)->sin_addr, AF_INET, Result) == EXIT_FAILURE)
							{
								fwprintf_s(stderr, L"\nParameter [-socks target] error, error code is %d.\n", (int)Result);
								return EXIT_FAILURE;
							}
						}
					}
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Specifie SOCKS username.
		else if (Parameter == L"-socks_username")
		{
			if (Index + 1U < (size_t)argc)
			{
			//Initialization
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif
				std::shared_ptr<char> ParameterPTR(new char[Parameter.length() + 1U]());
				memset(ParameterPTR.get(), 0, Parameter.length() + 1U);
			#if defined(PLATFORM_WIN)
				WideCharToMultiByte(CP_ACP, 0, Parameter.c_str(), (int)Parameter.length(), ParameterPTR.get(), (int)(Parameter.length() + 1U), nullptr, nullptr);
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				wcstombs(ParameterPTR.get(), Parameter.c_str(), Parameter.length());
			#endif
				ConfigurationParameter.SOCKS_Username = ParameterPTR.get();
				ParameterPTR.reset();

			//SOCKS username check
				if (ConfigurationParameter.SOCKS_Username.length() > SOCKS_USERNAME_PASSWORD_MAXNUM)
				{
					fwprintf_s(stderr, L"\nParameter [-socks_username] error.\n");
					return EXIT_FAILURE;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Specifie SOCKS password.
		else if (Parameter == L"-socks_password")
		{
			if (Index + 1U < (size_t)argc)
			{
			//Initialization
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif
				std::shared_ptr<char> ParameterPTR(new char[Parameter.length() + 1U]());
				memset(ParameterPTR.get(), 0, Parameter.length() + 1U);
			#if defined(PLATFORM_WIN)
				WideCharToMultiByte(CP_ACP, 0, Parameter.c_str(), (int)Parameter.length(), ParameterPTR.get(), (int)(Parameter.length() + 1U), nullptr, nullptr);
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				wcstombs(ParameterPTR.get(), Parameter.c_str(), Parameter.length());
			#endif
				ConfigurationParameter.SOCKS_Password = ParameterPTR.get();
				ParameterPTR.reset();

			//SOCKS password check
				if (ConfigurationParameter.SOCKS_Password.length() > SOCKS_USERNAME_PASSWORD_MAXNUM)
				{
					fwprintf_s(stderr, L"\nParameter [-socks_password] error.\n");
					return EXIT_FAILURE;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Specifie buffer size.
		else if (Parameter == L"-buf" || Parameter == L"--buffer-size")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

				Result = wcstoul(Parameter.c_str(), nullptr, 0);
				if (Result >= OLD_DNS_MAXSIZE && Result <= LARGE_PACKET_MAXSIZE)
				{
					ConfigurationParameter.BufferSize = Result;
				}
				else {
					fwprintf_s(stderr, L"\nParameter [-buf size] error.\n");
					return EXIT_FAILURE;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Disable packets validated.
		else if (Parameter == L"-dv" || Parameter == L"--disable-validate")
		{
			ConfigurationParameter.Validate = false;
		}
	//Show response.
		else if (Parameter == L"-show" || Parameter == L"--show-response")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

				if (Parameter == L"result")
				{
					ConfigurationParameter.ShowResponse = true;
				}
				else if (Parameter == L"hex" || Parameter == L"hexadecimal")
				{
					ConfigurationParameter.ShowResponseHex = true;
				}
				else {
					fwprintf_s(stderr, L"\nParameter [-show type] error.\n");
					return EXIT_FAILURE;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Output result to file.
		else if (Parameter == L"-of" || Parameter == L"--output-file")
		{
			if (Index + 1U < (size_t)argc)
			{
				++Index;
			#if defined(PLATFORM_WIN)
				Parameter = argv[Index];
			#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
				MBSToWCSString(Parameter, argv[Index]);
			#endif

				if (Parameter.length() <= MAX_PATH)
				{
					ConfigurationParameter.wOutputFileName = Parameter;

				#if (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
					std::shared_ptr<char> OutputFileNamePTR(new char[Parameter.length() + 1U]);
					memset(OutputFileNamePTR.get(), 0, Parameter.length() + 1U);
					wcstombs(OutputFileNamePTR.get(), Parameter.c_str(), Parameter.length());
					ConfigurationParameter.OutputFileName = OutputFileNamePTR.get();
				#endif
				}
				else {
					fwprintf_s(stderr, L"\nParameter [-of file_name] error.\n");
					return EXIT_FAILURE;
				}
			}
			else {
				fwprintf_s(stderr, L"\nNot enough parameters error.\n");
				return EXIT_FAILURE;
			}
		}
	//Using IPv6.
		else if (Parameter == L"-6" || Parameter == L"--ipv6")
		{
			ConfigurationParameter.Protocol = AF_INET6;
		}
	//Using IPv4.
		else if (Parameter == L"-4" || Parameter == L"--ipv4")
		{
			ConfigurationParameter.Protocol = AF_INET;
		}
	//Specifie Query Domain.
		else if (!ConfigurationParameter.RawData && ConfigurationParameter.TestDomain.empty() && Index == (size_t)(argc - 2))
		{
		//Check parameter.
			if (Parameter.length() <= DOMAIN_MINSIZE || Parameter.length() > DOMAIN_MAXSIZE)
			{
				fwprintf_s(stderr, L"\nTest domain length error.\n");
				return EXIT_FAILURE;
			}

		//Initialization
			std::shared_ptr<char> TestDomainPTR(new char[Parameter.length() + 1U]());
			memset(TestDomainPTR.get(), 0, Parameter.length() + 1U);
		#if defined(PLATFORM_WIN)
			WideCharToMultiByte(CP_ACP, 0, Parameter.c_str(), (int)Parameter.length(), TestDomainPTR.get(), (int)(Parameter.length() + 1U), nullptr, nullptr);
		#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
			wcstombs(TestDomainPTR.get(), Parameter.c_str(), Parameter.length());
		#endif

			ConfigurationParameter.TestDomain = TestDomainPTR.get();
		}
	//Specifie target.
		else if (Index == (size_t)(argc - 1))
		{
		//Check parameter.
			if (Parameter.length() < DOMAIN_MINSIZE || Parameter.length() > DOMAIN_MAXSIZE)
			{
				fwprintf_s(stderr, L"\nTarget length error.\n");
				return EXIT_FAILURE;
			}

		//Initialization
			std::shared_ptr<char> ParameterPTR(new char[Parameter.length() + 1U]());
			memset(ParameterPTR.get(), 0, Parameter.length() + 1U);
		#if defined(PLATFORM_WIN)
			WideCharToMultiByte(CP_ACP, 0, Parameter.c_str(), (int)Parameter.length(), ParameterPTR.get(), (int)(Parameter.length() + 1U), nullptr, nullptr);
		#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
			wcstombs(ParameterPTR.get(), Parameter.c_str(), Parameter.length());
		#endif
			std::string ParameterString(ParameterPTR.get());
			ParameterPTR.reset();

		//IPv6
			if (ParameterString.find(ASCII_COLON) != std::string::npos)
			{
			//Check parameter.
				if (Parameter.length() < 2U || Parameter.length() > 40U) //IPv6 format
				{
					fwprintf_s(stderr, L"\nTarget length error.\n");
					return EXIT_FAILURE;
				}
				else if (ConfigurationParameter.Protocol == AF_INET)
				{
					fwprintf_s(stderr, L"\nTarget protocol error.\n");
					return EXIT_FAILURE;
				}

			//Mark address.
				ConfigurationParameter.Protocol = AF_INET6;
				ConfigurationParameter.SockAddr_Normal.ss_family = AF_INET6;
				if (AddressStringToBinary((char *)ParameterString.c_str(), &((PSOCKADDR_IN6)&ConfigurationParameter.SockAddr_Normal)->sin6_addr, AF_INET6, Result) == EXIT_FAILURE)
				{
					fwprintf_s(stderr, L"\nTarget format error, error code is %d.\n", (int)Result);
					return EXIT_FAILURE;
				}

				ConfigurationParameter.TargetString_Normal.append("[");
				ConfigurationParameter.TargetString_Normal.append(ParameterString);
				ConfigurationParameter.TargetString_Normal.append("]");
			}
			else {
				for (auto StringIter = ParameterString.begin();StringIter != ParameterString.end();++StringIter)
				{
				//Domain
					if (*StringIter < ASCII_PERIOD || *StringIter == ASCII_SLASH || *StringIter > ASCII_NINE)
					{
					//Check parameter.
						if (Parameter.length() <= DOMAIN_MINSIZE)
						{
							fwprintf_s(stderr, L"\nTarget length error.\n");
							return EXIT_FAILURE;
						}

					//SOCKS mode
						if (ConfigurationParameter.SockAddr_SOCKS.ss_family > 0)
						{
							ConfigurationParameter.TargetString_Normal.append(ParameterString);
						}
					//Normal mode
						else {
						//Get address.
							ADDRINFOA AddrInfoHints = {0}, *AddrInfo = nullptr;
							AddrInfoHints.ai_family = ConfigurationParameter.Protocol;
							if (getaddrinfo(ParameterString.c_str(), nullptr, &AddrInfoHints, &AddrInfo) != 0)
							{
								fwprintf_s(stderr, L"\nResolve domain name error, error code is %d.\n", WSAGetLastError());
								return EXIT_FAILURE;
							}

						//Get address from result.
							if (AddrInfo != nullptr)
							{
								for (auto PTR = AddrInfo;PTR != nullptr;PTR = PTR->ai_next)
								{
								//IPv6
									if (PTR->ai_family == AF_INET6 && !IN6_IS_ADDR_LINKLOCAL((in6_addr *)(PTR->ai_addr)) && !(((PSOCKADDR_IN6)(PTR->ai_addr))->sin6_scope_id == 0)) //Get port from first(Main) IPv6 device
									{
										ConfigurationParameter.Protocol = AF_INET6;
										ConfigurationParameter.SockAddr_Normal.ss_family = AF_INET6;
										((PSOCKADDR_IN6)&ConfigurationParameter.SockAddr_Normal)->sin6_addr = ((PSOCKADDR_IN6)(PTR->ai_addr))->sin6_addr;

									//Get string of address.
										ConfigurationParameter.TargetAddressString = ParameterString;
										char Buffer[ADDR_STRING_MAXSIZE] = {0};

									//Minimum supported system of inet_ntop() and inet_pton() is Windows Vista. [Roy Tam]
									#if (defined(PLATFORM_WIN32) && !defined(PLATFORM_WIN64))
										DWORD BufferLength = ADDR_STRING_MAXSIZE;
										WSAAddressToStringA((PSOCKADDR)&ConfigurationParameter.SockAddr_Normal, sizeof(sockaddr_in6), nullptr, Buffer, &BufferLength);
									#else
										inet_ntop(AF_INET6, &((PSOCKADDR_IN6)&ConfigurationParameter.SockAddr_Normal)->sin6_addr, Buffer, ADDR_STRING_MAXSIZE);
									#endif
										CaseConvert(true, Buffer, strlen(Buffer));

										ConfigurationParameter.TargetString_Normal.append("[");
										ConfigurationParameter.TargetString_Normal.append(Buffer);
										ConfigurationParameter.TargetString_Normal.append("]");
										break;
									}
								//IPv4
									else if (PTR->ai_family == AF_INET && ((PSOCKADDR_IN)(PTR->ai_addr))->sin_addr.s_addr != INADDR_LOOPBACK && ((PSOCKADDR_IN)(PTR->ai_addr))->sin_addr.s_addr != INADDR_BROADCAST)
									{
										ConfigurationParameter.Protocol = AF_INET;
										ConfigurationParameter.SockAddr_Normal.ss_family = AF_INET;
										((PSOCKADDR_IN)&ConfigurationParameter.SockAddr_Normal)->sin_addr = ((PSOCKADDR_IN)(PTR->ai_addr))->sin_addr;

									//Get string of address.
										ConfigurationParameter.TargetAddressString = ParameterString;
										char Buffer[ADDR_STRING_MAXSIZE] = {0};

									//Minimum supported system of inet_ntop() and inet_pton() is Windows Vista. [Roy Tam]
									#if (defined(PLATFORM_WIN32) && !defined(PLATFORM_WIN64))
										DWORD BufferLength = ADDR_STRING_MAXSIZE;
										WSAAddressToStringA((PSOCKADDR)&ConfigurationParameter.SockAddr_Normal, sizeof(sockaddr_in), nullptr, Buffer, &BufferLength);
									#else
										inet_ntop(AF_INET, &((PSOCKADDR_IN)&ConfigurationParameter.SockAddr_Normal)->sin_addr, Buffer, ADDR_STRING_MAXSIZE);
									#endif

										ConfigurationParameter.TargetString_Normal = Buffer;
										break;
									}
								}

								freeaddrinfo(AddrInfo);
							}
							else {
								fwprintf_s(stderr, L"\nResolve domain name error, error code is %d.\n", WSAGetLastError());
								return EXIT_FAILURE;
							}
						}

						break;
					}

				//IPv4
					if (StringIter == ParameterString.end() - 1U)
					{
					//Check parameter.
						if (Parameter.length() < 7U || Parameter.length() > 15U) //IPv4 format
						{
							fwprintf_s(stderr, L"\nTarget length error.\n");
							return EXIT_FAILURE;
						}
						else if (ConfigurationParameter.Protocol == AF_INET6)
						{
							fwprintf_s(stderr, L"\nTarget protocol error.\n");
							return EXIT_FAILURE;
						}

					//Mark address.
						ConfigurationParameter.Protocol = AF_INET;
						ConfigurationParameter.SockAddr_Normal.ss_family = AF_INET;
						if (AddressStringToBinary((char *)ParameterString.c_str(), &((PSOCKADDR_IN)&ConfigurationParameter.SockAddr_Normal)->sin_addr, AF_INET, Result) == EXIT_FAILURE)
						{
							fwprintf_s(stderr, L"\nTarget format error, error code is %d.\n", (int)Result);
							return EXIT_FAILURE;
						}

						ConfigurationParameter.TargetString_Normal = ParameterString;
					}
				}
			}
		}
	}

	return EXIT_SUCCESS;
}

//Output result to file
size_t __fastcall OutputResultToFile(
	void)
{
	SSIZE_T Result = 0;
#if defined(PLATFORM_WIN)
	Result = _wfopen_s(&ConfigurationParameter.OutputFile, ConfigurationParameter.wOutputFileName.c_str(), L"a,ccs=UTF-8");
#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
	ConfigurationParameter.OutputFile = fopen(ConfigurationParameter.OutputFileName.c_str(), ("a"));
#endif
	if (ConfigurationParameter.OutputFile == nullptr)
	{
		fwprintf_s(stderr, L"Create output result file %ls error, error code is %d.\n", ConfigurationParameter.wOutputFileName.c_str(), (int)Result);
		return EXIT_FAILURE;
	}
	else {
		tm TimeStructure = {0};
		time_t TimeValues = 0;
		time(&TimeValues);
		localtime_s(&TimeStructure, &TimeValues);

		fwprintf_s(ConfigurationParameter.OutputFile, L"------------------------------ %d-%02d-%02d %02d:%02d:%02d ------------------------------\n", TimeStructure.tm_year + 1900, TimeStructure.tm_mon + 1, TimeStructure.tm_mday, TimeStructure.tm_hour, TimeStructure.tm_min, TimeStructure.tm_sec);
	}

	return EXIT_SUCCESS;
}

//Print header to screen. 
void __fastcall PrintHeaderToScreen(
	const std::wstring wTargetAddressString, 
	const std::wstring wTestDomain)
{
	fwprintf_s(stderr, L"\n");

//Reverse lookup
	if (ConfigurationParameter.ReverseLookup && ConfigurationParameter.SockAddr_SOCKS.ss_family == 0)
	{
		if (wTargetAddressString.empty())
		{
			char FQDN[NI_MAXHOST + 1U] = {0};
			if (getnameinfo((PSOCKADDR)&ConfigurationParameter.SockAddr_Normal, sizeof(sockaddr_in), FQDN, NI_MAXHOST, nullptr, 0, NI_NUMERICSERV) != 0)
			{
				fwprintf_s(stderr, L"Resolve addresses to host names error, error code is %d.\n", WSAGetLastError());
				fwprintf_s(stderr, L"DNSPing %ls:%u with %ls:\n", ConfigurationParameter.wTargetString.c_str(), ntohs(ConfigurationParameter.ServiceType), wTestDomain.c_str());
				if (ConfigurationParameter.OutputFile != nullptr)
					fwprintf_s(ConfigurationParameter.OutputFile, L"DNSPing %ls:%u with %ls:\n", ConfigurationParameter.wTargetString.c_str(), ntohs(ConfigurationParameter.ServiceType), wTestDomain.c_str());
			}
			else {
				if (ConfigurationParameter.TargetString_Normal == FQDN)
				{
					fwprintf_s(stderr, L"DNSPing %ls:%u with %ls:\n", ConfigurationParameter.wTargetString.c_str(), ntohs(ConfigurationParameter.ServiceType), wTestDomain.c_str());
					if (ConfigurationParameter.OutputFile != nullptr)
						fwprintf_s(ConfigurationParameter.OutputFile, L"DNSPing %ls:%u with %ls:\n", ConfigurationParameter.wTargetString.c_str(), ntohs(ConfigurationParameter.ServiceType), wTestDomain.c_str());
				}
				else {
					std::shared_ptr<wchar_t> wFQDN(new wchar_t[strnlen(FQDN, NI_MAXHOST) + 1U]());
					wmemset(wFQDN.get(), 0, strnlen(FQDN, NI_MAXHOST) + 1U);

				#if defined(PLATFORM_WIN)
					MultiByteToWideChar(CP_ACP, 0, FQDN, MBSTOWCS_NULLTERMINATE, wFQDN.get(), (int)strlen(FQDN));
				#elif (defined(PLATFORM_LINUX) || defined(PLATFORM_MACX))
					mbstowcs(wFQDN.get(), FQDN, strlen(FQDN));
				#endif

					fwprintf_s(stderr, L"DNSPing %ls:%u [%ls] with %ls:\n", wFQDN.get(), ntohs(ConfigurationParameter.ServiceType), ConfigurationParameter.wTargetString.c_str(), wTestDomain.c_str());
					if (ConfigurationParameter.OutputFile != nullptr)
						fwprintf_s(ConfigurationParameter.OutputFile, L"DNSPing %ls:%u [%ls] with %ls:\n", wFQDN.get(), ntohs(ConfigurationParameter.ServiceType), ConfigurationParameter.wTargetString.c_str(), wTestDomain.c_str());
				}
			}
		}
		else {
			fwprintf_s(stderr, L"DNSPing %ls:%u [%ls] with %ls:\n", wTargetAddressString.c_str(), ntohs(ConfigurationParameter.ServiceType), ConfigurationParameter.wTargetString.c_str(), wTestDomain.c_str());
			if (ConfigurationParameter.OutputFile != nullptr)
				fwprintf_s(ConfigurationParameter.OutputFile, L"DNSPing %ls:%u [%ls] with %ls:\n", wTargetAddressString.c_str(), ntohs(ConfigurationParameter.ServiceType), ConfigurationParameter.wTargetString.c_str(), wTestDomain.c_str());
		}
	}
//Normal mode
	else {
		if (!ConfigurationParameter.TargetAddressString.empty())
		{
			fwprintf_s(stderr, L"DNSPing %ls:%u [%ls] with %ls:\n", wTargetAddressString.c_str(), ntohs(ConfigurationParameter.ServiceType), ConfigurationParameter.wTargetString.c_str(), wTestDomain.c_str());
			if (ConfigurationParameter.OutputFile != nullptr)
				fwprintf_s(ConfigurationParameter.OutputFile, L"DNSPing %ls:%u [%ls] with %ls:\n", wTargetAddressString.c_str(), ntohs(ConfigurationParameter.ServiceType), ConfigurationParameter.wTargetString.c_str(), wTestDomain.c_str());
		}
		else {
			fwprintf_s(stderr, L"DNSPing %ls:%u with %ls:\n", ConfigurationParameter.wTargetString.c_str(), ntohs(ConfigurationParameter.ServiceType), wTestDomain.c_str());
			if (ConfigurationParameter.OutputFile != nullptr)
				fwprintf_s(ConfigurationParameter.OutputFile, L"DNSPing %ls:%u with %ls:\n", ConfigurationParameter.wTargetString.c_str(), ntohs(ConfigurationParameter.ServiceType), wTestDomain.c_str());
		}
	}

	return;
}
