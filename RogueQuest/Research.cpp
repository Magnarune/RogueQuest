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
	
		research->Oncer = data.lua_data["Parameters"]["Oncer"];
		research->Cost = data.lua_data["Parameters"]["InitialCost"];
		if(!research->Oncer)
			research->Scale = data.lua_data["Parameters"]["CostScale"];
		research->ResearchTime = data.lua_data["Parameters"]["ResearchTime"];		
		sol::table Requirement = data.lua_data["Requirements"]["Buildings"];	
		for (int i = 0; i < Requirement.size(); i++)
			research->BuildingRequirements.push_back(Requirement[i + 1]);

		sol::table Rewards = data.lua_data["Reward"];sol::table stats = Rewards["Stats"];
		sol::table::iterator it = stats.begin();		
		for (it; it != stats.end(); it++) {
			research->RewardList.insert_or_assign((*it).first.as<std::string>(), (*it).second.as<int>());//Good catch
		}
		
		Researchables.insert_or_assign(list, research);
	}
	ImportList.clear();
}

void Research::GetResearchdata(std::string name) {
	

}