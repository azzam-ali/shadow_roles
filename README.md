# Shadow Roles
### A Turn-Based Social Deduction Game in C++
**CS112 — Object-Oriented Programming & Design | GIK Institute**

## Gameplay Overview

### Setup
- Enter 3–8 player names.
- Each player privately sees their secret role card.
- **Pass the laptop** between players — no peeking!

### Roles

| Role      | Count    | Goal                                      |
|-----------|----------|-------------------------------------------|
| Civilian  | Majority | Complete all tasks before the crew falls  |
| Detective | 1 (=4p)  | Find the Imposter; also completes tasks   |
| Imposter  | 1        | Outnumber the crew; kill secretly         |

### Each Round

1. **Turn Phase** — Players take turns in randomised order (timed!)
   - **Civilian**: `do <n>` to attempt a task, `skip` to end turn
   - **Detective**: `do <n>` for tasks, `inspect <id>` to reveal a role (once/turn), `skip`
   - **Imposter**: `kill <id>` to silently eliminate one player, `skip`

2. **Voting Phase** — All alive players vote to eliminate a suspect.
   - Most votes ? eliminated. Ties ? no elimination.

### Win Conditions
| Team              | Win when...                                  |
|-------------------|----------------------------------------------|
| Civilians+Detect. | All alive civilians/detectives finish tasks  |
| Civilians+Detect. | All Imposters are eliminated by vote         |
| Imposter          | Imposters = remaining crew                   |

### Task Mini-Game
- Civilians type **`SHADOW`** to complete a task
- Detective types **`CLUE42`** to collect evidence

---

## OOP Concepts Applied

| Concept             | Where Applied                                              |
|---------------------|------------------------------------------------------------|
| Classes & Objects   | Player, Civilian, Detective, Imposter, Game, Utils         |
| Encapsulation       | `role` is private in Player; accessed only through methods |
| Inheritance         | Civilian, Detective, Imposter all inherit from Player      |
| Polymorphism        | `performAction()` and `showRoleCard()` are virtual/overridden |
| Abstraction         | Player is abstract (pure virtual methods)                  |
| Friend Function     | Detective is `friend` of Player ? reads private `role`     |
| Composition         | Game owns and manages Player objects                       |
| Association         | Game holds vector of Player pointers                       |
| File Handling       | Save/resume via `shadow_roles_save.txt`, event log in `shadow_roles_log.txt` |

---

## File Structure

```
shadow_roles/
+-- main.cpp          — Entry point, player name input, game launch
+-- Player.h/.cpp     — Abstract base class; encapsulates private role
+-- Civilian.h/.cpp   — Civilian role: timed turns, task system
+-- Detective.h/.cpp  — Detective role: inspect power (friend), tasks
+-- Imposter.h/.cpp   — Imposter role: kill action
+-- Game.h/.cpp       — Game engine: rounds, voting, win checks, file I/O
+-- Utils.h/.cpp      — clearScreen(), handoffScreen(), helpers
+-- Makefile          — Build script
+-- README.md         — This file
```

---

## Save System
- Game auto-saves after every round to `shadow_roles_save.txt`.
- On launch, you are offered to resume the saved game.
- The save is deleted automatically when the game ends.
- All events are appended to `shadow_roles_log.txt` (timestamps included).
