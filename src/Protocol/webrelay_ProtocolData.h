//------------------------------------------------------------------------
// Name:    webrelay_ProtocolData.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace webrelay {

using namespace network2;
using namespace marshalling_json;


	struct Welcome_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string msg;
	};

	struct AckLogin_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string name;
		int type;
		int result;
	};

	struct AckLogout_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string name;
		int result;
	};

	struct ReqStartInstance_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string name;
		string url;
	};

	struct ReqCloseInstance_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string name;
	};

	struct ReqConnectProxyServer_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string url;
		string name;
	};

	struct ReqPing_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int id;
	};





	struct ReqLogin_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string name;
		int type;
	};

	struct ReqLogout_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string name;
	};

	struct AckStartInstance_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string name;
		string url;
		int result;
	};

	struct AckCloseInstance_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string name;
		int result;
	};

	struct AckConnectProxyServer_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string url;
		string name;
		int result;
	};

	struct AckPing_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int id;
	};



}
