#ifndef DETECTIVE_H
#define DETECTIVE_H

#include "Player.h"
#include <vector>
#include <string>

using namespace std;

class Detective : public Player {
private:
    bool inspectUsed;
    static const int TURN_SECONDS = 20;

    void inspectPlayer(const vector<Player*>& allPlayers);

public:
    Detective(const string& name, int id);

    void performAction(vector<Player*>& allPlayers) override;
    void showRoleCard() const override;
    bool checkWin(const vector<Player*>& allPlayers) const override;
};

#endif