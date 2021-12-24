#include "C:/Users/Ty/Documents/SKSEPlugins/Build/Plugins/valhallaCombat/CMakeFiles/valhallaCombat.dir/Debug/cmake_pch.hxx"
#include "loadGame.h"
#include "avHandler.h"
using namespace Utils;
namespace loadGame {
	void EventCallBACK(SKSE::MessagingInterface::Message* msg)
	{
		if (msg->type == SKSE::MessagingInterface::kPostLoadGame) {
			DEBUG("save loaded, initializing...");
			RE::Actor* pc = RE::PlayerCharacter::GetSingleton();
			animEventHandler::RegisterSink(pc);
			onHitEventHandler::Register();
			init();
		}
	}
	//FIXME:
	void tweakGameSetting() {
		DEBUG("tweaking game setting");
		setGameSettingf(gameSettings::powerAtkStaminaCostMultiplier, 1);
		setGameSettingf(gameSettings::staminaRegenDelay, 3);
		setGameSettingf(gameSettings::blockStaminaRegen, 0);
		setGameSettingf(gameSettings::atkStaminaCostMult, 0);
		setGameSettingf(gameSettings::atkStaminaCostBase, 20);
		//TODO:set race stamina.
	}

	void init() {
		avHandler::meleeCost1h = 25;
		avHandler::meleeCost2h = 30;
		tweakGameSetting();
		DEBUG("initialization complete!");
	}


}


