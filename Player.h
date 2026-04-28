#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>

using namespace std;

class Detective;

class Player {
private:
    string role;

protected:
    string name;
    int playerID;
    bool alive;
    int voteCount;

public:
    Player(const string& n, int id, const string& r);
    virtual ~Player() {}

    virtual void performAction(vector<Player*>& allPlayers) = 0;
    virtual void showRoleCard() const = 0;
    virtual bool checkWin(const vector<Player*>& allPlayers) const = 0;

    string getName()  const { return name; }
    int    getID()    const { return playerID; }
    bool   isAlive()  const { return alive; }
    int    getVotes() const { return voteCount; }
    string getRole()  const { return role; }

    void addVote() { voteCount++; }
    void resetVotes() { voteCount = 0; }
    void eliminate() { alive = false; }

    friend class Detective;
};

#endif