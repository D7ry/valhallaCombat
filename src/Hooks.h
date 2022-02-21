#pragma once
#include "attackHandler.h"
#include "SKSE/Trampoline.h"
#include <xbyak\xbyak.h>


class Hook_GetAttackStaminaCost //Actor__sub_140627930+16E	call ActorValueOwner__sub_1403BEC90
{
	/*to cancel out vanilla power attack stamina consumption.*/
public:
	static void install() {
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<uintptr_t> hook{ REL::ID(37650) };

		_getHeavyAttackStaminaCost = trampoline.write_call<5>(hook.address() + 0x16E, getAttackStaminaCost);
		INFO("Heavy attack stamina hook installed.");
	}
private:
	static float getAttackStaminaCost(uintptr_t avOwner, RE::BGSAttackData* atkData);
	static inline REL::Relocation<decltype(getAttackStaminaCost)> _getHeavyAttackStaminaCost;
};

class Hook_CacheAttackStaminaCost //sub_14063CFB0+BB	call    ActorValueOwner__sub_1403BEC90 //this thing is useless
{
public:
	static void install() {
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<uintptr_t> hook{ REL::ID(38047) };

		_cacheAttackStaminaCost = trampoline.write_call<5>(hook.address() + 0xBB, cacheAttackStaminaCost);
		INFO("Heavy bash stamina hook installed.");
	}
private:
	static float cacheAttackStaminaCost(uintptr_t avOwner, RE::BGSAttackData* atkData);
	static inline REL::Relocation<decltype(cacheAttackStaminaCost)> _cacheAttackStaminaCost;
};
class getBashChanceHook
{

};

class getAttackChanceHook
{

};

class Hook_GetBlockChance
{
public:
	static void install() {
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<uintptr_t> hook{ REL::ID(37650) };

		_getBlockChance = trampoline.write_call<5>(hook.address() + 0x16E, getBlockChance);
		INFO("Heavy attack stamina hook installed.");
	}
private:
	static float getBlockChance(uintptr_t avOwner, RE::BGSAttackData* atkData);
	static inline REL::Relocation<decltype(getBlockChance)> _getBlockChance;
};


class Hook_StaminaRegen //block stamina regen during weapon swing
{
public:
	static void install() {
		REL::Relocation<uintptr_t> hook{ REL::ID(37510) };  // 620690 - a function that regenerates stamina
		auto& trampoline = SKSE::GetTrampoline();
		_HasFlags1 = trampoline.write_call<5>(hook.address() + 0x62, HasFlags1);
		INFO("Stamina Regen hook installed");
	}

private:
    static bool HasFlags1(RE::ActorState* a_this, uint16_t a_flags);
    static inline REL::Relocation<decltype(HasFlags1)> _HasFlags1;
};

class Hook_MeleeHit
{
public:
	static void install() {
		REL::Relocation<uintptr_t> hook{ REL::ID(37673) };
		auto& trampoline = SKSE::GetTrampoline();
		_ProcessHit = trampoline.write_call<5>(hook.address() + 0x3C0, processHit);
		INFO("Melee Hit hook installed.");
	}
private:
    static void processHit(RE::Actor* victim, RE::HitData& hitData);
    static inline REL::Relocation<decltype(processHit)> _ProcessHit;
};

class Hook_MainUpdate
{
public:
	static void install() {
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<uintptr_t> hook{ REL::ID(35551) };  // 5AF3D0, main loop

		_Update = trampoline.write_call<5>(hook.address() + 0x11F, Update);
		INFO("Main Update hook installed.");
	}

private:
	static void Update(RE::Main* a_this, float a2);
	static inline REL::Relocation<decltype(Update)> _Update;

};


class Hooks {
public:
	static void install() {
		SKSE::AllocTrampoline(1 << 8);
		Hook_GetAttackStaminaCost::install();
		//Hook_CacheAttackStaminaCost::install();
		Hook_StaminaRegen::install();
		Hook_MeleeHit::install();
		Hook_MainUpdate::install();
	}
};

