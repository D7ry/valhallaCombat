#include "include/executionHandler.h"
#include "include/data.h"
#include "include/settings.h"
#include "include/stunHandler.h"
#include "include/Utils.h"
#define DATA data::GetSingleton()
using namespace Utils;
void executionHandler::getExecutableTarget() {

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
void executionHandler::attemptExecute(RE::Actor* executor, RE::Actor* victim) {
	//INFO("attempting to execute {}, executor: {}", victim->GetName(), executor->GetName());

	//check if victim can be executed
	if (!settings::bStunToggle
		|| executor->IsDead() || victim->IsDead()
		|| !executor->Is3DLoaded() || !victim->Is3DLoaded()
		|| executor->IsInKillMove() || victim->IsInKillMove()
		|| executor->IsOnMount() || victim->IsOnMount()
		|| victim->IsPlayerRef() || victim->IsPlayerTeammate()
		|| victim->IsEssential() || victim->IsInKillMove()
		|| victim->HasEffectWithArchetype(RE::MagicTarget::Archetype::kParalysis)) {
		//INFO("Execution preconditions not met, terminating execution.");
		return;
	}

	auto executorRace = executor->GetRace();
	auto victimRace = victim->GetRace();
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
	auto weapon = executor->getWieldingWeapon();
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
	case RACE::Humanoid: executeHumanoid(executor, victim, weaponType); break;
	case RACE::Undead: executeDraugr(executor, victim, weaponType); break;
	case RACE::Falmer: executeFalmer(executor, victim, weaponType); break;
	case RACE::Spider: executeSpider(executor, victim, weaponType); break;
	case RACE::Gargoyle: executeGargoyle(executor, victim, weaponType); break;
	case RACE::Giant: executeGiant(executor, victim, weaponType); break;
	case RACE::Bear: executeBear(executor, victim, weaponType); break;
	case RACE::SabreCat: executeSabreCat(executor, victim, weaponType); break;
	case RACE::Wolf: executeWolf(executor, victim, weaponType); break;
	case RACE::Troll: executeTroll(executor, victim, weaponType); break;
	case RACE::Boar: executeBoar(executor, victim, weaponType); break;
	case RACE::Hagraven: executeHagraven(executor, victim, weaponType); break;
	case RACE::Spriggan: executeSpriggan(executor, victim, weaponType); break;
	case RACE::Riekling: executeRiekling(executor, victim, weaponType); break;
	case RACE::AshHopper: executeAshHopper(executor, victim, weaponType); break;
	case RACE::SteamCenturion: executeSteamCenturion(executor, victim, weaponType); break;
	case RACE::DwarvenBallista: executeDwarvenBallista(executor, victim, weaponType); break;
	case RACE::ChaurusFlyer: executeChaurusFlyer(executor, victim, weaponType); break;
	case RACE::Lurker: executeLurker(executor, victim, weaponType); break;
	case RACE::Dragon: executeDragon(executor, victim, weaponType); break;
	default: return; //iff no body part match, no need to set ghost.
	}

	victim->SetGraphVariableBool("bIdlePlaying", true); //DHAF compatibility
};

void executionHandler::concludeExecution(RE::Actor* executor) {
	if (!executor) {
		return;
	}

	if (executionMap.find(executor) != executionMap.end()) {
		if (executor->IsPlayerRef()) {
			RE::DebugNotification("Concluding player execution");
		}
		Utils::setIsGhost(executor, false);
		auto victim = executionMap.find(executor)->second;
		if (victim) {
			Utils::setIsGhost(victim, false);
		}
		executionMap.erase(executor);
	}
}


void executionHandler::playExecutionIdle(RE::Actor* executor, RE::Actor* victim, RE::TESIdleForm* executionIdle) {
	//INFO("playing execution idle: {}", executionIdle->GetFormID());
	playPairedIdle(executor->currentProcess, executor, RE::DEFAULT_OBJECT::kActionIdle, executionIdle, true, false, victim);
}

void executionHandler::playExecutionIdle(RE::Actor* executor, RE::Actor* victim, std::vector<RE::TESIdleForm*> executionIdleV) {
	//INFO("playing execution idle!");
	if (executionIdleV.size() == 0) {
		INFO("error: no idle present in vector");
		return;
	}
	auto idle = *Utils::select_randomly(executionIdleV.begin(), executionIdleV.end());
	if (!idle) {
		INFO("Error! no idle received");
	}
	else {
		//INFO("received idle with name {}", idle->GetFormID());
		playExecutionIdle(executor, victim, idle);
	}

}


