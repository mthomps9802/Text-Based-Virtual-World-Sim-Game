#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//door
typedef struct {
    int currentRoomId;
    int nextRoomId;
    bool isOpen;    // binary
} Door;

//key
typedef struct {
    int currentRoomId;         // shows what room key is located in
    int keyAccessibleDoor;    // shows what door this key opens
    bool isPickedUp;
} Key;

//struct for creature
typedef struct {
    int currentRoomId;      // shows what room creature is located in
    int typeOfCreatureId;  // 0 for PC, 1 for escapee, and 2 for guard
    int originalLocation; // holds the orginal location for creatures
} Creature;

//player who is also creature
typedef struct {
    Creature creature;        // accesses creature properties
    int leadership;          // how many escapees can follow you at a time
    int carryingCapacityId; // how many keys you can carry at a time
    int leadershipSize;    // number of followers
    int inventorySize;    // holds size of inv
} Player;


//guard who is also creature
typedef struct {
    Creature creature;     // accesses creature properties
    int targetCreatureId; // set target creature ID if creature is guard
    bool isFollowingTarget;
} Guard;

//escapee who is also creature
typedef struct {
    Creature creature;
    bool isLeadByPlayer;
} Escapee;

int numOfTimesCaught = 0; // global caught variable

// method declarations
void freeEverythingAndEndGame(Player *playerPtr, Door* doorPtr, Key *keyPtr, Guard *guardPtr, Escapee *escapeePtr);
void startGame(Player *playerPtr, Door *doorPtr, Key *keyPtr, Guard *guardPtr, Escapee *escapeePtr, int numKeys, int numDoors, int numCreatures,int winningDoorId, int numOfEscapees, int numOfGuards);
void lookCommand(Player *playerPtr, Door *doorPtr, Key *keyPtr, Guard *guardPtr, Escapee *escapeePtr, int numDoors, int numKeys, int numCreatures, int numOfEscapees, int numOfGuards, int winningDoorId);
void passCommand(Player *playerPtr, Door *doorPtr, Key *keyPtr, Guard *guardPtr, Escapee *escapeePtr, int doorId, int numDoors, int winningDoorId, int numKeys, int numOfEscapees, int numOfGuards);
void pickupCommand(Player *playerPtr, Key *keyPtr, int numKeys, int keyId);
void dropCommand(Player *playerPtr, Key *keyPtr, int numKeys, int keyId);
void invCommand(Player *playerPtr, Key *keyPtr, int numKeys);
void unlockCommand(Player *playerPtr, Key *keyPtr, Door *doorPtr, int doorId, int numKeys);
void leadCommand(Player *playerPtr, Escapee *escapeePtr, int numOfEscapees, int escapeeId);
void abandonCommand(Player *playerPtr, Escapee *escapeePtr, int numOfEscapees, int escapeeId);
void caughtMethod(Player *playerPtr, Door *doorPtr, Key *keyPtr, Guard *guardPtr, Escapee *escapeePtr, int numOfGuards, int numOfEscapees);
void leadCommand(Player *playerPtr, Escapee *escapeePtr, int numOfEscapees, int escapeeId);
void resetLocations(Player *playerPtr, Guard *guardPtr, Escapee *escapeePtr, int numOfGuards, int numOfEscapees);
bool checkMethod(Player *playerPtr, Guard *guardPtr, Door *doorPtr, Key *keyPtr,  Escapee *escapeePtr, int numOfGuards, int numOfEscapees);

