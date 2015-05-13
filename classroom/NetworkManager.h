#pragma once

#include "SmartFox.h"
#include "Requests\ExtensionRequest.h"
#include "Requests\JoinRoomRequest.h"
#include "Requests\LoginRequest.h"

/* Benchmarking reqs */
#include <omp.h>

class NetworkManager {

public:
	NetworkManager();
	virtual ~NetworkManager();
	void init();
	void startBenchmarking();
	void alarm();

private:

	// handle smartfox events
	static void OnSmartFoxConnection(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent);
	static void OnSmartFoxConnectionLost(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent);
	static void OnSmartFoxRoomJoined(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent);
	static void OnSmartFoxLogin(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent);
	static void OnSmartFoxDisconnection(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent);
	static void OnSmartFoxLoginError(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent);
	static void OnSmartFoxLogout(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent);
	static void OnSmartFoxExtensionResponse(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent);
	static void OnUDPInit(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent);

	HANDLE SmartFoxConnectionEstablished;
	boost::shared_ptr<Sfs2X::SmartFox> m_ptrSmartFox;

	// Benchmarking items
	static double start;
	static double end;
	static int itemsSent;

	static bool benchmarkingStarted;
};
