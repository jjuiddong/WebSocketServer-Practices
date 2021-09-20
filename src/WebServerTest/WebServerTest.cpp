//
// 2021-09-19, jjuiddong
// network2::WebServer test
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


class cPacketHandler : public webrelay::c2s_ProtocolHandler
					  , public network2::iSessionListener
{
public:
	virtual bool ReqLogin(webrelay::ReqLogin_Packet &packet) 
	{ 
		m_protocol.AckLogin(packet.senderId, true, packet.name, packet.type, 1);
		return true; 
	}

	virtual void RemoveSession(network2::cSession &session)
	{
		int a = 0;
	}

	virtual void AddSession(network2::cSession &session)
	{
		m_protocol.Welcome(session.m_id, true, "welcome webserver");
	}


public:
	webrelay::s2c_Protocol m_protocol;
};


// server thread function
int ThreadFunction()
{
	dbg::SetLogPath("./wsserver_");
	dbg::SetErrLogPath("./wsserver_");
	dbg::RemoveErrLog();
	dbg::RemoveLog();

	cPacketHandler handler;
	network2::cNetController netController;
	network2::cWebServer server;
	server.AddProtocolHandler(&handler);
	server.SetSessionListener(&handler);
	server.RegisterProtocol(&handler.m_protocol);

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
