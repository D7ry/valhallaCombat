#include "include/executionHandler.h"
#include "include/data.h"
#include "include/settings.h"
#define DATA data::GetSingleton()
using namespace Utils;
void executionHandler::attemptExecute(RE::Actor* executor, RE::Actor* victim) {
	DEBUG("attempting to execute {}, executor: {}", victim->GetName(), executor->GetName());

	//check if victim can be executed
	if (!settings::bStunToggle
		|| executor->IsDead() || victim->IsDead()
		|| !executor->Is3DLoaded() || !victim->Is3DLoaded()
		|| executor->IsInKillMove() || victim->IsInKillMove()
		|| (!settings::bPlayerExecution && (victim->IsPlayerTeammate() || victim->IsPlayer()))
		|| (!settings::bEssentialExecution && victim->IsEssential())
		|| executor->IsOnMount() || victim->IsOnMount()
		|| victim->HasEffectWithArchetype(RE::MagicTarget::Archetype::kParalysis)) {
		DEBUG("Execution preconditions not met, terminating execution.");
		return;
	}

	auto executorRace = executor->GetRace();
	auto victimRace = victim->GetRace();
	if (!executorRace || !victimRace) {
		return;
	}
	auto it1 = DATA->ExecutionRaceMap.find(executorRace);
	if (it1 == DATA->ExecutionRaceMap.end()) {
		return;
	}
	if (it1->second != data::raceCatagory::Humanoid) {
		DEBUG("executor is not human");
		return;
	}

	auto it2 = DATA->ExecutionRaceMap.find(victimRace);
	if (it2 == DATA->ExecutionRaceMap.end()) {
		DEBUG("Victim race not found on race map.");
		return;
	}
	auto victimRaceType = it2->second;

	if (activeExecutor.find(executor) != activeExecutor.end() //executor is executing
		|| activeExecutor.find(victim) != activeExecutor.end()) { //victim is executing
		return;
	}

	RE::WEAPON_TYPE weaponType;
	auto weapon = Utils::getWieldingWeapon(executor);
	if (!weapon) {
		DEBUG("Executor weapon not found, using unarmed as weapon type.");
		weaponType = RE::WEAPON_TYPE::kHandToHandMelee;
	}
	else {
		weaponType = weapon->GetWeaponType();
	}
	if (weaponType == RE::WEAPON_TYPE::kBow || weaponType == RE::WEAPON_TYPE::kCrossbow) {
		return;
	}

	DEBUG("weapon type is {}", weaponType);
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


	/*Set the executor as ghost and start tracking them.*/
	setIsGhost(executor, true);
	activeExecutor.emplace(executor);

	victim->SetGraphVariableBool("bIdlePlaying", true); //DHAF compatibility
};

void executionHandler::concludeExecution(RE::Actor* executor) {
	if (!executor) {
		return;
	}
	if (activeExecutor.find(executor) != activeExecutor.end()) {
		Utils::setIsGhost(executor, false);
		activeExecutor.erase(executor);
	}
}


void executionHandler::playExecutionIdle(RE::Actor* executor, RE::Actor* victim, RE::TESIdleForm* executionIdle) {
	playPairedIdle(executor->currentProcess, executor, RE::DEFAULT_OBJECT::kActionIdle, executionIdle, true, false, victim);
}

void executionHandler::playExecutionIdle(RE::Actor* executor, RE::Actor* victim, std::vector<RE::TESIdleForm*> executionIdleV) {
	DEBUG("playing execution idle!");
	DEBUG(executionIdleV.size());
	if (executionIdleV.size() == 0) {
		DEBUG("error: no idle present in vector");
		return;
	}
	auto idle = getRandomIdle(executionIdleV);
	if (!idle) {
		DEBUG("Error! no idle received");
	}
	else {
		DEBUG("received idle with name {}", idle->GetName());
		playExecutionIdle(executor, victim, idle);
	}

}


