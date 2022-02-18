#include "Hooks.h"
#include "Utils.h"
#include "blockHandler.h"
#include "ValhallaCombat.hpp"
#include "stunHandler.h"
#include "hitProcessor.h"
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

/*Function returning stamina cost of an actor when they perform a combat action.
Currently, all stamina cost except for bashing is negated.*/
float CalcStaminaHook::calcStamina(uintptr_t avOwner, RE::BGSAttackData* atkData)
{
	DEBUG("hooked attack data!");
	RE::Actor* a_actor = (RE::Actor*)(avOwner - 0xB0);

	if (atkData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) { //bash attack
		DEBUG("is bash attack!");
		if (atkData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)) { //power bash
			DEBUG("power bash!");
			return a_actor->GetPermanentActorValue(RE::ActorValue::kStamina) * 0.5; //TODO: a bashing formula
		}
		else {
			return a_actor->GetPermanentActorValue(RE::ActorValue::kStamina) * 0.33;
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


void hitEventHook::InstallHook() {
	REL::Relocation<uintptr_t> hook{ REL::ID(37673) };
	auto& trampoline = SKSE::GetTrampoline();
	_ProcessHit = trampoline.write_call<5>(hook.address() + 0x3C0, processHit);
	DEBUG("hit event hook installed!");
};


void hitEventHook::processHit(RE::Actor* victim, RE::HitData& hitData) {
	//hitDataProcessor::processHitData(hitData);
	
	using HITFLAG = RE::HitData::Flag;
	auto aggressor = hitData.aggressor.get().get();
	if (!victim || !aggressor || victim->IsDead()) {
		_ProcessHit(victim, hitData);
		return;
	}
	hitProcessor::GetSingleton()->processHit(aggressor, victim, hitData);
	_ProcessHit(victim, hitData);
};

void MainUpdateHook::Update(RE::Main* a_this, float a2) {
	ValhallaCombat::GetSingleton()->update();
	_Update(a_this, a2);
}