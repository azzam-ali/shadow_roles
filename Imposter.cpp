#include "Imposter.h"
#include "Utils.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <future>
#include <limits>

using namespace std;

Imposter::Imposter(const string& n, int id)
    : Player(n, id, "Imposter"), killUsed(false) {
}

void Imposter::showRoleCard() const {
    clearScreen();
    cout << "\n";
    cout << "  +======================================+\n";
    cout << "  |          YOU ARE THE IMPOSTER        |\n";
    cout << "  +======================================+\n";
    cout << "  |  Blend in. Deceive. Eliminate.       |\n";
    cout << "  |                                      |\n";
    cout << "  |  Use: kill <id>                      |\n";
    cout << "  |  ONE kill allowed per round only!    |\n";
    cout << "  |                                      |\n";
    cout << "  |  You have 25 seconds per turn.       |\n";
    cout << "  +======================================+\n\n";
    cout << "  Player: " << name << "  (ID #" << playerID << ")\n\n";
    pressEnterToContinue();
}

void Imposter::performAction(vector<Player*>& allPlayers) {
    clearScreen();
    // Reset kill each round (called once per round in runTurnPhase)
    killUsed = false;

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
        cout << "\n  Time remaining: " << left << "s  |  Imposter: " << name << "\n";
        cout << "\n  -- Alive Crew Members --\n";
        for (Player* p : allPlayers)
            if (p->isAlive() && p->getID() != playerID)
                cout << "    ID " << p->getID() << " - " << p->getName() << "\n";

        if (killUsed)
            cout << "\n  [Kill already used this round]\n";

        cout << "\n  Commands:  kill <id>   skip\n  > ";
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
        else if (cmd == "kill") {
            int id;
            cin >> id;
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');

            if (killUsed) {
                cout << "  You can only kill ONE player per round!\n";
                this_thread::sleep_for(chrono::milliseconds(1500));
                continue;
            }

            bool found = false;
            for (Player* p : allPlayers) {
                if (p->getID() == id && p->isAlive() && p->getID() != playerID) {
                    p->eliminate();
                    killUsed = true;
                    found = true;
                    cout << "  " << p->getName() << " has been silently eliminated.\n";
                    cout << "  This will be revealed at the community vote.\n";
                    this_thread::sleep_for(chrono::milliseconds(2000));
                    break;
                }
            }
            if (!found) {
                cout << "  Invalid target.\n";
                this_thread::sleep_for(chrono::milliseconds(1000));
            }
        }
        else {
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            cout << "  Unknown command.\n";
        }
    }

    clearScreen();
}

bool Imposter::checkWin(const vector<Player*>& allPlayers) const {
    int imposters = 0, others = 0;
    for (Player* p : allPlayers) {
        if (!p->isAlive()) continue;
        if (p->getRole() == "Imposter") imposters++;
        else others++;
    }
    return imposters >= others;
}