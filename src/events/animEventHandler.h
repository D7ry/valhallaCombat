#pragma once
#include "avHandler.h"
#include "attackHandler.h"
using EventResult = RE::BSEventNotifyControl;
class animEventHandler : public RE::BSTEventSink<RE::BSAnimationGraphEvent>
{
public:
	virtual EventResult ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource);
	static bool RegisterSink(RE::Actor* actor) {
		static animEventHandler g_eventhandler;

		RE::BSAnimationGraphManagerPtr graphManager;

		if (actor) {
			actor->GetAnimationGraphManager(graphManager);
		} else {
			INFO("Actor not found!");
		}

		if (!graphManager) {
			INFO("eRROR: animation graph not found!");
			return false;
		}

		if (!graphManager->graphs.cbegin()) {
			INFO("eRROR: animation graph.cbegin() not found!");
			return false;
		}

		graphManager->graphs.cbegin()->get()->AddEventSink(&g_eventhandler);

		DEBUG("Register Animation Event Handler onto {}", actor->GetName());

		return true;
	}
	static bool unRegisterSink(RE::Actor* actor) {
		static animEventHandler g_eventhandler;

		RE::BSAnimationGraphManagerPtr graphManager;

		if (actor) {
			actor->GetAnimationGraphManager(graphManager);
		}

		if (!graphManager || !graphManager->graphs.cbegin()) {
			ERROR("animation graph not found!");
			return false;
		}

		graphManager->graphs.cbegin()->get()->RemoveEventSink(&g_eventhandler);

		DEBUG("Unregister Animation Event Handler from {}!", actor->GetName());

		return true;
	}
private:
	animEventHandler() = default;
	~animEventHandler() = default;
};

