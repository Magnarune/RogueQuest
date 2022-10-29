#include "Highlighting.h"
#include "Engine.h"
Highlighting::Highlighting() {

}

Highlighting::~Highlighting(){
}

olc::Pixel Highlighting::OwnerColor(int owner){//If unit has Gold make him Golden
	auto& engine = Game_Engine::Current();
	return Color[owner];
}

void Highlighting::unitRing(int Owner){

}

olc::Pixel Highlighting::GoldIndicator(){
	return olc::Pixel(255, 255, 180, 255);//Yellow
}
