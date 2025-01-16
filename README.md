# DefendArland - PvE Wave Survival Gamemode for Arma Reforger

DefendArland is a custom PvE wave survival gamemode mod for **Arma Reforger**. The mod introduces a new game mode where players face off against waves of AI enemies, preparing their defenses, gearing up at an arsenal, and surviving as long as they can—or until the extraction vehicle arrives. The mod supports both **single-player** and **dedicated server** setups, offering a challenging survival experience.

## Project Overview

In DefendArland, players will need to prepare for a series of increasingly difficult enemy waves. The gameplay centers around strategically placing defenses, choosing the right gear at the arsenal, and cooperating with teammates or facing the challenge solo. The ultimate goal is to survive through as many waves as possible and ensure the safe arrival of an extraction vehicle to evacuate the players.

### Key Features:
- **PvE Wave Survival:** Fight against waves of AI enemies with varying difficulty.
- **Defense Building System:** Use the Conflict gamemode’s building system to construct defenses and fortifications.
- **Extraction Objective:** Survive waves until the extraction vehicle arrives to rescue you and your team.
- **Single Player and Dedicated Server Support:** Play solo or host on dedicated servers for multiplayer action.
- **Dynamic Waves:** Waves increase in difficulty with different types of enemies and scenarios.
- **Hint and HUD System:** Keep track of game progress and objectives with in-game hints and a heads-up display (HUD).
  
## Core Logic

Most of the game logic and flow is contained in the `DefendManager` class, which is located in the following file path:
`DefendArland/scripts/Game/Components/DefendManager.c`


### DefendManager Class
The `DefendManager` class is the heart of the gameplay. It manages the spawning of waves, player interactions, AI enemy groups, extraction processes, and overall game state. Key responsibilities of the `DefendManager` include:
- Managing waves of AI enemies.
- Handling the extraction process and ensuring the player or team can reach the extraction vehicle.
- Allowing players to build defenses using the Conflict gamemode's building system.
- Providing a dynamic game environment with varied scenarios depending on the wave progress.
- Tracking player and enemy actions, such as AI deaths, desertions, and wave status.

## Gameplay Overview

1. **Prepare for Battle:** 
   - The game begins with players having a period to prepare. During this time, players can use the arsenal to gear up and use the Conflict gamemode's building system to fortify their position.
   
2. **Survive the Waves:** 
   - Waves of AI enemies will spawn and attack the players' defenses. Each wave is progressively harder than the last, with more and stronger enemies.
   - Players must manage resources, build defenses, and work together (or alone) to survive each wave.

3. **Extraction:**
   - Once the final wave is reached, or if the player or team survives long enough, the extraction vehicle will be called.
   - Players must then survive until the extraction vehicle arrives and safely board it to win the game.
   
## Installation

### Requirements:
- **Arma Reforger** - This mod is designed for Arma Reforger and requires the base game to run.
- **Conflict Gamemode** - Required for the building and arsenal mechanics used within the DefendArland gamemode.

### Steps to Install:

1. Download or clone the repository to your local machine.
2. Place the `DefendArland` mod folder into your Arma Reforger mods directory.
3. Launch Arma Reforger and enable the **DefendArland** mod in your mod list.
4. Host a local or dedicated server, or start a single-player game to begin playing the DefendArland gamemode.

## Gameplay Walkthrough

### Starting the Game
- Select the **DefendArland** gamemode from the available scenarios.
- Once the game begins, you will be given time to prepare. Use this period to gear up at the arsenal and build your defenses using the **Conflict** gamemode's building system.
  
### Surviving the Waves
- Waves of AI enemies will start to attack your position.
- During the waves, you will need to manage resources, repair structures, and fight off the AI to survive.
  
### Extraction Phase
- After surviving enough waves (or once the maximum wave count is reached), the extraction vehicle will be called.
- You must stay alive long enough for the extraction vehicle to arrive and then board the vehicle to escape the enemy forces.

## Key Classes and Components

### DefendManager
- **Location:** `DefendArland/scripts/Game/Components/DefendManager.c`
- The core logic of the gamemode, managing everything from wave spawns to player interactions and extraction.

### HintHelpers
- Provides the system for sending dynamic in-game hints and messages to players to guide them through different stages of the game.
  
### AI Groups
- **SpawnedEnemyGroup:** This component manages the spawning and behavior of AI enemies, including group formation, movement, and engagement in combat.
  
### Extraction Logic
- Handles the spawning and movement of the extraction vehicle, along with tracking its progress and handling player interaction with the vehicle.

## Contributing

If you would like to contribute to the development of DefendArland, feel free to fork the repository and create pull requests. You can also report issues, suggest features, or help with bug fixes.

### Issues
If you encounter any issues with the mod or have suggestions for improvements, please open an issue on the GitHub repository.

---

Enjoy your survival experience in **DefendArland**—build, survive, and extract!
