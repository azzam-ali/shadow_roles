#include "Game.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <cstdlib>
#include <ctime>
using namespace std;

int main()
{
    // Seed random number generator with current time
    srand((unsigned)time(nullptr));//inside s rand we are expecting a positive integer

    clearScreen();// this is a function that clears the output window screen afer it is called 

    cout << "\n";
    cout << "  ========================================\n";
    cout << "         S H A D O W   R O L E S\n";
    cout << "    A Turn-Based Social Deduction Game\n";
    cout << "  ========================================\n\n";
    cout << "  Stay alive. Complete tasks. Trust no one.\n\n";

    Game game;// creation of class
    bool resumed = false;

    // Check if a saved game exists and offer to resume
    ifstream checkSave("shadow_roles_save.txt");//opening this filw
    if (checkSave.good())
    {
        checkSave.close();
        cout << "  A saved game was found. Do you want to resume? (y/n): ";
        char ch;
        cin >> ch;
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');

        if (ch == 'y' || ch == 'Y')
        {
            game.loadSave();
            resumed = true;
        }
        else
        {
            remove("shadow_roles_save.txt");
            cout << "  Old save deleted. Starting a new game.\n\n";
        }
    }
    else
    {
        checkSave.close();
    }

    if (!resumed)
    {
        // Ask how many players
        int n = 0;
        while (n < 3 || n > 8)
        {
            cout << "  How many players? (3 to 8): ";
            cin >> n;
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');

            if (n < 3 || n > 8)
                cout << "  Please enter a number between 3 and 8.\n";
        }

        // Get each player's name
        vector<string> names;
        for (int i = 0; i < n; i++)
        {
            cout << "  Player " << (i + 1) << " name: ";
            string nm;
            getline(cin, nm);

            // Use a default name if they left it blank
            if (nm.empty())
                nm = "Player" + to_string(i + 1);// to string converts a number to a string

            names.push_back(nm);//pushback is a vector function that adds elements in the vector . in this case the vector names is 
        }

        game.initWithNames(names);
    }

    // Start the game loop
    game.run();

    return 0;
}