void executionHandler::executeHumanoid(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	if (executor->isDualWielding()) {
		playExecutionIdle(executor, victim, DATA->KM_Humanoid_DW);
	}
	else if (isBackFacing(victim, executor)) {
		switch (weaponType) {
		case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_Humanoid_2hw_Back); break;
		case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_Humanoid_2hm_Back); break;
		case RE::WEAPON_TYPE::kHandToHandMelee: playExecutionIdle(executor, victim, DATA->KM_Humanoid_H2H_Back); break;
		default: playExecutionIdle(executor, victim, DATA->KM_Humanoid_1hm_Back); break;
		}
	}
	else {
		switch (weaponType) {
		case RE::WEAPON_TYPE::kOneHandSword: playExecutionIdle(executor, victim, DATA->KM_Humanoid_Sword); break;
		case RE::WEAPON_TYPE::kOneHandDagger: playExecutionIdle(executor, victim, DATA->KM_Humanoid_Dagger); break;
		case RE::WEAPON_TYPE::kOneHandAxe:  playExecutionIdle(executor, victim, DATA->KM_Humanoid_Axe); break;
		case RE::WEAPON_TYPE::kOneHandMace:  playExecutionIdle(executor, victim, DATA->KM_Humanoid_Mace); break;
		case RE::WEAPON_TYPE::kTwoHandAxe:  playExecutionIdle(executor, victim, DATA->KM_Humanoid_2hw); break;
		case RE::WEAPON_TYPE::kTwoHandSword:  playExecutionIdle(executor, victim, DATA->KM_Humanoid_GreatSword); break;
		case RE::WEAPON_TYPE::kHandToHandMelee:  playExecutionIdle(executor, victim, DATA->KM_Humanoid_H2H); break;
		}
	}
};
void executionHandler::executeDraugr(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	auto gameData = data::GetSingleton();
	switch (weaponType) {
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_Undead_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_Undead_2hm); break;
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	default: playExecutionIdle(executor, victim, DATA->KM_Undead_1hm); break;
	}
};
void executionHandler::executeFalmer(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	auto gameData = data::GetSingleton();
	switch (weaponType) {
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_Falmer_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_Falmer_2hm); break;
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	default: playExecutionIdle(executor, victim, DATA->KM_Falmer_1hm);
	}
};
void executionHandler::executeSpider(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_Spider_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_Spider_2hm); break;
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	default: playExecutionIdle(executor, victim, DATA->KM_Spider_1hm);
	}
}
void executionHandler::executeGargoyle(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_Gargoyle_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_Gargoyle_2hm); break;
	default: playExecutionIdle(executor, victim, DATA->KM_Gargoyle_1hm); break;
	}
}
void executionHandler::executeGiant(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_Giant_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_Giant_2hm); break;
	default: playExecutionIdle(executor, victim, DATA->KM_Giant_1hm); break;
	}
}
void executionHandler::executeBear(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_Bear_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_Bear_2hm); break;
	default: playExecutionIdle(executor, victim, DATA->KM_Bear_1hm); break;
	}
}
void executionHandler::executeSabreCat(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_SabreCat_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_SabreCat_2hm); break;
	default: playExecutionIdle(executor, victim, DATA->KM_SabreCat_1hm); break;
	}
}
void executionHandler::executeWolf(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_Wolf_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_Wolf_2hm); break;
	default: playExecutionIdle(executor, victim, DATA->KM_Wolf_1hm); break;
	}
}
void executionHandler::executeTroll(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_Troll_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_Troll_2hm); break;
	default: playExecutionIdle(executor, victim, DATA->KM_Troll_1hm); break;
	}
}
void executionHandler::executeHagraven(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_Hagraven_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_Hagraven_2hm);  break;
	default: playExecutionIdle(executor, victim, DATA->KM_Hagraven_1hm); break;
	}
}
void executionHandler::executeSpriggan(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_Spriggan_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_Spriggan_2hm); break;
	default: playExecutionIdle(executor, victim, DATA->KM_Spriggan_1hm); break;
	}
}
void executionHandler::executeBoar(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_Boar_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_Boar_2hm); break;
	default: playExecutionIdle(executor, victim, DATA->KM_Boar_1hm); break;
	}
}
void executionHandler::executeRiekling(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_Riekling_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_Riekling_2hm); break;
	default: playExecutionIdle(executor, victim, DATA->KM_Riekling_1hm); break;
	}
}
void executionHandler::executeAshHopper(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_AshHopper_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_AshHopper_2hm); break;
	default: playExecutionIdle(executor, victim, DATA->KM_AshHopper_1hm); break;
	}
}
void executionHandler::executeSteamCenturion(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_Centurion_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_Centurion_2hm); break;
	default: playExecutionIdle(executor, victim, DATA->KM_Centurion_1hm); break;
	}
}
void executionHandler::executeDwarvenBallista(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_Ballista_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_Ballista_2hm); break;
	default: playExecutionIdle(executor, victim, DATA->KM_Ballista_1hm); break;
	}
}
void executionHandler::executeChaurusFlyer(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_ChaurusFlyer_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_ChaurusFlyer_2hm); break;
	default: playExecutionIdle(executor, victim, DATA->KM_ChaurusFlyer_1hm); break;
	}
}
void executionHandler::executeLurker(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_Lurker_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_Lurker_2hm); break;
	default: playExecutionIdle(executor, victim, DATA->KM_Lurker_1hm); break;
	}
}
void executionHandler::executeDragon(RE::Actor* executor, RE::Actor* victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: playExecutionIdle(executor, victim, DATA->KM_Dragon_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: playExecutionIdle(executor, victim, DATA->KM_Dragon_2hm); break;
	default: playExecutionIdle(executor, victim, DATA->KM_Dragon_1hm); break;
	}
}

