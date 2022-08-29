#include "include/Hooks.h"
#include "include/Utils.h"
#include "include/blockHandler.h"
#include "ValhallaCombat.hpp"
#include "include/stunHandler.h"
#include "include/hitProcessor.h"
#include "include/AI.h"
#include "include/settings.h"
#include "include/attackHandler.h"
namespace Hooks
{
#pragma region GetHeavyStaminaCost
	float Hook_OnGetAttackStaminaCost::getAttackStaminaCost(uintptr_t avOwner, RE::BGSAttackData* atkData)
	{
		RE::Actor* a_actor = (RE::Actor*)(avOwner - 0xB0);
		if (!settings::bNonCombatStaminaCost && !a_actor->IsInCombat()) {
			return 0;
		}
		return _getAttackStaminaCost(avOwner, atkData);
	}
#pragma endregion

#pragma region StaminaRegen
	/*function generating conditions for stamina regen. Iff returned value is true, no regen.
used to block stamina regen in certain situations.*/
	bool Hook_OnCheckStaminaRegenCondition::HasFlags1(RE::ActorState* a_this, uint16_t a_flags)
	{
		//if bResult is true, prevents regen.
		bool bResult = _HasFlags1(a_this, a_flags);  // is sprinting?

		if (!bResult) {
			RE::Actor* actor = SKSE::stl::adjust_pointer<RE::Actor>(a_this, -0xB8);
			auto attackState = actor->GetAttackState();
			if (actor != attackHandler::GetSingleton()->actorToRegenStamina) {
				//if melee hit regen is needed, no need to disable regen.
				bResult = (attackState > RE::ATTACK_STATE_ENUM::kNone && attackState <= RE::ATTACK_STATE_ENUM::kBowFollowThrough);  //don't regen stamina if attacking
			}
		}
		return bResult;
	}

	void Hook_OnRestoreActorValue::RestoreActorValue(RE::Actor* a_actor, RE::ActorValue a_actorValue, float a_val)
	{
		switch (a_actorValue) {
		case RE::ActorValue::kStamina:
			if (a_actor->IsBlocking()) {
				a_val *= settings::fBlockingStaminaRegenMult;
			}
			break;
		case RE::ActorValue::kHealth:
			stunHandler::GetSingleton()->modStun(a_actor, a_val);//stun regen on health regen
		}

		_RestoreActorValue(a_actor, a_actorValue, a_val);
	}
#pragma endregion

//#pragma region getStaggerMagnitude_Weapon
//	float Hook_OnGetStaggerMagnitude::getStaggerMagnitude_Weapon(RE::ActorValueOwner* a1, RE::ActorValueOwner* a2, RE::TESObjectWEAP* a3, float a4)
//	{
//		if (settings::bBalanceToggle) {
//			return 0;
//		}
//		return _getStaggerMagnitude_Weapon(a1, a2, a3, a4);
//	}
//
//	float Hook_OnGetStaggerMagnitude::getStaggerManitude_Bash(uintptr_t a1, uintptr_t a2)
//	{
//		if (settings::bBalanceToggle) {
//			return 0;
//		}
//		return _getStaggerManitude_Bash(a1, a2);
//
//	}

#pragma endregion
#pragma region MeleeHit
	void Hook_OnMeleeHit::processHit(RE::Actor* victim, RE::HitData& hitData)
	{
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

//#pragma region MainUpdate
//	void Hook_MainUpdate::Update(RE::Main* a_this, float a2)
//	{
//		ValhallaCombat::GetSingleton()->update();
//		_Update(a_this, a2);
//	}
//#pragma endregion

#pragma region projectileHit
	/*Decide whether the collision is a actor-projectile collision. If it is, initialize a deflection attempt by the actor.
@param a_projectile: the projectile to be deflected.
@param a_AllCdPointCollector: pointer to the container storing all collision points.
@return whether a successful deflection is performed by the actor.*/
	inline bool shouldIgnoreHit(RE::Projectile* a_projectile, RE::hkpAllCdPointCollector* a_AllCdPointCollector)
	{
		if (a_AllCdPointCollector) {
			for (auto& hit : a_AllCdPointCollector->hits) {
				auto refrA = RE::TESHavokUtilities::FindCollidableRef(*hit.rootCollidableA);
				auto refrB = RE::TESHavokUtilities::FindCollidableRef(*hit.rootCollidableB);
				if (refrA && refrA->formType == RE::FormType::ActorCharacter) {
					return blockHandler::GetSingleton()->processProjectileBlock(refrA->As<RE::Actor>(), a_projectile, const_cast<RE::hkpCollidable*>(hit.rootCollidableB));
				}
				if (refrB && refrB->formType == RE::FormType::ActorCharacter) {
					return blockHandler::GetSingleton()->processProjectileBlock(refrB->As<RE::Actor>(), a_projectile, const_cast<RE::hkpCollidable*>(hit.rootCollidableA));
				}
			}
		}
		return false;
	}
	void Hook_OnProjectileCollision::OnArrowCollision(RE::Projectile* a_this, RE::hkpAllCdPointCollector* a_AllCdPointCollector)
	{
		//DEBUG("hooked arrow collission vfunc");
		if (shouldIgnoreHit(a_this, a_AllCdPointCollector)) {
			return;
		};
		_arrowCollission(a_this, a_AllCdPointCollector);
	};

	void Hook_OnProjectileCollision::OnMissileCollision(RE::Projectile* a_this, RE::hkpAllCdPointCollector* a_AllCdPointCollector)
	{
		//DEBUG("hooked missile collission vfunc");
		if (shouldIgnoreHit(a_this, a_AllCdPointCollector)) {
			return;
		};
		_missileCollission(a_this, a_AllCdPointCollector);
	}
#pragma endregion

	void Hook_OnMeleeCollision::processHit(RE::Actor* a_aggressor, RE::Actor* a_victim, std::int64_t a_int1, bool a_bool, void* a_unkptr)
	{
		if (settings::bTimedBlockToggle && blockHandler::GetSingleton()->processMeleeTimedBlock(a_victim, a_aggressor)) {
			return;
		}
		if (settings::bTackleToggle && blockHandler::GetSingleton()->processMeleeTackle(a_victim, a_aggressor)) {
			return;
		}
		_ProcessHit(a_aggressor, a_victim, a_int1, a_bool, a_unkptr);
	}

	/*Check if the attack action should be performed depending on the actor's debuff state.*/
	bool Hook_OnAttackAction::PerformAttackAction(RE::TESActionData* a_actionData)
	{
		if (!settings::bStaminaDebuffToggle) {
			return _PerformAttackAction(a_actionData);
		}
		if (!a_actionData->Subject_8) {
			return _PerformAttackAction(a_actionData);
		}

		RE::Actor* actor = a_actionData->Subject_8->As<RE::Actor>();

		if (debuffHandler::GetSingleton()->isInDebuff(actor)) {
			return false;
		}

		return _PerformAttackAction(a_actionData);
	}



}
