#include "olcPixelGameEngine.h"
#include <iostream>
#include <string>
#include <set>
#include <functional>
#include <map>
#include "sol/sol.hpp"
//This should prob be a Lua :/
/*
std::string StringifyObject(sol::object val) {
    std::set<const void*> tableMap;
    std::function<std::string(sol::object)> Stringify = [&](sol::object value) -> std::string {

        switch (value.get_type()) {
        case sol::type::userdata: return "<userdata>";
        case sol::type::boolean: return std::to_string(value.as<bool>());
        case sol::type::string: return value.as<std::string>();
        case sol::type::function: return "<function>";
        case sol::type::number: {
            double val = value.as<double>();
            return fmod(val, 1.0f) == 0.0f ? std::to_string(int64_t(val)) : std::to_string(val);
        }
        case sol::type::table: {
            if (tableMap.count(value.pointer())) {
                return "...";
            }
            tableMap.emplace(value.pointer());
            std::string copy = "{ ";
            sol::table table = value.as<sol::table>();
            for (auto it = table.begin(); it != table.end(); ++it) {
                std::pair<std::string, std::string> fix, fix2;
                if ((*it).first.get_type() == sol::type::table) {
                    fix.first = "[";
                    fix.second = "]";
                }
                if ((*it).second.get_type() == sol::type::string) {
                    fix2.first = "\"";
                    fix2.second = "\"";
                }
                copy += (it != table.begin() ? ", " : "")
                    + fix.first + Stringify((*it).first) + fix.second + " = "
                    + fix2.first + Stringify((*it).second) + fix2.second;
            }
            copy += "}";

            return copy;
        }
        default: return "nil";
        }
    };

    return Stringify(val); // begin recursion
}

class cAssets
{
	cAssets(std::string name);
	virtual ~cAssets();
public:
    
	sol::state lua_assets;
	sol::table UnitData, FileSets;
	olc::vi2d vTileSize;
    int Animation;
	std::vector<olc::Sprite*> renderGraphics;
    bool bMultiFile = false;
	void LoadUnitAssets();

    std::map<std::string, sol::table> m_UnitData;
};
*/