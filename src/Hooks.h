#include "attackHandler.h"
class AttackDataHook
{
public:
	static void InstallHook();
private:
	static void readFromAttackData(RE::Actor* a_actor, RE::BGSAttackData* atkData);
};

class StaminaRegenHook
{
public:
    static void InstallHook();

private:
    static bool HasFlags1(RE::ActorState* a_this, uint16_t a_flags);
    static inline REL::Relocation<decltype(HasFlags1)> _HasFlags1;
};