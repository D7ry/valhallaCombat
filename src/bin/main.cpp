#include "SimpleIni.h"
#include "include/Hooks.h"
#include "include/Papyrus.h"
#include "include/data.h"
#include "include/debuffHandler.h"
#include "include/events.h"
#include "include/lib/TrueHUDAPI.h"
#include "include/PCH.h"
#include "ValhallaCombat.hpp"
#include "include/settings.h"

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		INFO("Data loaded");
		settings::init();
		settings::readSettings();
		animEventHandler::hookAllActors();
		inputEventHandler::Register();
		data::GetSingleton()->loadData();
		stunHandler::launchStunMapCleaner();
		break;
	case SKSE::MessagingInterface::kPostLoad:
		INFO("Post load");
		ValhallaCombat::GetSingleton()->g_trueHUD = reinterpret_cast<TRUEHUD_API::IVTrueHUD3*>(TRUEHUD_API::RequestPluginAPI(TRUEHUD_API::InterfaceVersion::V3));
		if (ValhallaCombat::GetSingleton()->g_trueHUD) {
			INFO("Obtained TruehudAPI - {0:x}", (uintptr_t)ValhallaCombat::GetSingleton()->g_trueHUD);
			settings::TrueHudAPI = true;
		}
		else {
			INFO("Failed to obtain TrueHudAPI.");
			settings::TrueHudAPI = false;
		}
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
		INFO("Post load game");
		debuffHandler::GetSingleton()->quickStopStaminaDebuff(RE::PlayerCharacter::GetSingleton());
		stunHandler::GetSingleton()->refreshStun();
		settings::updateGlobals();
		INFO("debuff cleared");
		break;
	case SKSE::MessagingInterface::kPostPostLoad:
		INFO("Post post load");
		break;
	}
}
#if ANNIVERSARY_EDITION

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []()
{
	SKSE::PluginVersionData data{};

	data.PluginVersion(Version::MAJOR);
	data.PluginName(Version::NAME);
	data.AuthorName("Dropkicker"sv);

	data.CompatibleVersions({ SKSE::RUNTIME_LATEST });
	data.UsesAddressLibrary(true);

	return data;
}();

#else

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	DKUtil::Logger::Init(Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		ERROR("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		ERROR("Unable to load this plugin, incompatible runtime version!\nExpected: Newer than 1-5-39-0 (A.K.A Special Edition)\nDetected: {}", ver.string());
		return false;
	}

	return true;
}

#endif


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
#if ANNIVERSARY_EDITION

	DKUtil::Logger::Init(Version::PROJECT, Version::NAME);

	if (REL::Module::get().version() < SKSE::RUNTIME_1_6_317) {
		ERROR("Unable to load this plugin, incompatible runtime version!\nExpected: Newer than 1-6-317-0 (A.K.A Anniversary Edition)\nDetected: {}", REL::Module::get().version().string());
		return false;
	}

#endif

	INFO("{} v{} loaded", Version::PROJECT, Version::NAME);

	SKSE::Init(a_skse);
	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}
	Hooks::install();
	Papyrus::Register();
	srand(std::chrono::system_clock::now().time_since_epoch().count());
	return true;
}

//TODO: 1. Add bashing hook to cancel NPC bashing stamina cost OR!! 2.make stun entirely separate.