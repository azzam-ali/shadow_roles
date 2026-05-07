#ifndef CIVILIAN_H
#define CIVILIAN_H

#include "Player.h"
using namespace std;

class Civilian : public Player
{
public:
    Civilian(const string& name, int id);

    void performAction(vector<Player*>& allPlayers) override;
    void showRoleCard() const override;
    bool checkWin(const vector<Player*>& allPlayers) const override;
};

#endif