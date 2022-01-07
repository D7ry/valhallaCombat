#include "attackHandler.h"
class AttackDataHook
{
public:
	static void InstallHook();
private:
	static void readFromAttackData(uintptr_t avOwner, RE::BGSAttackData* atkData);
};

class StaminaRegenHook
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