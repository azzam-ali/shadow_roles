#include "Civilian.h"
#include "TaskPool.h"
#include "Utils.h"
#include <iostream>
#include <limits>
using namespace std;

Civilian::Civilian(const string& n, int id)
    : Player(n, id, "Civilian")
{
    // no extra setup needed
}

void Civilian::showRoleCard() const
{
    clearScreen();
    cout << "\n";
    cout << "  +======================================+\n";
    cout << "  |         YOU ARE A  CIVILIAN          |\n";
    cout << "  +======================================+\n";
    cout << "  |  Commands available each turn:       |\n";
    cout << "  |    task  - attempt the next task     |\n";
    cout << "  |    skip  - end your turn             |\n";
    cout << "  |                                      |\n";
    cout << "  |  Tasks are math questions.           |\n";
    cout << "  |  All crew share the same task list!  |\n";
    cout << "  +======================================+\n\n";
    cout << "  Player : " << name << "  (ID #" << playerID << ")\n\n";
    pressEnterToContinue();
}

void Civilian::performAction(vector<Player*>& allPlayers)
{
    (void)allPlayers;//to silence the warning that the all players is passed but not used

    clearScreen();
    cout << "\n  -- Your Turn: CIVILIAN (" << name << ") --\n";

    gTaskPool.showTasks();

    if (gTaskPool.allDone())
    {
        cout << "\n  All tasks are already done. Nothing to do this turn!\n";
        pressEnterToContinue();
        clearScreen();
        return;
    }

    cout << "\n  Commands:  task  |  skip\n";
    cout << "  > ";

    string cmd;
    cin >> cmd;
    cin.ignore((numeric_limits<streamsize>::max)(), '\n');

    if (cmd == "task")
    {
        gTaskPool.doNextTask();
    }
    else if (cmd == "skip")
    {
        cout << "\n  You skipped your turn.\n";
    }
    else
    {
        cout << "\n  Unknown command. Turn skipped.\n";
        cout << "  Valid commands: task | skip\n";
    }

    cout << "\n  Your turn is over.\n";
    pressEnterToContinue();
    clearScreen();
}

bool Civilian::checkWin(const vector<Player*>& allPlayers) const
{
    for (int i = 0; i < (int)allPlayers.size(); i++)
    {
        if (allPlayers[i]->isAlive() && allPlayers[i]->getRole() == "Imposter")
            return false;
    }
    return true;
}