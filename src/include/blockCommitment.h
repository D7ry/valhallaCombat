#pragma once
class blockCommitment
{
public:
	static blockCommitment* GetSingleton()
	{
		static blockCommitment singleton;
		return std::addressof(singleton);
	}

	void update();

	void queueUnblock(float a_delay);
};
