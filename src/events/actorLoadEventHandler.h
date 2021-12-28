#pragma once
#include "animEventHandler.h"
#include "menuEventHandler.h"
using EventResult = RE::BSEventNotifyControl;

class actorLoadEventHandler : public RE::BSTEventSink<RE::TESObjectLoadedEvent>{
public:
	EventResult ProcessEvent(const RE::TESObjectLoadedEvent* a_event, RE::BSTEventSource<RE::TESObjectLoadedEvent>* a_eventSource);
	static bool Register()
	{
		static actorLoadEventHandler singleton;
		auto ScriptEventSource = RE::ScriptEventSourceHolder::GetSingleton();

		if (!ScriptEventSource) {
			INFO("Script event source not found");
			return false;
		}

		ScriptEventSource->AddEventSink(&singleton);

		INFO("Register {}.", typeid(singleton).name());
		return true;
	}
};