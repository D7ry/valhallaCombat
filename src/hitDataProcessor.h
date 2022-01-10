#pragma once
#include "dataHandler.h"
#include "attackHandler.h"
using HITFLAG = RE::HitData::Flag;

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
		if (aggressor->IsPlayerRef()) {
			DEBUG("processing player hit!");
			processPlayerHit(weapon, hitFlag);
		}
		if (hitFlag & (int)HITFLAG::kBlocked) {
			DEBUG("hit blocked");
			if (settings::bckToggle) {
				processStaminaDamage(target, hitData, hitFlag);
			}
			return;					//blocked hit, no need to process stun separately.
		}
		if (!target->IsPlayerRef()) {	//player doesn't take stun damage.
			processStunDamage(target, weapon, hitData, hitFlag);
		}
	}
private:
	//reliable blocking
	static void processStaminaDamage(RE::ActorPtr target, RE::HitData& hitData, int hitFlag);

	static void processStunDamage(RE::ActorPtr target, RE::TESObjectWEAP* weapon, RE::HitData& hitData, int hitFlag);

	static void processPlayerHit(RE::TESObjectWEAP* weapon, int hitFlag); //processes player hit and regenerates stamina for player.
};