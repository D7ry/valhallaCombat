#include "menuEventHandler.h"
EventResult menuEventHandler::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) {

	if (!a_event || !a_eventSource) {
		ERROR("Event Source not found");
		return EventResult::kContinue;
	}
	DEBUG("menu event triggeres!!");
	INFO("+++++++++++++++++++++menu event triggeres!+++++++++++++++++++++");
	auto pc = RE::PlayerCharacter::GetSingleton();
	animEventHandler::RegisterSink(pc);
	debuffHandler::GetSingleton()->rmDebuffPerk();
	const auto task = SKSE::GetTaskInterface();
	if (task != nullptr) {
		task->AddTask(unRegister);
		INFO("tasked unregistration menu");
	}
	return EventResult::kStop;
}
