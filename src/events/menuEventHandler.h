#include "events/animEventHandler.h";
using EventResult = RE::BSEventNotifyControl;

class menuEventHandler : public RE::BSTEventSink<RE::MenuOpenCloseEvent> {
public:

	static menuEventHandler* GetSingleton() {
		static menuEventHandler singleton;
		return std::addressof(singleton);
	}
	EventResult ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource);
	static bool Register()
	{
		auto ui = RE::UI::GetSingleton();
		ui->AddEventSink<RE::MenuOpenCloseEvent>(GetSingleton());

		INFO("Register {}.", typeid(GetSingleton()).name());
		return true;
	}
	static bool unRegister() {
		static menuEventHandler singleton;
		auto ui = RE::UI::GetSingleton();
		ui->RemoveEventSink<RE::MenuOpenCloseEvent>(GetSingleton());

		INFO("Unregistered {}.", typeid(singleton).name());
		return true;
	}
};