int main() {
    int numRooms;
    int numDoors;
    int numKeys;
    int numCreatures;

    printf("\nInput the number of rooms, doors, keys and creatures below, respectively: \n");

    //add check for value between 1 and 500
    scanf("%d %d %d %d", &numRooms, &numDoors, &numKeys, &numCreatures);  //stdin
    printf("You have selected: "
           "%d rooms, "
           "%d doors, "
           "%d keys and "
           "%d creatures.\n",
           numRooms, numDoors, numKeys, numCreatures);


    // Taking door inputs and storing in allocated memory
    Door *doorPtr = malloc(numDoors * sizeof(Door));
    printf("\nInput room ID for current door, next door and key ID respectively for: \n");
    for (int i=0; i<numDoors; i++) {
        int currentRoomId;
        int nextRoomId;
        int isOpen;

        printf("Room %d: \n", i);
        scanf("%d %d %d", &currentRoomId, &nextRoomId, &isOpen);

        doorPtr[i].currentRoomId = currentRoomId;
        doorPtr[i].nextRoomId= nextRoomId;
        doorPtr[i].isOpen= isOpen;
    }

    // Taking key inputs and storing in allocated memory
    Key *keyPtr = malloc(numKeys * sizeof(Key));
    printf("\nInput room ID where the key is located and the door the key opens respectively for: \n");
    for (int i=0; i<numKeys; i++) {
        int currentRoomId;
        int keyAccessibleDoor;

        printf("Key %d: \n", i);
        scanf("%d %d", &currentRoomId, &keyAccessibleDoor);

        keyPtr[i].currentRoomId= currentRoomId;
        keyPtr[i].keyAccessibleDoor = keyAccessibleDoor;
        keyPtr[i].isPickedUp = false;
    }

    // Taking creature inputs and storing in allocated memory
    Player *playerPtr;
    Guard *guardPtr = malloc(numCreatures * sizeof(Guard));
    int numOfGuards = 0;
    Escapee *escapeePtr = malloc(numCreatures * sizeof(Escapee));
    int numOfEscapees = 0;
    printf("\nInput room ID where the creature is located, type of creature and creature ID of the guard's target: \n");
    for (int i=0; i<numCreatures; i++) {
        int currentRoomId;
        int typeOfCreatureId;
        int targetOfCreatureId;

        printf("Creature %d: \n", i);
        scanf("%d %d %d", &currentRoomId, &typeOfCreatureId, &targetOfCreatureId);

        if (typeOfCreatureId == 0) { // player
            Player player;
            player.creature.currentRoomId = currentRoomId;
            player.creature.typeOfCreatureId = typeOfCreatureId;
            player.creature.originalLocation = currentRoomId;
            player.inventorySize = 0;
            player.leadershipSize = 0;
            playerPtr = &player;    // set to address of this player
        } else if (typeOfCreatureId == 1) { // escapee
            escapeePtr[numOfEscapees].creature.currentRoomId = currentRoomId; // set address of esccapee
            escapeePtr[numOfEscapees].creature.typeOfCreatureId = typeOfCreatureId;
            escapeePtr[numOfEscapees].creature.originalLocation = currentRoomId;
            escapeePtr[numOfEscapees].isLeadByPlayer = false;
            numOfEscapees+=1;
        } else if(typeOfCreatureId == 2) { // guard
            guardPtr[numOfGuards].creature.currentRoomId = currentRoomId; // set address of guard
            guardPtr[numOfGuards].creature.originalLocation = currentRoomId;
            guardPtr[numOfGuards].creature.typeOfCreatureId = typeOfCreatureId;
            guardPtr[numOfGuards].targetCreatureId = targetOfCreatureId;
            guardPtr[numOfGuards].isFollowingTarget = false;
            numOfGuards+=1;
        } else {
            printf("\n%d is not a valid type of creature!", typeOfCreatureId);
            freeEverythingAndEndGame(playerPtr,doorPtr, keyPtr, guardPtr, escapeePtr);
        }
    }

    printf("\nInput player's carrying capacity and leadership below respectively: \n");
    scanf("%d %d", &playerPtr->carryingCapacityId, &playerPtr->leadership);

    int winningDoorIdInput;
    printf("\nInput the door the player must reach to win: \n");
    scanf("%d", &winningDoorIdInput);
    int winningDoorId = winningDoorIdInput;

    // start the game
    startGame(playerPtr, doorPtr, keyPtr, guardPtr, escapeePtr,numKeys, numDoors, numCreatures, winningDoorId, numOfEscapees, numOfGuards); //idk y a number works
    //freeEverythingAndEndGame(doorPtr, keyPtr, guardPtr, escapeePtr);
}



