# Text-Based_C_Game

Rules:

### Keys & Doors
In this game, you play as a character trying to escape a maze made up of multiple rooms connected by doors. Each door may be locked or unlocked and rooms may contain keys and other characters.
The world consists of a series of rooms, joined by doors. A room can have multiple doors to other rooms. Every door can be locked or not. A room can also contain keys and creatures.

The Player has a carrying capacity and a leadership parameter. The carrying capacity, which is an integer, indicates how many keys the PC can carry; the leadership parameter, which is another integer, indicates how many escapees can follow the player at the same time.

### Escapees and guards
Escapees are friendly characters who always want to escape. You can lead a limited number of them
Guards try to stop specific escapees, including yourself.

### Encounters with Guards
If guards outnumber escapees in a room, everyone is sent back to their starting positions. You can only be caught 5 times—after that, you lose.
If there are more escapees than guards, only guards whose targets are present will start chasing the group.

Your goal is to reach the exit door and escape!

#
Example Inputs:

3 2 1 1

0 1 1

0 2 0

1 1

0 0 0

1 0

1
