LandingPad
==========

small landing pad game

Basic Control:
Arrow Key for direction
Q for quit
Space for pause/start game

Resize:
If the window is smaller than 800*600 (either hight or width), the game will terminate and display error message "TOO SMALL"
If the window is bigger than 800*600, the game will be centered in the window.

Game:
After started, the ship will fall in a consant speed. Press any arrow key will active specific thruster and change the speed of the ship.
If the ship touchs terrain or landing in the pad too fast(greater than initial speed at y coordinate), the ship will crush.
If the ship touches right edge or left edge, you will lost the control of ship.
Ship going out of top edge is allowed to reduce the difficulty of the game.

Usage:
XWindow lab is needed as compiled in makefile
Simply type "make run" in terminal to run the game
