#include "loadGame.h"
#include "dataHandler.h"
#include "debuffHandler.h"
#include "events/actorLoadEventHandler.h"
using namespace Utils;
namespace loadGame {
	void EventCallBACK(SKSE::MessagingInterface::Message* msg)
	{
		if (msg->type == SKSE::MessagingInterface::kDataLoaded) {
			INFO("data loaded, initializing...");
			((AnimationGraphEventWatcher*)((uintptr_t)RE::PlayerCharacter::GetSingleton() + 0x30))->HookSink();
			onHitEventHandler::Register();
			dataHandler* data = dataHandler::GetSingleton();
			data->readSettings(); data->cancelVanillaPowerStamina();
			
			INFO("initialization complete!");
		}
	}


};


