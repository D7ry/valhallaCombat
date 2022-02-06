#include "Hooks.h"
#include "Utils.h"
#include "hitDataProcessor.h"
#pragma region attackDataHook
void CalcStaminaHook::InstallHook() {


	//apply_func<25863>(std::uintptr_t(CalcStaminaHook::calcStamina));  // SkyrimSE.exe+3BEC90
	struct Code : Xbyak::CodeGenerator
	{
		Code(uintptr_t jump_addr)
		{
			mov(rax, jump_addr);
			jmp(rax);
		}
	}xbyakCode{ (uintptr_t)CalcStaminaHook::calcStamina };

	//add_trampoline<6, ID>(&xbyakCode);
	//template <size_t offset = 0, bool call = false>
	const int ID = 25863;
	constexpr REL::ID funcOffset = REL::ID(ID);
	auto funcAddr = funcOffset.address();
	auto size = xbyakCode.getSize();
	auto& trampoline = SKSE::GetTrampoline();
	auto result = trampoline.allocate(size);
	std::memcpy(result, xbyakCode.getCode(), size);
	trampoline.write_branch<6>(funcAddr, (std::uintptr_t)result);

	INFO("attack data hook installed");
}

/*this function fires before every attack. Reading from this allows me to 
decide the attack's light/power therefore calculate its staina consumption accordingly. */
float CalcStaminaHook::calcStamina(uintptr_t avOwner, RE::BGSAttackData* atkData)
{
	DEBUG("hooked attack data!");
	RE::Actor* a_actor = (RE::Actor*)(avOwner - 0xB0);

	if (atkData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) { //bash attack
		DEBUG("is bash attack!");
		if (a_actor->IsPlayerRef()) {
			attackHandler::nextIsBashAtk = true;
		}
		if (atkData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)) { //power bash
			DEBUG("power bash!");
			return a_actor->GetPermanentActorValue(RE::ActorValue::kStamina) * 0.5; //TODO: a bashing formula
		}
		else {
			return a_actor->GetPermanentActorValue(RE::ActorValue::kStamina) * 0.33; 
		}
	} 
	else if (atkData->data.flags.all(RE::AttackData::AttackFlag::kPowerAttack)) { //power attack

		DEBUG("is power attack!");
		if (a_actor->IsPlayerRef()) {
				DEBUG("next is light == false");
				//attackHandler::nextIsLightAtk = false;
		}
		else {
			return a_actor->GetPermanentActorValue(RE::ActorValue::kStamina) * settings::fMeleeCostHeavyMiss_Percent;
		}
	}
	else {																			//has to be light attack
		DEBUG("is light attack!");
		if (a_actor->IsPlayerRef()) {
			//if (!attackHandler::attackFired) {
				//attackHandler::nextIsLightAtk = true;
			//}

		}
		else {
			return a_actor->GetPermanentActorValue(RE::ActorValue::kStamina) * settings::fMeleeCostLightMiss_Point;
		}
	}

	return 0;
}
#pragma endregion



#pragma region StaminaRegenHook
void StaminaRegenHook::InstallHook()
{
	REL::Relocation<uintptr_t> hook{ REL::ID(37510) };  // 620690 - a function that regenerates stamina
	auto& trampoline = SKSE::GetTrampoline();
	_HasFlags1 = trampoline.write_call<5>(hook.address() + 0x62, HasFlags1);
	INFO("stamina regen hook installed");
}

/*function generating conditions for stamina regen. Iff returned value is true, no regen.
used to block stamina regen in certain situations.*/
bool StaminaRegenHook::HasFlags1(RE::ActorState* a_this, uint16_t a_flags)
{
	//iff bResult is true, prevents regen.
	bool bResult = _HasFlags1(a_this, a_flags); // is sprinting?

	if (!bResult) {
		RE::Actor* actor = SKSE::stl::adjust_pointer<RE::Actor>(a_this, -0xB8);
		auto attackState = actor->GetAttackState();
		if (actor->IsPlayerRef() && attackHandler::meleeHitRegen) { 
			//iff melee hit regen is needed, no need to disable regen.
		}
		else {
			bResult = (attackState > RE::ATTACK_STATE_ENUM::kNone && attackState <= RE::ATTACK_STATE_ENUM::kBowFollowThrough) || actor->IsBlocking(); //when attacking or blocking, doens't regen stmaina.
		}
	}
	return bResult;
}
#pragma endregion


