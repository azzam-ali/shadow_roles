#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
using namespace std;

class Detective;   // forward declaration so we can declare it as friend

class Player
{
private:
    string role;   // private: only accessible through getRole() or via friend Detective

protected:
    string name;
    int    playerID;
    bool   alive;
    int    voteCount;

public:
    Player(const string& n, int id, const string& r);
    virtual ~Player() {}

    // Pure virtual functions - make Player an abstract class
    virtual void performAction(vector<Player*>& allPlayers) = 0;
    virtual void showRoleCard() const = 0;
    virtual bool checkWin(const vector<Player*>& allPlayers) const = 0;

    // Getters
    string getName()  const { return name; }
    int    getID()    const { return playerID; }
    bool   isAlive()  const { return alive; }
    int    getVotes() const { return voteCount; }
    string getRole()  const { return role; }

    // Actions
    void addVote() { voteCount++; }
    void resetVotes() { voteCount = 0; }
    void eliminate() { alive = false; }

    // Friend class - Detective can read the private 'role' member directly
    friend class Detective;
};

#endif