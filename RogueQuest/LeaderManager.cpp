#include "LeaderManager.h"

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
			std::erase(lead->Allies[i], allignment); //if is friend remove it
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