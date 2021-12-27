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
		if (msg->type == SKSE::MessagingInterface::kPostLoadGame) {
			DEBUG("data loaded, initializing...");
			if (!setup) {
				dataHandler* handler = dataHandler::GetSingleton();
				gameSettings::tweakGameSetting();
				onHitEventHandler::Register();
				setup = true;
			}
			RE::Actor* pc = RE::PlayerCharacter::GetSingleton();
			animEventHandler::RegisterSink(pc);
			debuffHandler::GetSingleton()->rmDebuffPerk();
			//FIXME::stop using animEventHandler and start using your own hooks chump
			//actionEventHandler::RegisterSink(); 
			DEBUG("initialization complete!");
		}
	}


};


