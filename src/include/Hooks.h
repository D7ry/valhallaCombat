#pragma once
#include "attackHandler.h"
#include "SKSE/Trampoline.h"

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

class Hook_GetBlockChance
{
public:
	static void install() { //Down	p	sub_140845C60+2E	call    Actor__GetEquippedShield_140625FA0
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<uintptr_t> hook{ REL::ID(49751) };

		_getBlockChance = trampoline.write_call<5>(hook.address() + 0x2E, getBlockChance);
		INFO("Get block chance hook installed.");
	}
private:
	static uintptr_t getBlockChance(RE::Actor* actor);
	static inline REL::Relocation<decltype(getBlockChance)> _getBlockChance;
};

class Hook_GetAttackChance1 {
public:
	static void install() { //Up	p	sub_14042F810+157A	call    Character__sub_140845B30
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<uintptr_t> hook{ REL::ID(28629) };

		_getAttackChance = trampoline.write_call<5>(hook.address() + 0x157A, getAttackChance);
		INFO("Get block chance hook installed.");
	}
private:
	static uintptr_t getAttackChance(RE::Actor* a1, RE::Actor* a2, RE::BGSAttackData* atkData);
	static inline REL::Relocation<decltype(getAttackChance)> _getAttackChance;
};

class Hook_GetAttackChance2 {
public:
	static void install() { //Up	p	sub_14080C020+2AE	call    Character__sub_140845B30
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<uintptr_t> hook{ REL::ID(48139) };

		_getAttackChance = trampoline.write_call<5>(hook.address() + 0x2AE, getAttackChance);
		INFO("Get block chance hook installed.");
	}
private:
	static uintptr_t getAttackChance(RE::Actor* a1, RE::Actor* a2, RE::BGSAttackData* atkData);
	static inline REL::Relocation<decltype(getAttackChance)> _getAttackChance;
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

/*class Hook_GetFallbackChance {
public:
	static void install() {
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<uintptr_t> hook{ REL::ID(35551) };  // 5AF3D0, main loop

		_Update = trampoline.write_call<5>(hook.address() + 0x11F, Update);
		INFO("Main Update hook installed.");
	}
};*/
class Hooks {
public:
	static void install() {
		SKSE::AllocTrampoline(1 << 8);
		Hook_GetAttackStaminaCost::install();
		//Hook_CacheAttackStaminaCost::install();
		//Hook_GetBlockChance::install();
		//Hook_GetAttackChance1::install();
		//Hook_GetAttackChance2::install();
		Hook_StaminaRegen::install();
		Hook_MeleeHit::install();
		Hook_MainUpdate::install();
	}
};

