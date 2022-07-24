#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Utils/PlatformDetection.h"

#ifdef PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#define SCANF(str, format,inAddr, inPort) sscanf_s(str, format, inAddr, (int)sizeof(inAddr), inPort)
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_ERROR -1
#define INVALID_SOCKET -1

#define SCANF(str, format, inAddr, inPort) sscanf(str, format, inAddr, inPort)
#endif

constexpr int MaxBufLen = 512;

// For Debugging
constexpr int SizeOfHeaderBytes = 4;

struct MessageHeader
{
	uint32_t MessageLength = 0; // Message len + sizeof(MessageHeader)
};

struct AddrAndPort
{
	std::string Address;
	uint32_t Port;

	bool operator==(const AddrAndPort& other)
	{
		return Address == other.Address && Port == other.Port;
	}

	std::string ToString() const
	{
		return Address + ":" + std::to_string(Port);
	}
};

class Client
{
public:
	virtual ~Client() = default;

	virtual void Socket() = 0;
	virtual bool Connect(const AddrAndPort& serverAddrAndPort) = 0;
	virtual void Send(const std::string& msg) = 0;
	virtual void Receive() = 0;
	virtual void Shutdown() = 0;

	virtual void SetAvailableServers(const std::vector<AddrAndPort>& availableServers) = 0;

	static std::unique_ptr<Client> Create();
	static std::vector<AddrAndPort> ParseCommandLineArgs(int argc, char* argv[]);
};

class Server
{
public:
	virtual ~Server() = default;

	virtual void Run() = 0;

	static std::unique_ptr<Server> Create(const AddrAndPort& serverAddrAndPort);
	static AddrAndPort ParseCommandLineArgs(int argc, char* argv[]);
protected:
	virtual void Socket() = 0;
	virtual void Bind() = 0;
	virtual void Listen() = 0;
};

// Helper function
in_addr StringToAddr(const std::string& addr);