void startGame(Player *playerPtr, Door *doorPtr, Key *keyPtr, Guard *guardPtr, Escapee *escapeePtr, int numKeys, int numDoors, int numCreatures, int winningDoorId, int numOfEscapees, int numOfGuards) {
    char command[10];
    int argument;
    while (1) {
        printf("\nPlease enter a command: ");
        scanf("%s", command);
        if (strcmp(command, "look") == 0) {
            lookCommand(playerPtr, doorPtr, keyPtr, guardPtr, escapeePtr, numDoors, numKeys, numCreatures, numOfEscapees, numOfGuards, winningDoorId);
        }else if (strcmp(command, "pass") == 0) {
            scanf("%d", &argument);
            passCommand(playerPtr, doorPtr, keyPtr, guardPtr, escapeePtr, argument, numDoors, winningDoorId, numKeys, numOfEscapees, numOfGuards);
        }else if (strcmp(command, "quit") == 0) {
            freeEverythingAndEndGame(playerPtr,doorPtr, keyPtr, guardPtr, escapeePtr);
        }else if (strcmp(command, "pickup") == 0) {
            scanf("%d", &argument);
            pickupCommand(playerPtr, keyPtr, numKeys, argument);
        }else if (strcmp(command, "drop") == 0) {
            scanf("%d", &argument);
            dropCommand(playerPtr, keyPtr, numKeys, argument);
        }else if (strcmp(command, "unlock") == 0) {
            scanf("%d", &argument);
            unlockCommand(playerPtr, keyPtr, doorPtr, argument, numKeys);
        }else if (strcmp(command, "lead") == 0) {
            scanf("%d", &argument);
            leadCommand(playerPtr, escapeePtr, numOfEscapees, argument);
        }else if (strcmp(command, "abandon") == 0) {
            scanf("%d", &argument);
            abandonCommand(playerPtr, escapeePtr, numOfEscapees, argument);
        }else if (strcmp(command, "inv") == 0) {
            invCommand(playerPtr, keyPtr, numKeys);
        }else {
            printf("Unknown command.\n");
        }
        printf("\n");
    }
}

//Look command - describe the room the PC is in
void lookCommand(Player *playerPtr, Door *doorPtr, Key *keyPtr, Guard *guardPtr, Escapee *escapeePtr, int numDoors, int numKeys, int numCreatures, int numOfEscapees, int numOfGuards, int winningDoorId) {
    int roomId = playerPtr->creature.currentRoomId; //where the PC is currently located
    // add winning door somehwere here
    printf("\nYou are in room %d.", roomId);

    // Keys are in the same room as PC
    bool foundKey = false;
    for(int i=0; i<numKeys; i++) {
        if(!keyPtr[i].isPickedUp && keyPtr[i].currentRoomId == roomId) { //if key roomId == roomId  then key and PC are in same room
            printf("\nThere is a key %d for door %d.", i, keyPtr[i].keyAccessibleDoor);
            foundKey = true;
        }
    }
    if(!foundKey) printf("\nThere are no item(s) here.");

    //Doors in the same room as PC
    bool foundDoor = false;
    for(int i=0; i<numDoors; i++) {
        if(doorPtr[i].currentRoomId == roomId) {
            printf("\nThere is an %s door %d going to room %d.", doorPtr[i].isOpen ? "unlocked" : "locked", i, doorPtr[i].nextRoomId);
            foundDoor = true;

            if (i == winningDoorId) {
                printf("\nDoor %d leads to the exit!", i);
            }
        }
    }
    if(!foundDoor) printf("\nThere are no doors here." );

    // creatures that's in the same room as PC
    // Escapees in the same room as PC
    bool foundEscapee = false;
    for (int i = 0; i < numOfEscapees; i++) { // Escapee count is numCreatures - 1
        if (escapeePtr[i].creature.currentRoomId == roomId) {
            printf("\nEscapee %d in the room %d.", i, roomId);
            foundEscapee = true;
        }
    }
    if(!foundEscapee) printf("\nThere are no Escapee(s) in the room.");

    bool foundGuard = false;
    for (int i = 0; i < numOfGuards; i++) { // Guard count is numCreatures - 1
        if (guardPtr[i].creature.currentRoomId == roomId) {
            printf("\nGuard %d in the room %d.", i, roomId);
            foundGuard = true;
        }
    }
    if(!foundGuard) printf("\nThere are no Guard(s) in the room.");
}

