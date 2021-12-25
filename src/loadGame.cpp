#include "C:/Users/Ty/Documents/SKSEPlugins/Build/Plugins/valhallaCombat/CMakeFiles/valhallaCombat.dir/Debug/cmake_pch.hxx"
#include "loadGame.h"
#include "avHandler.h"
#include "dataHandler.h"
using namespace Utils;
namespace loadGame {
	boolean setup = false;
	void EventCallBACK(SKSE::MessagingInterface::Message* msg)
	{
		if (msg->type == SKSE::MessagingInterface::kPostLoadGame) {
			DEBUG("data loaded, initializing...");
			RE::Actor* pc = RE::PlayerCharacter::GetSingleton();
			/*
			if (SKSE::GetActionEventSource()) {
				SKSE::GetActionEventSource()->AddEventSink(SKSE::ActionEvent);
				DEBUG("add weapon swing sink!");
			}
			*/
			//TODO:get SKSE action event sink working
			if (!setup) {
				dataHandler* handler = dataHandler::GetSingleton();
				gameSettings::tweakGameSetting();
				onHitEventHandler::Register();
			}
			animEventHandler::RegisterSink(pc);
			//actionEventHandler::RegisterSink(); 
			DEBUG("initialization complete!");
		}
	}



}


