#pragma once

#include <numeric>
#include <algorithm>
#include <functional>
#include <memory>
#include "Unit.h"
#include "Assets.h"

class UnitManager {
    std::vector<std::shared_ptr<Unit>> unitList; // fixed size for optimization
public:
    UnitManager();
    ~UnitManager() {}
    void MoveUnits(olc::vf2d Target);
    void SelectUnits(olc::vf2d Initial, olc::vf2d Final);
    void Update(float delta);
    void Draw();
    std::shared_ptr<Unit> GenerateUnit(const std::string& name, olc::vf2d pos={0.f, 0.f});
    size_t GetUnitCount(const std::string& name);
    std::shared_ptr<Unit> GetUnit(const std::string& name, size_t index=0);
};