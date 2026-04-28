#ifndef TASKPOOL_H
#define TASKPOOL_H

#include <string>
#include <vector>

using namespace std;

struct Task {
    string question;
    int    answer;
    bool   done;
};

class TaskPool {
private:
    vector<Task> tasks;
    int totalTasks;

    void generateTasks(int count);

public:
    TaskPool() : totalTasks(0) {}

    // Call once at game start with total player count
    void init(int playerCount);

    // Display all tasks (done / pending)
    void display() const;

    // Attempt the next undone task; returns true if answered correctly
    bool attemptNext(int secondsLeft);

    bool allDone() const;
    int  doneCount()  const;
    int  totalCount() const { return totalTasks; }
};

// Global singleton — shared across all players
extern TaskPool gTaskPool;

#endif