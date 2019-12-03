# Architecture of pacman program

## Aproach
The approach for this implementation of the game pacman is to divide the game in the following parts:
#### - Map
- The map is made with an array, each array element represents a space of the map and each 18 spaces represent a row, each space in this array can hold one of the following numbers to signalize the elements of the map:
- Wall: -1
- Empty space: 0
- Coin: 1
- Ghost Spawn: -2 and -3
#### - Characters
For the characters there is a second array of the same length as the map to signalize the position of the ghosts and Pacman as 2 and 1 respectively, making possible to check the position of this characters and send a signal when they collide, in order to declare GAME OVER.
  - Pacman:
    - In the pacman character there is the code to listen for user input through the arrow keys to move the character, also this replace the coins with empty spaces and increase the score and check if the player won, if he collect all coins the game declare he WON.
  - Ghosts:
    - For the ghosts a single function is enough because each one moves randomly, the movements of the ghosts is random but always moving forward and checking with the map array to avoid that a ghost go through walls, also it checks for collision with the player, if they collide the game declare GAME OVER.

## - Updater
With this approach the board can be called to be print every update to the character array, meaning that when a character does a change, the character responsible for the change notify the updater by a message through a special channel.
