#include "include/executionHandler.h"
#include "include/data.h"
#include "include/settings.h"
#include "include/stunHandler.h"
#include "include/Utils.h"
#define DATA data::GetSingleton()
using namespace Utils;
void executionHandler::tryPcExecution() {

	auto possibleTargets = stunHandler::GetSingleton()->stunnedActors;
	auto pc = RE::PlayerCharacter::GetSingleton();
	if (!pc) {
		return;
	}
	RE::Actor* optimalVictim = nullptr; //optimal target to execute
	float minRange = -1; //minimum range to the optimal victim so far

	for (auto& actor : possibleTargets) {
		if (!actor || !actor->Is3DLoaded() || actor->IsDead() || 
			!actor->currentProcess || !actor->currentProcess->InHighProcess()) {
			//remove actor from stunned map.
			stunHandler::GetSingleton()->untrackStun(actor);
		}
		auto range = ValhallaUtils::getInRange(pc, actor, 250);
		if (range > 0) { //actor is in range.
			if (minRange < 0  //min range not set yet
				|| minRange > range) {
				minRange = range;
				optimalVictim = actor;
			}
		}
	}

	if (optimalVictim) {
		attemptExecute(pc, optimalVictim);
	}
}
void executionHandler::attemptExecute(RE::Actor* a_executor, RE::Actor* a_victim) {
	//INFO("attempting to execute {}, executor: {}", victim->GetName(), executor->GetName());

	//check if victim can be executed
	if (!settings::bStunToggle
		|| a_executor->IsDead() || a_victim->IsDead()
		|| !a_executor->Is3DLoaded() || !a_victim->Is3DLoaded()
		|| a_executor->IsInKillMove() || a_victim->IsInKillMove()
		|| a_executor->IsOnMount() || a_victim->IsOnMount()
		|| a_victim->IsPlayerRef() || a_victim->IsPlayerTeammate()
		|| a_victim->IsEssential() || a_victim->IsInKillMove()
		|| a_victim->HasEffectWithArchetype(RE::MagicTarget::Archetype::kParalysis)) {
		//INFO("Execution preconditions not met, terminating execution.");
		return;
	}

	auto executorRace = a_executor->GetRace();
	auto victimRace = a_victim->GetRace();
	if (!executorRace || !victimRace) {
		//INFO("race not found, terminating execution");
		return;
	}

	auto it1 = data::raceMapping.find(executorRace);
	if (it1 == data::raceMapping.end()) {
		//INFO("race not found, terminating execution");
		return;
	}
	if (it1->second != data::raceCatagory::Humanoid) {
		//INFO("executor is not human");
		return;
	}
	
	auto it2 = data::raceMapping.find(victimRace);
	if (it2 == data::raceMapping.end()) {
		return;
	}
	auto victimRaceType = it2->second;

	RE::WEAPON_TYPE weaponType;
	auto weapon = a_executor->getWieldingWeapon();
	if (!weapon) {
		//DEBUG("Executor weapon not found, using unarmed as weapon type.");
		weaponType = RE::WEAPON_TYPE::kHandToHandMelee;
	}
	else {
		weaponType = weapon->GetWeaponType();
	}

	if (weaponType == RE::WEAPON_TYPE::kBow || weaponType == RE::WEAPON_TYPE::kCrossbow) {
		return;
	}

	//INFO("weapon type is {}", weaponType);
#define RACE data::raceCatagory
	switch (victimRaceType) {
	case RACE::Humanoid: executeHumanoid(a_executor, a_victim, weaponType); break;
	case RACE::Undead: executeDraugr(a_executor, a_victim, weaponType); break;
	case RACE::Falmer: executeFalmer(a_executor, a_victim, weaponType); break;
	case RACE::Spider: executeSpider(a_executor, a_victim, weaponType); break;
	case RACE::Gargoyle: executeGargoyle(a_executor, a_victim, weaponType); break;
	case RACE::Giant: executeGiant(a_executor, a_victim, weaponType); break;
	case RACE::Bear: executeBear(a_executor, a_victim, weaponType); break;
	case RACE::SabreCat: executeSabreCat(a_executor, a_victim, weaponType); break;
	case RACE::Wolf: executeWolf(a_executor, a_victim, weaponType); break;
	case RACE::Troll: executeTroll(a_executor, a_victim, weaponType); break;
	case RACE::Boar: executeBoar(a_executor, a_victim, weaponType); break;
	case RACE::Hagraven: executeHagraven(a_executor, a_victim, weaponType); break;
	case RACE::Spriggan: executeSpriggan(a_executor, a_victim, weaponType); break;
	case RACE::Riekling: executeRiekling(a_executor, a_victim, weaponType); break;
	case RACE::AshHopper: executeAshHopper(a_executor, a_victim, weaponType); break;
	case RACE::SteamCenturion: executeSteamCenturion(a_executor, a_victim, weaponType); break;
	case RACE::DwarvenBallista: executeDwarvenBallista(a_executor, a_victim, weaponType); break;
	case RACE::ChaurusFlyer: executeChaurusFlyer(a_executor, a_victim, weaponType); break;
	case RACE::Lurker: executeLurker(a_executor, a_victim, weaponType); break;
	case RACE::Dragon: executeDragon(a_executor, a_victim, weaponType); break;
	default: return; //iff no body part match, no need to set ghost.
	}

	a_victim->SetGraphVariableBool("bIdlePlaying", true); //DHAF compatibility
};

void executionHandler::concludeExecution(RE::Actor* a_executor) {
	if (!a_executor) {
		return;
	}

	if (executionMap.find(a_executor) != executionMap.end()) {
		if (a_executor->IsPlayerRef()) {
			RE::DebugNotification("Concluding player execution");
		}
		Utils::setIsGhost(a_executor, false);
		auto victim = executionMap.find(a_executor)->second;
		if (victim) {
			Utils::setIsGhost(victim, false);
		}
		executionMap.erase(a_executor);
	}
}


