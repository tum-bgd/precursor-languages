# Precursor Language Runtime - PLR - Terminal Version

This is an emulator for the Precursor Language Robot.

## Compile

make

## Run Interactive
```
./plr -w <world> -i
```

Use keys
- m to move
- t to turn
- d to deposit
- p to pick



## Run an Assembly File
```
./plr -w <world> -f <assembly file>
```
## File Formats

### World Files

A world file is a text file with an empty line at the end. It contains characters
- a `#` for a blocked location
- * for an item
- X for a goal (Niki exits happily when reaching this goal)
- one of S,W,E,N to place Niki looking South, West, East, North respectively

### Source Code

PL1 is a subset of C, see the script

### Assembly Language

The current robot implements instructions
#### Robot Action
- MOVE
- TURN
- PICK
- DEPO
#### Robot Sensors
- LOADHI
- LOADFB
#### Branch
- JMP
- JZ
- JNZ
#### Control
- HALT
#### Function Calls
- CALL
- RET

The compiler implements a bootloader calling the main function.
