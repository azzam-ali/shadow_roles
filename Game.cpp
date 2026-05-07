#include "Game.h"
#include "TaskPool.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <limits>
#include <algorithm>
using namespace std;

Game::Game()
{
    roundNumber = 0;
    gameOver = false;
    winnerTeam = "";
}

Game::~Game()
{
    // free all player objects we created with new
    for (int i = 0; i < (int)players.size(); i++)
        delete players[i];
}

void Game::buildFromNames(const vector<string>& names)
{
    int n = (int)names.size();

    // Build role list: always 1 Imposter, 1 Detective (if 4+ players), rest Civilians
    vector<string> roles;
    roles.push_back("Imposter");
    if (n >= 4)
        roles.push_back("Detective");
    while ((int)roles.size() < n)
        roles.push_back("Civilian");

    // Shuffle roles randomly
    for (int i = (int)roles.size() - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        swap(roles[i], roles[j]);
    }

    // Create player objects based on shuffled roles
    for (int i = 0; i < n; i++)
    {
        if (roles[i] == "Imposter")
            players.push_back(new Imposter(names[i], i + 1));// this is composition bec player object are being created inside anothere classs that is game 
        else if (roles[i] == "Detective")
            players.push_back(new Detective(names[i], i + 1));
        else
            players.push_back(new Civilian(names[i], i + 1));
    }
}

void Game::initWithNames(const vector<string>& names)
{
    buildFromNames(names);

    // Set up the shared task pool based on how many players there are
    gTaskPool.init((int)names.size());

    logEvent("=== NEW GAME === Players: " + to_string((int)names.size())
        + " | Tasks: " + to_string(gTaskPool.totalCount()));
    saveState();
}

void Game::loadSave()
{
    ifstream inFile("shadow_roles_save.txt");
    if (!inFile)
    {
        cout << "  Save file not found.\n";
        return;
    }

    string line;
    int playerCount = 0;

    while (getline(inFile, line))
    {
        if (line.substr(0, 6) == "ROUND=")
        {
            roundNumber = stoi(line.substr(6));
        }
        else if (line.substr(0, 7) == "PLAYER|")
        {
            // Format: PLAYER|id|name|role|alive
            istringstream ss(line.substr(7));
            string token;
            vector<string> parts;
            while (getline(ss, token, '|'))
                parts.push_back(token);

            if ((int)parts.size() >= 4)
            {
                int    id = stoi(parts[0]);
                string nm = parts[1];
                string role = parts[2];
                bool   alive = (parts[3] == "1");

                Player* p = nullptr;
                if (role == "Imposter")  p = new Imposter(nm, id);
                else if (role == "Detective") p = new Detective(nm, id);
                else                          p = new Civilian(nm, id);

                if (!alive) p->eliminate();
                players.push_back(p);
                playerCount++;
            }
        }
    }
    inFile.close();

    // Re-initialize the task pool (task progress is not saved)
    gTaskPool.init(playerCount);

    cout << "\n  Game loaded! Resuming from round " << roundNumber << ".\n";
    cout << "  Note: task progress resets on load.\n";
    pressEnterToContinue();
}

void Game::run()
{
    if (players.empty()) return;

    // Show role cards to each player before the first round
    if (roundNumber == 0)
        showRoleHandoffs();

    while (!gameOver)
    {
        roundNumber++;
        clearScreen();

        cout << "\n  ========================================\n";
        cout << "            R O U N D   " << roundNumber << "\n";
        cout << "  ========================================\n\n";

        listAlivePlayers();
        cout << "\n  Tasks done: " << gTaskPool.doneCount()
            << " / " << gTaskPool.totalCount() << "\n";

        cout << "\n  Press ENTER to start turns...";
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
        cin.get();

        runTurnPhase();
        if (checkGameOver()) break;

        runVotingPhase();
        if (checkGameOver()) break;

        saveState();
        logEvent("Round " + to_string(roundNumber) + " complete.");
    }

    printFinalSummary();
    remove("shadow_roles_save.txt");   // delete save once game ends
}

void Game::showRoleHandoffs()
{
    clearScreen();
    cout << "\n  Each player will now see their secret role card.\n";
    cout << "  Do NOT show your screen to other players!\n\n";
    cout << "  Press ENTER to begin...";
    cin.ignore((numeric_limits<streamsize>::max)(), '\n');
    cin.get();

    for (int i = 0; i < (int)players.size(); i++)
    {
        handoffScreen(players[i]->getName());
        players[i]->showRoleCard();   // polymorphism: calls the right showRoleCard()

        clearScreen();
        cout << "\n  " << players[i]->getName() << " has seen their role.\n";
        cout << "  Pass the laptop to the next player.\n\n";
        cout << "  Press ENTER...";
        cin.get();
    }

    clearScreen();
    cout << "\n  All players have seen their roles!\n";
    cout << "  Total crew tasks to complete: " << gTaskPool.totalCount() << "\n\n";
    cout << "  Press ENTER to start the game...";
    cin.get();
}

