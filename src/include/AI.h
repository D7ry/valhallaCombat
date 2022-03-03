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
};