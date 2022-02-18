#pragma once
#include "data.h"
#include "attackHandler.h"
#include "blockHandler.h"
using HITFLAG = RE::HitData::Flag;
class hitProcessor {
public:
	static hitProcessor* GetSingleton()
	{
		static hitProcessor singleton;
		return  std::addressof(singleton);
	}

	/*Process a valid hit passed from hook.*/
	void processHit(RE::Actor* aggressor, RE::Actor* victim, RE::HitData& hitData);
};
/*
class hitDataProcessor {
public:
	static void processHitData(RE::HitData& hitData) {
		int hitFlag = (int)hitData.flags;
		RE::TESObjectWEAP* weapon = hitData.weapon;
		RE::ActorPtr target;
		RE::ActorPtr aggressor;
		DEBUG("processing hit.");
		//checks completeness of hitData
		if (hitData.aggressor && hitData.target) {
			target = hitData.target.get();
			aggressor = hitData.aggressor.get();
			if (target && aggressor) {
				DEBUG("hit flag is {}", hitFlag);
				if (!weapon) {
					DEBUG("weapon not found!");	//weird bug, sometimes weapon is not found for the hitData.
					return;
				}
				DEBUG("WEAPON  is{}", weapon->GetName());
				DEBUG("target is {}", target->GetName());
				DEBUG("aggressor is {}", aggressor->GetName());
			}//prints log
			else {
				DEBUG("missing hit data");
				return;
			}
		}
		else {
			DEBUG("missing hit data");
			return;
		}
		DEBUG("total damage is {}", hitData.totalDamage);
		DEBUG("health damage is {}", hitData.healthDamage);
		DEBUG("physical damage is {}", hitData.physicalDamage);
		DEBUG("resisted physical damage is {}", hitData.resistedPhysicalDamage);
		DEBUG("reflected damage is {}", hitData.reflectedDamage);
		DEBUG("resisted type damage is {}", hitData.resistedTypedDamage);
		DEBUG("bonus health damage mult is {}", hitData.bonusHealthDamageMult);
		DEBUG("critical damage mult is {}", hitData.criticalDamageMult);
		DEBUG("targeted limb damage is {}", hitData.targetedLimbDamage);
		//perform fitting operations
		if (aggressor->IsPlayerRef() && !target->IsDead()) {
			DEBUG("processing player hit!");
			processPlayerHit(weapon, hitFlag); //for player stamina calculation
			DEBUG("player hit processed");
			//DEBUG("notifying animation graph!");
			//target->NotifyAnimationGraph("KillMoveB");
			//aggressor->NotifyAnimationGraph("PA_KillMoveB");
			//DEBUG("finished grappling!");
		}
		bool isPlayerTarget = target->IsPlayerRef();
		if (hitFlag & (int)HITFLAG::kBlocked) {
			DEBUG("hit blocked");
			if (settings::bckToggle) {
				processStaminaDamage(target, hitData, hitFlag, isPlayerTarget);
			}
			return;					//blocked hit, no need to process stun separately.
		}
		if (!isPlayerTarget) {	//player doesn't take stun damage.
			processStunDamage(aggressor, target, weapon, hitData, hitFlag);
		}
	}
private:
	//reliable blocking
	static void processStaminaDamage(RE::ActorPtr target, RE::HitData& hitData, int hitFlag, bool isPlayerTarget);

	static void damageStaminaPlayer();

	static void damageStaminaNPC();

	static void processStunDamage(RE::ActorPtr aggressor, RE::ActorPtr target, RE::TESObjectWEAP* weapon, RE::HitData& hitData, int hitFlag);

	static void processPlayerHit(RE::TESObjectWEAP* weapon, int hitFlag); //processes player hit and regenerates stamina for player.
};
*/