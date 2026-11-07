# Terminal Snake Game (Windows)

A colorful, retro console Snake game written in C for Windows, featuring custom maps, multiple speed levels, dynamic scoring, and persistent leaderboard tracking.

---

## Features

* **ANSI Color Graphics:** Uses ANSI escape codes and Unicode blocks for vibrant snake, apple, wall, and border visuals.
* **Selectable Maps:**
  * **Normal:** Open arena bounded only by outer walls.
  * **Divider:** Arena split horizontally by a central wall.
  * **Corridors:** Dual horizontal barrier walls creating narrow lanes.
  * **The Cross:** Segmented cross layout adding obstacles across both axes.
* **9 Difficulty Speeds:** Adjustable speed tiers with tick delays ranging from 300 ms (Level 1) to 30 ms (Level 9).
* **Dynamic Score Multipliers:** Score increments scale with snake length, chosen speed level, and map difficulty.
* **Persistent High Scores:** Prompts for 3-letter player initials on qualifying scores and saves top 10 rankings to disk (`C:\Tools\snakeHS.txt`).

---

## Controls

| Key | Action |
| :--- | :--- |
| `W` / `Up Arrow` | Move Up |
| `A` / `Left Arrow` | Move Left |
| `S` / `Down Arrow` | Move Down |
| `D` / `Right Arrow` | Move Right |
| `Enter` | Select / Confirm |
| `Esc` | Return / Back |

---

## Getting Started

### Prerequisites

* Windows OS
* A C compiler supporting Windows headers (MinGW `gcc`, MSVC, or Clang)
* A terminal supporting ANSI escape codes and UTF-8 output (Windows Terminal recommended)
* A directory named `C:\Tools` (required for saving leaderboard scores)

```cmd
if not exist "C:\Tools" mkdir C:\Tools
```

### Compilation

Compile using `gcc` (MinGW):

```cmd
gcc -Wall -Wextra -O2 snek.c -o snek.exe
```

---

## Running the Game

Launch the binary directly from Command Prompt or Windows Terminal:

```cmd
snek.exe
```

---

## License

This project is open-source and available under the [MIT License](LICENSE).
