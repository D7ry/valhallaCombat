#include "C:/Users/Ty/Documents/SKSEPlugins/Build/Plugins/valhallaCombat/CMakeFiles/valhallaCombat.dir/Debug/cmake_pch.hxx"
#include "loadGame.h"
#include "dataHandler.h"
#include "debuffHandler.h"
#include "events/actorLoadEventHandler.h"
#include "events/raceSwitchEventHandler.h"
using namespace Utils;
namespace loadGame {
	bool setup = false;
	void EventCallBACK(SKSE::MessagingInterface::Message* msg)
	{
		if (msg->type == SKSE::MessagingInterface::kPostLoadGame) {
			INFO("data loaded, initializing...");
			if (!setup) {
				dataHandler* handler = dataHandler::GetSingleton();
				gameSettings::tweakGameSetting();
				((AnimationGraphEventWatcher*)((uintptr_t)RE::PlayerCharacter::GetSingleton() + 0x30))->HookSink();
				onHitEventHandler::Register();
				attackHandler::updatePcStaminaRate();
				raceSwitchEventHandler::Register();
			}
			debuffHandler::GetSingleton()->rmDebuffPerk();
			attackHandler::checkout();
			//actionEventHandler::RegisterSink(); 
			INFO("initialization complete!");
		}
	}


};