void executionHandler::executeHumanoid(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	//INFO("executing humanoid!");
	if (executor->isDualWielding()) {
		//INFO("dual wielding!");
		playExecutionIdle(executor, victim, data::KM_Humanoid_DW);
	}
	else if (isBackFacing(victim, executor)) {
		//INFO("backstab!");
		switch (weaponType) {
		case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_Humanoid_2hw_Back); break;
		case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_Humanoid_2hm_Back); break;
		case RE::WEAPON_TYPE::kHandToHandMelee: playExecutionIdle(executor, victim, data::KM_Humanoid_H2H_Back); break;
		default: playExecutionIdle(executor, victim, data::KM_Humanoid_1hm_Back); break;
		}
	}
	else {
		switch (weaponType) {
		case RE::WEAPON_TYPE::kOneHandSword: playExecutionIdle(executor, victim, data::KM_Humanoid_Sword); break;
		case RE::WEAPON_TYPE::kOneHandDagger: playExecutionIdle(executor, victim, data::KM_Humanoid_Dagger); break;
		case RE::WEAPON_TYPE::kOneHandAxe:  playExecutionIdle(executor, victim, data::KM_Humanoid_Axe); break;
		case RE::WEAPON_TYPE::kOneHandMace:  playExecutionIdle(executor, victim, data::KM_Humanoid_Mace); break;
		case RE::WEAPON_TYPE::kTwoHandAxe:  playExecutionIdle(executor, victim, data::KM_Humanoid_2hw); break;
		case RE::WEAPON_TYPE::kTwoHandSword:  playExecutionIdle(executor, victim, data::KM_Humanoid_GreatSword); break;
		case RE::WEAPON_TYPE::kHandToHandMelee:  playExecutionIdle(executor, victim, data::KM_Humanoid_H2H); break;
		}
	}
};
void executionHandler::executeDraugr(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_Undead_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_Undead_2hm); break;
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	default: playExecutionIdle(executor, victim, data::KM_Undead_1hm); break;
	}
};
void executionHandler::executeFalmer(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_Falmer_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_Falmer_2hm); break;
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	default: playExecutionIdle(executor, victim, data::KM_Falmer_1hm);
	}
};
void executionHandler::executeSpider(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_Spider_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_Spider_2hm); break;
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	default: playExecutionIdle(executor, victim, data::KM_Spider_1hm);
	}
}
void executionHandler::executeGargoyle(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_Gargoyle_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_Gargoyle_2hm); break;
	default: playExecutionIdle(executor, victim, data::KM_Gargoyle_1hm); break;
	}
}
void executionHandler::executeGiant(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_Giant_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_Giant_2hm); break;
	default: playExecutionIdle(executor, victim, data::KM_Giant_1hm); break;
	}
}
void executionHandler::executeBear(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_Bear_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_Bear_2hm); break;
	default: playExecutionIdle(executor, victim, data::KM_Bear_1hm); break;
	}
}
void executionHandler::executeSabreCat(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_SabreCat_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_SabreCat_2hm); break;
	default: playExecutionIdle(executor, victim, data::KM_SabreCat_1hm); break;
	}
}
void executionHandler::executeWolf(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_Wolf_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_Wolf_2hm); break;
	default: playExecutionIdle(executor, victim, data::KM_Wolf_1hm); break;
	}
}
void executionHandler::executeTroll(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_Troll_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_Troll_2hm); break;
	default: playExecutionIdle(executor, victim, data::KM_Troll_1hm); break;
	}
}
void executionHandler::executeHagraven(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_Hagraven_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_Hagraven_2hm);  break;
	default: playExecutionIdle(executor, victim, data::KM_Hagraven_1hm); break;
	}
}
void executionHandler::executeSpriggan(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_Spriggan_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_Spriggan_2hm); break;
	default: playExecutionIdle(executor, victim, data::KM_Spriggan_1hm); break;
	}
}
void executionHandler::executeBoar(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_Boar_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_Boar_2hm); break;
	default: playExecutionIdle(executor, victim, data::KM_Boar_1hm); break;
	}
}
void executionHandler::executeRiekling(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_Riekling_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_Riekling_2hm); break;
	default: playExecutionIdle(executor, victim, data::KM_Riekling_1hm); break;
	}
}
void executionHandler::executeAshHopper(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_AshHopper_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_AshHopper_2hm); break;
	default: playExecutionIdle(executor, victim, data::KM_AshHopper_1hm); break;
	}
}
void executionHandler::executeSteamCenturion(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_Centurion_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_Centurion_2hm); break;
	default: playExecutionIdle(executor, victim, data::KM_Centurion_1hm); break;
	}
}
void executionHandler::executeDwarvenBallista(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_Ballista_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_Ballista_2hm); break;
	default: playExecutionIdle(executor, victim, data::KM_Ballista_1hm); break;
	}
}
void executionHandler::executeChaurusFlyer(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_ChaurusFlyer_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_ChaurusFlyer_2hm); break;
	default: playExecutionIdle(executor, victim, data::KM_ChaurusFlyer_1hm); break;
	}
}
void executionHandler::executeLurker(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_Lurker_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_Lurker_2hm); break;
	default: playExecutionIdle(executor, victim, data::KM_Lurker_1hm); break;
	}
}
void executionHandler::executeDragon(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, data::KM_Dragon_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, data::KM_Dragon_2hm); break;
	default: playExecutionIdle(executor, victim, data::KM_Dragon_1hm); break;
	}
}

