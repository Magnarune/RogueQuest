#include "Config.h"
#include "Engine.h"
namespace fs = std::filesystem;

Config::Config(const std::string& path, bool autosave): path(path), autosave(autosave) {
    lua.set_exception_handler(& __internal_lua_exception_handler);

    if(fs::exists(fs::path(path))){
        Import();
    } else {
        valid = true; // new config file - generate file
        Export();
    }
}

Config::~Config() {
    if(autosave)
        Export();
}

bool Config::Import() {
    data.clear();
    valid = false;

    try {
        sol::load_result script = lua.load_file(path);
        sol::function_result rcode = script();

        if(!rcode.valid())
            throw std::runtime_error("invalid config file");

        root = rcode; // root table Table of config values
        if(root.get_type() != sol::type::table)
            throw std::runtime_error("root is not a table");


        for(const auto& ii : root){
            const sol::object& key = ii.first;
            const sol::object& value = ii.second;

            if(key.get_type() != sol::type::string)
                throw std::runtime_error(std::string("invalid key type config file: ") + StringifyObject(key) + " is not a string");

            std::string name = key.as<std::string>();
            data.insert_or_assign(name, value);
        }

    } catch(std::exception e) {
        std::cerr << e.what() << "\n";
        return false;
    }

    valid = true;
    return true;
}

bool Config::Export() {
    if(!valid) return false;
    try {
        std::ofstream file(path, std::ios::out | std::ios::binary);

        file << "return\n{";

        for(const auto& [key,value] : data){
            file << "\n\t" << key << " = ";
            switch(value.get_type()){
                case sol::type::string:
                    file << "\"" << value.as<std::string>() << "\"";
                    break;
                case sol::type::boolean:
                    file << (value.as<bool>() ? "true" : "false");
                    break;
                case sol::type::number:
                    file << StringifyObject(value);
                    break;
                default:
                    throw std::runtime_error("invalid value type for key " + key + ":" + StringifyObject(value) + "\n");
            }
            file << ",";
        }

        file << "\n}";
    } catch(std::exception e){
        std::cerr << e.what() << "\n";
        return false;
    }

    return true;
}

int Config::__internal_lua_exception_handler(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description) {
    std::cerr << "Config::__lua_exception_handler: An exception occurred\n";
	if (maybe_exception) {
		const std::exception& ex = *maybe_exception;
		std::cerr << ex.what() << "\n";
	}
	else {
		std::cout.write(description.data(), static_cast<std::streamsize>(description.size()));
	}
	return sol::stack::push(L, description);
}