void executionHandler::playExecutionIdle(RE::Actor* a_executor, RE::Actor* a_victim, RE::TESIdleForm* a_executionIdle) {
	//INFO("playing execution idle: {}", executionIdle->GetFormID());
	playPairedIdle(a_executor->currentProcess, a_executor, RE::DEFAULT_OBJECT::kActionIdle, a_executionIdle, true, false, a_victim);
}

void executionHandler::playExecutionIdle(RE::Actor* a_executor, RE::Actor* a_victim, std::vector<RE::TESIdleForm*> a_executionIdleVector) {
	//INFO("playing execution idle!");
	if (a_executionIdleVector.size() == 0) {
		INFO("error: no idle present in vector");
		return;
	}
	auto idle = *Utils::select_randomly(a_executionIdleVector.begin(), a_executionIdleVector.end());
	if (!idle) {
		INFO("Error! no idle received");
	}
	else {
		//INFO("received idle with name {}", idle->GetFormID());
		playExecutionIdle(a_executor, a_victim, idle);
	}

}


void executionHandler::executeHumanoid(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE a_weaponType) {
	//INFO("executing humanoid!");
	if (a_executor->isDualWielding()) {
		//INFO("dual wielding!");
		playExecutionIdle(a_executor, a_victim, data::KM_Humanoid_DW);
	}
	else if (isBackFacing(a_victim, a_executor)) {
		//INFO("backstab!");
		switch (a_weaponType) {
		case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_Humanoid_2hw_Back); break;
		case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Humanoid_2hm_Back); break;
		case RE::WEAPON_TYPE::kHandToHandMelee: playExecutionIdle(a_executor, a_victim, data::KM_Humanoid_H2H_Back); break;
		default: playExecutionIdle(a_executor, a_victim, data::KM_Humanoid_1hm_Back); break;
		}
	}
	else {
		switch (a_weaponType) {
		case RE::WEAPON_TYPE::kOneHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Humanoid_Sword); break;
		case RE::WEAPON_TYPE::kOneHandDagger: playExecutionIdle(a_executor, a_victim, data::KM_Humanoid_Dagger); break;
		case RE::WEAPON_TYPE::kOneHandAxe:  playExecutionIdle(a_executor, a_victim, data::KM_Humanoid_Axe); break;
		case RE::WEAPON_TYPE::kOneHandMace:  playExecutionIdle(a_executor, a_victim, data::KM_Humanoid_Mace); break;
		case RE::WEAPON_TYPE::kTwoHandAxe:  playExecutionIdle(a_executor, a_victim, data::KM_Humanoid_2hw); break;
		case RE::WEAPON_TYPE::kTwoHandSword:  playExecutionIdle(a_executor, a_victim, data::KM_Humanoid_GreatSword); break;
		case RE::WEAPON_TYPE::kHandToHandMelee:  playExecutionIdle(a_executor, a_victim, data::KM_Humanoid_H2H); break;
		}
	}
};
void executionHandler::executeDraugr(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_Undead_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Undead_2hm); break;
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_Undead_1hm); break;
	}
};
void executionHandler::executeFalmer(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_Falmer_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Falmer_2hm); break;
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_Falmer_1hm);
	}
};
void executionHandler::executeSpider(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_Spider_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Spider_2hm); break;
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_Spider_1hm);
	}
}
void executionHandler::executeGargoyle(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_Gargoyle_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Gargoyle_2hm); break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_Gargoyle_1hm); break;
	}
}
void executionHandler::executeGiant(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_Giant_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Giant_2hm); break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_Giant_1hm); break;
	}
}
void executionHandler::executeBear(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_Bear_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Bear_2hm); break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_Bear_1hm); break;
	}
}
void executionHandler::executeSabreCat(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_SabreCat_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_SabreCat_2hm); break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_SabreCat_1hm); break;
	}
}
void executionHandler::executeWolf(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_Wolf_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Wolf_2hm); break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_Wolf_1hm); break;
	}
}
void executionHandler::executeTroll(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_Troll_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Troll_2hm); break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_Troll_1hm); break;
	}
}
void executionHandler::executeHagraven(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_Hagraven_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Hagraven_2hm);  break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_Hagraven_1hm); break;
	}
}
void executionHandler::executeSpriggan(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_Spriggan_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Spriggan_2hm); break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_Spriggan_1hm); break;
	}
}
void executionHandler::executeBoar(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_Boar_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Boar_2hm); break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_Boar_1hm); break;
	}
}
void executionHandler::executeRiekling(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_Riekling_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Riekling_2hm); break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_Riekling_1hm); break;
	}
}
void executionHandler::executeAshHopper(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_AshHopper_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_AshHopper_2hm); break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_AshHopper_1hm); break;
	}
}
void executionHandler::executeSteamCenturion(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_Centurion_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Centurion_2hm); break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_Centurion_1hm); break;
	}
}
void executionHandler::executeDwarvenBallista(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_Ballista_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Ballista_2hm); break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_Ballista_1hm); break;
	}
}
void executionHandler::executeChaurusFlyer(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_ChaurusFlyer_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_ChaurusFlyer_2hm); break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_ChaurusFlyer_1hm); break;
	}
}
void executionHandler::executeLurker(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_Lurker_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Lurker_2hm); break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_Lurker_1hm); break;
	}
}
void executionHandler::executeDragon(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(a_executor, a_victim, data::KM_Dragon_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(a_executor, a_victim, data::KM_Dragon_2hm); break;
	default: playExecutionIdle(a_executor, a_victim, data::KM_Dragon_1hm); break;
	}
}

