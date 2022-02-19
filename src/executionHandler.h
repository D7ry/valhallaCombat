#pragma once
/*Class handling all executions*/
class executionHandler {

private:
	static const uint32_t uIntBodyPartData_Default = 1;
	static const uint32_t uIntBodyPartData_Dragon;
	static const uint32_t uIntBodyPartData_FrostbiteSpider;
	static const uint32_t uIntBodyPartData_SabreCat;
	static const uint32_t uIntBodyPartData_Skeever;
	static const uint32_t uIntBodyPartData_DragonPriest;
	static const uint32_t uIntBodyPartData_Draugr;
	static const uint32_t uIntBodyPartData_Cow;
	static const uint32_t uIntBodyPartData_Dog;
	static const uint32_t uIntBodyPartData_Giant;
	static const uint32_t uIntBodyPartData_Chaurus;
	static const uint32_t uIntBodyPartData_ChaurusFlyer;
	static const uint32_t uIntBodyPartData_Mammoth;
	static const uint32_t uIntBodyPartData_AtronachFrost;
	static const uint32_t uIntBodyPartData_AtronachStorm;
	static const uint32_t uIntBodyPartData_AtronachFlame;
	static const uint32_t uIntBodyPartData_Falmer;
	static const uint32_t uIntBodyPartData_Horse;
	static const uint32_t uIntBodyPartData_Hare;
	static const uint32_t uIntBodyPartData_Goat;
	static const uint32_t uIntBodyPartData_Deer;
	static const uint32_t uIntBodyPartData_DwarvenSphereCenturion;
	static const uint32_t uIntBodyPartData_DwarvenSteamCenturion;
	static const uint32_t uIntBodyPartData_DwarvenBallistaCenturion;
	static const uint32_t uIntBodyPartData_DwarvenSpider;
	static const uint32_t uIntBodyPartData_Bear;
	static const uint32_t uIntBodyPartData_Horker;
	static const uint32_t uIntBodyPartData_Troll;
	static const uint32_t uIntBodyPartData_Hagraven;
	static const uint32_t uIntBodyPartData_Spriggan;
	static const uint32_t uIntBodyPartData_Chicken;
	static const uint32_t uIntBodyPartData_Mudcrab;
	static const uint32_t uIntBodyPartData_WerewolfBeast;
	static const uint32_t uIntBodyPartData_Gargoyle;
	static const uint32_t uIntBodyPartData_DwarvenBallistaCenturion;
	static const uint32_t uIntBodyPartData_DLC2_Riekling;
	static const uint32_t uIntBodyPartData_DLC2_Scrib;
	static const uint32_t uIntBodyPartData_DLC2_Dragon;
	static const uint32_t uIntBodyPartData_DLC2_HMDaedra;
	static const uint32_t uIntBodyPartData_DLC2_MountedRiekling;
	static const uint32_t uIntBodyPartData_DLC2_Netch;
	static const uint32_t uIntBodyPartData_DLC2_BenthicLurker;


	const std::vector<std::string> kmStr_Humanoid_Front = std::vector<std::string>{ "pa_1hmKillMovedecapbleedout", "233" };
	const std::vector<std::string> kmStr_Humanoid_Back;

public:


	static executionHandler* GetSingleton()
	{
		static executionHandler singleton;
		return  std::addressof(singleton);
	}

	enum EXECUTIONTYPE
	{
		humanoid = 0,


	};


	/*Attempt an execution. Help aggressor decide which killmove to use
	based on aggressor/victim's relative position, rotation, and victim's race.
	Also performs a check on whether the aggressor can execute victim.
	@param executer: actor to perform killmove.
	@param victim: actor to be executed.*/
	void attemptExecute(RE::Actor* executer, RE::Actor* victim);

private:
	/*Send an execution console command through console.
	@param uIntAggressorFormId: formId of aggressor, in unsigned int 32.
	@param uIntVictimFormId: formid of victim, in unsigned int 32.
	@param animStr: paired execution animation to be played, in unsigned int 32.*/
	void sendExecutionCommand(RE::Actor* executer, RE::Actor* victim, std::vector<std::string> executionStrSet);
	/*Let aggressor execute victim without condition checks, buffs/debuffs, or setting both parties to ghost.*/
	void execute(RE::Actor* executer, RE::Actor* victim, EXECUTIONTYPE executionType);




};