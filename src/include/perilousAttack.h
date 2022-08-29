#include <unordered_set>
class perilousAttack
{
public:
	static perilousAttack* GetSingleton() {
		static perilousAttack singleton;
		return std::addressof(singleton);
	}

	/// <summary>
	/// Called when the actor is about to attack.
	/// Make the actor perilous attacking by adding the dummy magic effect and 
	/// </summary>
	/// <param name="a_actor"></param>
	void startPerilousAttack(RE::Actor* a_actor);
	void endPerilousAttack(RE::Actor* a_actor);
	
private:
	std::unordered_set<RE::Actor*> m_perilousAttackActors;
	/
	
	
};
