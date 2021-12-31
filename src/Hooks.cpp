#include "Hooks.h"
REL::Relocation<uintptr_t> ptr_attackOverride{ REL::ID(38047), 0XBB };
void StaminaHook::InstallHook() {
	SKSE::AllocTrampoline(1 << 4);
	auto& trampoline = SKSE::GetTrampoline();
	trampoline.write_call<5>(ptr_attackOverride.address(), &readFromAttackData);
}

void StaminaHook::readFromAttackData(RE::Actor* a_actor, RE::BGSAttackData* atkData)
{
	DEBUG("hooked attack data!");
	typedef void (*func_t)(RE::Actor* a, RE::BGSAttackData* atkData);
	REL::Relocation<func_t> func{ REL::ID(25863) };
	if (atkData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)) {
		DEBUG("is power attack!");
		attackHandler::nextIsLightAtk = false;
	}
	if (atkData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) {
		DEBUG("is bash attack!");
		attackHandler::nextIsBashing = true;
	}
	func(a_actor, atkData);
}