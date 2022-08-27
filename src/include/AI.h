#pragma once
#include "include/PCH.h"
class AI
{
public:

	static AI* GetSingleton()
	{
		static AI singleton;
		return  std::addressof(singleton);
	}

	uintptr_t getAttackChance(RE::Actor* actor, RE::Actor* target);

	/*Perform a series of calculations to decide whether an actor should 
	do a guard-counter. If so, make the actor perform a guard counter.*/
	void calculateCounterChance(RE::Actor* actor);

	void action_PerformEldenCounter(RE::Actor* actor);

	bool getShouldTimedBlock(RE::Actor* actor);

	bool getShouldPerilousAttack(RE::Actor* a_actor);
};
