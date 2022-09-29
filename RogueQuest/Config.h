#pragma once

#include "sol/sol.hpp"
#include <map>
#include <string>
#include <filesystem>
#include <fstream>
#include <exception>
#include <optional>
#include <type_traits>

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
        T r {};
        sol::object& o = data.at(name);
        do {
            if(!valid || !data.count(name)) break;

            if(std::is_same<T, bool>::value){
                if(o.get_type() != sol::type::boolean) break;
            } else if(std::is_arithmetic<T>::value){
                if(o.get_type() != sol::type::number) break;
            } else if(std::is_same<T, std::string>::value){
                if(o.get_type() != sol::type::string) break;
            } else {
                break; // not a bool / number / or string so don't continue
            }

            r = o.as<T>();
        } while(0);
        return r;
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

    static int __internal_lua_exception_handler(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description);
};