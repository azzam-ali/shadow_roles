#include "Utils.h"
#include <iostream>
#include <limits>
using namespace std;

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pressEnterToContinue()
{
    cout << "\n  [ Press ENTER to continue... ]";
    cin.ignore((numeric_limits<streamsize>::max)(), '\n');//cleans buffer
    cin.get();
}

void handoffScreen(const string& nextPlayerName)
{
    clearScreen();
    cout << "\n\n";
    cout << "  +======================================+\n";
    cout << "  |          PASS THE LAPTOP             |\n";
    cout << "  +======================================+\n";
    cout << "  |                                      |\n";
    cout << "  |  Next player: " << nextPlayerName;

    // pad the line to keep the box neat
    int pad = 23 - (int)nextPlayerName.size();
    for (int i = 0; i < pad; i++) cout << ' ';
    cout << "|\n";

    cout << "  |                                      |\n";
    cout << "  |  When ready, press ENTER to see      |\n";
    cout << "  |  your secret role.                   |\n";
    cout << "  |                                      |\n";
    cout << "  +======================================+\n\n";
    cout << "  [ " << nextPlayerName << ", press ENTER ] ";

    cin.ignore((numeric_limits<streamsize>::max)(), '\n');
    cin.get();
}