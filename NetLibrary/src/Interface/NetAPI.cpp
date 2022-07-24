#include "NetAPI.h"

#include "Utils/Log.h"

#ifdef PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsClient.h"
	#include "Platform/Windows/WindowsServer.h"
#else
	#include "Platform/Linux/LinuxClient.h"
	#include "Platform/Linux/LinuxServer.h"
#endif

#pragma region Client
	std::unique_ptr<Client> Client::Create()
	{
	#ifdef PLATFORM_WINDOWS
		return std::make_unique<WindowsClient>();
	#else
		return std::make_unique<LinuxClient>();
	#endif
	}

	std::vector<AddrAndPort> Client::ParseCommandLineArgs(int argc, char* argv[])
	{
		std::vector<AddrAndPort> vec;

		for (int i = 1; i < argc; i++)
		{
			char address[256] = { '\0' };
			uint32_t port;

			if (SCANF(argv[i], "%[^:]:%d", address, &port) != 2)
			{
				Logger::Log("Usage: <executable> ip:port");
				std::exit(1);
			}

			vec.emplace_back(AddrAndPort{ std::string(address), port });
		}

		return vec;
	}
#pragma endregion

#pragma region Server
	std::unique_ptr<Server> Server::Create(const AddrAndPort& serverAddrAndPort)
	{
	#ifdef PLATFORM_WINDOWS
		return std::make_unique<WindowsServer>(serverAddrAndPort);
	#else
		return std::make_unique<LinuxServer>(serverAddrAndPort);
	#endif
	}

	AddrAndPort Server::ParseCommandLineArgs(int argc, char* argv[])
	{
		for (int i = 1; i < argc; i++)
		{
			char address[256] = { '\0' };
			uint32_t port;

			if (SCANF(argv[i], "%[^:]:%d", address, &port) != 2)
			{
				Logger::Log("Usage: <executable> ip:port");
				std::exit(1);
			}

			return AddrAndPort{ std::string(address), port };
		}

		return AddrAndPort();
	}
#pragma endregion

in_addr StringToAddr(const std::string& addr)
{
	auto record = gethostbyname(addr.c_str());

	if (record == NULL)
	{
		Logger::Log("Address", addr.c_str(), "is unavailable");
		return in_addr();
	}

	return *((in_addr*)record->h_addr);
}