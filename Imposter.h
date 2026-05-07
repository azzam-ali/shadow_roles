#ifndef IMPOSTER_H
#define IMPOSTER_H

#include "Player.h"
using namespace std;

class Imposter : public Player
{
public:
    Imposter(const string& name, int id);

    void performAction(vector<Player*>& allPlayers) override;
    void showRoleCard() const override;
    bool checkWin(const vector<Player*>& allPlayers) const override;
};

#endif