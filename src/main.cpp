/*bring AC Valhalla's stamina system into Skyrim.
@author TY
*/
#include "SimpleIni.h"
#include "Hooks.h"
#include "Papyrus.h"
#include "data.h"
#include "debuffHandler.h"
#include "events/animEventHandler.h"
#include "Utils.h"
#include "TrueHUDAPI.h"

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		INFO("Data loaded");
		settings::readSettings();
		animEventHandler::hookAllActors();
		break;
	case SKSE::MessagingInterface::kPostLoad:
		INFO("Post load");
		if (!TRUEHUD_API::RegisterInterfaceLoaderCallback(
			SKSE::GetMessagingInterface(),
			[](void* interfaceInstance, TRUEHUD_API::InterfaceVersion interfaceVersion) {
				if (interfaceVersion == TRUEHUD_API::InterfaceVersion::V2) {
					debuffHandler::g_trueHUD = reinterpret_cast<TRUEHUD_API::IVTrueHUD2*>(interfaceInstance);
					INFO("Obtained TrueHUD API");
				}
				else
					INFO("Unable to acquire requested TrueHUD API interface version");
			}))
			INFO("TRUEHUD_API::RegisterInterfaceLoaderCallback reported an error");
			break;
	case SKSE::MessagingInterface::kPostLoadGame:
		INFO("Post load game");
		INFO("clearing stamina debuff");
		debuffHandler::GetSingleton();
		//FIXME:clear debuff states for every load
		//debuffHandler::GetSingleton()->refresh();
		INFO("debuff cleared");
		break;
	case SKSE::MessagingInterface::kPostPostLoad:
		INFO("Post post load");
		if (!TRUEHUD_API::RequestInterface(
			SKSE::GetMessagingInterface(),
			TRUEHUD_API::InterfaceVersion::V2)) {
			INFO("TrueHUD API::RequestInterface reported an error");
		}
		else {
			INFO("TrueHUD API interface request success");
		}
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
	return true;
}

//TODO: 1. Add bashing hook to cancel NPC bashing stamina cost OR!! 2.make stun entirely separate.