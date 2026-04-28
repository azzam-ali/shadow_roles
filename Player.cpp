#include "Player.h"

using namespace std;

Player::Player(const string& n, int id, const string& r)
    : role(r), name(n), playerID(id), alive(true), voteCount(0) {
}