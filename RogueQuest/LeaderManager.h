#include "olcPixelGameEngine.h"
#include "Leaders.h"
#include "Building.h"
#include "Research.h"
#include "Unit.h"
#include "clock.h"

class LeaderManager {
public:
	LeaderManager();
	virtual ~LeaderManager();


	enum leaderList {
		Neutral,
		Leader_One,
		Leader_Two,
		Leader_Three,
		Leader_Four,
		Leader_Five,
		Leader_Six,
		Leader_Seven,
		Leader_Eight,
		Leader_Nine,
		Leader_Ten
	};

	std::vector<std::shared_ptr<Leader>> LeaderList;
	
	void DestroyLeader();
	
	void AddLeader(int Size);

	void AddFriend(std::shared_ptr<Leader> lead, int allignment);
	void RemoveFriend(std::shared_ptr<Leader> lead, int allignment);

	void AddEnemy(std::shared_ptr<Leader> lead, int allignment);
	void RemoveEnemy(std::shared_ptr<Leader> lead, int allignment);

	void AddResearch(int Owner, std::weak_ptr<Researchable> researchcomplete);

	void Gold(int owner, int cost);
	void Food(int owner, int Food);

	void FindHomeBase();
	void FoodMaintenance( std::vector<std::weak_ptr<Unit>> unilist);
	void BuildingMaintenance(std::vector<std::weak_ptr<Building>> buildlist);
};