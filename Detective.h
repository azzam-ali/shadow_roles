#ifndef DETECTIVE_H
#define DETECTIVE_H

#include "Player.h"
using namespace std;

class Detective : public Player
{
private:
    bool inspectedThisRound;

    void inspectPlayer(const string& targetName, vector<Player*>& allPlayers);

public:
    Detective(const string& name, int id);

    void performAction(vector<Player*>& allPlayers) override;
    void showRoleCard() const override;
    bool checkWin(const vector<Player*>& allPlayers) const override;
};

#endif