//
// 2021-09-19, jjuiddong
// Poco Library, WebSocket Server Customize
//
#include "stdafx.h"
#include "wsserver.h"

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


// server thread function
int ThreadFunction()
{
	cWsServer svs;
	svs.Init(9980);
	while (g_isLoop)
	{
		Sleep(1);
	}
	svs.Close();
	return 1;
}

