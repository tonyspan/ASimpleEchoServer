#ifdef PLATFORM_WINDOWS
#undef UNICODE
#endif

#include "Interface/NetAPI.h"

int main(int argc, char* argv[])
{
	auto serverSpec = Server::ParseCommandLineArgs(argc, argv);

	auto server = Server::Create(serverSpec);

	server->Run();
}
