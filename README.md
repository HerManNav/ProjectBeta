# Project Beta UE 5.3

So far, this full set of features has been implemented in C++:

- Movement:
  - Character (input-controlled, both keyboard and Gamepad):
    - Walk/run toggle
    - Jump
    - Equip weapons
    - Attack
    - Health & Death
  - Enemies (AI-controlled):
    - Patrol
    - Chasing character when seen
    - Attack
    - Health & Death

- Combat:
  - Box trace to detect collisions for different kind of weapons
  - Directional hits detection to play different animations
  - Enemy's translation Motion Warping to stay closer to character when attacking (used to set the difficulty)
  - Enemy's rotation Motion Warping to face character when attacking

- Death:
  - Enemies random animation deaths
  - Dithering & particles before destroying enemy instances
 
Besides, animations, SFx, and particles have been included for:
  - Hits
  - Steps, armor and main character exertion

Some destructibles / Geometry Collections have been also added to the repo.


## Demos

https://github.com/HerManNav/ProjectBeta/assets/2845892/b83e1b14-3a3e-4cfa-b319-a68d2c5bac46

https://github.com/HerManNav/ProjectBeta/assets/2845892/4c9c244b-af35-4167-acd6-c82eb103cabf

