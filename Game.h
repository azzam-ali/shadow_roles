#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Civilian.h"
#include "Detective.h"
#include "Imposter.h"
#include <vector>
#include <string>

using namespace std;

class Game {
private:
    vector<Player*> players;
    int  roundNumber;
    bool gameOver;
    string winnerTeam;
    vector<int> announcedDead;

    void buildFromNames(const vector<string>& names);
    void showRoleHandoffs();
    void runTurnPhase();
    void runVotingPhase();
    bool checkGameOver();
    void announceDeadThisRound();
    void saveState() const;
    void logEvent(const string& msg) const;
    void listAlivePlayers() const;

public:
    Game();
    ~Game();

    void initWithNames(const vector<string>& names);
    void loadSave();
    void run();
    void printFinalSummary() const;
};

#endif