#ifndef COMMANDS_H
#define COMMANDS_H

#define CMD_MAX_CHAR 6 

#define CMD_RESET 0
#define CMD_MOVE 1
#define CMD_ROTATE 2
#define CMD_JUMP 3
#define CMD_CAST 4
#define CMD_USE 5
#define CMD_GET 6
#define CMD_ATK 7
#define CMD_HELP 8
#define CMD_CHEAT 9 //Hehe :^)

#define SPELL_FIREBALL 0
#define SPELL_FROSTBOLT 1
#define SPELL_POISONBOLT 2

#define OBJ_PICK_KEY 1

#define CHEAT_IDKFA 0
#define CHEAT_KONAMI 1
#define CHEAT_JUSTINBAILEY 2
#define CHEAT_OTHERS 3

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "commandQueue.h"
#include "gameObjects.h"

extern Queue *commandList;
extern TPlayer player;
extern const int HEIGHT;
extern const int WIDTH;
extern int drawMapFog;

void addCommand(TCommand *command);
int getCommandParameters(int cmdType);
int getCommandType(char *cmdString);
int getSpellType(char *param);
int getObjectType(char *param);
int getCheatType(char *param);
void parseCommandText(char *s);
void parseTextInput(char *text);
void processCommandType1(int cmdType, char *cmd, int startParams);
void processCommandType0(int cmdType, char *cmd);
void commandMove(TPlayer *player, int steps);
void commandReset(TPlayer *player);
void commandRotate(TPlayer *player, int dir);
void commandCast(TPlayer *player, int spell);
void commandGet(TPlayer *player, int obj);
void commandAttack(TPlayer *player);
void commandJump(TPlayer *player);
void commandUse(TPlayer *player, int obj);
void commandCheat(TPlayer *player, int cheatCode);
void commandHelp();

void* interpretCommands(ALLEGRO_THREAD *thread, void *args);
#endif
