#include "C:/Users/Ty/Documents/SKSEPlugins/Build/Plugins/valhallaCombat/CMakeFiles/valhallaCombat.dir/Debug/cmake_pch.hxx"
#include "loadGame.h"
#include "avHandler.h"
#include "dataHandler.h"
#include "debuffHandler.h"
using namespace Utils;
namespace loadGame {
	bool setup = false;
	void EventCallBACK(SKSE::MessagingInterface::Message* msg)
	{
		if (msg->type == SKSE::MessagingInterface::kDataLoaded) {
			INFO("data loaded, initializing...");
			dataHandler* handler = dataHandler::GetSingleton();
			gameSettings::tweakGameSetting();
			onHitEventHandler::Register();
			((AnimationGraphEventWatcher*)((uintptr_t)RE::PlayerCharacter::GetSingleton() + 0x30))->HookSink();
			//actionEventHandler::RegisterSink(); 
			INFO("initialization complete!");
		}
	}


};


