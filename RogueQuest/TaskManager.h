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
        std::function<bool()> initTask;
        std::function<bool()> checkCompleted;
        std::any data;
    };  
    typedef std::function<bool(std::shared_ptr<Task> _this)> TaskCallback;

    struct RegisteredTask {
        TaskCallback initTask;
        TaskCallback checkCompleted;
        std::any metadata;
        olc::Key bind;
    };

    
    std::map<std::string, RegisteredTask> taskTypes;

    void RegisterTask(const std::string& name, RegisteredTask regtask);
    std::shared_ptr<Task> PrepareTask(const std::string& name, const std::any& data);

};