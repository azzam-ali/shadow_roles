#ifndef IMPOSTER_H
#define IMPOSTER_H

#include "Player.h"
#include <vector>
#include <string>

using namespace std;

class Imposter : public Player {
private:
    bool killUsed;      // ONE kill per round only
    static const int TURN_SECONDS = 25;

public:
    Imposter(const string& name, int id);

    void performAction(vector<Player*>& allPlayers) override;
    void showRoleCard() const override;
    bool checkWin(const vector<Player*>& allPlayers) const override;
};

#endif