#pragma once
#include "lib/robin_hood.h"
/*Class handling all executions*/
class executionHandler {
public:

	bool executionKeyDown;
	/*Mapping of all active executors -> people being executed.*/
	robin_hood::unordered_map<RE::Actor*, RE::Actor*> executionMap;
	static executionHandler* GetSingleton()
	{
		static executionHandler singleton;
		return  std::addressof(singleton);
	}

	/*Iterate through every actor that can be excuted, check their range to the player.
	* attempt to execute the actor closest to the player.
	* */
	void tryPcExecution();

	/// <summary>
	/// Queue an execution idle for 10 times.
	/// </summary>
	/// <param name="a_executor"></param>
	/// <param name="a_victim"></param>
	/// <param name="a_executionIdle"></param>
	/// <param name="i"></param>
	void queueExecutionIdle(RE::Actor* a_executor, RE::Actor* a_victim, RE::TESIdleForm* a_executionIdle, int i);

	/// <summary>
	/// Queue one of execution idle, selected randomly from the vector, for 10 times.
	/// </summary>
	/// <param name="a_executor"></param>
	/// <param name="a_victim"></param>
	/// <param name="a_executionIdleV"></param>
	/// <param name="i"></param>
	void queueExecutionIdle(RE::Actor* a_executor, RE::Actor* a_victim, std::vector<RE::TESIdleForm*> a_executionIdleV, int i);

	/*Attempt an execution. Help aggressor decide which killmove to use
	based on aggressor/victim's relative position, rotation, and victim's race.
	Also performs a check on whether the aggressor can execute victim.
	@param executor: actor to perform killmove.
	@param victim: actor to be executed.*/
	bool attemptExecute(RE::Actor* executor, RE::Actor* victim);


private:
	void executeHumanoid(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeDraugr(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeFalmer(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeSpider(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeGargoyle(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeGiant(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeBear(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeSabreCat(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeWolf(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeTroll(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeHagraven(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeSpriggan(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeBoar(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeRiekling(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeAshHopper(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeSteamCenturion(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeDwarvenBallista(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeChaurusFlyer(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeLurker(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	void executeDragon(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);

	static inline void async_queueExecutionThreadFunc(RE::Actor* a_executor, RE::Actor* a_victim, std::vector<RE::TESIdleForm*> a_executionIdleV, int i);
	static inline void async_queueExecutionThreadFun(RE::Actor* a_executor, RE::Actor* a_victim, RE::TESIdleForm* a_executionIdle, int i);

};

namespace inlineUtils
{
	inline void sendConsoleCommand(std::string a_command)
	{
		const auto scriptFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
		const auto script = scriptFactory ? scriptFactory->Create() : nullptr;
		if (script) {
			const auto selectedRef = RE::Console::GetSelectedRef();
			script->SetCommand(a_command);
			script->CompileAndRun(selectedRef.get());
			delete script;
		}
	}

	/*Whether the actor's back is facing the other actor's front.
	@param actor1: actor whose facing will be returned
	@param actor2: actor whose relative location to actor1 will be calculated.*/
	inline bool isBackFacing(RE::Actor* actor1, RE::Actor* actor2) {
		auto angle = actor1->GetHeadingAngle(actor2->GetPosition(), false);
		if (90 < angle || angle < -90) {
			return true;
		}
		else {
			return false;
		}
	}
	/*Get a random element from this vector.
	@param in the vector containing strings.
	@return a random element from the vector.*/
	inline std::string getRandomStr(std::vector<std::string> in) {
		return in[rand() % in.size()];
	}

	/*Get a random element from this vector.
	@param in the vector containing strings.
	@return a random element from the vector.*/
	inline RE::TESIdleForm* getRandomIdle(std::vector<RE::TESIdleForm*> in) {
		//
		// ("getting random idle");
		//DEBUG("in size: {}", in.size());
		auto i = rand() % in.size();
		//
		(i);
		return in[i];
	}



}