void Game::runTurnPhase()
{
    // Shuffle turn order so it's random every round
    vector<Player*> order = players;
    for (int i = (int)order.size() - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        swap(order[i], order[j]);
    }

    for (int i = 0; i < (int)order.size(); i++)
    {
        if (!order[i]->isAlive()) continue;

        handoffScreen(order[i]->getName());
        order[i]->performAction(players);   // polymorphism: Civilian / Detective / Imposter all do different things

        // Check if the game ended mid-turn (e.g. imposter killed someone)
        if (checkGameOver()) return;

        clearScreen();
        cout << "\n  " << order[i]->getName() << "'s turn is over.\n";
        cout << "  Tasks done: " << gTaskPool.doneCount()
            << " / " << gTaskPool.totalCount() << "\n\n";
        cout << "  Pass the laptop to the next player.\n";
        cout << "\n  Press ENTER...";
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
        cin.get();
    }
}

void Game::runVotingPhase()
{
    clearScreen();
    cout << "\n  ========================================\n";
    cout << "        C O M M U N I T Y   V O T E\n";
    cout << "  ========================================\n\n";

    // Reset all votes before voting starts
    for (int i = 0; i < (int)players.size(); i++)
        players[i]->resetVotes();

    // Show who the Imposter killed this round
    announceDeadThisRound();

    cout << "\n  Press ENTER to begin voting...";
    cin.ignore((numeric_limits<streamsize>::max)(), '\n');
    cin.get();

    // Each alive player casts one vote using CLI command: vote <name> or skip
    for (int i = 0; i < (int)players.size(); i++)
    {
        if (!players[i]->isAlive()) continue;

        clearScreen();
        cout << "\n  " << players[i]->getName() << ", cast your vote.\n\n";

        cout << "  Alive players:\n";
        for (int j = 0; j < (int)players.size(); j++)
        {
            if (players[j]->isAlive() && players[j]->getID() != players[i]->getID())
                cout << "    - " << players[j]->getName() << "\n";
        }

        cout << "\n  Commands:  vote <name>  |  skip\n";
        cout << "  > ";

        string cmd;
        cin >> cmd;

        if (cmd == "vote")
        {
            string targetName;
            cin >> targetName;
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');

            bool found = false;
            for (int j = 0; j < (int)players.size(); j++)
            {
                if (players[j]->getName() == targetName
                    && players[j]->isAlive()
                    && players[j]->getID() != players[i]->getID())
                {
                    players[j]->addVote();
                    cout << "  Voted against " << players[j]->getName() << ".\n";
                    found = true;
                    break;
                }
            }
            if (!found)
                cout << "  Player not found. Vote skipped.\n";
        }
        else if (cmd == "skip")
        {
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            cout << "  Vote skipped.\n";
        }
        else
        {
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            cout << "  Unknown command. Vote skipped.\n";
        }

        cout << "  Press ENTER...";
        cin.get();
    }

    // Count votes and eliminate the most voted player
    clearScreen();
    cout << "\n  -- Vote Results --\n";

    Player* mostVoted = nullptr;
    int     maxVotes = 0;
    bool    tie = false;

    for (int i = 0; i < (int)players.size(); i++)
    {
        if (!players[i]->isAlive()) continue;
        if (players[i]->getVotes() == 0) continue;

        cout << "  " << players[i]->getName()
            << " got " << players[i]->getVotes() << " vote(s)\n";

        if (players[i]->getVotes() > maxVotes)
        {
            maxVotes = players[i]->getVotes();
            mostVoted = players[i];
            tie = false;
        }
        else if (players[i]->getVotes() == maxVotes)
        {
            tie = true;
        }
    }

    cout << "\n";
    if (tie || maxVotes == 0 || mostVoted == nullptr)
    {
        cout << "  It's a tie! Nobody is eliminated this round.\n";
        logEvent("Round " + to_string(roundNumber) + ": Tie vote, no elimination.");
    }
    else
    {
        mostVoted->eliminate();
        alreadyAnnouncedDead.push_back(mostVoted->getID());

        cout << "  The crew votes to eliminate: " << mostVoted->getName()
            << "  [ " << mostVoted->getRole() << " ]\n";

        logEvent("Round " + to_string(roundNumber) + ": "
            + mostVoted->getName() + " (" + mostVoted->getRole() + ") voted off.");
    }

    cout << "\n  Press ENTER...";
    cin.get();
}