//Pickup command - picks up key
void pickupCommand(Player *playerPtr, Key *keyPtr, int numKeys, int keyId) {
    if (keyId > numKeys) {
        printf("Key %d does not exist in this world", keyId);
        return;
    }

    if(playerPtr->creature.currentRoomId == keyPtr[keyId].currentRoomId){
        if (keyPtr[keyId].isPickedUp) {
            printf("\nkey %d is already in your inventory", keyId);
            return;
        }
        if(playerPtr[0].inventorySize < playerPtr[0].carryingCapacityId){
            // set key as picked up and increase inventory size
            keyPtr[keyId].isPickedUp = true;
            playerPtr[0].inventorySize += 1;
            printf("\nPicked up key %d to room %d", keyId, keyPtr[keyId].keyAccessibleDoor);
        } else {
            printf("\nInventory Full!!!");
        }
    } else {
        printf("\nThe Key is not in the room.");
    }
}

//Drop command - drops key in the current room
void dropCommand(Player *playerPtr, Key *keyPtr, int numKeys, int keyId) {
    if (keyId > numKeys) {
        printf("\nKey %d does not exist in this world", keyId);
        return;
    }

    if (keyPtr[keyId].isPickedUp) {
        keyPtr[keyId].isPickedUp = false;
        playerPtr->inventorySize -= 1;
        printf("\nkey %d is dropped from your inventory", keyId);
    } else {
        printf("\nYou do not have key %d in your inventory", keyId);
    }
}

//inventory command - lists the keys the PC is currently carrying
void invCommand(Player *playerPtr, Key *keyPtr, int numKeys) {
    for (int i=0; i<numKeys; i++) {
        if (keyPtr[i].isPickedUp) {
            printf("\nYou are carrying key %d to door %d", i, keyPtr[i].keyAccessibleDoor);
        }
    }
    // a bit different from the actual
    printf("\nYou can pick up %d more items.", (playerPtr->carryingCapacityId) - (playerPtr->inventorySize));
}

//unlock command - unlocks door
void unlockCommand(Player *playerPtr, Key *keyPtr, Door *doorPtr, int doorId, int numKeys) {
    if(doorPtr[doorId].currentRoomId == playerPtr->creature.currentRoomId) {
        if (!doorPtr[doorId].isOpen) {
             for (int i = 0; i < numKeys; i++) {
                 if (keyPtr[i].isPickedUp && (keyPtr[i].keyAccessibleDoor == doorId)){
                     doorPtr[doorId].isOpen = true;
                     printf("\nYou use key %d to unlock door %d", i, doorId);
                     return;
                 }
             }
            printf("\nYou do not have the appropriate key to unlock this door.");
        } else {
            printf("\nDoor %d is already open.", doorId);
        }
    } else {
        printf("\nDoor %d is not in this room.", doorId);
    }
}

