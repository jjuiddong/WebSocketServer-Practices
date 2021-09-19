//
// 2021-09-19, jjuiddong
// WebServer test
//
#include "stdafx.h"

using namespace std;

bool g_isLoop = true;
BOOL CtrlHandler(DWORD fdwCtrlType);
int ThreadFunction();


int main()
{
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
	{
		cout << "SetConsoleCtrlHandler failed, code : " << GetLastError() << endl;
		return -1;
	}
	std::thread th(ThreadFunction);
	th.join();
	cout << "Finish WebSocket Server\n";
	return 0;
}


BOOL CtrlHandler(DWORD fdwCtrlType)
{
	g_isLoop = false;
	return TRUE;
}


class cPacketHandler : public all::ProtocolHandler
{
public:
	virtual void RecvPacket(const network2::cPacket &packet) 
	{

		int a = 0;
	}
};


// server thread function
int ThreadFunction()
{
	cPacketHandler handler;
	network2::cNetController netController;
	network2::cWebServer server;
	server.AddProtocolHandler(&handler);

	netController.StartWebServer(&server, 9980);

	while (g_isLoop)
	{
		netController.Process(0.1f);
		Sleep(1);
	}

	netController.RemoveServer(&server);
	server.Close();
	return 1;
}
