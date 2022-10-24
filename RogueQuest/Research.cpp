#include "Research.h"
#include "Engine.h"
Research::Research() {

}

Research::~Research() {
	ImportList.clear();
}

void Research::ImportResearch() {
	auto& engine = Game_Engine::Current();
	for (auto& list : ImportList) {
		if (!engine.assetManager->ResearchExists(list)) continue;
		const auto& data = engine.assetManager->GetResearchData(list);
		std::shared_ptr<Researchable> research;
		research.reset(new Researchable);
		research->name = list;
		research->researchlen = data.lua_data["ResearchAmount"];	
		sol::table cost = data.lua_data["Parameters"]["InitialCost"];
		for (int i = 0; i < cost.size(); i++)
			research->Cost.push_back(cost[i + 1]);

		research->ResearchTime = data.lua_data["Parameters"]["ResearchTime"];		
		sol::table Requirement = data.lua_data["Requirements"]["Buildings"];	
		for (int i = 0; i < Requirement.size(); i++)
			research->BuildingRequirements.push_back(Requirement[i + 1]);

		sol::table Rewards = data.lua_data["Reward"];
		if (Rewards["MeleeDamage"] != sol::nil) {
			sol::table damage = Rewards["MeleeDamage"];
			std::vector<int> damages;
			for (int i = 0; i < damage.size();i++) {
				damages.push_back(damage[i+1]);
			}
			research->RewardList.insert({ "MeleeDamage" ,damages });
		}
		if (Rewards["Armour"] != sol::nil) {
			sol::table damage = Rewards["Armour"];
			std::vector<int> damages;
			for (int i = 0; i < damage.size(); i++) {
				damages.push_back(damage[i + 1]);
			}
			research->RewardList.insert({ "Armour" ,damages });
		}
		if (Rewards["MoveSpeed"] != sol::nil) {
			sol::table damage = Rewards["MoveSpeed"];
			std::vector<int> damages;
			for (int i = 0; i < damage.size(); i++) {
				damages.push_back(damage[i + 1]);
			}
			research->RewardList.insert({ "MoveSpeed" ,damages });
		}
		if (Rewards["AttackSpeed"] != sol::nil) {
			sol::table damage = Rewards["AttackSpeed"];
			std::vector<int> damages;
			for (int i = 0; i < damage.size(); i++) {
				damages.push_back(damage[i + 1]);
			}
			research->RewardList.insert({ "AttackSpeed" ,damages });
		}


		if (Rewards["Unlocks"] != sol::nil) {
			sol::table damage = Rewards["Unlocks"];
			for (int i = 0; i < damage.size(); i++) {
				research->UnlockRewards.push_back(damage[i + 1]);
			}
		}
		Researchables.insert_or_assign(list, research);
	}
	ImportList.clear();
}

void Research::GetResearchdata(std::string name) {
	

}

