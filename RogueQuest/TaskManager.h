#pragma once
#include "olcPixelGameEngine.h"
#include <map>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <list>
#include <any>

/*
    Task Manager
    This is a generic task delegation system
    
    Register a task which can be delegated
*/

class TaskManager {
public:
	TaskManager();
	virtual ~TaskManager();

	struct RegisteredTask; // fwd


    struct Task {
        const RegisteredTask* regtask;
        std::any data;
    };
    typedef std::function<bool(std::shared_ptr<Task> _this)> TaskCallback;

    struct RegisteredTask {
        TaskCallback callback;
        std::any metadata;
    };


    std::map<std::string, RegisteredTask> taskTypes;

    void RegisterTask(const std::string& name, RegisteredTask regtask);
    std::shared_ptr<Task> DelegateTask(const std::string& name, std::any data);

};