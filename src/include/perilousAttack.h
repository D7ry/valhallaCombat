#include <unordered_set>
class perilousAttack
{
public:
	static perilousAttack* GetSingleton() {
		static perilousAttack singleton;
		return std::addressof(singleton);
	}

	void startPerilousAttack(RE::Actor* a_actor);
	void endPerilousAttack(RE::Actor* a_actor);
	
private:
	std::unordered_set<RE::Actor*> m_perilousAttackActors;

	
	
};