#pragma region hitEventHook
void hitEventHook::InstallHook() {
	REL::Relocation<uintptr_t> hook{ REL::ID(37673) };
	auto& trampoline = SKSE::GetTrampoline();
	_ProcessHit = trampoline.write_call<5>(hook.address() + 0x3C0, processHit);
	DEBUG("hit event hook installed!");
};

/*stamina blocking*/
void hitEventHook::processHit(RE::Actor* a_actor, RE::HitData& hitData) {
	//hitDataProcessor::processHitData(hitData);
	int hitFlag = (int)hitData.flags;
	using HITFLAG = RE::HitData::Flag;

	//nullPtr check in case Skyrim fucks up
	auto aggressor = hitData.aggressor.get();
	if (!a_actor || !aggressor) {
		_ProcessHit(a_actor, hitData);
		return;
	}
	bool isPlayerAggressor = aggressor->IsPlayerRef();
	if (hitFlag & (int)HITFLAG::kBlocked && settings::bStaminaBlocking) {		//shield of stamina
		DEBUG("process stamina blocking!");
		bool isPlayerTarget = a_actor->IsPlayerRef();
		float staminaDamageBase = hitData.totalDamage;
		float staminaDamageMult;
		DEBUG("base stamina damage is {}", staminaDamageBase);
		if (hitFlag & (int)HITFLAG::kBlockWithWeapon) {
			DEBUG("hit blocked with weapon");
			if (isPlayerTarget) {
				staminaDamageMult = settings::fBckWpnStaminaMult_PC_Block_NPC;
			}
			else {
				if (isPlayerAggressor) {
					staminaDamageMult = settings::fBckWpnStaminaMult_NPC_Block_PC;
				}
				else {
					staminaDamageMult = settings::fBckWpnStaminaMult_NPC_Block_NPC;
				}
			}
		}
		else {
			DEBUG("hit blocked with shield");
			if (isPlayerTarget) {
				staminaDamageMult = settings::fBckShdStaminaMult_PC_Block_NPC;
			}
			else {
				if (isPlayerAggressor) {
					staminaDamageMult = settings::fBckShdStaminaMult_NPC_Block_PC;
				}
				else {
					staminaDamageMult = settings::fBckShdStaminaMult_NPC_Block_NPC;
				}
			}
		}
		float staminaDamage = staminaDamageBase * staminaDamageMult;
		float targetStamina = a_actor->GetActorValue(RE::ActorValue::kStamina);

		//check whether there's enough stamina to block incoming attack
		if (targetStamina < staminaDamage) {
			DEBUG("not enough stamina to block, blocking part of damage!");
			if (settings::bGuardBreak) {
				DEBUG("guard break!");
				a_actor->NotifyAnimationGraph("staggerStart");
			}
			hitData.totalDamage = hitData.totalDamage - (targetStamina / staminaDamageMult);
			Utils::damageav(a_actor, RE::ActorValue::kStamina,
				targetStamina);
			DEBUG("failed to block {} damage", hitData.totalDamage);
		}
		else {
			hitData.totalDamage = 0;
			Utils::damageav(a_actor, RE::ActorValue::kStamina,
				staminaDamage);
		}
	}
	
	if (isPlayerAggressor) {
		//iff is player
		if (!(hitFlag & (int)HITFLAG::kBash)  //bash hit doesn't regen stamina
			&& (!(hitFlag & (int)HITFLAG::kBlocked) || settings::bBlockedHitRegenStamina)//blocked hit doesn't regen stamina unless set so
			&& !a_actor->IsDead()) { //dead actor doesn't regen stamina
			attackHandler::registerHit();
		}
	}

	_ProcessHit(a_actor, hitData);
};
#pragma endregion