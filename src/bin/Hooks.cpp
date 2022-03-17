#include "include/Hooks.h"
#include "include/Utils.h"
#include "include/blockHandler.h"
#include "ValhallaCombat.hpp"
#include "include/stunHandler.h"
#include "include/hitProcessor.h"
#include "include/AI.h"
#include "include/settings.h"
#pragma endregion
#pragma region GetHeavyStaminaCost
float Hook_GetAttackStaminaCost::getAttackStaminaCost(uintptr_t avOwner, RE::BGSAttackData* atkData) {
	DEBUG("hooked attack stamina cost!");
	RE::Actor* a_actor = (RE::Actor*)(avOwner - 0xB0);
	DEBUG("actor is {}", a_actor->GetName());
	if (settings::bAttackStaminaToggle) { //negate vanilla heavy attack stamina cost
		if (atkData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)
			&& !atkData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) {
			return 0;
		}
	}
	return _getHeavyAttackStaminaCost(avOwner, atkData);
}
float Hook_CacheAttackStaminaCost::cacheAttackStaminaCost(uintptr_t avOwner, RE::BGSAttackData* atkData) {
	
	RE::Actor* a_actor = (RE::Actor*)(avOwner - 0xB0);
	DEBUG("Cached attack stamina! Actor is {}.", a_actor->GetName());
	return _cacheAttackStaminaCost(avOwner, atkData);
}
#pragma endregion
#pragma region GetBlockChance
uintptr_t Hook_GetBlockChance::getBlockChance(RE::Actor* actor) {
	//DEBUG("hooked blockchance");
	if (debuffHandler::GetSingleton()->actorDebuffMap.find(actor) != debuffHandler::GetSingleton()->actorDebuffMap.end()) {
		return 0; //disable locking for exhausted actors
	}
	return _getBlockChance(actor);
}
#pragma endregion
#pragma region GetAttackChance
uintptr_t Hook_GetAttackChance1::getAttackChance(RE::Actor* actor, RE::Actor* target, RE::BGSAttackData* atkData) {
	if (settings::bAttackStaminaToggle
		&& debuffHandler::GetSingleton()->isInDebuff(actor)) {
		//DEBUG("attack denied");
		return 0;
	}
	return _getAttackChance(actor, target, atkData);
}
uintptr_t Hook_GetAttackChance2::getAttackChance(RE::Actor* actor, RE::Actor* target, RE::BGSAttackData* atkData) {
	if (settings::bAttackStaminaToggle
		&& debuffHandler::GetSingleton()->isInDebuff(actor)) {
		//DEBUG("attack denied");
		return 0;
	}
	return _getAttackChance(actor, target, atkData);
}
#pragma endregion
#pragma region StaminaRegen
/*function generating conditions for stamina regen. Iff returned value is true, no regen.
used to block stamina regen in certain situations.*/
bool Hook_StaminaRegen::HasFlags1(RE::ActorState* a_this, uint16_t a_flags)
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

#pragma region MeleeHit
void Hook_MeleeHit::processHit(RE::Actor* victim, RE::HitData& hitData) {
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