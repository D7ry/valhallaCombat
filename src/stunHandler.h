#pragma once
#include "dataHandler.h"

namespace stunUtils {
	//RE::Offset::SCRIPT_FUNCTION::FirstConsoleCommand
	
	static inline void ExecuteCommand(std::string a_command)
	{
		const auto scriptFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
		const auto script = scriptFactory ? scriptFactory->Create() : nullptr;
		if (script) {
			const auto selectedRef = RE::Console::GetSelectedRef();
			script->SetCommand(a_command);
			script->CompileAndRun(selectedRef.get());
			delete script;
		}
	}
	
}
/*handling enemy stun meter, damaging stun, as well as execution.*/
class stunHandler {
public:
	/*currently stun meter uses stamina*/

	/*applies stun damage to the actor
	@param baseDamage the total health damage applied onto actor, used as a base for calculation
	@param a_actor actor whose stun meter is to be damaged.
	@param isPowerAtk whether the attack is a power attack. Be ware, power attack itself already has more damage.*/
	static bool stunActor(float baseDamage, RE::ActorPtr a_actor, RE::TESObjectWEAP* weapon, bool isPowerAtk) {
		baseDamage *= settings::stunBaseMult;
		if (isPowerAtk) {
			baseDamage *= settings::stunHvyMult;
		}
		switch (weapon->GetWeaponType())
		{
		case RE::WEAPON_TYPE::kBow: baseDamage *= settings::stunBowMult; break;
		case RE::WEAPON_TYPE::kCrossbow: baseDamage *= settings::stunCrossBowMult; break;
		case RE::WEAPON_TYPE::kHandToHandMelee: baseDamage *= settings::stunUnarmedMult; break;
		case RE::WEAPON_TYPE::kOneHandDagger: baseDamage *= settings::stunDaggerMult; break;
		case RE::WEAPON_TYPE::kOneHandSword: baseDamage *= settings::stunSwordMult; break;
		case RE::WEAPON_TYPE::kOneHandAxe: baseDamage *= settings::stunAxeMult; break;
		case RE::WEAPON_TYPE::kOneHandMace: baseDamage *= settings::stunMaceMult; break;
		case RE::WEAPON_TYPE::kTwoHandAxe: baseDamage *= settings::stun2hwMult; break;
		case RE::WEAPON_TYPE::kTwoHandSword: baseDamage *= settings::stunGreatSwordMult; break;
		default: break;
		}
		Utils::damageav(a_actor.get(), RE::ActorValue::kStamina, baseDamage);
		return true;
	}
	static bool isStunMeterEmpty(RE::ActorPtr a_actor) {
		return a_actor->GetActorValue(RE::ActorValue::kStamina) <= 0;
	}

	static void execute(RE::ActorPtr aggressor, RE::ActorPtr target) {
		DEBUG("{} has executed {}!", aggressor->GetName(), target->GetName());
		//target->push
		//Utils::push::pushActor(aggressor, target, std::to_string(3));
		reFillStun(target);
	}

	/*refills actor's stun meter*/
	static void reFillStun(RE::ActorPtr a_actor) {
		Utils::restoreav(a_actor.get(), RE::ActorValue::kStamina, a_actor->GetPermanentActorValue(RE::ActorValue::kStamina));
		DEBUG("{}'s stun refilled.", a_actor->GetName());
	}
};