#include "Hooks.h"
#include "Utils.h"
#include "blockHandler.h"
#include "ValhallaCombat.hpp"
#include "stunHandler.h"
#include "hitProcessor.h"
#include "AI.h"
#pragma endregion
#pragma region GetHeavyStaminaCost
float Hook_GetAttackStaminaCost::getAttackStaminaCost(uintptr_t avOwner, RE::BGSAttackData* atkData) {
	DEBUG("hooked attack stamina cost!");
	RE::Actor* a_actor = (RE::Actor*)(avOwner - 0xB0);
	DEBUG("actor is {}", a_actor->GetName());
	if (atkData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack) 
		&& !atkData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) {
		return 0;
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
	//DEBUG("hooked getattackchance1");
	//DEBUG("actor 1: {}, actor2: {}", actor->GetName(), target->GetName());
	if (debuffHandler::GetSingleton()->isInDebuff(actor)) {
		//DEBUG("attack denied");
		return 0;
	}
	return _getAttackChance(actor, target, atkData);
}
uintptr_t Hook_GetAttackChance2::getAttackChance(RE::Actor* actor, RE::Actor* target, RE::BGSAttackData* atkData) {
	//DEBUG("hooked getattackchance2");
	//DEBUG("actor 1: {}, actor2: {}", actor->GetName(), target->GetName());
	if (debuffHandler::GetSingleton()->isInDebuff(actor)) {
		//DEBUG("attack denied");
		return 0;
	}
	return _getAttackChance(actor, target, atkData);
}
#pragma endregion
#pragma region StaminaRegen
/*function generating conditions for stamina regen. Iff returned value is true, no regen.
used to block stamina regen in certain situations.*/
float Hook_StaminaRegen::GetStaminaRateMult(RE::ActorValueOwner* a_this, RE::ActorValue a_aKValue)
{
	RE::Actor* actor = SKSE::stl::adjust_pointer<RE::Actor>(a_this, -0xB0);
	auto atkState = actor->GetAttackState();
	if (RE::ATTACK_STATE_ENUM::kNone < atkState && atkState < RE::ATTACK_STATE_ENUM::kBash
		&& actor != attackHandler::GetSingleton()->actorToRegenStamina) {
		return 0;
	}
	if (actor->IsBlocking()) {
		return _GetStaminaRateMult(a_this, a_aKValue) * 0.1;
	}
	return _GetStaminaRateMult(a_this, a_aKValue);

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