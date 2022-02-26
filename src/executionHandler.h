#pragma once
#include <boost/unordered_set.hpp>
#include "include/robin_hood.h"

/*Class handling all executions*/
class executionHandler {
private:
#pragma region CreatureBodyTypeData
	static const uint32_t uIntBodyPartData_Humanoid = 29;
	static const uint32_t uIntBodyPartData_Dragon = 78994;
	static const uint32_t uIntBodyPartData_FrostbiteSpider = 96553;
	static const uint32_t uIntBodyPartData_SabreCat = 134694;
	static const uint32_t uIntBodyPartData_Skeever;
	static const uint32_t uIntBodyPartData_DragonPriest;
	static const uint32_t uIntBodyPartData_Draugr = 298003;
	static const uint32_t uIntBodyPartData_Cow;
	static const uint32_t uIntBodyPartData_Dog = 326645;
	static const uint32_t uIntBodyPartData_Giant = 333739;
	static const uint32_t uIntBodyPartData_Chaurus;
	static const uint32_t uIntBodyPartData_ChaurusFlyer = 33575429;
	static const uint32_t uIntBodyPartData_Mammoth;
	static const uint32_t uIntBodyPartData_AtronachFrost;
	static const uint32_t uIntBodyPartData_AtronachStorm;
	static const uint32_t uIntBodyPartData_AtronachFlame;
	static const uint32_t uIntBodyPartData_Falmer = 384418;
	static const uint32_t uIntBodyPartData_Horse;
	static const uint32_t uIntBodyPartData_Hare;
	static const uint32_t uIntBodyPartData_Goat;
	static const uint32_t uIntBodyPartData_Deer;
	static const uint32_t uIntBodyPartData_DwarvenSphereCenturion = 493389;
	static const uint32_t uIntBodyPartData_DwarvenSteamCenturion = 524524;
	static const uint32_t uIntBodyPartData_DwarvenBallistaCenturion = 67285016;
	static const uint32_t uIntBodyPartData_DwarvenSpider;
	static const uint32_t uIntBodyPartData_Bear = 551164;
	static const uint32_t uIntBodyPartData_Horker;
	static const uint32_t uIntBodyPartData_Troll = 601235;
	static const uint32_t uIntBodyPartData_Hagraven = 619074;
	static const uint32_t uIntBodyPartData_Spriggan = 656416;
	static const uint32_t uIntBodyPartData_Chicken;
	static const uint32_t uIntBodyPartData_Mudcrab;
	static const uint32_t uIntBodyPartData_WerewolfBeast;
	static const uint32_t uIntBodyPartData_Gargoyle = 33596103;
	static const uint32_t uIntBodyPartData_DwarvenBallistaCenturion;
	static const uint32_t uIntBodyPartData_DLC2_Riekling = 67206995;
	static const uint32_t uIntBodyPartData_DLC2_Scrib = 67214034;
	static const uint32_t uIntBodyPartData_DLC2_Dragon;
	static const uint32_t uIntBodyPartData_DLC2_HMDaedra;
	static const uint32_t uIntBodyPartData_DLC2_MountedRiekling;
	static const uint32_t uIntBodyPartData_DLC2_Netch;
	static const uint32_t uIntBodyPartData_DLC2_BenthicLurker = 67274039;
#pragma endregion
public:


	/*Mapping of all active executors -> people being executed.*/
	robin_hood::unordered_set<RE::Actor*> activeExecutor;
	static executionHandler* GetSingleton()
	{
		static executionHandler singleton;
		return  std::addressof(singleton);
	}

	/*Play an execution idle animation.
	@param executor: the actor the execute.
	@param victim: the actor to be executed.
	@param executionIdle: the execution idle to be played.*/
	void playExecutionIdle(RE::Actor* executor, RE::Actor* victim, RE::TESIdleForm* executionIdle);
	void playExecutionIdle(RE::Actor* executor, RE::Actor* victim, std::vector<RE::TESIdleForm*> executionIdleV);
	/*Attempt an execution. Help aggressor decide which killmove to use
	based on aggressor/victim's relative position, rotation, and victim's race.
	Also performs a check on whether the aggressor can execute victim.
	@param executor: actor to perform killmove.
	@param victim: actor to be executed.*/
	void attemptExecute(RE::Actor* executor, RE::Actor* victim);

	/*Conclude an execution, cancle executor and victim's ghost state. Optionally apply buff onto executor.*/
	void concludeExecution(RE::Actor* executor);

private:
	/*Send an execution console command through console.
	@param uIntAggressorFormId: formId of aggressor, in unsigned int 32.
	@param uIntVictimFormId: formid of victim, in unsigned int 32.
	@param animStr: paired execution animation to be played, in unsigned int 32.*/
	void sendExecutionCommand(RE::Actor* executer, RE::Actor* victim, std::vector<std::string> executionStrSet) { };
	void sendExecutionCommand(RE::Actor* executer, RE::Actor* victim, std::string executionStr) { };

	void executeHumanoid(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeDraugr(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeFalmer(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeSpider(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeGargoyle(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeGiant(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeBear(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeSabreCat(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeWolf(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeTroll(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeHagraven(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeSpriggan(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeBoar(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeRiekling(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeAshHopper(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeSteamCenturion(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeDwarvenBallista(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeChaurusFlyer(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeLurker(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
	inline void executeDragon(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);

};

namespace Utils
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
		DEBUG("getting random idle");
		DEBUG("in size: {}", in.size());
		return in[rand() % in.size()];
	}



	typedef void(_fastcall* _setIsGhost)(RE::Actor* actor, bool isGhost);
	static REL::Relocation<_setIsGhost> setIsGhost{ REL::ID(36287) };

	inline bool playPairedIdle(RE::AIProcess* proc, RE::Actor* attacker, RE::DEFAULT_OBJECT smth, RE::TESIdleForm* idle, bool a5, bool a6, RE::TESObjectREFR* target)
	{
		using func_t = decltype(&playPairedIdle);
		REL::Relocation<func_t> func{ REL::ID(38290) };
		return func(proc, attacker, smth, idle, a5, a6, target);
	}
}

