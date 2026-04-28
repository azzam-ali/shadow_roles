// ============================================================
//  Shadow Roles — main entry point
//  CS112 OOP Project  |  GIK Institute
// ============================================================
#include "Game.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <cstdlib>
#include <ctime>

int main() {
    srand((unsigned)time(nullptr));
    clearScreen();

    std::cout << "\n";
    std::cout << "  =========================================\n";
    std::cout << "         S H A D O W   R O L E S\n";
    std::cout << "    A Turn-Based Social Deduction Game\n";
    std::cout << "  =========================================\n\n";
    std::cout << "  Stay alive. Trust no one.\n\n";

    Game game;

    // Check for a saved game
    bool resumed = false;
    {
        std::ifstream chk("shadow_roles_save.txt");
        if (chk.good()) {
            chk.close();
            std::cout << "  A saved game exists. Resume? (y/n): ";
            char ch; std::cin >> ch;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (ch == 'y' || ch == 'Y') {
                game.loadSave();
                resumed = true;
            }
            else {
                remove("shadow_roles_save.txt");
            }
        }
    }

    if (!resumed) {
        int n = 0;
        while (n < 3 || n > 8) {
            std::cout << "  Number of players (3-8): ";
            if (!(std::cin >> n))
            {
                std::cin.clear(); std::cin.ignore(1000, '\n'); n = 0; continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (n < 3 || n > 8) std::cout << "  Must be 3-8 players.\n";
        }

        std::vector<std::string> names;
        for (int i = 0; i < n; i++) {
            std::cout << "  Player " << (i + 1) << " name: ";
            std::string nm;
            std::getline(std::cin, nm);
            if (nm.empty()) nm = "Player" + std::to_string(i + 1);
            names.push_back(nm);
        }

        game.initWithNames(names);
    }

    game.run();
    return 0;
}
