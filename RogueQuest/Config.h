#pragma once

#include "sol/sol.hpp"
#include <map>
#include <string>
#include <filesystem>
#include <fstream>

class Config {
    sol::state lua;
    sol::table root;

    std::map<std::string, sol::object> data;
    std::string path;

    bool valid = false;
    bool autosave;

public:
    Config(const std::string& path, bool autosave=false);
    virtual ~Config();

    template<typename T>
    T GetValue(const std::string& name) {
        return valid && data.count(name) ?
            data.at(name).as<T>() : T {};
    }

    template<typename T>
    bool SaveValue(const std::string& name, const T& value) {
        if(!valid) return false;
        root["name"] = value;
        data.insert_or_assign(name, root["name"]);
        return true;
    }

    bool Import();
    bool Export();

};