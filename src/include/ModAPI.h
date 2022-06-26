#pragma once
#include "include/lib/ValhallaCombatAPI.h"
#include "include/stunHandler.h"
class ModAPI
{
	using APIResult = VAL_API::APIResult;
	using InterfaceVersion1 = VAL_API::IVVAL1;

public:
	class VALInterface : public InterfaceVersion1
	{
	private:
		VALInterface() noexcept {};
		virtual ~VALInterface() noexcept {};

	public:
		static VALInterface* GetSingleton() noexcept
		{
			static VALInterface singleton;
			return std::addressof(singleton);
		}

		// InterfaceVersion1
		virtual void processStunDamage(VAL_API::STUNSOURCE stunSource, RE::TESObjectWEAP* weapon, RE::Actor* aggressor, RE::Actor* victim, float baseDamage) noexcept override;

	private:
		unsigned long apiTID = 0;

	};
};

