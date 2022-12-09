#include "include/executionHandler.h"
#include "include/data.h"
#include "include/settings.h"
#include "include/stunHandler.h"
#include "include/Utils.h"
#define DATA data::GetSingleton()
using namespace inlineUtils;
void executionHandler::tryPcExecution() {

	auto pc = RE::PlayerCharacter::GetSingleton();
	if (!pc) {
		return;
	}
	RE::Actor* optimalVictim = nullptr; //optimal target to execute
	float curr_min_dist = HUGE_VALF; //minimum range to the optimal victim so far

	for (auto& handle : stunHandler::GetSingleton()->getStunBrokenActors()) {
		if (!handle) {
			continue;
		}
		auto actor = handle.get().get();
		
		if (!actor || !actor->Is3DLoaded() || actor->IsDead() || actor->IsInKillMove() ||
			!actor->GetActorRuntimeData().currentProcess || !actor->GetActorRuntimeData().currentProcess->InHighProcess()) {
			return;
		}
		float dist = pc->GetPosition().GetDistance(actor->GetPosition());
		if (dist < 250) {
			if (dist < curr_min_dist) {
				curr_min_dist = dist;
				optimalVictim = actor;
			}
		}
	}
	
	if (optimalVictim) {
		attemptExecute(pc, optimalVictim);
	}

}
bool executionHandler::attemptExecute(RE::Actor* a_executor, RE::Actor* a_victim) {
	//check if victim can be executed
	if (!settings::bStunToggle
		|| !Utils::Actor::isHumanoid(a_executor) || a_executor->IsDead() || a_victim->IsDead()
		|| !a_executor->Is3DLoaded() || !a_victim->Is3DLoaded()
		|| a_executor->IsInKillMove() || a_victim->IsInKillMove()
		|| a_executor->IsOnMount() || a_victim->IsOnMount()
		|| a_victim->IsPlayerRef() || a_victim->IsPlayerTeammate()
		|| a_victim->IsEssential() || a_victim->IsInKillMove() || a_victim->AsMagicTarget()->HasEffectWithArchetype(RE::MagicTarget::Archetype::kParalysis)) {
		//logger::info("Execution preconditions not met, terminating execution.");
		return false;
	}

	auto victimRace = a_victim->GetRace();
	if (!victimRace) {
		return false;
	}
	
	auto it2 = data::raceMapping.find(victimRace);
	if (it2 == data::raceMapping.end()) {
		return false;
	}
	auto victimRaceType = it2->second;

	RE::WEAPON_TYPE weaponType;
	auto weapon = Utils::Actor::getWieldingWeapon(a_executor);
	if (!weapon) {
		//logger::info("Executor weapon not found, using unarmed as weapon type.");
		weaponType = RE::WEAPON_TYPE::kHandToHandMelee;
	}
	else {
		weaponType = weapon->GetWeaponType();
	}

	if (weaponType == RE::WEAPON_TYPE::kBow || weaponType == RE::WEAPON_TYPE::kCrossbow) {
		//logger::info("long range weapon");
		return false;
	}

	//logger::info("weapon type is {}", weaponType);
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
	default: return false; //iff no body part match, no need to set ghost.
	}
	return true;
};

inline void playExecutionIdle(RE::Actor* a_executor, RE::Actor* a_victim, RE::TESIdleForm* a_executionIdle) {
	RE::Offset::playPairedIdle(a_executor->GetActorRuntimeData().currentProcess, a_executor, RE::DEFAULT_OBJECT::kActionIdle, a_executionIdle, true, false, a_victim);
}

void executionHandler::async_queueExecutionThreadFunc(RE::Actor* a_executor, RE::Actor* a_victim, std::vector<RE::TESIdleForm*> a_executionIdleV, int i)
{
	while (i > 0) {
		auto task = SKSE::GetTaskInterface();
		if (!task) {
			continue;
		}
		if (a_executionIdleV.size() == 0) {
			logger::error("Error: failed to find any execution idles.");
		}
		auto idle = *inlineUtils::select_randomly(a_executionIdleV.begin(), a_executionIdleV.end());
		task->AddTask([a_executor, a_victim, idle]() {
			playExecutionIdle(a_executor, a_victim, idle);
		});
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		i--;
	}
}

void executionHandler::async_queueExecutionThreadFun(RE::Actor* a_executor, RE::Actor* a_victim, RE::TESIdleForm* a_executionIdle, int i)
{
	while (i > 0) {
		auto task = SKSE::GetTaskInterface();
		if (!task) {
			continue;
		}
		task->AddTask([a_executor, a_victim, a_executionIdle]() {
			playExecutionIdle(a_executor, a_victim, a_executionIdle);
		});
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		i--;
	}
}



void executionHandler::queueExecutionIdle(RE::Actor* a_executor, RE::Actor* a_victim, RE::TESIdleForm* a_executionIdle, int i = 1) 
{
	std::thread t(async_queueExecutionThreadFun, a_executor, a_victim, a_executionIdle, i);
	t.detach();
}

void executionHandler::queueExecutionIdle(RE::Actor* a_executor, RE::Actor* a_victim, std::vector<RE::TESIdleForm*> a_executionIdleVector, int i = 1) 
{
	std::jthread t(async_queueExecutionThreadFunc, a_executor, a_victim, a_executionIdleVector, i);
	t.detach();
}


