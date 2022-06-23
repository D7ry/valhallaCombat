#pragma once
#include "lib/robin_hood.h"
#include <mutex>
#include <shared_mutex>

/*Class handling actor debuffs and corresponding UI alerts.*/
class debuffHandler
{

public:
	/*Set of all actors currently in debuff*/
	robin_hood::unordered_set<RE::Actor*> actorInDebuff;
	mutable std::shared_mutex mtx_actorInDebuff;
	static debuffHandler* GetSingleton()
	{
		static debuffHandler singleton;
		return  std::addressof(singleton);
	}

	/*called along with the main update hook.*/
	void update();

	/*@return whether this actor is in stamina debuff.*/
	bool isInDebuff(RE::Actor* actor);

	void initStaminaDebuff(RE::Actor* actor);
	void stopStaminaDebuff(RE::Actor* actor);
	/*Quickly stop stamina debuff without using the iterator.
	a convenience method for outside class.*/
	void quickStopStaminaDebuff(RE::Actor* actor);
private:


	void addDebuffPerk(RE::Actor* a_actor);

	void removeDebuffPerk(RE::Actor* a_actor);

	static void async_pcStaminaMeterFlash();
	static inline std::atomic<bool> async_pcStaminaMeterFlash_b;

	RE::BGSPerk* debuffPerk;

};


