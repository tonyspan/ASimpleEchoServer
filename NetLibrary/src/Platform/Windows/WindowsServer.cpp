#include "WindowsServer.h"

#include "Utils/Log.h"

WindowsServer::WindowsServer(const AddrAndPort& addrAndPort)
	: m_AddrAndPort(addrAndPort)
{
	InitWSADATA();
	Socket();
	Bind();
	Listen();
}

WindowsServer::~WindowsServer()
{
	WSACleanup();
}

void WindowsServer::InitWSADATA()
{
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR)
	{
		Logger::Log("Error:", WSAGetLastError(), "in InitWSADATA");
		std::exit(1);
	}
}

void WindowsServer::Socket()
{
	m_ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_ListenSocket == INVALID_SOCKET)
	{
		Logger::Log("Error:", WSAGetLastError(), "in Socket");
		closesocket(m_ListenSocket);
		WSACleanup();
		std::exit(1);
	}
}

void WindowsServer::Bind()
{
	struct sockaddr_in sin;

	sin.sin_family = AF_INET;
	sin.sin_addr = StringToAddr(m_AddrAndPort.Address);
	sin.sin_port = htons(m_AddrAndPort.Port);

	auto res = bind(m_ListenSocket, (SOCKADDR*)&sin, sizeof(sin));
	if (res == SOCKET_ERROR)
	{
		Logger::Log("Error:", WSAGetLastError(), "in Bind");
		closesocket(m_ListenSocket);
		WSACleanup();
		std::exit(1);
	}
}

void WindowsServer::Listen()
{
	auto res = listen(m_ListenSocket, SOMAXCONN);
	if (res == SOCKET_ERROR)
	{
		Logger::Log("Error:", WSAGetLastError(), "in Listen");
		closesocket(m_ListenSocket);
		WSACleanup();
		std::exit(1);
	}
}

static void Broadcast(char* recvBuf, int recvBufLen)
{
	std::lock_guard<std::mutex> lock(WindowsServer::s_Mutex);

	for (auto socket : WindowsServer::s_ConnectedClients)
	{
		auto sentBytes = send(socket, recvBuf, recvBufLen, 0);
		if (sentBytes == SOCKET_ERROR)
		{
			Logger::Log("Error:", WSAGetLastError(), "in Broadcast");
			closesocket(socket);
			WSACleanup();
		}
	}
}

static void HandleConnection(SOCKET socket)
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
				std::lock_guard<std::mutex> lock(WindowsServer::s_Mutex);
				auto found = std::find_if(WindowsServer::s_ConnectedClients.begin(), WindowsServer::s_ConnectedClients.end(), [=](auto clientSocket) { return clientSocket == socket; });

				if (found != WindowsServer::s_ConnectedClients.end())
					WindowsServer::s_ConnectedClients.erase(found);
			}
			return;
		}
	}
}

void WindowsServer::Run()
{
	Logger::Log("Server listening on", m_AddrAndPort.ToString());

	while (true)
	{
		auto socket = accept(m_ListenSocket, NULL, NULL);
		if (socket == INVALID_SOCKET)
		{
			Logger::Log("Error:", WSAGetLastError(), "in Accept");
			closesocket(socket);
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
