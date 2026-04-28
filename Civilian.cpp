#include "Civilian.h"
#include "TaskPool.h"
#include "Utils.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <future>
#include <atomic>
#include <limits>

using namespace std;

Civilian::Civilian(const string& n, int id)
    : Player(n, id, "Civilian") {
}

void Civilian::showRoleCard() const {
    clearScreen();
    cout << "\n";
    cout << "  +======================================+\n";
    cout << "  |         YOU ARE A  CIVILIAN          |\n";
    cout << "  +======================================+\n";
    cout << "  |  Complete crew tasks each turn.      |\n";
    cout << "  |  Vote out the Imposter each round.   |\n";
    cout << "  |                                      |\n";
    cout << "  |  You have 10 seconds per turn.       |\n";
    cout << "  |  Solve the math task shown to you.   |\n";
    cout << "  |  Tasks are shared — help the crew!   |\n";
    cout << "  +======================================+\n\n";
    cout << "  Player: " << name << "  (ID #" << playerID << ")\n\n";
    pressEnterToContinue();
}

void Civilian::performAction(vector<Player*>& allPlayers) {
    (void)allPlayers;
    clearScreen();

    auto start = chrono::steady_clock::now();

    auto elapsed = [&]() {
        return (int)chrono::duration_cast<chrono::seconds>(
            chrono::steady_clock::now() - start).count();
        };

    while (true) {
        int left = TURN_SECONDS - elapsed();
        if (left <= 0) {
            cout << "\n  Time is up!\n";
            chrono::duration<double> wait(1.5);
            this_thread::sleep_for(wait);
            break;
        }

        clearScreen();
        cout << "\n  Time remaining: " << left << "s  |  Player: " << name << "\n";
        gTaskPool.display();

        if (gTaskPool.allDone()) {
            cout << "\n  All tasks are done! Nothing left to do.\n";
            break;
        }

        // Attempt the next task with remaining time
        left = TURN_SECONDS - elapsed();
        if (left <= 0) break;

        bool answered = gTaskPool.attemptNext(left);
        (void)answered;

        // After one attempt (right or wrong), check time
        left = TURN_SECONDS - elapsed();
        if (left <= 0) {
            cout << "\n  Time is up!\n";
            chrono::duration<double> wait(1.5);
            this_thread::sleep_for(wait);
            break;
        }

        // Give the player a brief moment to see the result, then auto-proceed
        cout << "\n  (Turn ends automatically when time is up — or press ENTER to skip)\n  > ";
        cout.flush();

        // Wait for either ENTER or timeout
        auto fut = async(launch::async, []() {
            string s;
            getline(cin, s);
            });

        left = TURN_SECONDS - elapsed();
        if (left <= 0 || fut.wait_for(chrono::seconds(max(left, 0))) == future_status::timeout) {
            cout << "\n  Time is up!\n";
            chrono::duration<double> wait(1.0);
            this_thread::sleep_for(wait);
            break;
        }
        // Player pressed ENTER — end turn early
        break;
    }

    clearScreen();
}

bool Civilian::checkWin(const vector<Player*>& allPlayers) const {
    for (Player* p : allPlayers) {
        if (!p->isAlive()) continue;
        if (p->getRole() == "Imposter") return false;
    }
    return true;
}