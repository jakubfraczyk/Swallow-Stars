# Swallow-Stars
Event-driven text-mode action game in C/C++ (ncurses) - PG academic project.
# ✨ Swallow Stars - Text-Mode Action Game

![Gameplay Demo](![Uploading swallowstars.gif…]()
)

## 📌 About The Project
"Swallow Stars" is a real-time, event-driven text-mode arcade game developed as an academic project during my 1st semester of Data Engineering at Gdańsk University of Technology. 

The player controls a swallow dodging hunters and collecting stars. The game enforces a strict procedural programming paradigm—Object-Oriented Programming (OOP) constructs and STL containers were explicitly forbidden. 

## 🚀 Key Features
* **Event-Driven Architecture:** Implemented an event-driven main loop handling real-time asynchronous user input and rendering.
* **Custom Text Rendering:** Handled multi-cell sprite management, rendering, and complex clean screen-border reflections for various geometric shapes using text graphics (`ncurses`).
* **Real-time Collision Detection:** Utilized custom data structures and cell occupancy maps to track and resolve collisions between moving entities.
* **Configuration-Driven Gameplay:** Complete level environments (star quotas, timers, spawn rates, hunter behaviors, and swallow speed bounds) are dynamically loaded from text configuration files.

## 🛠️ Tech Stack
* **Language:** C / C++ 
* **Paradigm:** Procedural Programming
* **Graphics Library:** `ncurses.h` 

## 💻 How to Run
1. Clone the repository.
2. Compile using a standard C/C++ compiler with the ncurses library linked
   ```bash
   gcc project1.c config.c hunter.c bird.c game_mechanics.c menu.c stars.c -lncurses -lm -o PROJECT1
3. Run this project ./PROJECT1