bool Game::checkGameOver()
{
    int imposters = 0;
    int others = 0;

    for (int i = 0; i < (int)players.size(); i++)
    {
        if (!players[i]->isAlive()) continue;
        if (players[i]->getRole() == "Imposter") imposters++;
        else                                      others++;
    }

    // Imposter wins by outnumbering the crew
    if (imposters >= others && others > 0)
    {
        gameOver = true;
        winnerTeam = "Imposter";
        logEvent("GAME OVER: Imposter wins!");
        return true;
    }

    // Crew wins by eliminating all imposters
    if (imposters == 0)
    {
        gameOver = true;
        winnerTeam = "Civilians & Detective";
        logEvent("GAME OVER: Crew wins by vote!");
        return true;
    }

    // Crew wins by completing all tasks
    if (gTaskPool.allDone())
    {
        gameOver = true;
        winnerTeam = "Civilians & Detective (completed all tasks!)";
        logEvent("GAME OVER: All tasks done - Crew wins!");
        return true;
    }

    return false;
}

void Game::announceDeadThisRound()
{
    cout << "  -- Eliminated This Round --\n";
    bool anyFound = false;

    for (int i = 0; i < (int)players.size(); i++)
    {
        if (!players[i]->isAlive())
        {
            // Check if we already told the crew about this player before
            bool alreadyTold = false;
            for (int j = 0; j < (int)alreadyAnnouncedDead.size(); j++)
            {
                if (alreadyAnnouncedDead[j] == players[i]->getID())
                {
                    alreadyTold = true;
                    break;
                }
            }

            if (!alreadyTold)
            {
                cout << "  [DEAD] " << players[i]->getName()
                    << " was found eliminated!\n";
                alreadyAnnouncedDead.push_back(players[i]->getID());
                logEvent(players[i]->getName()
                    + " killed by Imposter in round " + to_string(roundNumber));
                anyFound = true;
            }
        }
    }

    if (!anyFound)
        cout << "  Nobody was killed by the Imposter this round.\n";
}

void Game::saveState() const
{
    ofstream outFile("shadow_roles_save.txt");
    if (!outFile) return;

    outFile << "ROUND=" << roundNumber << "\n";
    for (int i = 0; i < (int)players.size(); i++)
    {
        outFile << "PLAYER|"
            << players[i]->getID() << "|"
            << players[i]->getName() << "|"
            << players[i]->getRole() << "|"
            << (players[i]->isAlive() ? 1 : 0) << "\n";
    }
}

void Game::logEvent(const string& msg) const
{
    ofstream logFile("shadow_roles_log.txt", ios::app);
    if (!logFile) return;

    // Get current time as a timestamp
    time_t now = time(nullptr);
    char   buf[32];

#ifdef _WIN32
    struct tm tmInfo;
    localtime_s(&tmInfo, &now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tmInfo);
#else
    struct tm* tmInfo = localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tmInfo);
#endif

    logFile << "[" << buf << "] " << msg << "\n";
}

void Game::listAlivePlayers() const
{
    cout << "  -- Alive Players --\n";
    for (int i = 0; i < (int)players.size(); i++)
    {
        if (players[i]->isAlive())
            cout << "    ID " << players[i]->getID()
            << "  -  " << players[i]->getName() << "\n";
    }
}

void Game::printFinalSummary() const
{
    clearScreen();
    cout << "\n  ========================================\n";
    cout << "              G A M E   O V E R\n";
    cout << "  ========================================\n\n";
    cout << "  WINNER: " << winnerTeam << "\n\n";
    cout << "  -- Final Player Roster --\n";

    for (int i = 0; i < (int)players.size(); i++)
    {
        cout << "  " << players[i]->getName()
            << "  [ " << players[i]->getRole() << " ]"
            << (players[i]->isAlive() ? "  -- SURVIVED" : "  -- ELIMINATED")
            << "\n";
    }

    cout << "\n  Rounds played : " << roundNumber << "\n";
    cout << "  Tasks done    : " << gTaskPool.doneCount()
        << " / " << gTaskPool.totalCount() << "\n";
    cout << "\n  Full event log saved to: shadow_roles_log.txt\n\n";
}