#pragma once
using EventResult = RE::BSEventNotifyControl;
class raceSwitchEventHandler : public RE::BSTEventSink<RE::TESSwitchRaceCompleteEvent> {
	EventResult ProcessEvent(const RE::TESSwitchRaceCompleteEvent* a_event, RE::BSTEventSource<RE::TESSwitchRaceCompleteEvent>* a_eventSource);
public:

	static bool Register() {
		static raceSwitchEventHandler singleton;
		auto ScriptEventSource = RE::ScriptEventSourceHolder::GetSingleton();
		if (!ScriptEventSource) {
			ERROR("Script event source not found");
			return false;
		}
		ScriptEventSource->AddEventSink(&singleton);
		INFO("Register {}.", typeid(singleton).name());
		return true;
	}
};




