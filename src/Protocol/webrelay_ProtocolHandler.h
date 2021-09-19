//------------------------------------------------------------------------
// Name:    webrelay_ProtocolHandler.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

#include "webrelay_ProtocolData.h"

namespace webrelay {

using namespace network2;
using namespace marshalling_json;
static const int s2c_Dispatcher_ID = 12000;

// Protocol Dispatcher
class s2c_Dispatcher: public network2::cProtocolDispatcher
{
public:
	s2c_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
	static cPacketHeaderJson s_packetHeader;
};
static s2c_Dispatcher g_webrelay_s2c_Dispatcher;


// ProtocolHandler
class s2c_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class s2c_Dispatcher;
	s2c_ProtocolHandler() { m_format = ePacketFormat::JSON; }
	virtual bool Welcome(webrelay::Welcome_Packet &packet) { return true; }
	virtual bool AckLogin(webrelay::AckLogin_Packet &packet) { return true; }
	virtual bool AckLogout(webrelay::AckLogout_Packet &packet) { return true; }
	virtual bool ReqStartInstance(webrelay::ReqStartInstance_Packet &packet) { return true; }
	virtual bool ReqCloseInstance(webrelay::ReqCloseInstance_Packet &packet) { return true; }
	virtual bool ReqConnectProxyServer(webrelay::ReqConnectProxyServer_Packet &packet) { return true; }
	virtual bool ReqPing(webrelay::ReqPing_Packet &packet) { return true; }
};


static const int c2s_Dispatcher_ID = 12100;

// Protocol Dispatcher
class c2s_Dispatcher: public network2::cProtocolDispatcher
{
public:
	c2s_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
	static cPacketHeaderJson s_packetHeader;
};
static c2s_Dispatcher g_webrelay_c2s_Dispatcher;


// ProtocolHandler
class c2s_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class c2s_Dispatcher;
	c2s_ProtocolHandler() { m_format = ePacketFormat::JSON; }
	virtual bool ReqLogin(webrelay::ReqLogin_Packet &packet) { return true; }
	virtual bool ReqLogout(webrelay::ReqLogout_Packet &packet) { return true; }
	virtual bool AckStartInstance(webrelay::AckStartInstance_Packet &packet) { return true; }
	virtual bool AckCloseInstance(webrelay::AckCloseInstance_Packet &packet) { return true; }
	virtual bool AckConnectProxyServer(webrelay::AckConnectProxyServer_Packet &packet) { return true; }
	virtual bool AckPing(webrelay::AckPing_Packet &packet) { return true; }
};


}
