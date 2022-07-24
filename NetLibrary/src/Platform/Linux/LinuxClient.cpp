#include "LinuxClient.h"

#include "Utils/Log.h"

LinuxClient::LinuxClient()
{
	Socket();
}

LinuxClient::~LinuxClient()
{
	if (m_Socket != INVALID_SOCKET)
	{
		Logger::Log("Disconnecting");
		Shutdown();
	}
}

void LinuxClient::Socket()
{
	m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Socket == INVALID_SOCKET)
	{
		Logger::Log("Error in Socket");
		close(m_Socket);
		std::exit(1);
	}
}

bool LinuxClient::Connect(const AddrAndPort& serverAddrAndPort)
{
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr = StringToAddr(serverAddrAndPort.Address);
	sin.sin_port = htons(serverAddrAndPort.Port);

	m_ServerAddrAndPort = serverAddrAndPort;

	Logger::Log("Trying to connect to server", serverAddrAndPort.ToString());

	auto res = connect(m_Socket, (struct sockaddr*)&sin, sizeof(sin));
	if (res == SOCKET_ERROR)
	{
		Logger::Log("Error in Connect");
		close(m_Socket);
		std::exit(1);
	}

	std::cout << "Connection established" << std::endl;
	
	// Reaching this point means that a connection is succesfully established
	return true;
}

// Prepares the payload = MessageHeader + (actual) msg
void LinuxClient::Send(const std::string& msg)
{
	char* buf = new char[sizeof(MessageHeader) + msg.length()];

	MessageHeader* header = reinterpret_cast<MessageHeader*>(buf);
	header->MessageLength = (uint32_t)sizeof(MessageHeader) + (uint32_t)msg.length();

	memcpy(buf + sizeof(MessageHeader), msg.data(), msg.length());

	SendMessageWithHeader(header);

	delete[] buf;
}

void LinuxClient::SendMessageWithHeader(MessageHeader* payload)
{
	sendRawData(reinterpret_cast<char*>(payload), payload->MessageLength);
}

void LinuxClient::sendRawData(char* data, uint32_t len)
{
	auto remainingBytes = len;
	int offset = 0;

	while (remainingBytes > 0)
	{
		int sentBytes = send(m_Socket, data + offset, remainingBytes, 0);

		if (sentBytes == SOCKET_ERROR)
			ReConnect();

		if (sentBytes > 0)
		{
			remainingBytes -= sentBytes;
			offset += sentBytes;
			continue;
		}

		if (sentBytes == 0)
			Logger::Log("Connection closed by server");
		else
			Logger::Log("Error in SendRawData");
	}
}

void LinuxClient::Receive()
{
	if (m_Socket == INVALID_SOCKET)
		return;

	while (true)
	{
		char buf[MaxBufLen] = { '\0' };

		auto recvedBytes = recv(m_Socket, buf, MaxBufLen, 0);
		if (recvedBytes > 0)
		{
			Logger::Log("Received:", buf + sizeof(MessageHeader));
		}
		else if (recvedBytes == 0)
		{
			Logger::Log("Connection closed");
			return;
		}
		else
		{
			Logger::Log("Connection lost with", m_ServerAddrAndPort.ToString());
			ReConnect();
		}
	}
}

void LinuxClient::Shutdown()
{
	auto res = shutdown(m_Socket, SHUT_WR);
	if (res == SOCKET_ERROR)
	{
		Logger::Log("Error in Shutdown");
		close(m_Socket);
		return;
	}
	
	Logger::Log("Shutdown completed");
}

void LinuxClient::ReConnect()
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	if (m_AvailableServers.empty())
		return;

	// Find the dropped server
	auto found = std::find_if(m_AvailableServers.begin(), m_AvailableServers.end(), [=](const auto& addrPort) { return m_ServerAddrAndPort == addrPort; });

	if (found != m_AvailableServers.end())
		m_AvailableServers.erase(found);

	Shutdown();
	Socket();

	// Try connect to another
	for (auto& server : m_AvailableServers)
	{
		if (Connect(server))
			return;
	}

	Logger::Log("No available servers to connect");
	
	Shutdown();
}

void LinuxClient::SetAvailableServers(const std::vector<AddrAndPort>& servers)
{
	m_AvailableServers = servers;
}
