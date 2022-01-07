#include "Hooks.h"

#pragma region attackDataHook
void AttackDataHook::InstallHook() {
	REL::Relocation<uintptr_t> ptr_attackOverride{ REL::ID(38047), 0XBB };
	SKSE::AllocTrampoline(1 << 4);
	auto& trampoline = SKSE::GetTrampoline();
	trampoline.write_call<5>(ptr_attackOverride.address(), &readFromAttackData);
	INFO("attack data hook installed");
}

void AttackDataHook::readFromAttackData(uintptr_t avOwner, RE::BGSAttackData* atkData)
{
	DEBUG("hooked attack data!");
	typedef void (*func_t)(uintptr_t avOwner, RE::BGSAttackData* atkData);
	REL::Relocation<func_t> func{ REL::ID(25863) };
	RE::Actor* a_actor = (RE::Actor*)(avOwner - 0xB0);
	if (a_actor->IsPlayerRef()) {
		DEBUG("player attack!");
		if (atkData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)) {
			DEBUG("is power attack!");
			attackHandler::nextIsLightAtk = false;
		}
		if (atkData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) {
			DEBUG("is bash attack!");
			attackHandler::nextIsBashing = true;
		}
	}
	func(avOwner, atkData);
}
#pragma endregion


#pragma region StaminaRegenHook
void StaminaRegenHook::InstallHook()
{
	REL::Relocation<uintptr_t> hook{ REL::ID(37510) };  // 620690 - a function that regenerates stamina
	SKSE::AllocTrampoline(1 << 4);
	auto& trampoline = SKSE::GetTrampoline();
	_HasFlags1 = trampoline.write_call<5>(hook.address() + 0x62, HasFlags1);
	INFO("stamina regen hook installed");
}

/*Ersh's stuff*/
bool StaminaRegenHook::HasFlags1(RE::ActorState* a_this, uint16_t a_flags)
{
	//iff bResult is true, prevents regen.
	bool bResult = _HasFlags1(a_this, a_flags); // is sprinting?

	if (!bResult && !attackHandler::meleeHitRegen) {
		RE::Actor* actor = SKSE::stl::adjust_pointer<RE::Actor>(a_this, -0xB8);
		auto attackState = actor->GetAttackState();
		bResult = (attackState > RE::ATTACK_STATE_ENUM::kNone && attackState <= RE::ATTACK_STATE_ENUM::kBowFollowThrough) || actor->IsBlocking(); // if not sprinting, check if is attacking/drawing a bow
	}

	return bResult;
}
#pragma endregion


#pragma region hitEventHook
void hitEventHook::InstallHook() {
	REL::Relocation<uintptr_t> hook{ REL::ID(37673) };
	SKSE::AllocTrampoline(1 << 4);
	auto& trampoline = SKSE::GetTrampoline();
	_ProcessHit = trampoline.write_call<5>(hook.address() + 0x3C0, processHit);
	DEBUG("hit event hook installed!");
};

void hitEventHook::processHit(RE::Actor* a_actor, RE::HitData& hitData) {
	DEBUG("hooked hit event! actor is {}", a_actor->GetName());
	if (hitData.flags == RE::HitData::Flag::kBlocked && hitData.flags != RE::HitData::Flag::kBlockWithWeapon) {
		DEBUG("")
	}
	_ProcessHit(a_actor, hitData);
};
#pragma endregion