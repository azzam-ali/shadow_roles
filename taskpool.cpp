#include "TaskPool.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <limits>
#include <future>

using namespace std;

// Global shared task pool
TaskPool gTaskPool;

void TaskPool::generateTasks(int count) {
    tasks.clear();
    totalTasks = count;

    // Generate 'count' random math questions with unique ops
    for (int i = 0; i < count; i++) {
        int op = rand() % 4;   // 0=add, 1=sub, 2=mul, 3=div
        int a, b, ans;

        switch (op) {
        case 0: // addition
            a = rand() % 50 + 1;
            b = rand() % 50 + 1;
            ans = a + b;
            tasks.push_back({ to_string(a) + " + " + to_string(b) + " = ?", ans, false });
            break;
        case 1: // subtraction (ensure non-negative)
            a = rand() % 50 + 20;
            b = rand() % 20 + 1;
            ans = a - b;
            tasks.push_back({ to_string(a) + " - " + to_string(b) + " = ?", ans, false });
            break;
        case 2: // multiplication (small numbers)
            a = rand() % 12 + 1;
            b = rand() % 12 + 1;
            ans = a * b;
            tasks.push_back({ to_string(a) + " x " + to_string(b) + " = ?", ans, false });
            break;
        case 3: // division (exact)
            b = rand() % 10 + 2;
            ans = rand() % 10 + 1;
            a = b * ans;
            tasks.push_back({ to_string(a) + " / " + to_string(b) + " = ?", ans, false });
            break;
        }
    }
}

void TaskPool::init(int playerCount) {
    int count;
    if (playerCount <= 7)  count = 15;
    else if (playerCount <= 10) count = 25;
    else                        count = 30;

    generateTasks(count);
}

void TaskPool::display() const {
    cout << "\n  -- Crew Tasks (" << doneCount() << "/" << totalTasks << " done) --\n";
    int shown = 0;
    for (int i = 0; i < (int)tasks.size(); i++) {
        if (!tasks[i].done) {
            cout << "  [" << (i + 1) << "] " << tasks[i].question << "\n";
            shown++;
            if (shown >= 5) { // show at most 5 pending tasks at a time
                int rem = 0;
                for (int j = i + 1; j < (int)tasks.size(); j++) if (!tasks[j].done) rem++;
                if (rem > 0) cout << "  ... and " << rem << " more pending tasks.\n";
                break;
            }
        }
    }
    if (shown == 0) cout << "  All tasks completed!\n";
    cout << "  ------------------------------------------------\n";
}

bool TaskPool::attemptNext(int secondsLeft) {
    // Find the first undone task
    int idx = -1;
    for (int i = 0; i < (int)tasks.size(); i++) {
        if (!tasks[i].done) { idx = i; break; }
    }
    if (idx == -1) {
        cout << "  No tasks left!\n";
        return false;
    }

    cout << "\n  >> TASK [" << (idx + 1) << "]: " << tasks[idx].question << "\n";
    cout << "  You have " << secondsLeft << "s remaining. Enter answer: ";
    cout.flush();

    // Timed input using a future
    string inputStr = "";
    auto fut = async(launch::async, [&]() -> string {
        string s;
        getline(cin, s);
        return s;
        });

    if (fut.wait_for(chrono::seconds(secondsLeft)) == future_status::ready) {
        inputStr = fut.get();
    }
    else {
        // Time ran out while waiting — drain cin on next read
        cout << "\n  Time's up!\n";
        return false;
    }

    int userAns = 0;
    bool valid = true;
    try {
        userAns = stoi(inputStr);
    }
    catch (...) {
        valid = false;
    }

    if (valid && userAns == tasks[idx].answer) {
        tasks[idx].done = true;
        cout << "  Correct! Task completed.\n";
        return true;
    }
    else {
        cout << "  Wrong answer! Task stays pending.\n";
        return false;
    }
}

bool TaskPool::allDone() const {
    for (auto& t : tasks) if (!t.done) return false;
    return true;
}

int TaskPool::doneCount() const {
    int c = 0;
    for (auto& t : tasks) if (t.done) c++;
    return c;
}