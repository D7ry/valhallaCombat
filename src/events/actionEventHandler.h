#pragma once
#include "Utils.h"
using EventResult = RE::BSEventNotifyControl;
class actionEventHandler : public RE::BSTEventSink<SKSE::ActionEvent>
{
public:
	virtual EventResult ProcessEvent(const SKSE::ActionEvent* a_event, RE::BSTEventSource<SKSE::ActionEvent>* a_eventSource);
	static bool RegisterSink() {
		static actionEventHandler g_eventhandler;
		if (SKSE::GetActionEventSource()) {
			SKSE::GetActionEventSource()->AddEventSink(&g_eventhandler);
			DEBUG("Registered action Event Handler");
			return true;
		} else {
			DEBUG("failed to register action event handler");
			return false;
		}
	}
private:
	actionEventHandler() = default;
	~actionEventHandler() = default;
};

