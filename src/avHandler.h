#pragma once
#include "Utils.h"
using namespace Utils;
namespace avHandler
{

	inline float meleeCost1h = 20;
	inline float meleeCost2h = 35;
	inline float meleeHitStaminaRecover = 50; //stamina recovered from hitting an enemy with a light attack.


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




};

