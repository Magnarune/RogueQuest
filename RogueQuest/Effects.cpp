#include "Effects.h"
#include "Engine.h"

//Effects::Effects() {
//}
//
//Effects::~Effects(){
//}
//
//
//void Effects::UpdateEffect(float fElapseTime){
//    for (int i = 0; i < Instance.size(); i++) {
//        if (Instance[i].second == true) {//smoke effect
//            for (int i = 0; i < EffectPositions.size(); i++) {
//                EffectPositions[i].x += rand() % 5 - 2.f;
//                EffectPositions[i].y += (rand() % 2) * 0.4f;
//            }
//            Instance[i].first -= Instance[i].first * fElapseTime;
//
//        }
//        else {
//            m_ftimer += fElapseTime;
//            if (m_ftimer > 0.2f) {
//                frame++;
//                frame % animation_length;
//            }
//
//        }
//    }
//
//}
//
//void Effects::DrawEffect(){
//	auto& engine = Game_Engine::Current();
//
//	//engine.tv.DrawPolygonDecal()
//}
