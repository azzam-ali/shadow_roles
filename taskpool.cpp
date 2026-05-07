#include "TaskPool.h"
#include <iostream>
#include <cstdlib>
#include <limits>
using namespace std;

// Define the global shared task pool
TaskPool gTaskPool;

TaskPool::TaskPool()
{
    total = 0;
}

void TaskPool::init(int playerCount)
{
    tasks.clear();

    // Decide how many tasks based on player count
    if (playerCount <= 7)
        total = 15;
    else if (playerCount <= 10)
        total = 25;
    else
        total = 30;

    // Generate random math questions
    for (int i = 0; i < total; i++)
    {
        Task t;
        t.done = false;

        int op = rand() % 4;   // 0=add, 1=sub, 2=mul, 3=div
        int a, b;

        if (op == 0)
        {
            a = rand() % 50 + 1;
            b = rand() % 50 + 1;
            t.question = to_string(a) + " + " + to_string(b) + " = ?";
            t.answer = a + b;
        }
        else if (op == 1)
        {
            a = rand() % 50 + 20;
            b = rand() % 20 + 1;
            t.question = to_string(a) + " - " + to_string(b) + " = ?";
            t.answer = a - b;
        }
        else if (op == 2)
        {
            a = rand() % 12 + 1;
            b = rand() % 12 + 1;
            t.question = to_string(a) + " x " + to_string(b) + " = ?";
            t.answer = a * b;
        }
        else
        {
            // division: pick answer and divisor, then compute dividend
            b = rand() % 10 + 2;
            int ans = rand() % 10 + 1;
            a = b * ans;
            t.question = to_string(a) + " / " + to_string(b) + " = ?";
            t.answer = ans;
        }

        tasks.push_back(t);
    }
}

void TaskPool::showTasks() const
{
    cout << "\n  -- Crew Tasks (" << doneCount() << " / " << total << " done) --\n";

    int shown = 0;
    for (int i = 0; i < (int)tasks.size(); i++)
    {
        if (!tasks[i].done)
        {
            cout << "  [" << (i + 1) << "] " << tasks[i].question << "\n";
            shown++;
            if (shown == 5) break;   // only show first 5 pending tasks
        }
    }

    if (shown == 0)
        cout << "  All tasks are done!\n";

    cout << "  -----------------------------------------------\n";
}

bool TaskPool::doNextTask()
{
    // Find the first task that is not done yet
    int idx = -1;
    for (int i = 0; i < (int)tasks.size(); i++)
    {
        if (!tasks[i].done)
        {
            idx = i;
            break;
        }
    }

    if (idx == -1)
    {
        cout << "  No tasks left to do!\n";
        return false;// function returns false bec all the tasks are done and the civilias should winn
    }

    cout << "\n  Task: " << tasks[idx].question << "\n";// these questions are sset in init function
    cout << "  Your answer: ";

    int userAnswer;
    cin >> userAnswer;
    cin.ignore((numeric_limits<streamsize>::max)(), '\n');

    if (userAnswer == tasks[idx].answer)//checking if the answer is equal to the answers mentioned above in init
    {
        tasks[idx].done = true;
        cout << "  Correct! Task completed.\n";
        return true;
    }
    else
    {
        cout << "  Wrong answer. Task stays pending.\n";
        return false;
    }
}

bool TaskPool::allDone() const
{
    for (int i = 0; i < (int)tasks.size(); i++)
        if (!tasks[i].done) return false;
    return true;
}

int TaskPool::doneCount() const // at the end of each round this runs and sees which of thee total tasks are done and returns that many tasks
{
    int count = 0;
    for (int i = 0; i < (int)tasks.size(); i++)
        if (tasks[i].done) count++;
    return count;
}

int TaskPool::totalCount() const// returns total no of tasks
{
    return total;
}