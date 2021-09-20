//
// 2021-09-20, jjuiddong
// WebSocket Transaction Test
//	- Many WebSocket Client
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


// server -> client packet handler
class cClientHandler : public webrelay::s2c_ProtocolHandler
	, public network2::iSessionListener
{
public:
	cClientHandler(const string &name)
		: m_client(name.c_str())
		, m_tick(0)
	{
	}
	virtual ~cClientHandler()
	{
		Clear();
	}

	bool Init(network2::cNetController &netController)
	{
		m_client.AddProtocolHandler(this);
		m_client.RegisterProtocol(&m_protocol);
		netController.StartWebClient(&m_client, "127.0.0.1:9980");
		return true;
	}

	void Update()
	{
	}

	virtual bool Welcome(webrelay::Welcome_Packet &packet) 
	{ 
		cout << packet.msg << endl;
		m_protocol.ReqLogin(network2::SERVER_NETID, true, "test", 1);
		return true; 
	}

	virtual bool AckLogin(webrelay::AckLogin_Packet &packet) 
	{ 
		++m_cnt;
		cout << "AckLogin " << packet.result << " " << m_cnt << endl;
		m_protocol.ReqLogin(network2::SERVER_NETID, true, "test", 1);
		return true; 
	}

	virtual void RemoveSession(network2::cSession &session)
	{
		int a = 0;
	}

	virtual void AddSession(network2::cSession &session)
	{
		
	}

	void Clear()
	{
		m_client.Close();
	}


public:
	network2::cWebClient m_client;
	webrelay::c2s_Protocol m_protocol;
	int m_tick;
	static int m_cnt;
};
int cClientHandler::m_cnt = 0;


// server thread function
int ThreadFunction()
{
	dbg::SetLogPath("./wstransaction_");
	dbg::SetErrLogPath("./wstransaction_");
	dbg::RemoveErrLog();
	dbg::RemoveLog();

	const int MaxClientCount = 10;
	network2::cNetController netController;

	vector<cClientHandler*> clients;
	clients.reserve(MaxClientCount);
	for (int i = 0; i < MaxClientCount; ++i)
	{
		cClientHandler *client =
			new cClientHandler(common::format("client%d", i + 1));
		client->Init(netController);
		clients.push_back(client);
	}

	while (g_isLoop)
	{
		netController.Process(0.1f);
		for (auto &client : clients)
			client->Update();
		Sleep(1);
	}

	for (auto &client : clients)
		delete client;
	netController.Clear();
	return 1;
}
