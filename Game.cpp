#include "Game.h"
#include "TaskPool.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <limits>

using namespace std;

Game::Game() : roundNumber(0), gameOver(false), winnerTeam("") {}

Game::~Game() {
    for (int i = 0; i < (int)players.size(); i++) delete players[i];
}

void Game::buildFromNames(const vector<string>& names) {
    int n = (int)names.size();
    vector<string> roles;
    roles.push_back("Imposter");
    if (n >= 4) roles.push_back("Detective");
    while ((int)roles.size() < n) roles.push_back("Civilian");

    for (int i = (int)roles.size() - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(roles[i], roles[j]);
    }
    for (int i = 0; i < n; i++) {
        if (roles[i] == "Imposter")  players.push_back(new Imposter(names[i], i + 1));
        else if (roles[i] == "Detective") players.push_back(new Detective(names[i], i + 1));
        else                              players.push_back(new Civilian(names[i], i + 1));
    }
}

void Game::initWithNames(const vector<string>& names) {
    buildFromNames(names);
    // Initialize the shared task pool based on total player count
    gTaskPool.init((int)names.size());
    logEvent("=== NEW GAME === Players: " + to_string((int)names.size()) +
        " | Tasks: " + to_string(gTaskPool.totalCount()));
    saveState();
}

void Game::loadSave() {
    ifstream in("shadow_roles_save.txt");
    if (!in) { cout << "  Save file not found.\n"; return; }
    string line;
    int playerCount = 0;
    while (getline(in, line)) {
        if (line.substr(0, 6) == "ROUND=")
            roundNumber = stoi(line.substr(6));
        if (line.substr(0, 7) == "PLAYER|") {
            istringstream ss(line.substr(7));
            string tok;
            vector<string> parts;
            while (getline(ss, tok, '|')) parts.push_back(tok);
            if ((int)parts.size() >= 4) {
                int  id = stoi(parts[0]);
                string nm = parts[1];
                string role = parts[2];
                bool alive = (parts[3] == "1");
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
    in.close();
    gTaskPool.init(playerCount);
    cout << "\n  Game loaded. Resuming from round " << roundNumber << ".\n";
    cout << "  Note: Task progress resets on resume.\n";
    cout << "  Press ENTER...";
    cin.ignore((numeric_limits<streamsize>::max)(), '\n');
    cin.get();
}

void Game::run() {
    if (players.empty()) return;
    if (roundNumber == 0) showRoleHandoffs();

    while (!gameOver) {
        roundNumber++;
        clearScreen();
        cout << "\n  ========================================\n";
        cout << "            R O U N D   " << roundNumber << "\n";
        cout << "  ========================================\n\n";
        listAlivePlayers();
        cout << "\n  Tasks: " << gTaskPool.doneCount() << "/" << gTaskPool.totalCount() << " completed\n";
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
    remove("shadow_roles_save.txt");
}

void Game::showRoleHandoffs() {
    clearScreen();
    cout << "\n  Each player will now see their secret role.\n";
    cout << "  Do NOT show your screen to others!\n\n";
    cout << "  Press ENTER to begin...";
    cin.ignore((numeric_limits<streamsize>::max)(), '\n');
    cin.get();

    for (int i = 0; i < (int)players.size(); i++) {
        handoffScreen(players[i]->getName());
        players[i]->showRoleCard();
        clearScreen();
        cout << "\n  " << players[i]->getName() << " has seen their role. Pass the laptop.\n\n";
        cout << "  Press ENTER...";
        cin.get();
    }
    clearScreen();
    cout << "\n  All players have seen their roles. The game begins!\n";
    cout << "  Total crew tasks to complete: " << gTaskPool.totalCount() << "\n\n";
    cout << "  Press ENTER...";
    cin.get();
}

void Game::runTurnPhase() {
    vector<Player*> order = players;
    for (int i = (int)order.size() - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(order[i], order[j]);
    }
    for (int i = 0; i < (int)order.size(); i++) {
        if (!order[i]->isAlive()) continue;

        handoffScreen(order[i]->getName());
        order[i]->performAction(players);

        if (checkGameOver()) return;

        clearScreen();
        cout << "\n  " << order[i]->getName() << "'s turn is over. Pass the laptop.\n\n";
        cout << "  Tasks: " << gTaskPool.doneCount() << "/" << gTaskPool.totalCount() << " done.\n\n";
        cout << "  Press ENTER...";
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
        cin.get();
    }
}

void Game::runVotingPhase() {
    clearScreen();
    cout << "\n  ========================================\n";
    cout << "          C O M M U N I T Y   V O T E\n";
    cout << "  ========================================\n\n";

    for (int i = 0; i < (int)players.size(); i++) players[i]->resetVotes();

    announceDeadThisRound();

    cout << "\n  Press ENTER to begin voting...";
    cin.ignore((numeric_limits<streamsize>::max)(), '\n');
    cin.get();

    for (int i = 0; i < (int)players.size(); i++) {
        if (!players[i]->isAlive()) continue;
        clearScreen();
        cout << "\n  " << players[i]->getName() << ", cast your vote:\n\n";
        listAlivePlayers();
        cout << "\n  Vote to eliminate (enter player ID, or 0 to skip): ";
        int id; cin >> id;
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
        if (id == 0) {
            cout << "  Vote skipped.\n";
        }
        else {
            for (int j = 0; j < (int)players.size(); j++) {
                if (players[j]->getID() == id &&
                    players[j]->isAlive() &&
                    players[j]->getID() != players[i]->getID()) {
                    players[j]->addVote();
                    cout << "  Voted against " << players[j]->getName() << ".\n";
                    break;
                }
            }
        }
        cout << "  Press ENTER...";
        cin.get();
    }

    clearScreen();
    cout << "\n  -- Vote Tally --\n";
    Player* mostVoted = nullptr;
    int maxVotes = 0;
    bool tie = false;

    for (int i = 0; i < (int)players.size(); i++) {
        if (!players[i]->isAlive()) continue;
        if (players[i]->getVotes() > 0)
            cout << "    " << players[i]->getName() << " - " << players[i]->getVotes() << " vote(s)\n";
        if (players[i]->getVotes() > maxVotes) {
            maxVotes = players[i]->getVotes();
            mostVoted = players[i];
            tie = false;
        }
        else if (players[i]->getVotes() == maxVotes && maxVotes > 0) {
            tie = true;
        }
    }

    cout << "\n";
    if (tie || maxVotes == 0 || !mostVoted) {
        cout << "  Tie vote — nobody eliminated this round!\n";
        logEvent("Round " + to_string(roundNumber) + ": Tie, no elimination.");
    }
    else {
        mostVoted->eliminate();
        announcedDead.push_back(mostVoted->getID());
        cout << "  The crew eliminates: " << mostVoted->getName()
            << "  [ " << mostVoted->getRole() << " ]\n";
        logEvent("Round " + to_string(roundNumber) + ": " +
            mostVoted->getName() + " (" + mostVoted->getRole() + ") voted off.");
    }
    cout << "\n  Press ENTER...";
    cin.get();
}

bool Game::checkGameOver() {
    int imposters = 0, others = 0;
    for (int i = 0; i < (int)players.size(); i++) {
        if (!players[i]->isAlive()) continue;
        if (players[i]->getRole() == "Imposter") imposters++;
        else others++;
    }
    // Imposter wins by outnumbering crew
    if (imposters >= others && others > 0) {
        gameOver = true; winnerTeam = "Imposter";
        logEvent("GAME OVER: Imposter wins!"); return true;
    }
    // All imposters eliminated
    if (imposters == 0) {
        gameOver = true; winnerTeam = "Civilians & Detective";
        logEvent("GAME OVER: Civilians win!"); return true;
    }
    // Task completion win
    if (gTaskPool.allDone()) {
        gameOver = true; winnerTeam = "Civilians & Detective (all tasks done!)";
        logEvent("GAME OVER: All tasks completed — Civilians win!"); return true;
    }
    return false;
}

void Game::announceDeadThisRound() {
    cout << "  -- Crew Members Lost This Round --\n";
    bool any = false;
    for (int i = 0; i < (int)players.size(); i++) {
        if (!players[i]->isAlive()) {
            bool alreadyAnnounced = false;
            for (int j = 0; j < (int)announcedDead.size(); j++)
                if (announcedDead[j] == players[i]->getID()) { alreadyAnnounced = true; break; }
            if (!alreadyAnnounced) {
                cout << "  [DEAD] " << players[i]->getName() << " was found eliminated!\n";
                announcedDead.push_back(players[i]->getID());
                logEvent(players[i]->getName() + " eliminated by Imposter in round " + to_string(roundNumber));
                any = true;
            }
        }
    }
    if (!any) cout << "  Nobody was killed by the Imposter this round.\n";
}

void Game::saveState() const {
    ofstream out("shadow_roles_save.txt");
    if (!out) return;
    out << "ROUND=" << roundNumber << "\n";
    for (int i = 0; i < (int)players.size(); i++)
        out << "PLAYER|" << players[i]->getID() << "|" << players[i]->getName()
        << "|" << players[i]->getRole() << "|" << (players[i]->isAlive() ? 1 : 0) << "\n";
}

void Game::logEvent(const string& msg) const {
    ofstream log("shadow_roles_log.txt", ios::app);
    if (!log) return;
    time_t t = time(nullptr);
    char buf[32];
#ifdef _WIN32
    struct tm tmInfo;
    localtime_s(&tmInfo, &t);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tmInfo);
#else
    struct tm* tmInfo = localtime(&t);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tmInfo);
#endif
    log << "[" << buf << "] " << msg << "\n";
}

void Game::listAlivePlayers() const {
    cout << "  -- Alive Players --\n";
    for (int i = 0; i < (int)players.size(); i++)
        if (players[i]->isAlive())
            cout << "    ID " << players[i]->getID() << " - " << players[i]->getName() << "\n";
}

void Game::printFinalSummary() const {
    clearScreen();
    cout << "\n  ========================================\n";
    cout << "              G A M E   O V E R\n";
    cout << "  ========================================\n\n";
    cout << "  WINNER: " << winnerTeam << "\n\n";
    cout << "  -- Final Roster --\n";
    for (int i = 0; i < (int)players.size(); i++) {
        cout << "    " << players[i]->getName()
            << "  [" << players[i]->getRole() << "]"
            << (players[i]->isAlive() ? "  SURVIVED" : "  ELIMINATED") << "\n";
    }
    cout << "\n  Rounds played: " << roundNumber << "\n";
    cout << "  Tasks completed: " << gTaskPool.doneCount() << "/" << gTaskPool.totalCount() << "\n";
    cout << "  Log saved to: shadow_roles_log.txt\n\n";
}