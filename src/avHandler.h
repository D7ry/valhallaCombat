#pragma once
#include "Utils.h"
#include "dataHandler.h"
using namespace dataHandler;
using namespace Utils;
namespace avHandler
{



	/*causes 1h weapon stamina damage to actor A*/
	inline void damageStamina(RE::Actor* a) {
		if (a->GetEquippedObject(false)->IsWeapon()) {
			RE::WEAPON_TYPE wpnType = a->GetEquippedObject(false)->As<RE::TESObjectWEAP>()->GetWeaponType();
			if (wpnType >= RE::WEAPON_TYPE::kHandToHandMelee && wpnType <= RE::WEAPON_TYPE::kOneHandMace) {
				DEBUG("swinging 1h weapon!");
				damageav(a, RE::ActorValue::kStamina, meleeCost1h);
			}
			else {
				DEBUG("swinging 2h weapon!");
				damageav(a, RE::ActorValue::kStamina, meleeCost2h);
			}
		}
		
	}

	/*restores stamina for actor A*/
	inline void restoreStamina(RE::Actor* a) {
		restoreav(a, RE::ActorValue::kStamina, meleeHitStaminaRecover);
	}

	inline boolean canAttack = true;



};

