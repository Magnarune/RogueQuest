#include "TaskManager.h"

TaskManager::TaskManager() {

}
TaskManager::~TaskManager() {

}

void TaskManager::RegisterTask(const std::string& name, RegisteredTask task) {
    if(!taskTypes.count(name))
        taskTypes.insert_or_assign(name, task);
}

std::shared_ptr<TaskManager::Task> TaskManager::DelegateTask(const std::string& name, std::any data) {
    if(!taskTypes.count(name)) return nullptr;
    const auto& regtask = taskTypes.at(name);
    
    std::shared_ptr<Task> task;
    task.reset(new Task {&regtask, std::move(data)} );

    if(!regtask.callback(task)) return nullptr; // task delegation receives termination request

    return task; // return delegated task
}