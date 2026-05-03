#include "Imposter.h"
#include "Utils.h"
#include <iostream>
#include <limits>
using namespace std;

Imposter::Imposter(const string& n, int id)
    : Player(n, id, "Imposter")
{
    // nothing extra to set up
}

void Imposter::showRoleCard() const
{
    clearScreen();
    cout << "\n";
    cout << "  +======================================+\n";
    cout << "  |          YOU ARE THE IMPOSTER        |\n";
    cout << "  +======================================+\n";
    cout << "  |  Commands available each turn:       |\n";
    cout << "  |    kill <name>  - eliminate a player |\n";
    cout << "  |    skip         - do nothing         |\n";
    cout << "  |                                      |\n";
    cout << "  |  One kill per round only!            |\n";
    cout << "  +======================================+\n\n";
    cout << "  Player : " << name << "  (ID #" << playerID << ")\n\n";
    pressEnterToContinue();
}

void Imposter::performAction(vector<Player*>& allPlayers)
{
    clearScreen();

    cout << "\n  -- Your Turn: IMPOSTER (" << name << ") --\n";
    cout << "  You may eliminate ONE crew member this round.\n\n";

    cout << "  Alive crew members:\n";
    for (int i = 0; i < (int)allPlayers.size(); i++)
    {
        if (allPlayers[i]->isAlive() && allPlayers[i]->getID() != playerID)// checks alive players and then alive players instead of the same player
            cout << "    - " << allPlayers[i]->getName() << "\n";
    }

    cout << "\n  Commands:  kill <name>  |  skip\n";
    cout << "  > ";

    string cmd;
    cin >> cmd;

    if (cmd == "kill")
    {
        string targetName;
        cin >> targetName;
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');

        bool found = false;
        for (int i = 0; i < (int)allPlayers.size(); i++)
        {
            if (allPlayers[i]->getName() == targetName
                && allPlayers[i]->isAlive()
                && allPlayers[i]->getID() != playerID)
            {
                allPlayers[i]->eliminate();
                found = true;
                cout << "\n  Done. " << allPlayers[i]->getName()
                    << " has been quietly eliminated.\n";
                cout << "  The crew will find out during the vote.\n";
                break;
            }
        }

        if (!found)//bool not found meaning that the above if condition wasnt satisfied
            cout << "  Player \"" << targetName << "\" not found or invalid target.\n";
    }
    else if (cmd == "skip")
    {
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
        cout << "\n  You chose to skip. No kill this round.\n";
    }
    else
    {
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
        cout << "\n  Unknown command. Turn skipped.\n";
        cout << "  Valid commands: kill <name> | skip\n";
    }

    cout << "\n  Your turn is over.\n";
    pressEnterToContinue();
    clearScreen();
}

bool Imposter::checkWin(const vector<Player*>& allPlayers) const
{
    int imposters = 0;
    int others = 0;

    for (int i = 0; i < (int)allPlayers.size(); i++)
    {
        if (!allPlayers[i]->isAlive()) continue;

        if (allPlayers[i]->getRole() == "Imposter")
            imposters++;
        else
            others++;
    }

    return imposters >= others;
}