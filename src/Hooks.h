#pragma once
#include "attackHandler.h"
#include "SKSE/Trampoline.h"
#include <xbyak\xbyak.h>


class CalcStaminaHook
{
public:
	static void InstallHook();
private:
	static float calcStamina(uintptr_t avOwner, RE::BGSAttackData* atkData);
};


class StaminaRegenHook //block stamina regen during weapon swing
{
public:
    static void InstallHook();

private:
    static bool HasFlags1(RE::ActorState* a_this, uint16_t a_flags);
    static inline REL::Relocation<decltype(HasFlags1)> _HasFlags1;
};

class hitEventHook
{
public:
    static void InstallHook();
private:
    static void processHit(RE::Actor* a_actor, RE::HitData& hitData);
    static inline REL::Relocation<decltype(processHit)> _ProcessHit;
};

class MainUpdateHook
{
public:
	static void InstallHook()
	{
		auto& trampoline = SKSE::GetTrampoline();

		REL::Relocation<uintptr_t> hook{ REL::ID(35551) };  // 5AF3D0, main loop

		_Update = trampoline.write_call<5>(hook.address() + 0x11F, Update);
	}

private:
	static void Update(RE::Main* a_this, float a2);
	static inline REL::Relocation<decltype(Update)> _Update;

};


class Hooks {
public:
	static void install() {
		SKSE::AllocTrampoline(1 << 6);
		CalcStaminaHook::InstallHook();
		StaminaRegenHook::InstallHook();
		hitEventHook::InstallHook();
		MainUpdateHook::InstallHook();
	}
};

