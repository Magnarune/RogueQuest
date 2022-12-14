#pragma once
#include "olcPixelGameEngine.h"
#include <map>
class Researchable {
public:
	Researchable() {};
	virtual ~Researchable() {};
	std::vector<std::string> BuildingRequirements;	//buildings that have to be alive to research
	std::vector<std::string> OtherRequirements;		//IDK like are you in iron age and do you have lvl 3 roads?
	std::string name;
	int researchlen=0;//Max Level
	int Level = 0; //Level of Research
	//      Level of Research, Reward Type
	std::map<std::string, std::vector<int>> RewardList;	//You get rewarded with the damage type reward
	std::vector<std::string> UnlockRewards;
	std::vector<int> Cost;	//This goes up per research
	int Scale=0;	//This will be a complex function
	int ResearchTime=0;//Time it will take to research
	bool Oncer=false;		 //Done once no scale nothing more ie research bows bam you got bows
};
class Research {//Manages all the "Leader's" Research
	std::vector<std::string> ImportList; //List Research to Import
public:
	std::map<std::string ,std::shared_ptr<Researchable>> Researchables;//List of Researchables
	virtual ~Research();
	Research();
	void ImportResearch();//Takes all the lua files and turns them into Researchables
	void addResearch();//Gived the leader the Research
	void GetResearchdata(std::string nameofresearch);//data upon request

	struct Rewardtype {//type of reward
		std::string name;
		int damage;
		int health;
		float speed;
		int defense;		
	};
	friend class cAssets;
};
