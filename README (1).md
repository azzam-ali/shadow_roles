# Shadow Roles 🎭
A turn-based social deduction game made in C++ for our CS112 OOP project at GIK Institute.

Basically its like Among Us but in the terminal lol. One person is the Imposter, one is the Detective, and everyone else is a Civilian. You pass the laptop around between players so nobody sees each other's screens.

---

## What you need
- Visual Studio 2019 or later (we used Visual Studio)
- The C++ desktop development workload installed in Visual Studio
- 3 to 8 friends (or just yourself testing it out)

---

## How to run it in Visual Studio

1. Open Visual Studio
2. Click **Create a new project** (or File → New → Project)
3. Select **Empty Project** under C++
4. Click Create and give it a name like `shadow_roles`
5. In the **Solution Explorer** on the right, right click on **Source Files**
6. Click **Add → Existing Item**
7. Select ALL the `.cpp` files at once:
   - main.cpp
   - Player.cpp
   - Civilian.cpp
   - Detective.cpp
   - Imposter.cpp
   - Game.cpp
   - TaskPool.cpp
   - Utils.cpp
8. Do the same for the **Header Files** folder — add all the `.h` files
9. Right click on your project name in Solution Explorer → **Properties**
10. Go to **Configuration Properties → General** and make sure **C++ Language Standard** is set to **ISO C++17**
11. Hit **Ctrl + F5** to build and run (or just click the green play button at the top)

> **Note:** If you get an error saying the .exe can't be opened for writing, it just means the program is still running in the background. Close the terminal window first then try again.

---

## How to play

### Setup
1. Run the game
2. Enter how many players there are (between 3 and 8)
3. Type each player's name one by one
4. Each player will then privately see their role card — **make sure everyone else looks away!**
5. Pass the laptop to the next person after each turn

### Roles

| Role | How many | What they do |
|------|----------|--------------|
| Civilian | Most players | Complete math tasks to win |
| Detective | 1 (only if 4+ players) | Inspect one player per round to find the imposter |
| Imposter | 1 | Kill one crew member per round secretly |

### Each round has two phases

**Turn Phase** — players go in random order, each gets their own turn

- **Civilian:** type `task` to attempt a math question, or `skip` to end your turn
- **Detective:** type `task` to do a task, `inspect <name>` to reveal someone's role (once per round only!), or `skip`
- **Imposter:** type `kill <name>` to silently eliminate someone, or `skip` to do nothing

**Voting Phase** — everyone discusses and votes

- Type `vote <name>` to vote against someone
- Type `skip` if you don't want to vote
- The person with the most votes gets eliminated and their role is revealed
- If it's a tie nobody gets eliminated

### Tasks
Tasks are math questions (addition, subtraction, multiplication, division). The whole crew shares the same task list — when one person completes a task it's done for everyone. How many tasks there are depends on how many players:

- 3 to 7 players → 15 tasks
- 8 to 10 players → 25 tasks
- 11 to 15 players → 30 tasks

### How to win

| Who wins | Condition |
|----------|-----------|
| Crew (Civilians + Detective) | All tasks completed |
| Crew (Civilians + Detective) | Imposter gets voted out |
| Imposter | Kills enough people to equal or outnumber the remaining crew |

---

## Files in this project

```
shadow_roles/
├── main.cpp          ← where the program starts
├── Player.h/.cpp     ← base class for all players (abstract)
├── Civilian.h/.cpp   ← civilian role
├── Detective.h/.cpp  ← detective role
├── Imposter.h/.cpp   ← imposter role
├── TaskPool.h/.cpp   ← the shared math task system
├── Game.h/.cpp       ← runs the whole game loop
├── Utils.h/.cpp      ← helper functions (clear screen, handoff screen etc)
└── README.md         ← this file
```

---

## Extra files that get created when you play

- **shadow_roles_save.txt** — the game auto saves after every round so you can resume later if you close it. Gets deleted automatically when the game ends normally.
- **shadow_roles_log.txt** — keeps a log of everything that happened (kills, votes, who won) with timestamps. Useful for looking back at what happened.

These files will appear in the same folder as your `.exe` which is usually inside `x64/Debug/` inside your project folder.

---

## OOP concepts used (for the report)

- **Abstraction** — Player is an abstract class with pure virtual functions
- **Inheritance** — Civilian, Detective and Imposter all inherit from Player
- **Polymorphism** — performAction() and showRoleCard() are virtual, so the game loop doesn't need to know which role each player is
- **Encapsulation** — the role field in Player is private
- **Friend class** — Detective is a friend of Player so it can directly read the private role field
- **Composition** — Game creates and owns all Player objects
- **Aggregation** — players receive the allPlayers list during their turn but don't own it
- **Association** — all player classes use the global gTaskPool without owning it
- **File handling** — save/resume system and event log

---

## Known stuff to be aware of

- Names with spaces might cause issues with the inspect/kill/vote commands since those read one word. Use single word names like "Ali" or "Sara" not "Ali Khan"
- The game saves progress automatically so if you close it mid game you can resume from where you left off next time you run it
- The log file keeps adding to itself across multiple games so it can get long over time

---

*Made by [your names here] — CS112 OOP Project, GIK Institute*
