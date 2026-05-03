#include "Detective.h"
#include "TaskPool.h"
#include "Utils.h"
#include <iostream>
#include <limits>
using namespace std;

Detective::Detective(const string& n, int id)
    : Player(n, id, "Detective"), inspectedThisRound(false)
{
    // nothing else to set up
}

void Detective::showRoleCard() const
{
    clearScreen();
    cout << "\n";
    cout << "  +======================================+\n";
    cout << "  |         YOU ARE THE DETECTIVE        |\n";
    cout << "  +======================================+\n";
    cout << "  |  Commands available each turn:       |\n";
    cout << "  |    task            - do a crew task  |\n";
    cout << "  |    inspect <name>  - reveal a role   |\n";
    cout << "  |    skip            - end your turn   |\n";
    cout << "  |                                      |\n";
    cout << "  |  One inspect per round only!         |\n";
    cout << "  +======================================+\n\n";
    cout << "  Player : " << name << "  (ID #" << playerID << ")\n\n";
    pressEnterToContinue();
}

void Detective::inspectPlayer(const string& targetName, vector<Player*>& allPlayers)
{
    if (inspectedThisRound)
    {
        cout << "  You already used inspect this round.\n";
        return;
    }

    bool found = false;
    for (int i = 0; i < (int)allPlayers.size(); i++)
    {
        if (allPlayers[i]->getName() == targetName
            && allPlayers[i]->isAlive()
            && allPlayers[i]->getID() != playerID)
        {
            // FRIEND ACCESS: Detective can read the private 'role' member
            // directly because it is declared as friend in Player.h
            cout << "\n  *** CLASSIFIED REPORT ***\n";
            cout << "  Player : " << allPlayers[i]->name << "\n";
            cout << "  Role   : [ " << allPlayers[i]->role << " ]\n";
            cout << "  *************************\n";

            inspectedThisRound = true;
            found = true;
            break;
        }
    }

    if (!found)
    {
        cout << "  Player \"" << targetName << "\" not found or already eliminated.\n";
    }
}

void Detective::performAction(vector<Player*>& allPlayers)
{
    clearScreen();
    inspectedThisRound = false;

    cout << "\n  -- Your Turn: DETECTIVE (" << name << ") --\n";
    gTaskPool.showTasks();

    cout << "\n  Alive players:\n";
    for (int i = 0; i < (int)allPlayers.size(); i++)
    {
        if (allPlayers[i]->isAlive() && allPlayers[i]->getID() != playerID)
            cout << "    - " << allPlayers[i]->getName() << "\n";
    }

    cout << "\n  Commands:  task  |  inspect <name>  |  skip\n";
    cout << "  > ";

    string cmd;
    cin >> cmd;

    if (cmd == "task")
    {
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
        if (gTaskPool.allDone())
            cout << "\n  All tasks are already done!\n";
        else
            gTaskPool.doNextTask();
    }
    else if (cmd == "inspect")
    {
        string targetName;
        cin >> targetName;
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
        inspectPlayer(targetName, allPlayers);
    }
    else if (cmd == "skip")
    {
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
        cout << "\n  You skipped your turn.\n";
    }
    else
    {
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
        cout << "\n  Unknown command. Turn skipped.\n";
        cout << "  Valid commands: task | inspect <name> | skip\n";
    }

    cout << "\n  Your turn is over.\n";
    pressEnterToContinue();
    clearScreen();
}

bool Detective::checkWin(const vector<Player*>& allPlayers) const
{
    for (int i = 0; i < (int)allPlayers.size(); i++)
    {
        if (allPlayers[i]->isAlive() && allPlayers[i]->getRole() == "Imposter")
            return false;
    }
    return true;
}