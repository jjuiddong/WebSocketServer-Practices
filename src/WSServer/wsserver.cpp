
#include "stdafx.h"
#include "wsserver.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/HTTPClientSession.h"


using namespace network2;
using Poco::Net::ServerSocket;
using Poco::Net::WebSocket;
using Poco::Net::WebSocketException;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;


//----------------------------------------------------------------------------------
// cWebSocketRequestHandler
class cWebSocketRequestHandler : public HTTPRequestHandler
{
public:
	cWsServer &m_webServer;
	cWebSocketRequestHandler(cWsServer &wsServer) : m_webServer(wsServer) {}

	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
	{
		try
		{
			WebSocket ws(request, response);
			char buffer[1024];
			int flags;
			int n;
			do
			{
				n = ws.receiveFrame(buffer, sizeof(buffer), flags);
				ws.sendFrame(buffer, n, flags);
			} while (n > 0 && (flags & WebSocket::FRAME_OP_BITMASK) != WebSocket::FRAME_OP_CLOSE);
		}
		catch (WebSocketException& e)
		{
			switch (e.code())
			{
			case WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
				response.set("Sec-WebSocket-Version", WebSocket::WEBSOCKET_VERSION);
				// fallthrough
			case WebSocket::WS_ERR_NO_HANDSHAKE:
			case WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
			case WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
				response.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
				response.setContentLength(0);
				response.send();
				break;
			}
		}
	}
};


//----------------------------------------------------------------------------------
// cWsServer
cWsServer::cWsServer(
	iSessionFactory *sessionFactory //= new cSessionFactory()
	, const StrId &name //= "TcpServer"
	, const int logId //= -1
)
	: cNetworkNode(name, logId)
	, m_httpServer(nullptr)
	, m_maxBuffLen(RECV_BUFFER_LENGTH)
	, m_sleepMillis(10)
	, m_sessionFactory(sessionFactory)
	, m_sendQueue(this, logId)
	, m_recvQueue(this, logId)
	, m_sessionListener(nullptr)
	, m_tempRecvBuffer(NULL)
	, m_lastAcceptTime(0)
	, m_isThreadMode(true)
{
}

cWsServer::~cWsServer()
{
	Close();
}


// initialize websocket server
bool cWsServer::Init(const int bindPort
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_MAX_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const bool isThreadMode //= true
)
{
	Close();

	m_websocket = new Poco::Net::ServerSocket(bindPort);
	m_httpServer = new Poco::Net::HTTPServer(this, *m_websocket, new HTTPServerParams);
	m_httpServer->start();
	return true;
}


bool cWsServer::Process()
{
	return true;
}


bool cWsServer::AddSession(const SOCKET sock, const Str16 &ip, const int port)
{
	RETV(!m_sessionFactory, false);

	if (FindSessionBySocket(sock))
		return false; // Error!! Already Exist

	common::AutoCSLock cs(m_cs);
	cSession *session = m_sessionFactory->New();
	session->m_id = common::GenerateId();
	session->m_socket = sock;
	session->m_ip = ip;
	session->m_port = port;
	session->m_state = eState::Connect;
	m_sessions.insert({ session->m_id, session });
	m_sockets.insert({ sock, session });

	if (m_logId >= 0)
		network2::LogSession(m_logId, *session);

	if (m_sessionListener)
		m_sessionListener->AddSession(*session);

	return true;
}


bool cWsServer::RemoveSession(const netid netId)
{
	cSession *session = NULL;
	{
		common::AutoCSLock cs(m_cs);

		auto it = m_sessions.find(netId);
		if (m_sessions.end() == it)
			return false; // not found session
		session = it->second;
	}

	if (m_sessionListener)
		m_sessionListener->RemoveSession(*session);

	{
		common::AutoCSLock cs(m_cs);

		const SOCKET sock = session->m_socket;
		session->Close();
		SAFE_DELETE(session);
		m_sessions.remove(netId);
		m_sockets.remove(sock);
	}

	return true;
}


bool cWsServer::IsExist(const netid netId)
{
	common::AutoCSLock cs(m_cs);

	auto it = m_sessions.find(netId);
	return m_sessions.end() != it;
}


cSession* cWsServer::FindSessionBySocket(const SOCKET sock)
{
	common::AutoCSLock cs(m_cs);

	auto it = m_sockets.find(sock);
	if (m_sockets.end() == it)
		return NULL; // not found session
	return it->second;
}


cSession* cWsServer::FindSessionByNetId(const netid netId)
{
	common::AutoCSLock cs(m_cs);

	auto it = m_sessions.find(netId);
	if (m_sessions.end() == it)
		return NULL; // not found session
	return it->second;
}


cSession* cWsServer::FindSessionByName(const StrId &name)
{
	common::AutoCSLock cs(m_cs);

	for (auto &session : m_sessions.m_seq)
		if (session->m_name == name)
			return session;
	return NULL; // not found session
}


void cWsServer::SetSessionListener(iSessionListener *listener)
{
	m_sessionListener = listener;
}


// httprequesthandler factory hanlder
Poco::Net::HTTPRequestHandler* cWsServer::createRequestHandler(
	const Poco::Net::HTTPServerRequest& request)
{
	if (request.find("Upgrade") != request.end()
		&& Poco::icompare(request["Upgrade"], "websocket") == 0)
		return new cWebSocketRequestHandler(*this);
	return nullptr;
}


// clear
void cWsServer::Close()
{
	if (m_httpServer)
	{
		m_httpServer->stop();
		if (m_websocket)
			m_websocket->close();
		SAFE_DELETE(m_websocket);
		//SAFE_DELETE(m_session);
		//SAFE_DELETE(m_request);
		//SAFE_DELETE(m_response);
		SAFE_DELETE(m_httpServer);
	}
}