void executionHandler::executeHumanoid(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE a_weaponType) {
	//logger::info("executing humanoid!");
	if (Utils::Actor::isDualWielding(a_executor)) {
		//logger::info("dual wielding!");
		queueExecutionIdle(a_executor, a_victim, data::KM_Humanoid_DW);
	}
	else if (Utils::Actor::isBackFacing(a_victim, a_executor)) {
		//logger::info("backstab!");
		switch (a_weaponType) {
		case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_Humanoid_2hw_Back); break;
		case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Humanoid_2hm_Back); break;
		case RE::WEAPON_TYPE::kHandToHandMelee: queueExecutionIdle(a_executor, a_victim, data::KM_Humanoid_H2H_Back); break;
		default: queueExecutionIdle(a_executor, a_victim, data::KM_Humanoid_1hm_Back); break;
		}
	}
	else {
		switch (a_weaponType) {
		case RE::WEAPON_TYPE::kOneHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Humanoid_Sword); break;
		case RE::WEAPON_TYPE::kOneHandDagger: queueExecutionIdle(a_executor, a_victim, data::KM_Humanoid_Dagger); break;
		case RE::WEAPON_TYPE::kOneHandAxe:  queueExecutionIdle(a_executor, a_victim, data::KM_Humanoid_Axe); break;
		case RE::WEAPON_TYPE::kOneHandMace:  queueExecutionIdle(a_executor, a_victim, data::KM_Humanoid_Mace); break;
		case RE::WEAPON_TYPE::kTwoHandAxe:  queueExecutionIdle(a_executor, a_victim, data::KM_Humanoid_2hw); break;
		case RE::WEAPON_TYPE::kTwoHandSword:  queueExecutionIdle(a_executor, a_victim, data::KM_Humanoid_GreatSword); break;
		case RE::WEAPON_TYPE::kHandToHandMelee:  queueExecutionIdle(a_executor, a_victim, data::KM_Humanoid_H2H); break;
		}
	}
};
void executionHandler::executeDraugr(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_Undead_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Undead_2hm); break;
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_Undead_1hm); break;
	}
};
void executionHandler::executeFalmer(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_Falmer_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Falmer_2hm); break;
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_Falmer_1hm);
	}
};
void executionHandler::executeSpider(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_Spider_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Spider_2hm); break;
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_Spider_1hm);
	}
}
void executionHandler::executeGargoyle(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_Gargoyle_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Gargoyle_2hm); break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_Gargoyle_1hm); break;
	}
}
void executionHandler::executeGiant(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_Giant_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Giant_2hm); break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_Giant_1hm); break;
	}
}
void executionHandler::executeBear(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_Bear_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Bear_2hm); break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_Bear_1hm); break;
	}
}
void executionHandler::executeSabreCat(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_SabreCat_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_SabreCat_2hm); break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_SabreCat_1hm); break;
	}
}
void executionHandler::executeWolf(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_Wolf_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Wolf_2hm); break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_Wolf_1hm); break;
	}
}
void executionHandler::executeTroll(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_Troll_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Troll_2hm); break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_Troll_1hm); break;
	}
}
void executionHandler::executeHagraven(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_Hagraven_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Hagraven_2hm);  break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_Hagraven_1hm); break;
	}
}
void executionHandler::executeSpriggan(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_Spriggan_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Spriggan_2hm); break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_Spriggan_1hm); break;
	}
}
void executionHandler::executeBoar(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_Boar_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Boar_2hm); break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_Boar_1hm); break;
	}
}
void executionHandler::executeRiekling(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_Riekling_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Riekling_2hm); break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_Riekling_1hm); break;
	}
}
void executionHandler::executeAshHopper(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_AshHopper_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_AshHopper_2hm); break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_AshHopper_1hm); break;
	}
}
void executionHandler::executeSteamCenturion(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_Centurion_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Centurion_2hm); break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_Centurion_1hm); break;
	}
}
void executionHandler::executeDwarvenBallista(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_Ballista_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Ballista_2hm); break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_Ballista_1hm); break;
	}
}
void executionHandler::executeChaurusFlyer(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_ChaurusFlyer_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_ChaurusFlyer_2hm); break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_ChaurusFlyer_1hm); break;
	}
}
void executionHandler::executeLurker(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_Lurker_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Lurker_2hm); break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_Lurker_1hm); break;
	}
}
void executionHandler::executeDragon(RE::Actor* a_executor, RE::Actor* a_victim, RE::WEAPON_TYPE weaponType) {
	switch (weaponType) {
	case RE::WEAPON_TYPE::kHandToHandMelee: break;
	case RE::WEAPON_TYPE::kTwoHandAxe: queueExecutionIdle(a_executor, a_victim, data::KM_Dragon_2hw); break;
	case RE::WEAPON_TYPE::kTwoHandSword: queueExecutionIdle(a_executor, a_victim, data::KM_Dragon_2hm); break;
	default: queueExecutionIdle(a_executor, a_victim, data::KM_Dragon_1hm); break;
	}
}

