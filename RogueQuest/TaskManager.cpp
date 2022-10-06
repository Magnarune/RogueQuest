#include "TaskManager.h"

TaskManager::TaskManager() {

}
TaskManager::~TaskManager() {

}

void TaskManager::RegisterTask(const std::string& name, RegisteredTask task) {
    if(!taskTypes.count(name))
        taskTypes.insert_or_assign(name, task);
}

std::shared_ptr<TaskManager::Task> TaskManager::PrepareTask(const std::string& name, const std::any& data) {
    if(!taskTypes.count(name)) return nullptr;
    const auto& regtask = taskTypes.at(name);
    
    std::shared_ptr<Task> task;
    task.reset(new Task{ &regtask, {},{},{}, data});//added third option
    task->taskName = name;
    
    std::weak_ptr<Task> _wtask(task); // copy a weak reference to prevent memleak caused by having a closure hold a shared ref to itself
    task->initTask = [=]() -> bool { return _wtask.expired() ? false : regtask.initTask(_wtask.lock()); };

    task->performTask = [=]() -> bool { return _wtask.expired() ? false : regtask.preformTask(_wtask.lock()); };//added

    task->checkCompleted = [=]() -> bool { return _wtask.expired() ? true : regtask.checkCompleted(_wtask.lock()); };
    
    return task; // return prepared task
}