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

	// Available True Directional Movement interface versions
	enum class InterfaceVersion : uint8_t
	{
		V1,
	};

	// Error types that may be returned by the True Directional Movement API
	enum class APIResult : uint8_t
	{
		// Your API call was successful
		OK,

		// You tried to release a resource that was not allocated to you
		// Do not attempt to manipulate the requested resource if you receive this response
		NotOwner,

		// True Directional Movement currently must keep control of this resource for proper functionality
		// Do not attempt to manipulate the requested resource if you receive this response
		MustKeep,

		// You have already been given control of this resource
		AlreadyGiven,

		// Another mod has been given control of this resource at the present time
		// Do not attempt to manipulate the requested resource if you receive this response
		AlreadyTaken,

		// You sent a command on a thread that could cause a data race were it to be processed
		// Do not attempt to manipulate the requested resource if you receive this response
		BadThread,
	};

	enum STUNSOURCE
	{
		lightAttack,
		powerAttack,
		bash,
		powerBash,
		timedBlock,
		parry
	};

	// True Directional Movement's modder interface
	class IVVAL1
	{
	public:
		virtual void processStunDamage(STUNSOURCE stunSource, RE::TESObjectWEAP* weapon, RE::Actor* aggressor, RE::Actor* victim, float baseDamage) noexcept = 0;
	};


	typedef void* (*_RequestPluginAPI)(const InterfaceVersion interfaceVersion);

	/// <summary>
	/// Request the True Directional Movement API interface.
	/// Recommended: Send your request during or after SKSEMessagingInterface::kMessage_PostLoad to make sure the dll has already been loaded
	/// </summary>
	/// <param name="a_interfaceVersion">The interface version to request</param>
	/// <returns>The pointer to the API singleton, or nullptr if request failed</returns>
	[[nodiscard]] inline void* RequestPluginAPI(const InterfaceVersion a_interfaceVersion = InterfaceVersion::V1)
	{
		auto pluginHandle = GetModuleHandle("ValhallaCombat.dll");
		_RequestPluginAPI requestAPIFunction = (_RequestPluginAPI)GetProcAddress(pluginHandle, "RequestPluginAPI");
		if (requestAPIFunction) {
			return requestAPIFunction(a_interfaceVersion);
		}
		return nullptr;
	}
}
