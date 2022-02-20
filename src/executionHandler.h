#pragma once
/*Class handling all executions*/
class executionHandler {
private:
#pragma region CreatureBodyTypeData
	static const uint32_t uIntBodyPartData_Humanoid = 29;
	static const uint32_t uIntBodyPartData_Dragon;
	static const uint32_t uIntBodyPartData_FrostbiteSpider = 96553;
	static const uint32_t uIntBodyPartData_SabreCat;
	static const uint32_t uIntBodyPartData_Skeever;
	static const uint32_t uIntBodyPartData_DragonPriest;
	static const uint32_t uIntBodyPartData_Draugr = 298003;
	static const uint32_t uIntBodyPartData_Cow;
	static const uint32_t uIntBodyPartData_Dog;
	static const uint32_t uIntBodyPartData_Giant = 333739;
	static const uint32_t uIntBodyPartData_Chaurus;
	static const uint32_t uIntBodyPartData_ChaurusFlyer;
	static const uint32_t uIntBodyPartData_Mammoth;
	static const uint32_t uIntBodyPartData_AtronachFrost;
	static const uint32_t uIntBodyPartData_AtronachStorm;
	static const uint32_t uIntBodyPartData_AtronachFlame;
	static const uint32_t uIntBodyPartData_Falmer = 384418;
	static const uint32_t uIntBodyPartData_Horse;
	static const uint32_t uIntBodyPartData_Hare;
	static const uint32_t uIntBodyPartData_Goat;
	static const uint32_t uIntBodyPartData_Deer;
	static const uint32_t uIntBodyPartData_DwarvenSphereCenturion;
	static const uint32_t uIntBodyPartData_DwarvenSteamCenturion;
	static const uint32_t uIntBodyPartData_DwarvenBallistaCenturion;
	static const uint32_t uIntBodyPartData_DwarvenSpider;
	static const uint32_t uIntBodyPartData_Bear = 551164;
	static const uint32_t uIntBodyPartData_Horker;
	static const uint32_t uIntBodyPartData_Troll = 601235;
	static const uint32_t uIntBodyPartData_Hagraven = 619074;
	static const uint32_t uIntBodyPartData_Spriggan = 656416;
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
#pragma endregion
#pragma region Humanoid
	/*Sets of strings corresponding to different types of executions.*/
	const std::vector<std::string> kmStr_Humanoid_1hm_Dagger = std::vector<std::string>{
		"Val_1HMKillMoveShieldBashAttack",
		"Val_1HMKillMoveStabUpFace",
		"Val_KillMoveDecapSlash",
		"Val_KillingBlowBleedOutDecap",
		"Val_KillingBlowBleedOut",
		"Val_1HMKillMoveStabDownChest",
		"Val_1HMKillMoveRepeatStabDowns",
		"Val_1HMKillMoveGrappleStab",
		"Val_1HMKillMoveD",
		"Val_1HMKillMoveE", //seems to need shield
		"Val_KillMoveShortD",
		"Val_1HMKillMoveI",
		"Val_KillMoveShortB",
		"Val_1HMKillMoveJ"
	};
	const std::vector<std::string> kmStr_Humanoid_1hm_Sword = std::vector<std::string>{ 
	"Val_1HMKillMoveA_NoShield", "Val_1HMKillMoveB", 
	"Val_1HMKillMoveC", //throw over shoulder and stab in the heart
	//"Val_1HMKillMoveF", //shield bash all the way to very far
	"Val_1HMKillMoveG", "Val_1HMKillMoveH", "Val_1HMKillMoveK", 
	"Val_1HMKillMoveM",
	//"Val_1HMKillMoveL" //very long distance
	/*The following are shortblade killmoves i.e. shared with dagger*/
	"Val_1HMKillMoveShieldBashAttack",
	"Val_1HMKillMoveStabUpFace",
	"Val_KillMoveDecapSlash",
	"Val_KillingBlowBleedOutDecap",
	"Val_KillingBlowBleedOut",
	"Val_1HMKillMoveStabDownChest",
	"Val_1HMKillMoveRepeatStabDowns",
	"Val_1HMKillMoveGrappleStab",
	"Val_1HMKillMoveD",
	"Val_1HMKillMoveE", //seems to need shield
	"Val_KillMoveShortD",
	"Val_1HMKillMoveI",
	"Val_KillMoveShortB",
	"Val_1HMKillMoveJ"
	};
	const std::vector<std::string> kmStr_Humanoid_1hm_Axe_Mace = std::vector<std::string>{
		"Val_KillingBlowBleedOutAxeDecap",
		"Val_KillMoveDecapSlashAxe",
		"Val_KillMoveShieldBashSlash",
		"Val_KillMoveShortC",
		"Val_KillMoveShortAD"
	};
	const std::string kmStr_Humanoid_1hm_Back = "Val_KillMove1HMBackStab";
	const std::vector<std::string> kmStr_Humanoid_h2h = std::vector<std::string>{
		"Val_H2HKillMoveKneeThrow", "Val_H2HKillMoveBodySlam", "Val_H2HKillMoveComboA",
		"Val_H2HKillMoveSlamA"
	};
	const std::string kmStr_Humanoid_h2h_Back = "Val_KillMoveH2HSuplex";
	const std::vector<std::string> kmStr_Humanoid_2hm = std::vector<std::string>{
		"Val_KillMove2HMDecapBleedOut", "Val_KillMove2HMDecapSlash", "Val_KillMove2HMSlash", 
		"Val_KillMove2HM3Slash", "Val_KillMove2HMUnderSwingLeg", "Val_KillMove2HMStab"
	};
	const std::vector<std::string> kmStr_Humanoid_2hm_Back = std::vector<std::string>{
		"Val_KillMove2HMStabFromBehind"
	};
	const std::vector<std::string> kmStr_Humanoid_2hw = std::vector<std::string>{
		"Val_KillMove2HWDecapBleedOut", "Val_KillMove2HWDecap", "Val_KillMove2HWHeadButt",
		"Val_KillMove2HWChopKick", "Val_KillMove2HWB"
	};
	const std::string kmStr_Humanoid_2hw_Back = "Val_KillMove2HWHackFromBehind";
	const std::vector<std::string> kmStr_Humanoid_dw = std::vector<std::string>{
		"Val_KillMoveDualWieldBleedOutDecap",
		"Val_KillMoveDualWieldDecap",
		"Val_KillMoveDualWield",
		"Val_KillMoveDualWieldXSlash",
		"Val_KillMoveDualWieldDualSlash"
	};
#pragma endregion
#pragma region Falmer
	const std::vector<std::string> kmStr_Falmer_1hm = std::vector<std::string>{
	"Val_1HMKillMoveFalmerA", "Val_1HMKillMoveFalmerB", "Val_1HMKillMoveFalmerC"
	};
	const std::vector<std::string> kmStr_Falmer_2hm = std::vector<std::string>{
		"Val_2HMKillMoveFalmerA", "Val_2HMKillMoveFalmerB"
	};
	const std::string kmStr_Falmer_2hw = "Val_2HWKillMoveFalmer";
#pragma endregion
#pragma region Draugr&Skeletons
	const std::vector<std::string> kmStr_Undead_1hm = std::vector<std::string>{
	"Val_1HM_KillMoveDraugrShort", "Val_1HM_KillMoveDraugrShortB", "Val_1HM_KillMoveDraugrB",
	"Val_1HM_KillMoveDraugr"
	};
	const std::vector<std::string> kmStr_Undead_2hm = std::vector<std::string>{
		"Val_2HM_KillMoveDraugr"
	};
	const std::vector<std::string> kmStr_Undead_2hw = std::vector<std::string>{
		"Val_2HW_KillMoveDraugr",
	};

#pragma endregion
#pragma region Spider
	const std::vector<std::string> kmStr_Spider_1hm = std::vector<std::string>{
	"Val_1HMKillMoveSpiderStabA", "Val_1HMKillMoveSpiderSmashA"
	};
	const std::string kmStr_Spider_2hm = "Val_2HMSpiderKillMoveA";
	const std::string kmStr_Spider_2hw = "Val_2HWSpiderKillMoveA";
#pragma endregion
#pragma region Gargoyle
	const std::vector<std::string> kmStr_Gargoyle_1hm = std::vector<std::string>{
		"Val_1HMVBSoccerKick"
	};
	const std::vector<std::string> kmStr_Gargoyle_1hm_Back = std::vector<std::string>{
		"Val_1HMVBSlowBackStab"
	};
	const std::vector<std::string> kmStr_Gargoyle_2hm = std::vector<std::string>{
	"Val_2HMKillMoveVampireBrute"
	};
	const std::vector<std::string> kmStr_Gargoyle_2hw = std::vector<std::string>{
		"Val_2HWKillMoveVampireBrute"
	};
#pragma endregion
#pragma region Giant
	const std::vector<std::string> kmStr_Giant_1h = std::vector<std::string>{
	"Val_1HMGiantKillMoveBleedOutA", "Val_1HMGiantKillMoveA", "Val_1HMGiantKillMoveB"
	};
	const std::string kmStr_Giant_2hm = "Val_2HMGiantKillMove";
	const std::string kmStr_Giant_2hw = "Val_2HWGiantKillMove";
	const std::vector<std::string> kmStr_Bear = std::vector<std::string>{
		"Val_1HMKillMoveBearA", "Val_1HMKillMoveBearB", "Val_2HMKillMoveBearA",
		"Val_2HMKillMoveBearB", "Val_2HWKillMoveBear"
	};
#pragma endregion
#pragma region Bear
	const std::vector<std::string> kmStr_Bear_1hm = std::vector<std::string>{
		"Val_1HMKillMoveBearA", "Val_1HMKillMoveBearB"
	};
	const std::vector<std::string> kmStr_Bear_2hm = std::vector<std::string>{
		"Val_2HMKillMoveBearA", "Val_2HMKillMoveBearB"
	};
	const std::string kmStr_Bear_2hw = "Val_2HWKillMoveBear";
#pragma endregion
#pragma region SabreCat
	const std::vector<std::string> kmStr_SabreCat_1hm = std::vector<std::string>{
		"Val_1HmKillMoveSabreCatD", 
		"Val_1HMKillMoveSabreCatShortA", 
		"Val_1HMKillMoveSabreCatShortB"
	};
	const std::vector<std::string> kmStr_SabreCat_2hm = std::vector<std::string>{
		"Val_2HmKillMoveSabreCatB", "Val_2HmKillMoveSabreCatA"
	};
	const std::string kmStr_SabreCat_2hw = "Val_2HWSabreCatKillMove";
#pragma endregion
#pragma region Wolf
	const std::vector<std::string> kmStr_Wolf_1hm = std::vector<std::string>{
		"Val_1HMWolfKillMoveA", "Val_1HMWolfKillMoveB"
	};
	const std::vector<std::string> kmStr_Wolf_2hm = std::vector<std::string>{
		"Val_2HMWolfKillMoveA", "Val_2HMWolfKillMoveB"
	};
	const std::string kmStr_Wolf_2hw = "2HWWolfKillMoveA";
#pragma endregion
#pragma region Troll
	const std::vector<std::string> kmStr_Troll_1hm = std::vector<std::string>{
	"Val_1HMTrollKillMoveA", "Val_1HMTrollKillMoveB"
	};
	const std::vector<std::string> kmStr_Troll_2hm = std::vector<std::string>{
		"Val_2HMTrollKillMoveA", "Val_2HMTrollKillMoveB"
	};
	const std::string kmStr_Troll_2hw = "Val_2HWTrollKillMoveA";
#pragma endregion
#pragma region Hagraven
	const std::vector<std::string> kmStr_Hagraven_1hm = std::vector<std::string>{
	"Val_1HMKillMoveHagravenB", 
	"1HMKillMoveHagravenA"
	};
	const std::string kmStr_Hagraven_2hm = "Val_2HMHagravennKillMoveA";
	const std::string kmStr_Hagraven_2hw = "Val_2HWHagravenKillMoveA";
#pragma endregion
#pragma region Spriggan
	const std::vector<std::string> kmStr_Spriggan_1hm = std::vector<std::string>{
		"Val_1HMKillMoveSprigganA",
		"Val_1HMKillMoveSprigganB"
	};
	const std::string kmStr_Spriggan_2hm = "Val_2HMSprigganKillMoveA";
	const std::string kmStr_Spriggan_2hw = "Val_2HWSprigganKillMoveA";
#pragma endregion
#pragma region Boar
	const std::vector<std::string> kmStr_Boar_1hm = std::vector<std::string>{
	"Val_1HMBoarKillMovesStab",
	"Val_1HMBoarKillMovesSmash"
	};
	const std::string kmStr_Boar_2hm = "Val_2HMBoarKillMovesSlash";
	const std::string kmStr_Boar_2hw = "Val_2HWBoarKillMovesSmash";
#pragma endregion
#pragma region Riekling
	const std::vector<std::string> kmStr_Riekling_1hm = std::vector<std::string>{
		"Val_1HMKillRieklingKillMovesStab",
		"Val_1HMRieklingKillMovesSlash"
	};
	const std::string kmStr_Riekling_2hm = "Val_2HMRieklingKillMovesSlash";
	const std::string kmStr_Riekling_2hw = "Val_2HWRieklingKillMovesSmash";
#pragma endregion
#pragma region AshHopper
	const std::vector<std::string> kmStr_AshHopper_1hm = std::vector<std::string>{
	"Val_1HMScribKillMovesSlash",
	"Val_1HMKillScribKillMovesStab"
	};
	const std::string kmStr_AshHopper_2hm = "Val_2HMScribKillMovesSlash";
	const std::string kmStr_AshHopper_2hw = "Val_2HWScribKillMovesSmash";
#pragma endregion
#pragma region SteamCenturion
	const std::vector<std::string> kmStr_SteamCenturion_1hm = std::vector<std::string>{
		"Val_1HMKillMoveSteamCenturionA", "Val_1HMKillMoveSteamCenturionB"
	};
	const std::string kmStr_SteamCenturion_2hm = "Val_2HMSteamCenturionKillMoveA";
	const std::string kmStr_SteamCenturion_2hw = "Val_2HWSteamCenturionKillMoveA";
#pragma endregion
#pragma region DwarwenBallista
	const std::string kmStr_DwarwenBallista_1hm = "Val_1HMDwarvenBallistarKillMoves";
	const std::string kmStr_DwarwenBallista_2hm = "Val_2HMDwarvenBallistaKillMovesSlash";
	const std::string kmStr_DwarwenBallista_2hw = "Val_2HWDwarvenBallistaKillMovesSmash";
#pragma endregion
#pragma region ChaurusFlyer
	const std::vector<std::string> kmStr_ChaurusFlyer_1hm = std::vector<std::string>{
	"Val_1HMKillMoveChaurusFlyerStomp", "Val_1HMKillMoveChaurusFlyerKick",
	};
	const std::string kmStr_ChaurusFlyer_2hm = "Val_2HMKillMoveChaurusFlyer";
	const std::string kmStr_ChaurusFlyer_2hw = "Val_2HWKillMoveChaurusFlyer";
#pragma endregion
#pragma region Lurker
	const std::string kmStr_Lurker_1hm = "Val_1HMBenthicLurkerKillMoves";
	const std::string kmStr_Lurker_2hm = "Val_2HMBenthicLurkerKillMovesSlash";
	const std::string kmStr_Lurker_2hw = "Val_2HWBenthicLurkerKillMovesSmash";
#pragma endregion
#pragma region Dragon
	const std::vector<std::string> kmStr_Dragon_1HM = std::vector<std::string>{
	"Val_1HMSword_KillMoveDragonRodeoStabShort", "Val_1HMSword_KillMoveDragonRodeoSlash"};
	const std::string kmStr_Dragon_2hm = "Val_2HMDragonKillMoveSlash";
	const std::vector<std::string> kmStr_Dragon_2hw = std::vector<std::string>{
		"Val_2HWDragonKillMoveRodeoSlash", 
		"Val_2HWDragonKillMoveSlash"
	};
#pragma endregion
public:


	static executionHandler* GetSingleton()
	{
		static executionHandler singleton;
		return  std::addressof(singleton);
	}



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
	void sendExecutionCommand(RE::Actor* executer, RE::Actor* victim, std::string executionStr);

	inline void executeHumanoid(RE::Actor* executer, RE::Actor* victim, RE::WEAPON_TYPE weaponType);
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

}