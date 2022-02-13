#pragma once
/*Handling block stamina damage and perfect blocking.*/
class blockHandler {
public:
	static blockHandler* GetSingleton()
	{
		static blockHandler singleton;
		return  std::addressof(singleton);
	}

	void processBlock(RE::Actor* victim, RE::Actor* aggressor, int iHitflag, RE::HitData hitData) {
		DEBUG("process blocking! Blocker: {} Aggressor: {}", victim->GetName(), aggressor->GetName());
		processStaminaBlock(victim, aggressor, iHitflag, hitData);
	}
private:
	inline void processStaminaBlock(RE::Actor* victim, RE::Actor* aggressor, int iHitflag, RE::HitData hitData);

	inline void processPerfectBlock();
};