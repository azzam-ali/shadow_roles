#ifndef TASKPOOL_H
#define TASKPOOL_H

#include <string>
#include <vector>
using namespace std;

// One math task
struct Task
{
    string question;
    int    answer;
    bool   done;
};

// Holds all tasks for the whole crew - shared by everyone
class TaskPool
{
private:
    vector<Task> tasks;
    int total;

public:
    TaskPool();

    void init(int playerCount);   // call once at game start
    void showTasks() const;       // print pending tasks
    bool doNextTask();            // attempt the first undone task, return true if correct
    bool allDone() const;
    int  doneCount()  const;
    int  totalCount() const;
};

// Global task pool shared by all players
extern TaskPool gTaskPool;

#endif