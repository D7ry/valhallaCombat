#include "C:/Users/Ty/Documents/SKSEPlugins/Build/Plugins/valhallaCombat/CMakeFiles/valhallaCombat.dir/Debug/cmake_pch.hxx"
#include "loadGame.h"
#include "avHandler.h"
#include "dataHandler.h"
using namespace Utils;
namespace loadGame {
	boolean dataSetup = false;
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
			//TODO:switch to SKSE action event sink
			animEventHandler::RegisterSink(pc);
			onHitEventHandler::Register();
			if (!dataSetup) {
				dataHandler::setupData();
				dataSetup = true;
			} else {
				DEBUG("data already setup!");
			}
			DEBUG("initialization complete!");
		}
	}



}


