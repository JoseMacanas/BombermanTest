# BombermanTest


Time spent on the test:
Around the 15 hours allowed in development, though I had to spend a bit more researching features of Unreal Engine I was not familiar with.


References used:
How to change the color of a material using blueprints
https://answers.unrealengine.com/questions/178343/blueprint-change-base-color-of-material.html

EnGame Screen UI
https://answers.unrealengine.com/questions/367338/bp-set-input-mode-ui-only.html



Note: 
- The Remote Controlled Bombs pickup doesn't prevent bombs from exploding when their timer runs out. It just allows triggering the explosion manually ahead of time. This was a design decision since the specifications do not describe the desired behaviour completely.


What to do next:
- Display player colors next to their names in the end game screen

- Complete Camera pan when characters move (partially implemented)

- Improve the AI, with proper pathfinding and better awareness of the whole arena (right now only one danger is taken into account). Add levels of difficulty.

- Cleanup the LevelGrid class to make it lighter and improve class encapsulation.
	
- Improve Unreal reflection to allow better usage of Blueprints.

- Assign different random chances to different pickups.

- Adapt the game structure to Unreal standards to make easier a possible implementation of Online gameplay.


	