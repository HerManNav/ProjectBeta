# Project Beta UE 5.3

So far, this full set of features has been implemented in C++:

- Movement:
  - Character (input-controlled, both keyboard and Gamepad):
    - Walk/run toggle
    - Jump
    - Dodge
    - Equip weapons
    - Sheathe/Unseathe weapons
    - Attack
    - Health & Death
    - Gold & Souls (HUD & inventory)
  - Enemies (AI-controlled):
    - Patrol
    - Chasing the character when seen
    - Attack
    - Health & Death

- Combat:
  - Box trace to detect collisions for different kinds of weapons
  - Directional hits detection to play different animations
  - Enemy's translation Motion Warping to stay closer to character when attacking (used to set the difficulty)
  - Enemy's rotation Motion Warping to face character when attacking
  - Enemy's XP points spawned when killed (Souls)
  - LockOn plugin integration

- Death:
  - Main character and Enemies random animation deaths
  - Dithering & particles before destroying Enemy instances (dithering only for Paladin)

Different types of Enemies have been implemented:
- Paladin (Terrestrial, Weaponized)
- Dinosaur (Terrestrial, Bite)
- Mosquito (Aerial, Bite)

Besides, animations, SFx, and particles have been included for:
  - Hits, movements (steps, armor, dodge), Items collection, and exertions for main character and NPCs
  - Items: Treasures (for Gold), Souls (for XP)

Some destructibles / Geometry Collections have been also added to the repo.


## Demos

https://github.com/HerManNav/ProjectBeta/assets/2845892/8a5f3159-fc01-42a4-9535-d5c90a1dc66a

https://github.com/HerManNav/ProjectBeta/assets/2845892/b83e1b14-3a3e-4cfa-b319-a68d2c5bac46

https://github.com/HerManNav/ProjectBeta/assets/2845892/4c9c244b-af35-4167-acd6-c82eb103cabf

