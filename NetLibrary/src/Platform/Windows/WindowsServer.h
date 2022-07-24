#pragma once

#include "Interface/NetAPI.h"

#include <iostream>
#include <set>
#include <string>
#include <algorithm>
#include <mutex>

class WindowsServer : public Server
{
public:
	WindowsServer(const AddrAndPort& addrAndPort);
	~WindowsServer();

	void Run() override;
public:
	static inline std::set<SOCKET> s_ConnectedClients;
	static inline std::mutex s_Mutex;
private:
	void Socket() override;
	void Bind() override;
	void Listen() override;
private:
	void InitWSADATA();
private:
	SOCKET m_ListenSocket = INVALID_SOCKET;

	AddrAndPort m_AddrAndPort;
};