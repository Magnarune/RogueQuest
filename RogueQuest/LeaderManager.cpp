#include "LeaderManager.h"
#include "Engine.h"

LeaderManager::LeaderManager() {
	
}

LeaderManager::~LeaderManager() {
	LeaderList.clear();
}

void LeaderManager::AddLeader(int Size) {//Only done at start of game. Must be 10 or less
	
	for (int i = 0; i < Size + 1; i++) {
		std::shared_ptr<Leader> playerleader;
		playerleader.reset(new Leader);
		playerleader->Allies.insert({ static_cast<leaderList>(i), {i}});
		playerleader->AGE="Stone Age";//Starting "Age"
		
		LeaderList.push_back(playerleader);
	}		
}

void LeaderManager::AddFriend(std::shared_ptr<Leader> lead , int allignment) {
	for (auto& leader : LeaderList) {
		int i = 0;
		if (leader == lead) {
			lead->Allies[i].push_back(allignment);
		}
		else
			i++;
	}
}

void LeaderManager::RemoveFriend(std::shared_ptr<Leader> lead, int allignment) {
	for (auto& leader : LeaderList) {
		int i = 0;
		if (leader == lead) {
			std::erase(lead->Allies[i], allignment); //if is friend remove it
		}
		else
			i++;
	}
}

void LeaderManager::AddEnemy(std::shared_ptr<Leader> lead, int allignment) {

	for (auto& leader : LeaderList) {
		int i = 0;
		if (leader == lead) {
			std::erase(lead->Allies[i], allignment); //if add enemy
			lead->Allies[i].push_back(-allignment);
		}
		else
			i++;
	}
}

void LeaderManager::RemoveEnemy(std::shared_ptr<Leader> lead, int allignment) {

	for (auto& leader : LeaderList) {
		int i = 0;
		if (leader == lead) {
			std::erase(lead->Allies[i], -allignment); //if is enemy remove it
		}
		else
			i++;
	}
}

void LeaderManager::Gold(int owner, int cost) {
	LeaderList[owner]->Gold += cost;
}

void LeaderManager::Food(int owner, int cost) {
	LeaderList[owner]->Food += cost;
}

void LeaderManager::FoodMaintenance( std::vector<std::weak_ptr<Unit>> unilist) {

	for (int i = 0; i < LeaderList.size(); i++) {
		int fos = 0;
		for (int j = 0; j < unilist.size(); j++) {
			if (i == unilist[j].lock()->Owner)
				fos += unilist[j].lock()->food;
		}
		LeaderList[i]->Food = fos;
	}
}

void LeaderManager::BuildingMaintenance(std::vector<std::weak_ptr<Building>> buildlist) {
	for (int i = 0; i < LeaderList.size(); i++) {
		LeaderList[i]->ResearchUpgrades.clear();
		for (int j = 0; j < buildlist.size(); j++) {
			if (buildlist[j].expired()) continue;
			if (i == buildlist[j].lock()->Owner)
				LeaderList[i]->ResearchUpgrades.push_back( buildlist[j].lock()->name);
		}	
		std::vector<std::string>::iterator it;
		it =std::unique(LeaderList[i]->ResearchUpgrades.begin(), LeaderList[i]->ResearchUpgrades.end());
		LeaderList[i]->ResearchUpgrades.erase(it, LeaderList[i]->ResearchUpgrades.end());
	}
}

void LeaderManager::AddResearch(int Owner, std::weak_ptr<Researchable> research) {
			
	auto findval = std::find_if(LeaderList[Owner]->researchUpgrades.begin(), LeaderList[Owner]->researchUpgrades.end(),
		[&research](std::weak_ptr<Researchable>& val) {
			return val.lock() == research.lock();
		}
	);
			
	
	
	if(findval == LeaderList[Owner]->researchUpgrades.end()){		
		LeaderList[Owner]->researchUpgrades.push_back(research);		
	}


	for (int i = 0; i < LeaderList[Owner]->researchUpgrades.size(); i++) {
		if (LeaderList[Owner]->researchUpgrades[i].lock() == research.lock()) {
			LeaderList[Owner]->researchUpgrades[i].lock()->Level += 1;
		}
	}
	for (int i = 0; i < research.lock()->UnlockRewards.size(); i++) {
		if (std::find(LeaderList[Owner]->Unlockes.begin(), LeaderList[Owner]->Unlockes.end(), research.lock()->UnlockRewards[i]) != LeaderList[Owner]->Unlockes.end())
			continue;
		else
			LeaderList[Owner]->Unlockes.push_back(research.lock()->UnlockRewards[i]);
	}
	
	LeaderList[Owner]->UpdateBonus();
}

void LeaderManager::FindHomeBase() {
	auto& engine = Game_Engine::Current();
	for (int Owner = 1; Owner < engine.leaders->LeaderList.size(); Owner++) {		
		engine.buildingManager->IterateAllBuildings(([&](auto build) -> bool {

			if (build->name == "Castle" && build->Owner == Owner) {
				engine.leaders->LeaderList[Owner]->HomeBase = build;
				return false;
			}
			return true;
			}));
	}
}