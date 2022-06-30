#include "include/Hooks.h"
#include "include/Utils.h"
#include "include/blockHandler.h"
#include "ValhallaCombat.hpp"
#include "include/stunHandler.h"
#include "include/hitProcessor.h"
#include "include/AI.h"
#include "include/settings.h"
#include "include/attackHandler.h"
#pragma endregion
#pragma region GetHeavyStaminaCost
float Hook_OnGetAttackStaminaCost::getAttackStaminaCost(uintptr_t avOwner, RE::BGSAttackData* atkData) {
	RE::Actor* a_actor = (RE::Actor*)(avOwner - 0xB0);
	if (!settings::bNonCombatStaminaCost && !a_actor->IsInCombat()) {
		return 0;
	}
	else if (settings::bAttackStaminaToggle) { //negate vanilla heavy attack stamina cost
		RE::Actor* a_actor = (RE::Actor*)(avOwner - 0xB0);
		if (atkData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)
			&& !atkData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) {
			return 0;
		}
	}

	return _getHeavyAttackStaminaCost(avOwner, atkData);
}
#pragma endregion
#pragma region GetBlockChance
uintptr_t Hook_GetBlockChance::getBlockChance(RE::Actor* actor) {
	//DEBUG("hooked blockchance");
	if (debuffHandler::GetSingleton()->actorInDebuff.find(actor) != debuffHandler::GetSingleton()->actorInDebuff.end()) {
		return 0; //disable locking for exhausted actors
	}
	return _getBlockChance(actor);
}
#pragma endregion
#pragma region GetAttackChance
uintptr_t Hook_GetAttackChance1::getAttackChance(RE::Actor* actor, RE::Actor* target, RE::BGSAttackData* atkData) {
	if (settings::bAttackStaminaToggle
		&& debuffHandler::GetSingleton()->isInDebuff(actor)) {
		return 0;
	}
	else {
		return _getAttackChance(actor, target, atkData);
	}
	
}
uintptr_t Hook_GetAttackChance2::getAttackChance(RE::Actor* actor, RE::Actor* target, RE::BGSAttackData* atkData) {
	if (settings::bAttackStaminaToggle
		&& debuffHandler::GetSingleton()->isInDebuff(actor)) {
		return 0;
	}
	else {
		return _getAttackChance(actor, target, atkData);
	}
	
}
#pragma endregion
#pragma region StaminaRegen
/*function generating conditions for stamina regen. Iff returned value is true, no regen.
used to block stamina regen in certain situations.*/
bool Hook_OnStaminaRegen::HasFlags1(RE::ActorState* a_this, uint16_t a_flags)
{
	//if bResult is true, prevents regen.
	bool bResult = _HasFlags1(a_this, a_flags); // is sprinting?

	if (!bResult) {
		RE::Actor* actor = SKSE::stl::adjust_pointer<RE::Actor>(a_this, -0xB8);
		auto attackState = actor->GetAttackState();
		if (actor != attackHandler::GetSingleton()->actorToRegenStamina) {
			//if melee hit regen is needed, no need to disable regen.
			bResult = (attackState > RE::ATTACK_STATE_ENUM::kNone && attackState <= RE::ATTACK_STATE_ENUM::kBowFollowThrough) || actor->IsBlocking(); //when attacking or blocking, doens't regen stmaina.
		}
	}
	return bResult;

}
#pragma endregion

#pragma region getStaggerMagnitude_Weapon
float Hook_OnGetStaggerMagnitude::getStaggerMagnitude_Weapon(RE::ActorValueOwner* a1, RE::ActorValueOwner* a2, RE::TESObjectWEAP* a3, float a4) {
	if (settings::bBalanceToggle) {
		return 0;
	}
	
}

float Hook_OnGetStaggerMagnitude::getStaggerManitude_Bash(uintptr_t a1, uintptr_t a2) {
	if (settings::bBalanceToggle) {
		return 0;
	}
}

#pragma endregion
#pragma region MeleeHit
void Hook_OnPhysicalHit::processHit(RE::Actor* victim, RE::HitData& hitData) {
	//hitDataProcessor::processHitData(hitData);
	
	using HITFLAG = RE::HitData::Flag;
	auto aggressor = hitData.aggressor.get().get();
	if (!victim || !aggressor || victim->IsDead()) {
		_ProcessHit(victim, hitData);
		return;
	}
	hitProcessor::GetSingleton()->processHit(aggressor, victim, hitData);
	_ProcessHit(victim, hitData);
}

#pragma endregion

#pragma region MainUpdate
void Hook_MainUpdate::Update(RE::Main* a_this, float a2) {
	ValhallaCombat::GetSingleton()->update();
	_Update(a_this, a2);
}
#pragma endregion


#pragma region projectileHit
/*Decide whether the collision is a actor-projectile collision. If it is, initialize a deflection attempt by the actor.
@param a_projectile: the projectile to be deflected.
@param a_AllCdPointCollector: pointer to the container storing all collision points.
@return whether a successful deflection is performed by the actor.*/
inline bool initProjectileBlock(RE::Projectile* a_projectile, RE::hkpAllCdPointCollector* a_AllCdPointCollector) {
	if (a_AllCdPointCollector) {
		for (auto& hit : a_AllCdPointCollector->hits) {
			auto refrA = RE::TESHavokUtilities::FindCollidableRef(*hit.rootCollidableA);
			auto refrB = RE::TESHavokUtilities::FindCollidableRef(*hit.rootCollidableB);
			if (refrA && refrA->formType == RE::FormType::ActorCharacter) {
				return blockHandler::GetSingleton()->preProcessProjectileBlock(refrA->As<RE::Actor>(), a_projectile, const_cast<RE::hkpCollidable*>(hit.rootCollidableB));
			}
			if (refrB && refrB->formType == RE::FormType::ActorCharacter) {
				return blockHandler::GetSingleton()->preProcessProjectileBlock(refrB->As<RE::Actor>(), a_projectile, const_cast<RE::hkpCollidable*>(hit.rootCollidableA));
			}
		}
	}
	return false;
}
void Hook_OnProjectileCollision::OnArrowCollision(RE::Projectile* a_this, RE::hkpAllCdPointCollector* a_AllCdPointCollector) {
	//DEBUG("hooked arrow collission vfunc");
	if (initProjectileBlock(a_this, a_AllCdPointCollector)) {
		return;
	};
	_arrowCollission(a_this, a_AllCdPointCollector);
};

void Hook_OnProjectileCollision::OnMissileCollision(RE::Projectile* a_this, RE::hkpAllCdPointCollector* a_AllCdPointCollector) {
	//DEBUG("hooked missile collission vfunc");
	if (initProjectileBlock(a_this, a_AllCdPointCollector)) {
		return;
	};
	_missileCollission(a_this, a_AllCdPointCollector);
}
#pragma endregion

std::int32_t& Hook_GetWantBlock::GetWantBlock(void* unk_ptr, const RE::BSFixedString& a_channelName, std::uint8_t unk_int, RE::Actor* a_actor, std::int32_t& a_result) {
	if (debuffHandler::GetSingleton()->isInDebuff(a_actor)) {
		int32_t i = 0;
		return i;
	}
	return _GetWantBlock(unk_ptr, a_channelName, unk_int, a_actor, a_result);
}
