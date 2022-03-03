#pragma once
#include "lib/robin_hood.h"
/*Actor's metadata*/
struct actorMetaData
{
public:
	RE::Actor* actor;
	/*Blocking*/
	bool isPerfectBlocking;
	bool isPerfectBlockingCoolingDown;
	bool isPreviousPerfectBlockingSuccess;
	float perfectBlockingTimer;
	float perfectBlockingCooldownTimer;

	/*Attacking*/
	bool hasAttackToCheckOut;
	enum checkoutAttackType
	{
		light = 0,
		power = 1,
	};

	/*Debuff*/
	bool isInDebuff;
	float staminaMeterBlinkingTimer;

	/*Stun*/
	float maxStun;
	float currentStun;
	float stunRegenTimer;

};
/*Managing additional information for all actors and storing metadatas.*/
class actorManager {
private:
	robin_hood::unordered_map <RE::Actor*, actorMetaData*> managedActors;
	/*Check if the actor's metadata is managed.*/
	bool isActorManaged(RE::Actor* actor);
public:
	/*Start managing this actor and tracking their metadata.
	@param actor: actor to be managed.*/
	void manageActor(RE::Actor* actor);

	bool isInDebuff(RE::Actor* actor);
	bool isPerfectBlocking(RE::Actor* actor);
	bool isPerfectBlockingCoolingDown(RE::Actor* actor);

	void update();
private:
	inline void updateStun(actorMetaData* metaData, float deltaTime);
	inline void updateStagger(actorMetaData* metaData, float deltaTime);
	inline void updateBlocking(actorMetaData* metaData, float deltaTime);
	//inline void updateDebuff(actorMetaData* metaData, float deltaTime);
};
