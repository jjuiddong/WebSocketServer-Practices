//------------------------------------------------------------------------
// Name:    webrelay_Protocol.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace webrelay {

using namespace network2;
using namespace marshalling_json;
static const int s2c_Protocol_ID = 12000;

class s2c_Protocol : public network2::iProtocol
{
public:
	s2c_Protocol() : iProtocol(s2c_Protocol_ID, ePacketFormat::JSON) {}
	void Welcome(netid targetId, bool isBinary, const string &msg);
	void AckLogin(netid targetId, bool isBinary, const string &name, const int &type, const int &result);
	void AckLogout(netid targetId, bool isBinary, const string &name, const int &result);
	void ReqStartInstance(netid targetId, bool isBinary, const string &name, const string &url);
	void ReqCloseInstance(netid targetId, bool isBinary, const string &name);
	void ReqConnectProxyServer(netid targetId, bool isBinary, const string &url, const string &name);
	void ReqPing(netid targetId, bool isBinary, const int &id);
	static cPacketHeaderJson s_packetHeader;
};
static const int c2s_Protocol_ID = 12100;

class c2s_Protocol : public network2::iProtocol
{
public:
	c2s_Protocol() : iProtocol(c2s_Protocol_ID, ePacketFormat::JSON) {}
	void ReqLogin(netid targetId, bool isBinary, const string &name, const int &type);
	void ReqLogout(netid targetId, bool isBinary, const string &name);
	void AckStartInstance(netid targetId, bool isBinary, const string &name, const string &url, const int &result);
	void AckCloseInstance(netid targetId, bool isBinary, const string &name, const int &result);
	void AckConnectProxyServer(netid targetId, bool isBinary, const string &url, const string &name, const int &result);
	void AckPing(netid targetId, bool isBinary, const int &id);
	static cPacketHeaderJson s_packetHeader;
};
}
