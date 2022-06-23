/*Handle all hit reactions: triggerStagger, knock back, guard break etc*/
class reactionHandler {
public:
	enum reactionType
	{
		kSmall = 0,
		kMedium,
		kLarge,
		kLargest,
		kKnockBack,
	};
	/*Stagger the actor with magnitude and correct stagger direction, 0.1 seconds after sending a staggerStop event.*/
	static void triggerContinuousStagger(RE::Actor* causer, RE::Actor* reactor, reactionType reactionType);
	/*Trigger a stagger reaction.
	@param causer: actor that causes the stagger.
	@param reactor: actor receiving the stagger.
	@param reactionType: type of the reaction the actor receives.*/
	static void triggerStagger(RE::Actor* causer, RE::Actor* reactor, reactionType reactionType);

	/*Trigger a recoil reaction.*/
	static void triggerRecoil(RE::Actor* reactor, reactionType);
	/*Initialize a downed state for an actor.
	@param a_actor: actor to enter downed state*/
	static void triggerDownedState(RE::Actor* a_actor);

	/*Recover an actor from the downed state.
	@param a_actor: actor to recover from downed state.*/
	static void recoverDownedState(RE::Actor* a_actor);
private:
	/*Asynchronous function to trigger a stagger after 0.1 sec of waiting time, to ensure the previous "staggerstop" event is received.*/
	static void async_triggerContinuousStagger(RE::Actor* causer, RE::Actor* reactor, reactionType reactionType);

	/*Initialize a knockback.
	@param causer: actor to cause the knockback.
	@param reactor: actor to be knocked back.*/
	static void triggerKnockBack(RE::Actor* causer, RE::Actor* reactor);
	/*Stagger the reactor with magnitude and correct stagger direction.
	@param causer: the cause of the stagger.
	@param reactor: actor who staggers.
	@param magnitude: magnitude of the triggerStagger.*/
	static void triggerStagger(RE::Actor* causer, RE::Actor* reactor, float magnitude);

	/*Inflict a "poise" reaction onto reactor.
	@param causer: the cause of the reaction.
	@param reactor: actor who react.
	@param reactionType: type of the poise reaction.*/
	static void triggerPoiseReaction(RE::Actor* causer, RE::Actor* reactor, reactionType reactionType);

	
};