//pass command - goes through door
void passCommand(Player *playerPtr, Door *doorPtr, Key *keyPtr, Guard *guardPtr, Escapee *escapeePtr, int doorId, int numDoors, int winningDoorId, int numKeys, int numOfEscapees, int numOfGuards) {
    if (doorId > numDoors - 1 || doorId < 0) { // Check if doorId is within valid range
        printf("\nInvalid door ID. Please enter a door ID between 0 and %d.\n", numDoors - 1);
        return;
    }

    if(doorPtr[doorId].currentRoomId == playerPtr->creature.currentRoomId) {
        if (doorPtr[doorId].isOpen) {
            if(doorId == winningDoorId) {   //if door is open and winningDoor you win end game and clear heap
                printf("\nYou go through door %d and you are outside. Congratulations, you won!!!", doorId);
                freeEverythingAndEndGame(playerPtr,doorPtr, keyPtr, guardPtr, escapeePtr);
            }else { //if door is open and not winning you advance to new room AKA updating current room ID
                playerPtr->creature.currentRoomId = (doorPtr[doorId].currentRoomId == playerPtr->creature.currentRoomId)
                                                    ? doorPtr[doorId].nextRoomId
                                                    : doorPtr[doorId].currentRoomId;
                //conditional statement thing: IF PC.currentRoomId is = Door.currentRoomId with door return next room if not return current

                for (int i=0; i<numKeys; i++) {
                    // make picked up key follow the player
                    if (keyPtr[i].isPickedUp) {
                        keyPtr[i].currentRoomId = playerPtr->creature.currentRoomId;
                    }
                }

                for (int i=0; i<numOfEscapees; i++) {
                    // make followed escapee up key follow the player
                    if (escapeePtr[i].isLeadByPlayer) {
                        escapeePtr[i].creature.currentRoomId = playerPtr->creature.currentRoomId;
                    }
                }
                for (int i=0; i<numOfGuards; i++) {
                    // make followed guard up key follow the player
                    if (guardPtr[i].isFollowingTarget) {
                        guardPtr[i].creature.currentRoomId = playerPtr->creature.currentRoomId;
                    }
                }

                //check has to be after guards/escapees rooms are updated
                if(checkMethod(playerPtr, guardPtr, doorPtr, keyPtr, escapeePtr, numOfGuards, numOfEscapees)) {
                    return;
                }

                // swap room id of door when player passes it
                int temp = doorPtr[doorId].currentRoomId;
                doorPtr[doorId].currentRoomId = doorPtr[doorId].nextRoomId;
                doorPtr[doorId].nextRoomId = temp;

                printf("\nYou go through door %d and enter room %d.", doorId, playerPtr->creature.currentRoomId);
            }
        } else printf("\nDoor %d is locked. You can't go through it.", doorId);
    }else {     //door is locked dont proceed anywhere
        printf("\nDoor %d is not in this room.", doorId);
    }
}

//lead command - escapee starts following the PC
void leadCommand(Player *playerPtr, Escapee *escapeePtr, int numOfEscapees, int escapeeId) {
    if (escapeeId > numOfEscapees) {
        printf("\nEscapee %d does not exist in this world", escapeeId);
        return;
    }

    if(escapeePtr[escapeeId].creature.currentRoomId == playerPtr->creature.currentRoomId) {
        if (playerPtr->leadershipSize < playerPtr->leadership) {
            escapeePtr[escapeeId].isLeadByPlayer = true;
            printf("\nEscapee %d is now lead by player.", escapeeId);
        } else {
            printf("\nYou have reached your leadership limit!");
        }
    } else {
        printf("\nEscapee %d is not in this room.", escapeeId);
    }
}

//abandon command - escapee stops following the PC
void abandonCommand(Player *playerPtr, Escapee *escapeePtr, int numOfEscapees, int escapeeId) {
    if(escapeeId > numOfEscapees) {
        printf("\nEscapee %d does not exist in this world", escapeeId);
        return;
    }

    if (escapeePtr[escapeeId].isLeadByPlayer) {
        escapeePtr[escapeeId].isLeadByPlayer = false;
        playerPtr->leadershipSize -= 1;
        printf("\nEscapee %d is no longer following you", escapeeId);
    } else {
        printf("\nEscapee %d was never following you.", escapeeId);
    }
}

