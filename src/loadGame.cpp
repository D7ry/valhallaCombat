#include "loadGame.h"
#include "dataHandler.h"
#include "debuffHandler.h"
#include "events/actorLoadEventHandler.h"
#include "events/animEventHandler.h"
#include "events/onHitEventHandler.h"
#include "events/actionEventHandler.h"
#include "events/actorLoadEventHandler.h"
#include "Utils.h"
using namespace Utils;
namespace loadGame {
	void EventCallBACK(SKSE::MessagingInterface::Message* msg)
	{
		if (msg->type == SKSE::MessagingInterface::kDataLoaded) {
			INFO("data loaded, initializing...");
			((animEventHandler*)((uintptr_t)RE::PlayerCharacter::GetSingleton() + 0x30))->HookSink();
			//onHitEventHandler::Register();
			dataHandler* data = dataHandler::GetSingleton();
			data->readSettings(); data->cancelVanillaPowerStamina();
			
			INFO("initialization complete!");
		}
	}


};


