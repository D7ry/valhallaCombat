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
#include "include/Utils.h"
void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		logger::info("Data loaded");
		settings::init();
		settings::readSettings();
		events::registerAllEventHandlers();
		data::loadData();
		ValhallaCombat::GetSingleton()->launchCleanUpThread();
		break;
	case SKSE::MessagingInterface::kPostLoad:
		logger::info("Post load");
		ValhallaCombat::GetSingleton()->ersh = reinterpret_cast<TRUEHUD_API::IVTrueHUD3*>(TRUEHUD_API::RequestPluginAPI(TRUEHUD_API::InterfaceVersion::V3));
		if (ValhallaCombat::GetSingleton()->ersh) {
			logger::info("Obtained TruehudAPI - {0:x}", (uintptr_t)ValhallaCombat::GetSingleton()->ersh);
			settings::facts::TrueHudAPI_Obtained = true;
		} else {
			logger::info("Failed to obtain TrueHudAPI.");
			settings::facts::TrueHudAPI_Obtained = false;
		}
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
		logger::info("Post load game");
		debuffHandler::GetSingleton()->quickStopStaminaDebuff(RE::PlayerCharacter::GetSingleton());
		stunHandler::GetSingleton()->reset();
		//balanceHandler::GetSingleton()->reset();
		settings::updateGlobals();
		break;
	case SKSE::MessagingInterface::kPostPostLoad:
		logger::info("Post post load");
		break;
	}
}
namespace
{
	void InitializeLog()
	{
#ifndef NDEBUG
		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
		auto path = logger::log_directory();
		if (!path) {
			util::report_and_fail("Failed to find standard logging directory"sv);
		}

		*path /= fmt::format("{}.log"sv, Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

#ifndef NDEBUG
		const auto level = spdlog::level::trace;
#else
		const auto level = spdlog::level::info;
#endif

		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
		log->set_level(level);
		log->flush_on(level);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);
	}
}
std::string wstring2string(const std::wstring& wstr, UINT CodePage)

{

	std::string ret;

	int len = WideCharToMultiByte(CodePage, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);

	ret.resize((size_t)len, 0);

	WideCharToMultiByte(CodePage, 0, wstr.c_str(), (int)wstr.size(), &ret[0], len, NULL, NULL);

	return ret;

}


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	//DKUtil::Logger::Init(Version::PROJECT, Version::NAME);
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Plugin::NAME.data();
	a_info->version = Plugin::VERSION[0];


	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		logger::critical("Unable to load this plugin, incompatible runtime version!\nExpected: Newer than 1-5-39-0 (A.K.A Special Edition)\nDetected: {}", ver.string());
		return false;
	}

	return true;
}

#ifdef SKYRIM_SUPPORT_AE
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;

	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);

	v.UsesAddressLibrary(true);
	v.CompatibleVersions({ SKSE::RUNTIME_LATEST });

	return v;
}();
#endif


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
#ifdef SKYRIM_SUPPORT_AE

	DKUtil::Logger::Init(Version::PROJECT, Version::NAME);

	if (REL::Module::get().version() < SKSE::RUNTIME_1_6_317) {
		logger::critical("Unable to load this plugin, incompatible runtime version!\nExpected: Newer than 1-6-317-0 (A.K.A Anniversary Edition)\nDetected: {}", REL::Module::get().version().string());
		return false;
	}

#endif
	InitializeLog();
	logger::info("{} v{} loaded", Plugin::NAME, Plugin::VERSION.string());
	SKSE::Init(a_skse);
	logger::info("Initializing...");
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
