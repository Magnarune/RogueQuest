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
					Bonus.meleedamage += rewards.second[i];
				}
				if (rewards.first == "Armor") {
					Bonus.armour += rewards.second[i];
				}
				if (rewards.first == "Health") {
					Bonus.health += rewards.second[i];
				}
				if (rewards.first == "Movespeed") {
					Bonus.movespeed += rewards.second[i];
				}
				if (rewards.first == "Attackspeed") {
					Bonus.attackspeed += rewards.second[i];
				}
				if (rewards.first == "Movespeed") {
					Bonus.movespeed += rewards.second[i];
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