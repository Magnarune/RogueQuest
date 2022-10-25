#include "Leaders.h"
Leader::Leader() {

}
Leader::~Leader() {

}

void Leader::UpdateBonus() {
	for (int i = 0; i < researchUpgrades.size(); i++) {

		for (int j = 0; j < researchUpgrades[i].lock()->RewardList.size(); j++){

			for(auto& rewards : researchUpgrades[i].lock()->RewardList)	{
				if (rewards.first == "MeleeDamage") {
					Bonus.meleedamage += rewards.second[researchUpgrades[i].lock()->Level - 1];
				}
				if (rewards.first == "Armour") {
					Bonus.armour += rewards.second[researchUpgrades[i].lock()->Level - 1];
				}
				if (rewards.first == "Health") {
					Bonus.health += rewards.second[researchUpgrades[i].lock()->Level - 1];
				}
				if (rewards.first == "MoveSpeed") {
					Bonus.movespeed += rewards.second[researchUpgrades[i].lock()->Level - 1];
				}
				if (rewards.first == "Attackspeed") {
					Bonus.attackspeed += rewards.second[researchUpgrades[i].lock()->Level - 1];
				}
				if (rewards.first == "Movespeed") {
					Bonus.movespeed += rewards.second[researchUpgrades[i].lock()->Level - 1];
				}
			}
		}
	}
}

int Leader::CheckCost(std::string name) {
	for (int i = 0; i < researchUpgrades.size(); i++) {
		if (researchUpgrades[i].lock()->name == name){
			return researchUpgrades[i].lock()->Level;
		}
	}
	return 0;
}