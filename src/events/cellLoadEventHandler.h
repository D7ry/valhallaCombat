#pragma once
#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>
using EventResult = RE::BSEventNotifyControl;

class cellLoadEventHandler : public RE::BSTEventSink<RE::TESCellFullyLoadedEvent> {
public:
	
	static cellLoadEventHandler* GetSingleton(){
		static cellLoadEventHandler singleton;
		return std::addressof(singleton);
	}
	EventResult ProcessEvent(const RE::TESCellFullyLoadedEvent* a_event, RE::BSTEventSource<RE::TESCellFullyLoadedEvent>* a_eventSource);
	static bool Register()
	{
		static cellLoadEventHandler singleton;
		auto ScriptEventSource = RE::ScriptEventSourceHolder::GetSingleton();

		if (!ScriptEventSource) {
			ERROR("Script event source not found");
			return false;
		}

		ScriptEventSource->AddEventSink(&singleton);

		DEBUG("Register {}.", typeid(singleton).name());
		return true;
	}
	static bool unRegister() {
		static cellLoadEventHandler singleton;
		auto ScriptEventSource = RE::ScriptEventSourceHolder::GetSingleton();
		if (!ScriptEventSource) {
			ERROR("Script event source not found");
			return false;
		}
		ScriptEventSource->RemoveEventSink(&singleton);

		DEBUG("Unregistered {}.", typeid(singleton).name());
		return true;
	}
};