void caughtMethod(Player *playerPtr, Door *doorPtr, Key *keyPtr, Guard *guardPtr, Escapee *escapeePtr, int numOfGuards, int numOfEscapees){
    numOfTimesCaught += 1;
    if(numOfTimesCaught < 5) {
        printf("\nRoom %d has at least as many guards as escapees in it now.", playerPtr->creature.currentRoomId);
        resetLocations(playerPtr, guardPtr, escapeePtr, numOfGuards, numOfEscapees);
        printf("\nYou have been caught %d already. Careful!", numOfTimesCaught);
    } else if (numOfTimesCaught == 5) {
        printf("\nYou have been caught 5 times! You Lose, Game Over!");
        freeEverythingAndEndGame(playerPtr,doorPtr, keyPtr, guardPtr, escapeePtr);
    }
}

void resetLocations(Player *playerPtr, Guard *guardPtr, Escapee *escapeePtr, int numOfGuards, int numOfEscapees){
    playerPtr->creature.currentRoomId = playerPtr->creature.originalLocation;   // player returned to original location

    printf("\nYou were caught! Back to your starting location.");

    for (int i=0; i<numOfGuards; i++) {
        guardPtr[i].creature.currentRoomId = guardPtr[i].creature.originalLocation; // iterates through guards to return each to orginal location
    }

    for (int i=0; i<numOfEscapees; i++) {
        escapeePtr[i].creature.currentRoomId = escapeePtr[i].creature.originalLocation; // iterates through escapees to return each to orginal location
    }
}

bool checkMethod(Player *playerPtr, Guard *guardPtr, Door *doorPtr, Key *keyPtr,  Escapee *escapeePtr, int numOfGuards, int numOfEscapees){
   int roomId = playerPtr->creature.currentRoomId; //where the PC is currently located
   int numOfEscapeesInRoom = 1;
   int numOfGuardsInRoom = 0;
   for (int i = 0; i < numOfEscapees; i++) {
        if(escapeePtr[i].creature.currentRoomId == roomId) {
            numOfEscapeesInRoom += 1;
        }
   }
   for (int i = 0; i < numOfEscapees; i++) {
        if(guardPtr[i].creature.currentRoomId == roomId) {
           numOfGuardsInRoom += 1;
        }
   }
   if(numOfGuardsInRoom >= numOfEscapeesInRoom) {
       caughtMethod(playerPtr, doorPtr, keyPtr, guardPtr, escapeePtr, numOfGuards, numOfEscapees);
       return true;
   }else{
       if(numOfGuardsInRoom >= 1 && numOfGuardsInRoom < numOfEscapeesInRoom) {
           printf("\nRoom %d has more escapees (including you) than guards. The guards are waiting for you to make a mistake.", roomId);
       }
       for(int i = 0; i<numOfGuards; i++) {
           if(guardPtr[i].creature.currentRoomId == roomId) {
               int targetId = guardPtr[i].targetCreatureId;
               for(int j = 0; j<numOfEscapees; j++) {
                   if(escapeePtr[j].creature.currentRoomId == roomId && targetId - 1 == j) {
                       guardPtr[i].isFollowingTarget = true;
                       printf("\nGuard %d starts following escapee %d.", i, j);
                   }
               }
           }
       }
       return false;
   }
}

void freeEverythingAndEndGame(Player *playerPtr, Door* doorPtr, Key *keyPtr, Guard *guardPtr, Escapee *escapeePtr) {
    // freeing allocated memory
    free(doorPtr);
    free(keyPtr);
    free(guardPtr);
    free(escapeePtr);


    printf("\nSAYONARA!!!\n");
    exit(0);
}