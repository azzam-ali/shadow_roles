#include "Detective.h"
#include "TaskPool.h"
#include "Utils.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <future>
#include <limits>

using namespace std;

Detective::Detective(const string& n, int id)
    : Player(n, id, "Detective"), inspectUsed(false) {
}

void Detective::showRoleCard() const {
    clearScreen();
    cout << "\n";
    cout << "  +======================================+\n";
    cout << "  |         YOU ARE THE DETECTIVE        |\n";
    cout << "  +======================================+\n";
    cout << "  |  Once per round: guess who the       |\n";
    cout << "  |  Imposter is by their ID.            |\n";
    cout << "  |  Their REAL role will be revealed!   |\n";
    cout << "  |                                      |\n";
    cout << "  |  Also complete crew tasks.           |\n";
    cout << "  |  You have 20 seconds per turn.       |\n";
    cout << "  +======================================+\n\n";
    cout << "  Player: " << name << "  (ID #" << playerID << ")\n\n";
    pressEnterToContinue();
}

// Detective reads Player's private 'role' via friend access
void Detective::inspectPlayer(const vector<Player*>& allPlayers) {
    if (inspectUsed) {
        cout << "  You have already inspected someone this round!\n";
        return;
    }

    cout << "\n  -- Alive Players --\n";
    for (Player* p : allPlayers)
        if (p->isAlive() && p->getID() != playerID)
            cout << "    ID " << p->getID() << " - " << p->getName() << "\n";

    cout << "\n  Enter player ID to inspect (one chance per round): ";
    int id;
    if (!(cin >> id)) { cin.clear(); cin.ignore(10000, '\n'); return; }
    cin.ignore((numeric_limits<streamsize>::max)(), '\n');

    for (Player* p : allPlayers) {
        if (p->getID() == id && p->isAlive() && p->getID() != playerID) {
            // FRIEND ACCESS — reading private member 'role'
            cout << "\n  *** CLASSIFIED REPORT ***\n";
            cout << "  Player: " << p->name << "\n";
            cout << "  Role:   [ " << p->role << " ]\n";
            cout << "  *************************\n";
            inspectUsed = true;
            return;
        }
    }
    cout << "  Player not found or already eliminated.\n";
}

void Detective::performAction(vector<Player*>& allPlayers) {
    clearScreen();
    inspectUsed = false;

    auto start = chrono::steady_clock::now();

    auto elapsed = [&]() {
        return (int)chrono::duration_cast<chrono::seconds>(
            chrono::steady_clock::now() - start).count();
        };

    while (true) {
        int left = TURN_SECONDS - elapsed();
        if (left <= 0) {
            cout << "\n  Time is up!\n";
            this_thread::sleep_for(chrono::milliseconds(1500));
            break;
        }

        clearScreen();
        cout << "\n  Time remaining: " << left << "s  |  Detective: " << name << "\n";
        gTaskPool.display();
        cout << "\n  Commands:\n";
        cout << "    task     — attempt the next crew task\n";
        if (!inspectUsed)
            cout << "    inspect  — reveal one player's role (once per round)\n";
        else
            cout << "    [inspect already used this round]\n";
        cout << "    skip     — end your turn\n";
        cout << "  > ";
        cout.flush();

        string cmd;
        auto fut = async(launch::async, [&]() -> string {
            string s;
            cin >> s;
            return s;
            });

        left = TURN_SECONDS - elapsed();
        if (left <= 0 || fut.wait_for(chrono::seconds(max(left, 0))) == future_status::timeout) {
            cout << "\n  Time is up!\n";
            this_thread::sleep_for(chrono::milliseconds(1500));
            break;
        }
        cmd = fut.get();

        if (cmd == "skip") {
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            break;
        }
        else if (cmd == "task") {
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            left = TURN_SECONDS - elapsed();
            if (left <= 0) break;
            if (gTaskPool.allDone()) {
                cout << "  All tasks done!\n";
            }
            else {
                gTaskPool.attemptNext(left);
            }
            this_thread::sleep_for(chrono::milliseconds(1200));
        }
        else if (cmd == "inspect") {
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            inspectPlayer(allPlayers);
            this_thread::sleep_for(chrono::milliseconds(2000));
        }
        else {
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            cout << "  Unknown command.\n";
        }
    }

    clearScreen();
}

bool Detective::checkWin(const vector<Player*>& allPlayers) const {
    for (Player* p : allPlayers)
        if (p->isAlive() && p->getRole() == "Imposter") return false;
    return true;
}