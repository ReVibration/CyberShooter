# CyberShooter

**CyberShooter** is a portfolio-focused roguelike top-down shooter prototype built in **Unreal Engine 5** using a C++-first architecture. The project focuses on responsive top-down combat, room-based run progression, upgrade rewards, enemy AI, and a complete playable build.

## Project Overview

CyberShooter is a run-based shooter where the player moves through a sequence of combat and reward rooms. Each run is built from authored rooms selected during gameplay, creating a roguelike-style structure while keeping the level design controlled and readable.

The project was developed as a portfolio piece to demonstrate Unreal Engine 5 C++ gameplay programming, clean system separation, event-driven room progression, and packaged-build completion.

## Key Features

* Top-down player movement with mouse-facing aiming
* Projectile-based weapon system
* Weapon muzzle socket projectile spawning
* Muzzle flash, recoil, camera shake, and shooting feedback
* Player health and damage system
* Enemy AI finite state machine
* Enemy states including idle, wander, chase, and attack
* Slot-based enemy positioning around the player
* Melee enemy attack behaviour
* Enemy death events used for room progression
* Room-based roguelike run structure
* Combat rooms and reward rooms
* Reward upgrade system
* Game over and restart flow
* Main menu and in-game UI
* Audio feedback for combat, UI, and music
* Packaged Windows build

## Gameplay Loop

1. The player starts a new run from the main menu.
2. A combat room is selected and loaded.
3. The player defeats all enemies in the room.
4. Room completion advances the run.
5. After a set number of combat rooms, the player enters a reward room.
6. The player selects an upgrade and continues the run.
7. The loop continues until the player dies.
8. On death, the game over screen allows the player to restart or return to the main menu.

## Technical Focus

The project was built with a strong focus on C++ gameplay systems. Blueprints are mainly used for asset assignment, UI layout, animation setup, and editor-facing tuning.

### Main Technical Systems

#### Player Character

* C++ movement and aiming logic
* Plane-projected mouse aiming to avoid camera/geometry aiming issues
* Projectile firing with fire-rate limiting
* Weapon mesh attached through sockets
* Projectile spawning from a muzzle socket
* Health and damage handling
* Combat feedback including muzzle flash, recoil, sound, and camera shake

#### Enemy AI

* Finite State Machine architecture
* Idle, wander, chase, and attack states
* Slot-based positioning around the player
* Basic crowd avoidance
* Stuck detection and recovery
* Event broadcast on enemy death

#### Room Progression

* Rooms are implemented as separate levels
* Room metadata is stored in room definition Data Assets
* Combat room completion is event-driven
* Room managers bind to enemy death events
* Reward room exits advance the run
* Room selection avoids immediate repeats

#### Run Flow

* Persistent run data is stored in the GameInstance
* GameInstance stores room pools, run state, current room index, and run seed
* Run flow follows a combat/combat/reward pattern
* Restart and return-to-menu flow is supported

#### Upgrade System

* Upgrade choices are presented during reward rooms
* Player upgrades affect run progression and combat strength
* Upgrade data is stored separately from the player for cleaner run-state management

#### Audio

* Combat sound effects for firing, impacts, damage, and death
* UI click feedback
* Menu/gameplay music support
* Sound assets assigned through the editor while playback is triggered through gameplay/UI events

## Architecture Goals

The project was designed around the following principles:

* C++ first gameplay implementation
* Clear responsibility per class
* Reusable components where appropriate
* Event-driven systems instead of unnecessary ticking
* Authored rooms selected procedurally rather than runtime-generated geometry
* Portfolio-readable code structure
* Systems that can be expanded without requiring a full rewrite

## Technologies Used

* Unreal Engine 5
* C++
* Unreal Motion/Animation Blueprint workflow
* UMG for UI layout
* Data Assets for room definitions and configurable gameplay data
* Windows packaged build

## Controls

| Input             | Action                                              |
| ----------------- | --------------------------------------------------- |
| WASD              | Move player                                         |
| Mouse             | Aim                                                 |
| Left Mouse Button | Fire weapon                                         |
| UI Buttons        | Start run, select upgrades, restart, return to menu |

## Project Status

This project is considered a complete portfolio vertical slice. The focus was not to create a full commercial roguelike, but to finish a playable, packaged prototype that demonstrates core gameplay programming and Unreal Engine project completion.

Completed areas include:

* Core combat
* Enemy AI
* Room progression
* Reward flow
* Upgrade system
* UI flow
* Audio feedback
* Packaged build

## Possible Future Improvements

If expanded further, future development could include:

* More enemy types
* More upgrade variety
* Boss encounters
* Additional room layouts
* More advanced encounter scaling
* Improved visual effects
* More advanced music transitions
* Additional animation polish

## Portfolio Summary

CyberShooter demonstrates a complete Unreal Engine 5 C++ gameplay prototype with a functioning roguelike room loop, enemy AI, upgrade progression, combat feedback, UI flow, and packaged Windows release.

The project shows the ability to design, implement, debug, polish, and ship a playable game prototype using Unreal Engine 5.
