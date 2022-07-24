#pragma once

#include "Interface/NetAPI.h"

#include <iostream>
#include <set>
#include <string>
#include <algorithm>
#include <thread>
#include <mutex>

class LinuxServer : public Server
{
public:
	LinuxServer(const AddrAndPort& addrAndPort);
	~LinuxServer() = default;

	void Run() override;
public:
	static inline std::set<int> s_ConnectedClients;
	static inline std::mutex s_Mutex;
private:
	void Socket() override;
	void Bind() override;
	void Listen() override;
private:
	int m_ListenSocket = INVALID_SOCKET;

	AddrAndPort m_AddrAndPort;
};