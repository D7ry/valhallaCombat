#include "Hooks.h"
REL::Relocation<uintptr_t> ptr_attackOverride{ REL::ID(38047), 0XBB };
void AttackDataHook::InstallHook() {
	SKSE::AllocTrampoline(1 << 4);
	auto& trampoline = SKSE::GetTrampoline();
	trampoline.write_call<5>(ptr_attackOverride.address(), &readFromAttackData);
}

void AttackDataHook::readFromAttackData(RE::Actor* a_actor, RE::BGSAttackData* atkData)
{
	DEBUG("hooked attack data!");
	typedef void (*func_t)(RE::Actor* a, RE::BGSAttackData* atkData);
	REL::Relocation<func_t> func{ REL::ID(25863) };
	if (a_actor && a_actor->IsPlayerRef()) {
		if (atkData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)) {
			DEBUG("is power attack!");
			attackHandler::nextIsLightAtk = false;
		}
		if (atkData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) {
			DEBUG("is bash attack!");
			attackHandler::nextIsBashing = true;
		}
	}
	func(a_actor, atkData);
}
void StaminaRegenHook::InstallHook() 
{
	REL::Relocation<uintptr_t> hook{ REL::ID(37510) };  // 620690 - a function that regenerates stamina
	SKSE::AllocTrampoline(1 << 4);
	auto& trampoline = SKSE::GetTrampoline();
	_HasFlags1 = trampoline.write_call<5>(hook.address() + 0x62, HasFlags1);
}

/*Ersh's stuff*/
bool StaminaRegenHook::HasFlags1(RE::ActorState* a_this, uint16_t a_flags)
{
	bool bResult = _HasFlags1(a_this, a_flags); // is sprinting?

	if (!bResult) {
		RE::Actor* actor = SKSE::stl::adjust_pointer<RE::Actor>(a_this, -0xB8);
		auto attackState = actor->GetAttackState();
		bResult = attackState > RE::ATTACK_STATE_ENUM::kNone && attackState < RE::ATTACK_STATE_ENUM::kBowDraw; // if not sprinting, check if is attacking
	}

	return bResult;
}