#include "LinuxServer.h"

#include "Utils/Log.h"

LinuxServer::LinuxServer(const AddrAndPort& addrAndPort)
	: m_AddrAndPort(addrAndPort)
{
	Socket();
	Bind();
	Listen();
}

void LinuxServer::Socket()
{
	m_ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_ListenSocket == SOCKET_ERROR)
	{
		Logger::Log("Error in Socket");
		close(m_ListenSocket);
		std::exit(1);
	}
}

void LinuxServer::Bind()
{
	struct sockaddr_in sin;

	sin.sin_family = AF_INET;
	sin.sin_addr = StringToAddr(m_AddrAndPort.Address);
	sin.sin_port = htons(m_AddrAndPort.Port);

	auto res = bind(m_ListenSocket, (struct sockaddr*)&sin, sizeof(sin));
	if (res == SOCKET_ERROR)
	{
		Logger::Log("Error in Bind");
		close(m_ListenSocket);
		std::exit(1);
	}
}

void LinuxServer::Listen()
{
	auto res = listen(m_ListenSocket, SOMAXCONN);
	if (res == SOCKET_ERROR)
	{
		Logger::Log("Error in Listen");
		close(m_ListenSocket);
		std::exit(1);
	}
}

static void Broadcast(char* recvBuf, int recvBufLen)
{
	std::lock_guard<std::mutex> lock(LinuxServer::s_Mutex);

	for (auto socket : LinuxServer::s_ConnectedClients)
	{
		auto sentBytes = send(socket, recvBuf, recvBufLen, 0);
		if (sentBytes == SOCKET_ERROR)
		{
			Logger::Log("Error in Broadcast");
			close(socket);
		}
	}
}

static void HandleConnection(int socket)
{
	while (true)
	{
		char recvBuf[MaxBufLen] = { '\0' };

		auto recvedBytes = recv(socket, recvBuf, MaxBufLen, 0);
		if (recvedBytes > 0)
		{
			Logger::Log("(debug) HandleConnection Bytes received:", recvedBytes);
			Logger::Log("(debug) HandleConnection Data:", recvBuf + SizeOfHeaderBytes);

			Broadcast(recvBuf, recvedBytes);
		}
		else
		{
			Logger::Log("A Client disconnected");
			{
				std::lock_guard<std::mutex> lock(LinuxServer::s_Mutex);
				auto found = std::find_if(LinuxServer::s_ConnectedClients.begin(), LinuxServer::s_ConnectedClients.end(), [=](auto clientSocket) { return clientSocket == socket; });

				if (found != LinuxServer::s_ConnectedClients.end())
					LinuxServer::s_ConnectedClients.erase(found);
			}
			return;
		}
	}
}

void LinuxServer::Run()
{
	Logger::Log("Server listening on", m_AddrAndPort.ToString());

	while (true)
	{
		auto socket = accept(m_ListenSocket, NULL, NULL);
		if (socket == INVALID_SOCKET)
		{
			Logger::Log("Error in Accept");
			close(socket);
		}

		{
			std::lock_guard<std::mutex> lock(s_Mutex);

			s_ConnectedClients.insert(socket);
			Logger::Log("A Client connected");

			std::thread newConnection = std::thread(HandleConnection, socket);
			newConnection.detach();

			if (newConnection.joinable())
				newConnection.join();
		}
	}
}