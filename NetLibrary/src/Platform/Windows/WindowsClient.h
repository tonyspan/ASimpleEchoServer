#pragma once

#include "Interface/NetAPI.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <mutex>

class WindowsClient : public Client
{
public:
	WindowsClient();
	~WindowsClient();

	virtual void Socket() override;
	virtual bool Connect(const AddrAndPort& serverAddrAndPort) override;
	virtual void Send(const std::string& msg) override;
	virtual void Receive() override;
	virtual void Shutdown() override;

	void SetAvailableServers(const std::vector<AddrAndPort>& servers);
private:
	void InitWSADATA();

	void SendMessageWithHeader(MessageHeader* payload);
	void sendRawData(char* data, uint32_t len);

	void ReConnect();
private:
	SOCKET m_Socket = INVALID_SOCKET;

	AddrAndPort m_ServerAddrAndPort;
	std::vector<AddrAndPort> m_AvailableServers;

	std::mutex m_Mutex;
};