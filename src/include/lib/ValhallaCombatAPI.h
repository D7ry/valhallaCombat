#pragma once
#include <functional>
#include <stdint.h>
#include <PCH.h>
/*
* For modders: Copy this file into your own project if you wish to use this API
*/
namespace VAL_API
{
	constexpr const auto ValhallaCombatPluginName = "ValhallaCombat";

	using PluginHandle = SKSE::PluginHandle;
	using ActorHandle = RE::ActorHandle;

	enum class InterfaceVersion : uint8_t
	{
		V1,
		V2
	};

	enum STUNSOURCE
	{
		lightAttack,
		powerAttack,
		bash,
		powerBash,
		timedBlock,
		parry,
		counterAttack
	};

	// Valhalla Combat's modder interface
	class IVVAL1
	{
	public:
		virtual void processStunDamage(STUNSOURCE stunSource, RE::TESObjectWEAP* weapon, RE::Actor* aggressor, RE::Actor* victim, float baseDamage) noexcept = 0;
	};

	class IVVAL2 : IVVAL1
	{
	public:
		virtual bool getIsPCTimedBlocking() noexcept = 0;
		virtual bool getIsPCPerfectBlocking() noexcept = 0;
		virtual void triggerPcTimedBlockSuccess() noexcept = 0;

		virtual bool isActorStunned(RE::Actor* a_actor) noexcept = 0;
		virtual bool isActorExhausted(RE::Actor* a_actor) noexcept = 0;
	};


	typedef void* (*_RequestPluginAPI)(const InterfaceVersion interfaceVersion);

	/// <summary>
	/// Request the Valhalla Combat API interface.
	/// Recommended: Send your request during or after SKSEMessagingInterface::kMessage_PostLoad to make sure the dll has already been loaded
	/// </summary>
	/// <param name="a_interfaceVersion">The interface version to request</param>
	/// <returns>The pointer to the API singleton, or nullptr if request failed</returns>
	[[nodiscard]] inline void* RequestPluginAPI(const InterfaceVersion a_interfaceVersion = InterfaceVersion::V2)
	{
		auto pluginHandle = GetModuleHandle("ValhallaCombat.dll");
		_RequestPluginAPI requestAPIFunction = (_RequestPluginAPI)GetProcAddress(pluginHandle, "RequestPluginAPI");
		if (requestAPIFunction) {
			return requestAPIFunction(a_interfaceVersion);
		}
		return nullptr;
	}
}
