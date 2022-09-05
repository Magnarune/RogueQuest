#pragma once

#include <memory>
#include "Unit.h"
#include "Assets.h"

class UnitManager {

    std::vector<std::shared_ptr<Unit>> unitList; // fixed size for optimization

public:
    UnitManager();
    ~UnitManager() {}

    void Update(float delta);

    std::shared_ptr<Unit> GenerateUnit(const std::string& name);
};