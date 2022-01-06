#include "loadGame.h"
#include "dataHandler.h"
#include "debuffHandler.h"
#include "events/actorLoadEventHandler.h"
using namespace Utils;
namespace loadGame {
	bool setup = false;
	void EventCallBACK(SKSE::MessagingInterface::Message* msg)
	{
		if (msg->type == SKSE::MessagingInterface::kPostLoadGame) {
			INFO("data loaded, initializing...");
			if (!setup) {
				((AnimationGraphEventWatcher*)((uintptr_t)RE::PlayerCharacter::GetSingleton() + 0x30))->HookSink();
				onHitEventHandler::Register();
				dataHandler* data = dataHandler::GetSingleton();
				data->readSettings(); data->cancelVanillaPowerStamina();
				setup = true;
			}
			debuffHandler::GetSingleton()->refresh();
			INFO("initialization complete!");
		}
	}


};


