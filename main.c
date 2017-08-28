/**
 *                       <Práticas de Programação>
 *                             Eduardo Porto
 *                              <codeGame>
 *                              
 *
 *                      Objetivo: Um jogo educativo para crianças aprenderem a programar.
 *                      Tempo: 2 semanas
 *                      Resultado: Uma galinha que atira bola de fogo e vira uma poça de sangue ao tocar em um inimigo.
 */

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "gameObjects.h"
#include "commands.h"

/* idkfa: all weapons
   iddqd: god mode
   konami: up up down down left right left right left right B A Start
   there is now cow level
   allyourbasearebelongtous
   justin bailey
*/

//==============================
//GLOBAIS                                          
//============================== 
const int MAX_TEXT_INPUT = 100;
const int HEIGHT         = 600; 
const int WIDTH          = 800;
const int MAP_WIDTH      = 23;
const int MAP_HEIGHT     = 16;
const int FLOOR_TILES    = 6;
const int WALL_TILES     = 7;
const int MAX_SPELLS     = 3;   //Os tipos de magia diferente, PORÉM SOMOS POBRES E SÓ TEMOS DINHEIRO PRA UMA BOLA DE FOGO. (Update: PARECE QUE AGORA TEM SETA DE GELO E SETA DE VENENO (isso faz sentido? lul))

Queue *commandList;
TPlayer player;
Tile tileFloor[1];
Tile tileWall[12];
int gameState = -1; //Init State == NULL 

//GLOBAL "DATABASES"
ALLEGRO_BITMAP *GAME_OBJECTS[10];
ALLEGRO_BITMAP *GAME_MONSTERS[5];
ALLEGRO_BITMAP *GAME_SPELLS[3][4]; //Database global com todas as spells possiveis do jogo, sendo assim se algum monstro for ter a mesma spell do personagem eu não preciso ter que criar tudo de novo também pra ele.
ALLEGRO_BITMAP *GAME_ICON = NULL;

//GLOBAL DEBUGS
int textCount;
ALLEGRO_BITMAP *ColBox    = NULL;

//flags (1 = True | 0 = False)
int drawGridLines_flag = 0;     
int drawCollisionBox_flag = 0;
int drawMapFog = 1;
int debugMode = 0;

//==============================
//PROTOTYPES                                          
//==============================
int init(ALLEGRO_DISPLAY **display, const char *title);
void init_timer_events(ALLEGRO_TIMER **Timer, ALLEGRO_EVENT_QUEUE **event_queue, ALLEGRO_DISPLAY *display, int fps);
void getKeyInput(ALLEGRO_EVENT event, char *text);
void updatePlayer(TPlayer *player, int mapObjects[17][24], int mapCollision[16][24], int mapMonsters[17][24]);
void movePlayer(TPlayer *player,int mapCollision[16][24]);
void initPlayer(TPlayer *player, int character_type, ALLEGRO_BITMAP *sprite);
void drawCommandText (ALLEGRO_FONT *font, char *text);
void drawGridLines();
void drawCollisionBox(int mapCollision[16][24]);
void drawMap(int map[MAP_HEIGHT][MAP_WIDTH]);
void drawMapMonsters(int mapEnemies[MAP_HEIGHT][MAP_WIDTH]);
void drawMapObjects(int mapObjects[17][24]);
void initSpells(TSpell spellBook[]);
void initBloodEffect(TBloodEffect *effect);
void drawSpells(TPlayer *player, TSpell spellBook[]);
void updateSpells(TPlayer *player, TSpell spellBook[],int mapCollision[16][24]);
void checkMapTraps(TPlayer *player, int mapCollision[16][24]);
void drawPlayerDeath(TPlayer *player, TBloodEffect *bloodEffect);
void checkSpellHit(TSpell spellBook[], int mapMonsters[16][24]);
void updateMapMechanics(TPlayer *player, int mapCollision[17][24], int mapObjects[17][24], int mapMonsters[17][24],int mapExploration[17][24], int mapBackground[16][23]);
void drawPlayerInventory(TPlayer *player);
void drawMapExploration(int mapExploration[17][24], ALLEGRO_BITMAP *sprUnseen);
void changeState(int *gameState, int newState);
void drawGameMenu(ALLEGRO_FONT *titleFont, ALLEGRO_FONT *menuItemFont, ALLEGRO_FONT *menuBackgroundFont, int menuItem);
void drawGameOver(TPlayer *player, ALLEGRO_FONT *font);


int main(void) {
    //==============================
    //VARIÁVEIS ALLEGRO
    //==============================
    ALLEGRO_TIMER *timer                = NULL;
    ALLEGRO_DISPLAY *display            = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue    = NULL;
    
    ALLEGRO_FONT *libMonof              = NULL;
    ALLEGRO_FONT *titleMenuFont         = NULL;
    ALLEGRO_FONT *menuItemFont          = NULL;
    ALLEGRO_FONT *menuBackgroundFont    = NULL;

    ALLEGRO_BITMAP *playerSprite        = NULL;
    ALLEGRO_BITMAP *floor0              = NULL;
    ALLEGRO_BITMAP *wall0               = NULL;
    ALLEGRO_BITMAP *wall1               = NULL;
    ALLEGRO_BITMAP *wall2               = NULL;
    ALLEGRO_BITMAP *wall3               = NULL;
    ALLEGRO_BITMAP *wall4               = NULL;
    ALLEGRO_BITMAP *wall5               = NULL;
    ALLEGRO_BITMAP *wall6               = NULL;
    ALLEGRO_BITMAP *wallUndead          = NULL;
    ALLEGRO_BITMAP *wallEye             = NULL;
    ALLEGRO_BITMAP *wallExit            = NULL;
    ALLEGRO_BITMAP *wallStoneBlack      = NULL;
    ALLEGRO_BITMAP *bloodPool           = NULL;
    ALLEGRO_BITMAP *sprUnseen           = NULL;
    
    ALLEGRO_BITMAP *objKey              = NULL;
    ALLEGRO_BITMAP *objTrap             = NULL;
    ALLEGRO_BITMAP *objChest            = NULL;
    ALLEGRO_BITMAP *objDoor             = NULL; 
    ALLEGRO_BITMAP *objOpenDoor         = NULL;
    ALLEGRO_BITMAP *objPortal           = NULL;
    ALLEGRO_BITMAP *altarOff            = NULL;
    ALLEGRO_BITMAP *altarOn             = NULL;
    
    //ALLEGRO_BITMAP *fireball[4];
    ALLEGRO_BITMAP *bloodAnimation[11];
    
    ALLEGRO_BITMAP *monsKobold          = NULL;
    ALLEGRO_BITMAP *monsDragon          = NULL;
    ALLEGRO_BITMAP *monsSkeletonBat     = NULL;
    ALLEGRO_BITMAP *monsEye             = NULL;


    
    //==============================
    //VARIÁVEIS JOGO
    //==============================
    enum KEYS {u,d};
    int keys[2] = {0,0};
    int gameOver = 0;
    int gameTime = 0;
    int gameFPS = 60; //60 fps = 1/60
    int frames = 0;
    int render = 1;
    TBloodEffect bloodEffect;
    int menuItem = 0;
    int gambiarra = 0;
    char textInput[100];

    int gameMapDrawBackground[16][23] = {
                                            {10,10,10,10,10,10,2,10,10,10,10,10,10,10,10,9,10,10,10,10,10,10,10}, 
                                            {9,14, 0, 0, 0,10, 0, 8, 9, 8, 9, 0, 9, 8, 9, 9, 9, 8, 8, 9, 8, 9, 9},
                                            {8, 9, 9, 9, 8, 9, 0, 9, 8, 9, 8, 0, 8, 9, 9, 0, 0, 0, 0, 0, 0, 9, 9},   
                                            {9, 0, 0, 9, 8, 8, 0, 9, 0, 0, 0, 0, 0,14 ,0 ,0 ,0 ,0 ,0, 0, 0,10,10}, 
                                            {9, 0, 0, 9, 9, 8, 0, 8, 8, 8, 0, 0, 8, 9, 8,10, 0, 0, 0, 0, 0, 9,10},
                                            {8, 0, 0, 9, 8, 8, 0, 0, 0, 0, 0, 0, 9, 8, 8, 8, 8, 9, 8, 9, 0,10, 9},
                                            {9, 8, 9, 8, 0, 0, 0, 0, 8, 7, 8, 8, 8, 7, 9, 8, 9, 8, 9, 8, 0, 8, 9},
                                            {8, 7, 8, 8, 0, 0, 0, 0, 9, 8, 8, 9, 8, 8, 9, 7, 8, 9, 8, 9, 0, 9, 9},
                                            {8, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,14, 0, 0, 0, 9, 9},
                                            {9, 0, 0, 8, 7, 8, 0, 8, 9, 8, 9, 8, 8, 9, 0, 9, 9, 0, 0, 0, 0,10,10},
                                            {8, 0, 0, 7, 8,10, 0,10, 9, 8, 9, 8, 8,10, 0, 9, 9, 0, 0, 9,10,10,10},
                                            {7, 0, 0, 8, 8, 0, 0, 0, 8, 7, 8, 0, 0, 0, 0, 9, 9, 0, 0, 9, 8, 9, 9},
                                            {8, 0, 0, 0, 0, 0, 0, 0, 9, 7, 8, 7, 9, 8, 9, 8, 7, 0,0, 9, 9,10,10},
                                            {8,10, 9, 9,10, 0, 0, 0, 8, 8, 9, 8, 9, 7, 8, 9, 9, 0,0, 0, 0, 0,10},
                                            {0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,10, 9, 9, 8, 8, 0, 0, 0, 0, 0, 0},
                                            {9, 9, 9, 9, 9, 9, 8, 8, 7, 9, 7, 8, 7, 8, 9, 9, 9, 7, 9, 8, 9, 9, 9}
                                         };
    int gameMapCollision[17][24] = {
                                            {1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                                            {1,1,0,0,0,0,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1},
                                            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1},   
                                            {1,1,0,0,1,1,1,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,1}, 
                                            {1,1,0,0,1,1,1,0,1,1,1,0,0,1,1,1,1,0,0,0,0,0,1,1},
                                            {1,1,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,1,1},
                                            {1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1},
                                            {1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1},
                                            {1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1},
                                            {1,1,0,0,1,1,1,0,1,1,1,1,1,1,1,0,1,1,0,0,0,0,1,1},
                                            {1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,0,1,1,1,1},
                                            {1,1,0,0,1,1,0,0,0,1,1,1,0,0,0,0,1,1,0,0,1,1,1,1},
                                            {1,1,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1},
                                            {1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1},
                                            {1,1,0,0,0,0,0,0,0,1,0,0,0,1,1,1,1,1,0,0,0,0,0,1},
                                            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
                                         };

    int gameMapDrawObjects[17][24] = {
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,2,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,2,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0},   
                                            {0,0,0,0,0,0,0,0,0,6,2,0,0,0,2,0,0,0,0,0,0,0,0,0}, 
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,1,0,0,0,0,0,0,5,0,5,0,5,0,0,5,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,2,0,0,0,0,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0,7},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
                                         };

    int gameMapMonsters[17][24] = {
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,2,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},   
                                            {0,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0}, 
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,5,0,5,0,5,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
                                         };
    
    int gameMapExploration[17][24] = {
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},   
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
                                         };

    //Inicializa Allegro & Addons e seta propriedades da janela
    if(!init(&display,"Code Game !")) {
        return -1;
    }

    //==============================
    //INIT VARIÁVEIS ALLEGRO
    //==============================
    //libMonof = al_load_font("/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf", 16, 1);
    libMonof = al_load_font("./fonts/PrStartk.ttf",10,1);
    titleMenuFont = al_load_font("./fonts/pdark.ttf",42,1);
    menuBackgroundFont = al_load_font("./fonts/Inconsolata-Regular.ttf",16,1);
    menuItemFont = al_load_font("./fonts/FSEX300.ttf",18,1); //fixedsys, prstartk

    //Personagem
    playerSprite = al_load_bitmap("./sprites/galinha.png");

    //Animação de morte (Game over)    
    //PS.: Ana me tirou 2 pontos por isso. WORTH IT? FUCK YEAH
    bloodEffect.bloodSprite[0] = al_load_bitmap("./sprites/utility/blood1.png");
    bloodEffect.bloodSprite[1] = al_load_bitmap("./sprites/utility/blood2.png");
    bloodEffect.bloodSprite[2] = al_load_bitmap("./sprites/utility/blood3.png");
    bloodEffect.bloodSprite[3] = al_load_bitmap("./sprites/utility/blood4.png");
    bloodEffect.bloodSprite[4] = al_load_bitmap("./sprites/utility/blood5.png");
    bloodEffect.bloodSprite[5] = al_load_bitmap("./sprites/utility/blood6.png");
    bloodEffect.bloodSprite[6] = al_load_bitmap("./sprites/utility/blood7.png");
    bloodEffect.bloodSprite[7] = al_load_bitmap("./sprites/utility/blood8.png");
    bloodEffect.bloodSprite[8] = al_load_bitmap("./sprites/utility/blood9.png");
    bloodEffect.bloodSprite[9] = al_load_bitmap("./sprites/utility/blood10.png");
    bloodEffect.bloodSprite[10] = al_load_bitmap("./sprites/utility/blood11.png");
    
    //Objetos 
    objKey = al_load_bitmap("./sprites/objects/key.png");
    objTrap = al_load_bitmap("./sprites/objects/dngn_trap_bolt.png");
    objChest = al_load_bitmap("./sprites/objects/chest.png");
    objDoor = al_load_bitmap("./sprites/objects/dngn_closed_door.png");
    objOpenDoor = al_load_bitmap("./sprites/objects/dngn_open_door.png");
    objPortal = al_load_bitmap("./sprites/objects/dngn_portal.png");
    altarOn = al_load_bitmap("./sprites/objects/dngn_altar_xom3.png");
    altarOff = al_load_bitmap("./sprites/objects/dngn_altar_xom1.png");

    //Database de Magias
    GAME_SPELLS[SPELL_FIREBALL][DIR_RIGHT] = al_load_bitmap("./sprites/fireballRight.png");
    GAME_SPELLS[SPELL_FIREBALL][DIR_LEFT] = al_load_bitmap("./sprites/fireballLeft.png");
    GAME_SPELLS[SPELL_FIREBALL][DIR_UP] = al_load_bitmap("./sprites/fireballUp.png");
    GAME_SPELLS[SPELL_FIREBALL][DIR_DOWN] = al_load_bitmap("./sprites/fireballDown.png");

    //Estrutura do mapa (Chão, paredes e portas)
    floor0 = al_load_bitmap("./sprites/ground/grey_dirt3.png");
    wallUndead = al_load_bitmap("./sprites/wall/undead1.png");
    wallExit = al_load_bitmap("./sprites/wall/dngn_enter_zot_open.png");
    wallStoneBlack = al_load_bitmap("./sprites/wall/stone_black_marked6.png");
    wallEye = al_load_bitmap("./sprites/wall/relief3.png");
    wall0 = al_load_bitmap("./sprites/wall/wall_vines0.png");
    wall1 = al_load_bitmap("./sprites/wall/wall_vines1.png");
    wall2 = al_load_bitmap("./sprites/wall/wall_vines2.png");
    wall3 = al_load_bitmap("./sprites/wall/wall_vines3.png");
    wall4 = al_load_bitmap("./sprites/wall/wall_vines4.png");
    wall5 = al_load_bitmap("./sprites/wall/wall_vines5.png");
    wall6 = al_load_bitmap("./sprites/wall/wall_vines6.png");
    bloodPool = al_load_bitmap("./sprites/wall/dngn_blood_fountain.png");


    //Fog
    sprUnseen = al_load_bitmap("./sprites/utility/dngn_unseen.png");

    //Monsteros
    monsKobold = al_load_bitmap("./sprites/monsters/big_kobold.png");
    monsDragon = al_load_bitmap("./sprites/monsters/dragon.png");
    monsSkeletonBat = al_load_bitmap("./sprites/monsters/skeleton_bat.png");
    monsEye = al_load_bitmap("./sprites/monsters/eye_of_draining.png");
    
    //Debug colisão
    ColBox = al_load_bitmap("./sprites/utility/travel_exclusion_centre.png");

    //==============================
    //INIT VARIÁVEIS JOGO
    //==============================
    commandList = createQueue();
    
    initPlayer(&player,CHAR_GALINHA, playerSprite);
    initBloodEffect(&bloodEffect);

    tileFloor[0].tileSprite = floor0;
    tileWall[0].tileSprite  = wall0;
    tileWall[1].tileSprite  = wall1;
    tileWall[2].tileSprite  = wall2;
    tileWall[3].tileSprite  = wall3;
    tileWall[4].tileSprite  = wall4;
    tileWall[5].tileSprite  = wall5;
    tileWall[6].tileSprite  = wall6;
    tileWall[7].tileSprite  = bloodPool;
    tileWall[8].tileSprite  = wallUndead;
    tileWall[9].tileSprite  = wallExit;
    tileWall[10].tileSprite = wallStoneBlack;
    tileWall[11].tileSprite = wallEye;

    GAME_MONSTERS[MONS_KOBOLD] = monsKobold;
    GAME_MONSTERS[MONS_DRAGON] = monsDragon;
    GAME_MONSTERS[MONS_SKELETON_BAT] = monsSkeletonBat;
    GAME_MONSTERS[MONS_EYE] = monsEye;
    GAME_MONSTERS[MONS_TRAP] = objTrap;

    GAME_OBJECTS[OBJ_KEY] = objKey;
    GAME_OBJECTS[OBJ_TRAP] = objTrap; 
    GAME_OBJECTS[OBJ_CHEST] = objChest; 
    GAME_OBJECTS[OBJ_PORTAL] = objPortal;   
    GAME_OBJECTS[OBJ_DOOR] = objDoor; 
    GAME_OBJECTS[OBJ_OPEN_DOOR] = objOpenDoor; 
    GAME_OBJECTS[OBJ_ALTAR] = altarOff;
    GAME_OBJECTS[OBJ_ALTAR2] = altarOn;

    memset(textInput, '\0', sizeof(textInput));

    //=============================
    //GAME STATE INIT
    //==============================
    changeState(&gameState, GAME_TITLE);

    //=============================
    //ALLEGRO TIMER & EVENT QUEUE
    //==============================
    init_timer_events(&timer,&event_queue,display,gameFPS);
    
    //O ideal é startar o timer uma linha antes do game loop
    al_start_timer(timer); 
    while (!gameOver) {
        ALLEGRO_EVENT e;
        al_wait_for_event(event_queue, &e); //Jogo para enquanto espera por event
    
        if (e.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            gameOver = 1;
            printf("Exiting...\n");
        }

        //===[ MENU KEY EVENTS ]===
        if(gameState == GAME_TITLE) {
            if(e.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch(e.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:
                        menuItem++;
                        break;
                    case ALLEGRO_KEY_DOWN:
                        menuItem--;
                        break;
                    case ALLEGRO_KEY_ESCAPE:
                        gameOver = 1;
                        printf("Exiting...\n");
                        break;
                    case ALLEGRO_KEY_ENTER:
                        if(menuItem == 0) {
                            changeState(&gameState, GAME_PLAYING);
                        }
                        else
                            gameOver = 1;
                        break;
                }
                if (menuItem > 1)
                    menuItem = 0;
                else if (menuItem < 0)
                    menuItem = 1;

            }

        }

        //===[ IN GAME KEY EVENTS ]===
        else if(gameState == GAME_PLAYING) {
            if(gambiarra == 0) {
                al_wait_for_event(event_queue, &e);
                gambiarra = 1;
            }
            getKeyInput(e,textInput); 
            if(e.type == ALLEGRO_EVENT_KEY_DOWN) {
                if(e.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    gameOver = 1;
                    printf("Exiting...\n");
                }
                        
                //COMMANDS EXECUTE
                if(e.keyboard.keycode == ALLEGRO_KEY_F1 || e.keyboard.keycode == ALLEGRO_KEY_F2 || e.keyboard.keycode == ALLEGRO_KEY_F5) { //Run code
                    parseTextInput(textInput);
                    memset(textInput, '\0', sizeof(textInput));
                }
            }

        }
        else if(gameState == GAME_WIN) {
            if(e.type == ALLEGRO_EVENT_KEY_DOWN) {
                if(e.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    gameOver = 1;
                    printf("Exiting...\n");
                }
            }
        }
 
         //=============================
         //GAME UPDATE
         //=============================
         if (e.type == ALLEGRO_EVENT_TIMER) {
           /* posy -= keys[u] * 10;
            posy += keys[d] * 10;*/
            /* FPS
             * - Armazeno a quantidade de vezes (frames) que o Timer é chamado.
             * - Sempre que passar 1s eu salvo a quantidade de frames até então na variável gameFPS.
             * - Reseto (0) a quantidade de frames para que possa refazer todo o processo.
             * - O gameFPS tem exatamente a quantidade de vezes que o jogo foi redesenhado em 1s. O ideal é ser 60
             *   menos que isso significa algum problema no Allegro ou computador.
             */
            frames++;                               // Quantidade de vezes que o timer foi chamado.
            if(al_current_time() - gameTime >= 1) { //Verifica se já passou 1s
                gameTime = al_current_time();
                gameFPS = frames;
                frames = 0;
            }

            if(gameState == GAME_TITLE) {
                //updateMenu(&menuItem);
            }
            else if (gameState == GAME_PLAYING) {
                checkSpellHit(player.spellBook, gameMapMonsters);
                if(!player.isDeadYet) {
                    updateMapMechanics(&player, gameMapCollision, gameMapDrawObjects, gameMapMonsters, gameMapExploration, gameMapDrawBackground);
                    updatePlayer(&player, gameMapDrawObjects, gameMapCollision, gameMapMonsters);
                    updateSpells(&player, player.spellBook, gameMapCollision);
                    checkMapTraps(&player, gameMapMonsters); 
                }
             }
             else if(gameState == GAME_WIN) {} 
             else if(gameState == GAME_LOST) {} 

            render = 1; //Indico que ao programa que já posso Redesenhar
         }

        //=============================
        //GAME RENDER
        //=============================
        /*
         *  Isso faz com que o jogo será renderizado somente a cada 60s.
         *  O motivo que o código não está dentro do event ALLEGRO_EVENT_TIMER é que é uma boa prática não deixar
         *  tanto código dentro dos manipuladores de events, já que, uma vez que a execução esteja em um desses os outros terão
         *  que ficar esperando, fora que o processo de renderização de um jogo é a etapa que mais demanda tempo.
         *   
         *   al_is_event_queue_empty(event_queue): Só irá renderizar se não tiver nenhum event pendente
         */
        if(render && al_is_event_queue_empty(event_queue)) {
            render = 0;
            
            if(gameState == GAME_TITLE) {
                drawGameMenu(titleMenuFont, menuItemFont,menuBackgroundFont, menuItem);
            }
            else if (gameState == GAME_PLAYING) {
                /* MAPAS */
                drawMap(gameMapDrawBackground);         //Mapa
                drawMapMonsters(gameMapMonsters);       //Monstros
                drawMapObjects(gameMapDrawObjects);     //Objetos
                if (drawMapFog) drawMapExploration(gameMapExploration, sprUnseen); //Fog (Exploração)
                drawPlayerInventory(&player);           //Inventorio do jogador

                /* Textos */
                //al_draw_textf(libMonof, al_map_rgb(255, 0, 255),0,60, 0, "Text Count: %d", textCount);
                al_draw_text(libMonof, al_map_rgb(255, 0, 0),WIDTH-150,0, 0, "Player {");
                al_draw_textf(libMonof, al_map_rgb(255, 0, 0),WIDTH-140,10, 0, "posX: %d",player.posX); 
                al_draw_textf(libMonof, al_map_rgb(255, 0, 0),WIDTH-140,25, 0, "posY: %d",player.posY);
                if(player.dragonSlayer) {
                    al_draw_text(libMonof, al_map_rgb(255, 255, 0),WIDTH-140,40, 0, "Slayer");
                    al_draw_text(libMonof, al_map_rgb(255, 0, 0),WIDTH-150,50, 0, "}");
                }
                else
                    al_draw_text(libMonof, al_map_rgb(255, 0, 0),WIDTH-150,35, 0, "}");

                /*if(player.direction == DIR_DOWN) al_draw_text(libMonof, al_map_rgb(255, 0, 255),0,20, 0, "Dir: D");
                else if(player.direction == DIR_UP) al_draw_text(libMonof, al_map_rgb(255, 0, 255),0,20, 0, "Dir: U");
                else if(player.direction == DIR_LEFT) al_draw_text(libMonof, al_map_rgb(255, 0, 255),0,20, 0, "Dir: L");
                else if(player.direction == DIR_RIGHT) al_draw_text(libMonof, al_map_rgb(255, 0, 255),0,20, 0, "Dir: R");*/
                al_draw_textf(libMonof, al_map_rgb(255, 0, 0),WIDTH-66, HEIGHT-20, 1, "X: %d , Y: %d", player.mapX, player.mapY); 
                al_draw_textf(libMonof, al_map_rgb(0, 255, 0),WIDTH-60, HEIGHT-40, 1,"Steps: %d", player.steps);
                //al_draw_textf(libMonof, al_map_rgb(0, 255, 0),WIDTH-60, HEIGHT-60, 1,"isAnimate: %d", player.isAnimate);

                /* Personagem & Spells */
                if(!player.isDeadYet) {
                    if(player.dragonSlayer)
	                    al_draw_tinted_bitmap_region(player.sprite, al_map_rgb(255,100,0),player.curFrame * player.frameWidth, player.animationColumn * player.frameHeight, player.frameWidth, player.frameHeight, player.posX, player.posY, 0);
                    else
	                    al_draw_bitmap_region(player.sprite, player.curFrame * player.frameWidth, player.animationColumn * player.frameHeight, player.frameWidth, player.frameHeight, player.posX, player.posY, 0);
                    drawSpells(&player, player.spellBook);
                }
                else 
                    drawPlayerDeath(&player, &bloodEffect); //Morte
        
                //Debug Lines
                if (drawGridLines_flag) drawGridLines();
                if (drawCollisionBox_flag) drawCollisionBox(gameMapCollision);

                al_draw_text(libMonof, al_map_rgb(0, 255, 0), 0,0,0, "FPS"); 
                al_draw_textf(libMonof, al_map_rgb(0, 255, 0),5,15,0, "%d",gameFPS); 
                al_draw_text(libMonof, al_map_rgb(255, 0, 0), 0,HEIGHT-(16*5), 0, ">> ");
                drawCommandText(libMonof,textInput);
                //al_draw_textf(libMonof, al_map_rgb(255,255, 255), 30, HEIGHT-(16*5), 0, "%s",textInput); //Cmd Text Input 
            }
            else if (gameState == GAME_WIN) {
                drawGameOver(&player,libMonof);           
            }
            else if (gameState == GAME_LOST) {}     
            al_flip_display(); //flush back buffer para front buffer(screen)
            al_clear_to_color(al_map_rgb(0,0,0));    
       }
    }
  
   //al_rest(3);
   //=============================
   //LIBERAÇÃO DE OBJETOS
   //=============================
    al_destroy_display(display);            //Destroy: Janela
    al_destroy_event_queue(event_queue);    //Destroy: Event_Queue
    al_destroy_timer(timer);                //Destroy: Main Timer
    al_destroy_font(libMonof);              //Destroy: Font (libMonof,16)
    destroyQueue(commandList);              //Destroy: (Queue) CommandQueue

    //Imagens
    al_destroy_bitmap(playerSprite);
    al_destroy_bitmap(floor0);
    al_destroy_bitmap(wall0);
    al_destroy_bitmap(wall1);
    al_destroy_bitmap(wall2);
    al_destroy_bitmap(wall3);
    al_destroy_bitmap(wall4);
    al_destroy_bitmap(wall5);
    al_destroy_bitmap(wall6);
    al_destroy_bitmap(wallUndead);
    al_destroy_bitmap(wallEye);
    al_destroy_bitmap(wallExit);
    al_destroy_bitmap(wallStoneBlack);
    al_destroy_bitmap(altarOff);
    al_destroy_bitmap(altarOn);
    al_destroy_bitmap(sprUnseen);

    al_destroy_bitmap(objKey);
    al_destroy_bitmap(objTrap);
    al_destroy_bitmap(objChest);
    al_destroy_bitmap(objDoor);
    al_destroy_bitmap(objOpenDoor);
    al_destroy_bitmap(objPortal);

    al_destroy_bitmap(monsKobold);
    al_destroy_bitmap(monsDragon);
    al_destroy_bitmap(monsSkeletonBat);
    al_destroy_bitmap(monsEye);

    return 0; 
}

void changeState(int *gameState, int newState) {
    *gameState = newState;
}

/* P O D R E */
void drawGameMenu(ALLEGRO_FONT *titleFont, ALLEGRO_FONT *menuItemFont, ALLEGRO_FONT *menuBackgroundFont, int menuItem) {
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 0,0,0, "void updateMapMechanics(TPlayer*, int[][], int[][], int[][],int[][], int[][]) {");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 10,15,0, "//Kobold door open");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 10,30,0, "if (mapMonsters[11][7] == 0) {");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 20,45,0, "mapObjects[12][5] = 3;");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 20,60,0, "mapObjects[12][5] = 0;");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 10,75,0, "}");

    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 10,90,0, "//Monster Skeleton_Bat's Range (After door open)");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 10,105,0, "if(mapMonsters[3][20] != 0) {");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 20,120,0, "if((player->mapY == 3 && player->mapX == 20) || (player->mapY == 3 && player->mapX == 19) ||");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 52,135,0, "(player->mapY == 3 && player->mapX == 18)|| (player->mapY == 3 && player->mapX == 17) ||");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 52,150,0, "(player->mapY == 2 && player->mapX == 17)|| (player->mapY == 2 && player->mapX == 18) ) {");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 20,165,0, "player->isDeadYet = 1;");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 20,180,0, "player->isAnimate = 1;");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 10,195,0, "}");

    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 10,210,0, "//If Dragon's door opens, player is killed");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 10,225,0, "if(mapCollision[2][7] != 1 || mapObjects[2][7] != 2){");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 20,240,0, "mapExploration[1][7] = 1;");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 20,255,0, "if(!player->dragonSlayer) {");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 30,270,0, "player->isDeadYet = 1;");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 30,285,0, "player->isAnimate = 1;");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 20,300,0, "}");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 10,315,0, "}");

    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 10,330,0, "//Secret Black Stone Portal");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 10,345,0, "if(player->mapY == 14 && player->mapX == 8) {");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 20,360,0, "if(player->jump && player->direction == DIR_RIGHT) {");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 30,375,0, "player->posX += 64;");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 30,390,0, "player->mapX += 2;");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 30,405,0, "player->jump = 0;");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 20,420,0, "}");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 10,435,0, "}");

    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 10,450,0, "//Game Over");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 10,465,0, "if(player->mapY == 0 && player->mapX == 7) {");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 20,480,0, "if(mapMonsters[1][7] == 0) {");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 30,495,0, "if(player->dragonSlayer) {");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 40,510,0, "printf(\"You win !\n\");");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 30,525,0, "}");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 20,540,0, "}");
    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 10,555,0, "}");

    al_draw_text(menuBackgroundFont, al_map_rgb(0, 255, 0), 0,570,0, "}");

    al_draw_text(titleFont, al_map_rgb(255, 255, 255), WIDTH/2-150,100,0, "CODE GAME"); 
    
    if(menuItem == 0) {
        al_draw_text(menuItemFont, al_map_rgb(255, 255, 0), WIDTH/2-50,200,0, "startGame();");
        al_draw_text(menuItemFont, al_map_rgb(0, 255, 0), WIDTH/2-50,250,0, "Exit(0);");
    } else {
        al_draw_text(menuItemFont, al_map_rgb(0, 255, 0), WIDTH/2-50,200,0, "startGame();");
        al_draw_text(menuItemFont, al_map_rgb(255, 255, 0), WIDTH/2-50,250,0, "Exit(0);");
    }
   
}

void drawGameOver(TPlayer *player, ALLEGRO_FONT *font) {
    al_draw_text(font, al_map_rgb(255,255,255),WIDTH/2-150,200,0,"Congratulations, You Win !");
    if(player->cheater)
        al_draw_text(font, al_map_rgb(255,255,0),WIDTH/2-150,250,0,"Cheater !");
}

void initPlayer(TPlayer *player, int character_type, ALLEGRO_BITMAP *sprite) {
    player->posX = 2 * 32;//WIDTH/2-16;
    player->posY = 14 * 32;//448
    player->speed = 2;
    player->direction = DIR_RIGHT;
    player->steps = 0;
    player->isAnimate = 0; 
    player->isDeadYet = 0; 
    player->state = PLAYER_IDLE;
    player->mapY = 14;
    player->mapX = 2;
    player->pickUpItem[0] = 0;
    player->pickUpItem[1] = 0;
    player->useItem[0] = 0;
    player->useItem[1] = 0;
    memset(player->inventory,0,sizeof(player->inventory));
    player->inventoryCount = 0;
    player->attack = 0;
    player->jump = 0;
    player->dragonSlayer = 0;
    player->cheater = 0;

    /* Spells */
    initSpells(player->spellBook);

    switch(character_type) {
        case CHAR_GALINHA:  //GALINHA É MUITO MAIS DAORA QUE CHICKEN, DSCLP
            player->charType = character_type;
            player->maxFrame = 2;
            player->frameCount = 0;
            player->frameDelay = 5;
	        player->frameWidth = 32;
	        player->frameHeight = 32;
	        player->animationDirection[DIR_UP] = 3;
            player->animationDirection[DIR_DOWN] = 0;
            player->animationDirection[DIR_LEFT] = 1;
            player->animationDirection[DIR_RIGHT] = 2;
            player->animationIdleFrame = 1;
            player->animationColumn = player->animationDirection[DIR_RIGHT];
            player->curFrame = player->animationIdleFrame;
            al_convert_mask_to_alpha(sprite, al_map_rgb(120, 195, 128));
            player->sprite = sprite;
            break;
    }
}

//SE PREPARANDO PRA PERDER 2 PONTOS DE ANA, LUL
void initBloodEffect(TBloodEffect *effect) {
    effect->width = 48;
    effect->height = 48;
    effect->posX = 0;
    effect->posY = 0;
    effect->curFrame = 0;
    effect->frameCount = 0;
    effect->frameDelay = 2;
    effect->curColumn = 0;
    effect->maxColumn = 0;

}

//Aumenta a posição do player até que ele chegue no destino.
void updatePlayer(TPlayer *player, int mapObjects[17][24], int mapCollision[16][24], int mapMonsters[17][24]) {
    //player->mapX = player->posX/32;
    //player->mapY = player->posY/32;
    switch(player->direction) {
        case DIR_UP:
            player->animationColumn = player->animationDirection[DIR_UP];
            break;
        case DIR_DOWN:
            player->animationColumn = player->animationDirection[DIR_DOWN];
            break;
        case DIR_LEFT:
            player->animationColumn = player->animationDirection[DIR_LEFT];
           break;
        case DIR_RIGHT:
            player->animationColumn = player->animationDirection[DIR_RIGHT];
           break;
    }

    if(player-> steps != 0)
        player->isAnimate = 1;
    else {
        player->isAnimate = 0;
        player->curFrame = player->animationIdleFrame;
    }

    if(player->steps > 0) {
        player->state = PLAYER_WALK;
        movePlayer(player,mapCollision);
        if(player->state == PLAYER_IDLE)
            player->steps--;
        
    }

    if(player->isAnimate) { //Tô em fase de animação?
	    if(player->frameCount++ >= player->frameDelay) {   //Delay para não percorrer todas as animações de uma vez
	        player->curFrame += 1;     //
		        if(player->curFrame >= player->maxFrame)
			        player->curFrame = 0;
		
		    player->frameCount = 0;
        }
	}

//PEGAR ITENS
    if(!(player->isAnimate)) {
        if(player->pickUpItem[0]) {
            //printf("Pick: %d, Item: %d\n",player->pickUpItem[0],player->pickUpItem[1]);
            if(player->pickUpItem[1] == mapObjects[player->mapY][player->mapX]) { //É esse item que eu quero
                switch(mapObjects[player->mapY][player->mapX]) {
                    case OBJ_PICK_KEY:
                        printf("Peguei uma chave!\n");
                        player->inventory[player->inventoryCount] = OBJ_PICK_KEY;
                        player->inventoryCount++;
                        break;
                    //default:
                        //printf("Erro tentando pegar item\n");
                }
                mapObjects[player->mapY][player->mapX] = 0;
            } else
                printf("Nada para pegar !\n");
            player->pickUpItem[0] = 0;
            player->pickUpItem[1] = 0;
         }
    }

//USAR ITENS
    if(player->useItem[0]) {
        switch(player->useItem[1]) {
            case OBJ_PICK_KEY:
                if(player->mapY == 11 && player->mapX == 7) {
                    //Primeira porta
                    if(mapCollision[10][7] != 0) {  
                        mapCollision[10][7] = 0;
                        mapObjects[10][7] = 3;
                        player->inventoryCount--;  
                    }
                }
                //Porta do dragão
                else if (player->mapY == 3 && player->mapX == 7) {
                    if(mapCollision[2][7] != 0) {  
                        mapCollision[2][7] = 0;
                        mapObjects[2][7] = 3;
                        player->inventoryCount--;  
                    }
                }
                //Porta do Portal
                else if (player->mapY == 3 && player->mapX == 11) {
                    if(mapCollision[3][10] != 0) {  
                        mapCollision[3][10] = 0;
                        mapObjects[3][10] = 3;
                        player->inventoryCount--;  
                    }
                }
                //Porta do Olho (MONS_EYE)
                else if (player->mapY == 3 && player->mapX == 12) {
                    if(mapCollision[2][12] != 0) {  
                        mapCollision[2][12] = 0;
                        mapObjects[2][12] = 3;
                        player->inventoryCount--;  
                    }
                }
                //Porta da direita (Correta)
                else if (player->mapY == 3 && player->mapX == 13) {
                    if(mapCollision[3][14] != 0) {  
                        mapCollision[3][14] = 0;
                        mapObjects[3][14] = 3;
                        player->inventoryCount--;  
                    }
                }
                break;
        }
        player->useItem[0] = 0;
        player->useItem[1] = 0;
    }

//ATACAR
    if(player->attack) {
        switch(player->direction) {
            case DIR_UP:
                mapMonsters[player->mapY-1][player->mapX] = 0;
                break;
            case DIR_DOWN:
                mapMonsters[player->mapY+1][player->mapX] = 0;
                break;
            case DIR_LEFT:
                mapMonsters[player->mapY][player->mapX-1] = 0;
                break;
            case DIR_RIGHT:
                mapMonsters[player->mapY][player->mapX+1] = 0;
                break;
        }
        player->attack = 0;
    }

//PULAR
    if(player->jump) {
        switch(player->direction) {
            case DIR_UP:
                if(mapMonsters[player->mapY-1][player->mapX] == 0 || mapMonsters[player->mapY-1][player->mapX] == 5) {
                    if(mapCollision[player->mapY-1][player->mapX] == 0 && mapCollision[player->mapY-2][player->mapX] == 0) {
                        player->posY -= 64;
                        player->mapY -= 2;
                    }
                } else {
                    player->posY -= 32;
                    player->mapY -= 1;
                }
                break;
            case DIR_DOWN:
                if(mapMonsters[player->mapY+1][player->mapX] == 0 || mapMonsters[player->mapY+1][player->mapX] == 5) {
                    if(mapCollision[player->mapY+1][player->mapX] == 0 && mapCollision[player->mapY+2][player->mapX] == 0) {
                        player->posY += 64;
                        player->mapY += 2;
}
                } else {
                    player->posY += 32;
                    player->mapY += 1;
                }
                break;
            case DIR_LEFT:
                if(mapMonsters[player->mapY][player->mapX-1] == 0 || mapMonsters[player->mapY][player->mapX-1] == 5) {
                    if(mapCollision[player->mapY][player->mapX-1] == 0 && mapCollision[player->mapY][player->mapX-2] == 0) {
                        player->posX -= 64;
                        player->mapX -= 2;
}
                } else {
                    player->posX -= 32;
                    player->mapX -= 1;
                }
                break;
            case DIR_RIGHT:
                if(mapMonsters[player->mapY][player->mapX+1] == 0 || mapMonsters[player->mapY][player->mapX+1] == 5) {
                    if(mapCollision[player->mapY][player->mapX+1] == 0 && mapCollision[player->mapY][player->mapX+2] == 0) {
                        player->posX += 64;
                        player->mapX += 2;
}
                } else {
                    player->posX += 32;
                    player->mapX += 1;
                }
                break;
        }
        player->jump = 0;
    }
}

//Move 1 (32x32 tile size) casa apenas
void movePlayer(TPlayer *player, int mapCollision[16][24]) { 
    //player->state = PLAYER_WALK;
    switch(player->direction) {
        case DIR_UP:
            if(mapCollision[player->mapY-1][player->mapX] != 1) {
                player->animationColumn = player->animationDirection[DIR_UP];
                player->posY-=player->speed;
                if(player->posY/32 < player->mapY - 1) {
                    player->posY = (player->mapY-1) * 32;
                    player->state = PLAYER_IDLE;
                    player->mapY--;  
                }
            }
            else {
                player->isAnimate = 0;
                player->state = PLAYER_IDLE;
            }
            break;
        case DIR_DOWN:
            if(mapCollision[player->mapY+1][player->mapX] != 1) {
                player->animationColumn = player->animationDirection[DIR_DOWN];
                player->posY+=player->speed;
                if(player->posY/32 >= player->mapY + 1) {
                    player->posY = (player->mapY+1) * 32;
                    player->state = PLAYER_IDLE;
                    player->mapY++;  
                }
            }
            else {
                player->isAnimate = 0;
                player->state = PLAYER_IDLE;
            }
            break;
        case DIR_LEFT:
            if(mapCollision[player->mapY][player->mapX-1] != 1) {
                player->animationColumn = player->animationDirection[DIR_LEFT];
                player->posX-=player->speed;
                if(player->posX/32 < player->mapX - 1) {
                    player->posX = (player->mapX-1) * 32;
                    player->state = PLAYER_IDLE;
                    player->mapX--;          
                }
            }
            else {
                player->isAnimate = 0;
                player->state = PLAYER_IDLE;
            }
           break;
        case DIR_RIGHT:
            if(mapCollision[player->mapY][player->mapX+1] != 1) {
                player->animationColumn = player->animationDirection[DIR_RIGHT];
                player->posX += player->speed;
                if(player->posX/32 >= player->mapX + 1) {
                    player->posX = (player->mapX+1) * 32;
                    player->state = PLAYER_IDLE;
                    player->mapX++;          
                }
            }
            else {
                player->isAnimate = 0;
                player->state = PLAYER_IDLE;
            }
           break;
        default:
            player->state = PLAYER_IDLE;
            printf("Error ao movimentar personagem. {Player Dir: %d}\n",player->direction);
    }

}

void drawPlayerInventory(TPlayer *player) {
    int i;
    for(i = 0; i < player->inventoryCount; i++) {
        al_draw_bitmap(GAME_OBJECTS[i], WIDTH-32,i*32,0);
    }
}

//And this is how i met -2 pts :winky_face:
void drawPlayerDeath(TPlayer *player, TBloodEffect *blood) {
    blood->posX = player->posX-10;
    blood->posY = player->posY-10;
    
    if(player->isAnimate) { 
	    if(blood->frameCount++ >= blood->frameDelay) {   //Delay para não percorrer todas as animações de uma vez
	        blood->curFrame += 1;     //
		        if(blood->curFrame == 10) {
			        blood->curFrame = 0;
                    player->isAnimate = 0;
                }
		
		    blood->frameCount = 0;
        }
	}
    al_draw_bitmap(blood->bloodSprite[blood->curFrame],blood->posX, blood->posY,0); 
}

void initSpells(TSpell spellBook[]) {
  int i;
  for(i=0; i < MAX_SPELLS; i++) {
        if(i == SPELL_FIREBALL) {
            spellBook[i].spellType = SPELL_FIREBALL;
            spellBook[i].speed = 5;
            spellBook[i].spellLeft = 1;
            spellBook[i].isActive = 0; 
            spellBook[i].spellSprite[DIR_RIGHT] = GAME_SPELLS[SPELL_FIREBALL][DIR_RIGHT];
            spellBook[i].spellSprite[DIR_UP] = GAME_SPELLS[SPELL_FIREBALL][DIR_UP];
            spellBook[i].spellSprite[DIR_LEFT] = GAME_SPELLS[SPELL_FIREBALL][DIR_LEFT];
            spellBook[i].spellSprite[DIR_DOWN] = GAME_SPELLS[SPELL_FIREBALL][DIR_DOWN];
       
        }
        else if(i == SPELL_FROSTBOLT) {
            spellBook[i].spellType = SPELL_FROSTBOLT;
            spellBook[i].speed = 2;
            spellBook[i].spellLeft = 0;
            spellBook[i].isActive = 0; 
            spellBook[i].spellSprite[DIR_RIGHT] = GAME_SPELLS[SPELL_FIREBALL][DIR_RIGHT];
            spellBook[i].spellSprite[DIR_UP] = GAME_SPELLS[SPELL_FIREBALL][DIR_UP];
            spellBook[i].spellSprite[DIR_LEFT] = GAME_SPELLS[SPELL_FIREBALL][DIR_LEFT];
            spellBook[i].spellSprite[DIR_DOWN] = GAME_SPELLS[SPELL_FIREBALL][DIR_DOWN];           
        } 
        else if(i == SPELL_POISONBOLT) {
            spellBook[i].spellType = SPELL_POISONBOLT;
            spellBook[i].speed = 10;
            spellBook[i].spellLeft = 1;
            spellBook[i].isActive = 0; 
            spellBook[i].spellSprite[DIR_RIGHT] = GAME_SPELLS[SPELL_FIREBALL][DIR_RIGHT];
            spellBook[i].spellSprite[DIR_UP] = GAME_SPELLS[SPELL_FIREBALL][DIR_UP];
            spellBook[i].spellSprite[DIR_LEFT] = GAME_SPELLS[SPELL_FIREBALL][DIR_LEFT];
            spellBook[i].spellSprite[DIR_DOWN] = GAME_SPELLS[SPELL_FIREBALL][DIR_DOWN];           
        }
        //spellBook[i].mapX = spellBook[i].posX/32; 
        //spellBook[i].mapY = spellBook[i].posY/32;
    }

}

void updateSpells(TPlayer *player, TSpell spellBook[], int mapCollision[16][24]) {
    int i;
    for (i = 0; i < MAX_SPELLS; i++) {
        if(spellBook[i].isActive) {
            switch(player->direction) {
                case DIR_UP:
                   if(mapCollision[spellBook[i].mapY-1][spellBook[i].mapX] != 1) {
                        spellBook[i].posY -= spellBook[i].speed;
                        if(spellBook[i].posY/32 < spellBook[i].mapY - 1) {
                            spellBook[i].mapY--;  
                        }
                    }
                    else {
                        spellBook[i].isActive = 0;
                    }
                    break;
                case DIR_DOWN:
                   if(mapCollision[spellBook[i].mapY+1][spellBook[i].mapX] != 1) {
                        spellBook[i].posY += spellBook[i].speed;
                        if(spellBook[i].posY/32 >= spellBook[i].mapY + 1) {
                            spellBook[i].mapY++;  
                        }
                    }
                    else {
                        spellBook[i].isActive = 0;
                    }
                    break;
                case DIR_LEFT:
                   if(mapCollision[spellBook[i].mapY][spellBook[i].mapX-1] != 1) {
                        spellBook[i].posX -= spellBook[i].speed;
                        if(spellBook[i].posX/32 < spellBook[i].mapX - 1) {
                            spellBook[i].mapX--;  
                        }
                    }
                    else {
                        spellBook[i].isActive = 0;
                    }
                   break;
                case DIR_RIGHT:
                   if(mapCollision[spellBook[i].mapY][spellBook[i].mapX+1] != 1) {
                        spellBook[i].posX += spellBook[i].speed;
                        if(spellBook[i].posX/32 >= spellBook[i].mapX + 1) {
                            spellBook[i].mapX++;  
                        }
                    }
                    else {
                        spellBook[i].isActive = 0;
                    }
                   break;
            } 
        }
       // spellBook[i].mapX = spellBook[i].posX/32; 
       // spellBook[i].mapY = spellBook[i].posY/32;
    }

}


void drawSpells(TPlayer *player, TSpell spellBook[]) {
    int i;
    for (i = 0; i < MAX_SPELLS; i++) {
        if(spellBook[i].isActive) {
            if(spellBook[i].spellType == SPELL_FIREBALL)
                al_draw_bitmap(spellBook[i].spellSprite[player->direction],spellBook[i].posX,spellBook[i].posY,0);
            else if (spellBook[i].spellType == SPELL_POISONBOLT)
                al_draw_tinted_bitmap(spellBook[i].spellSprite[player->direction],al_map_rgb(0,255,0),spellBook[i].posX,spellBook[i].posY,0);
            else if (spellBook[i].spellType == SPELL_FROSTBOLT)
                al_draw_tinted_bitmap(spellBook[i].spellSprite[player->direction],al_map_rgb(0,42,255),spellBook[i].posX,spellBook[i].posY,0);
        }
    }
}

void checkMapTraps(TPlayer *player, int mapMonsters[16][24]) {
    if(!(player->isDeadYet)) {
        if (mapMonsters[player->mapY][player->mapX] != 0) {
            switch(mapMonsters[player->mapY][player->mapX]) {
                case 1:
                    printf("Morreu esmagado por um KOBOLD !\n");
                    break;
                case 2:
                    printf("Morreu pisoteado, queimado, esquartejado e desmembrado por um DRAGÃO. Hue !\n");
                    break;
                case 3:
                    printf("Morreu pelas garras de um ESQUELOCÊGO!\n");
                    break;
                case 8:
                    printf("Morreu pela magia de um ESQUELOCÊGO!\n");
                    break;
                case 4:
                    printf("Morreu pelo encarar de um OLHO DEMONIACO!\n");
                    break;
                case 5:
                    printf("Morreu por uma ARMADILHA previsivel mas mortal !!\n");
                    break;
                default:
                    printf("Morreu pra alguma coisa, só não sei o que. (%d)\n",mapMonsters[player->mapY][player->mapX]);
            }
            player->isDeadYet = 1;
            player->isAnimate = 1; //Chamando a animação no sangue
        }
    }
}

void checkSpellHit(TSpell spellBook[], int mapMonsters[16][24]) {
    int i;
    for (i = 0; i < MAX_SPELLS; i++) {
        if(spellBook[i].isActive) {
            if (mapMonsters[spellBook[i].mapY][spellBook[i].mapX] != 0) {  
                mapMonsters[spellBook[i].mapY][spellBook[i].mapX] = 0;
                printf("Matou 1\n");
            }  
        }
    }
}

int init(ALLEGRO_DISPLAY **display, const char *title) {
    if (!al_init()) {
        printf("Falha ao inicializar o Allegro.\n");
        return 0;
    }

    if (!al_install_keyboard()) {
        printf("Falha ao inicializar o Teclado.\n");
        return 0;
    }

    if (!al_init_image_addon()) {
        printf("Falha ao inicializar o Addon allegro_image.\n");
        return 0;
    }
    
    if (!al_init_font_addon()) {
        printf("Falha ao inicializar o Addon allegro_font.\n");
        return 0;
    }

    if (!al_init_ttf_addon()) {
        printf("Falha ao inicializar o Addon allegro_ttf.\n");
        return 0;
    }
    //ALPHA (Substituir por sprites) //Nem lembro se eu resolvi esse problema, lul
    if (!al_init_primitives_addon()) {
        printf("Falha ao inicializar o Addon allegro_primitives.\n");
        return 0;
    }

    *display = al_create_display(WIDTH, HEIGHT);
    if (!(*display)) {
        printf("Falha ao criar a janela.\n");
        return 0;
    }

    al_set_window_title(*display, title);
    al_set_window_position(*display, 260,100);
    GAME_ICON = al_load_bitmap("./sprites/utility/game_icon.png");
    al_set_display_icon(*display, GAME_ICON);
    return 1;
}

void init_timer_events(ALLEGRO_TIMER **Timer, ALLEGRO_EVENT_QUEUE **event_queue, ALLEGRO_DISPLAY *display, int fps) {
    //Objeto do tipo timer a cada X sec ele dispara um event. al_wat_event é disparado a cada timer(sec)
    *Timer = al_create_timer(1.0 / fps); //timer(speed)
    *event_queue = al_create_event_queue();
    al_register_event_source(*event_queue, al_get_keyboard_event_source());
    al_register_event_source(*event_queue, al_get_display_event_source(display));
    al_register_event_source(*event_queue, al_get_timer_event_source(*Timer));
}

void getKeyInput(ALLEGRO_EVENT event, char *text) {
    if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
        if (strlen(text) < MAX_TEXT_INPUT) {
            char temp[] = {event.keyboard.unichar, '\0'};
            
            if (event.keyboard.unichar >= 'a' && event.keyboard.unichar <= 'z') {
                strcat(text, temp);
            }
            
            else if (event.keyboard.unichar == ' ') {
                strcat(text, temp);
            }

            else if (event.keyboard.unichar >= '0' && event.keyboard.unichar <= '9') {
                strcat(text, temp);
            }

            else if (event.keyboard.unichar >= 'A' && event.keyboard.unichar <= 'Z') {
                strcat(text, temp);
            }

            else if (event.keyboard.unichar == ';') {
                strcat(text, temp);
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                temp[0] = '\n';
                strcat(text,temp);
            }
        }
 
        if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(text) != 0) {
            text[strlen(text) - 1] = '\0';
        }
    }
    textCount = strlen(text);
}

void drawCommandText (ALLEGRO_FONT *font, char *text) {
    int slen = strlen(text);
    int offSety = 5;
    int offSetx = 0;
    int i;
    for (i = 0; i < slen; i++) {
        if(text[i] == '\n'){
            offSety--;
            offSetx = 0;
            continue;
        }
        al_draw_textf(font, al_map_rgb(255,255, 255), 30+offSetx, HEIGHT-(16*offSety), 0, "%c",text[i]); //Cmd Text Input
        offSetx += 10;    
    }

}

//Grid Lines
void drawGridLines() {
    int tileSize = 32;
    int i;
    for (i = 0; i < 25; i++) {
        al_draw_line(tileSize*i, 0, tileSize*i, HEIGHT,al_map_rgb(0,255,0), 0);
    }
    for (i = 0; i < 25; i++) {
        al_draw_line(0, tileSize*i, WIDTH, tileSize*i,al_map_rgb(0,255,0), 0);
    }
}

void drawMap(int map[MAP_HEIGHT][MAP_WIDTH]) {
    int row,col;
    int offSet = 32 * 1;
    for (row = 0; row < MAP_HEIGHT; row++) {
        for (col = 0; col < MAP_WIDTH; col++) {
//FLOORS
            if(map[row][col] == 0) { 
                al_draw_bitmap(tileFloor[0].tileSprite, col*32+offSet,row*32,0);
            }
//WALLS & DOORS
            else if (map[row][col] == 1)
                al_draw_bitmap(tileWall[9].tileSprite, col*32+offSet,row*32,0);
            else if (map[row][col] == 2)
                al_draw_bitmap(tileWall[9].tileSprite, col*32+offSet,row*32,0);
            else if (map[row][col] == 3)
                al_draw_bitmap(tileWall[10].tileSprite, col*32+offSet,row*32,0);
            else if (map[row][col] == 7)
                al_draw_bitmap(tileWall[0].tileSprite, col*32+offSet,row*32,0);
            else if (map[row][col] == 8)
                al_draw_bitmap(tileWall[1].tileSprite, col*32+offSet,row*32,0);
            else if (map[row][col] == 9)
                al_draw_bitmap(tileWall[2].tileSprite, col*32+offSet,row*32,0);
            else if (map[row][col] == 10)
                al_draw_bitmap(tileWall[3].tileSprite, col*32+offSet,row*32,0);
            else if (map[row][col] == 11)
                al_draw_bitmap(tileWall[4].tileSprite, col*32+offSet,row*32,0);
            else if (map[row][col] == 12)
                al_draw_bitmap(tileWall[5].tileSprite, col*32+offSet,row*32,0);
            else if (map[row][col] == 13)
                al_draw_bitmap(tileWall[6].tileSprite, col*32+offSet,row*32,0);
            else if (map[row][col] == 14)
                al_draw_bitmap(tileWall[7].tileSprite, col*32+offSet,row*32,0); 

            else if (map[row][col] == 15)
                al_draw_bitmap(tileWall[8].tileSprite, col*32+offSet,row*32,0);          
        }
    }     
}

void drawCollisionBox(int mapCollision[17][24]) {
    int row,col;
    for (row = 0; row < 16; row++) {
        for (col = 0; col < 24; col++) {
            if(mapCollision[row][col] == 1) {
                al_draw_bitmap(ColBox,col*32,row*32,0);
            }
        }
    }
}

void drawMapObjects(int mapObjects[17][24]) {
    int row,col;
    for (row = 0; row < 16; row++) {
        for (col = 0; col < 24; col++) {
            if(mapObjects[row][col] == 1) {
                al_draw_bitmap(GAME_OBJECTS[OBJ_KEY],col*32,row*32,0);
            }
            else if(mapObjects[row][col] == 2) {
                al_draw_bitmap(GAME_OBJECTS[OBJ_DOOR], col*32,row*32,0);
            }
            else if(mapObjects[row][col] == 3) {
                al_draw_bitmap(GAME_OBJECTS[OBJ_OPEN_DOOR], col*32,row*32,0);
            }
            else if(mapObjects[row][col] == 4) {
                al_draw_bitmap(GAME_OBJECTS[OBJ_CHEST], col*32,row*32,0); //erro aqui
            }
            else if(mapObjects[row][col] == 5) {
                al_draw_bitmap(GAME_OBJECTS[OBJ_TRAP], col*32,row*32,0);
            }
            else if(mapObjects[row][col] == 6) {
                al_draw_bitmap(GAME_OBJECTS[OBJ_PORTAL], col*32,row*32,0);
            }
            else if(mapObjects[row][col] == 7) {
                al_draw_bitmap(GAME_OBJECTS[OBJ_ALTAR], col*32,row*32,0);
            }
            else if(mapObjects[row][col] == 71) {
                al_draw_tinted_bitmap(GAME_OBJECTS[OBJ_ALTAR2], al_map_rgb(255,0,0), col*32,row*32,0);
            }
        }
    }
}

void drawMapMonsters(int mapMonsters[17][24]) {
    int row,col;
    for (row = 0; row < 16; row++) {
        for (col = 0; col < 24; col++) {
            if(mapMonsters[row][col] == 1) {
                al_draw_bitmap(GAME_MONSTERS[MONS_KOBOLD],col*32,row*32,0);
            }
            else if(mapMonsters[row][col] == 2) {
                al_draw_bitmap(GAME_MONSTERS[MONS_DRAGON], col*32,row*32,0);
            }
            else if(mapMonsters[row][col] == 3) {
                al_draw_bitmap(GAME_MONSTERS[MONS_SKELETON_BAT], col*32,row*32,0);
            }
            else if(mapMonsters[row][col] == 4) {
                al_draw_bitmap(GAME_MONSTERS[MONS_EYE], col*32,row*32,0);
            }
        }
    }
}
void drawMapExploration(int mapExploration[17][24], ALLEGRO_BITMAP *sprUnseen) {
    int row,col;
    for (row = 0; row < 16; row++) {
        for (col = 0; col < 24; col++) {
            if(mapExploration[row][col] == 0) {
                al_draw_bitmap(sprUnseen,col*32,row*32,0);
            }
        }
     }
}

//Eventos do jogo
void updateMapMechanics(TPlayer *player, int mapCollision[17][24], int mapObjects[17][24], int mapMonsters[17][24],int mapExploration[17][24], int mapBackground[16][23]) {

//EVENTOS ENVOLVENDO MONSTROS
    //Se eu matar o primeiro Kobold, a porta a esquerda deve abrir
    if(mapMonsters[11][7] == 0) {
        mapObjects[12][5] = 3;
        mapCollision[12][5] = 0;
    }
    //Área com 2 MONS_EYE (Morte na certa)
    if(player->mapY == 5 && (player->mapX == 2 || player->mapX == 3)) {
        player->isDeadYet = 1;
        player->isAnimate = 1;
    }

    //MONS_EYE atrás da porta
    if(player->mapY == 2 && player->mapX == 12) {
        player->isDeadYet = 1;
        player->isAnimate = 1;
    }

    //Entrando na área do MONS_SKELETON_BAT
    //Ele tem um "Range" alto
    if(mapMonsters[3][20] != 0) {
        if((player->mapY == 3 && player->mapX == 20) || (player->mapY == 3 && player->mapX == 19) ||
            (player->mapY == 3 && player->mapX == 18)|| (player->mapY == 3 && player->mapX == 17) ||
            (player->mapY == 2 && player->mapX == 17)|| (player->mapY == 2 && player->mapX == 18) ) {
            player->isDeadYet = 1;
            player->isAnimate = 1;
        }
    }

    //Ao abrir a porta da sala do MONS_SKELETON_BAT revelar o caminho até ele
    if(player->mapY == 3 && player->mapX == 13){
        if(mapCollision[3][14] == 0) {
            mapExploration[3][16] = 1;
            mapExploration[3][17] = 1;
            mapExploration[3][18] = 1;
            mapExploration[3][19] = 1;
            mapExploration[3][20] = 1;
        }   
    }

    //Se a porta do dragão abrir, o player morre independente onde esteja, HUE
    if(mapCollision[2][7] != 1 || mapObjects[2][7] != 2){
        mapExploration[1][7] = 1;
        if(!player->dragonSlayer) {
            player->isDeadYet = 1;
            player->isAnimate = 1;
        }
    }

    
//EVENTOS ENVOLVENDO COLISÕES
    //Portal (Leva pra área dos 2 MONS_EYE)
    if(player->mapY == 3 && player->mapX == 9) {
        player->posX = 64;
        player->posY = 160;
        player->mapY = 5;
        player->mapX = 2;    
    }
    //Portal Secreto (Leva pra área com o Baú)
    if(player->mapY == 14 && player->mapX == 12) {
        player->posX = 160;
        player->posY = 32;
        player->mapY = 1;
        player->mapX = 5;  
       /* player->posX = 640; // final
        player->posY = 448;
        player->mapY = 14;
        player->mapX = 20;  */
    }

    //Jump na pedra do Portal Secreto (Pulando pra direita)
    //É o único lugar onde o jogo deixa você pular por uma parede
    if(player->mapY == 14 && player->mapX == 8) {
        if(player->jump && player->direction == DIR_RIGHT) {
            player->posX += 64;
            player->mapX += 2; 
            player->jump = 0;  
        } 
    }

    //Jump na pedra do Portal Secreto (Pulando pra esquerda)
    //tudo que vai, volta
    else if(player->mapY == 14 && player->mapX == 10) {
        if(player->jump && player->direction == DIR_LEFT) {
            player->posX -= 64;
            player->mapX -= 2; 
            player->jump = 0; 
        } 
    }

    //Portal leva pra frente da porte do dragão
    //Se eu já tiver com o dragonSlayer então o portal pra voltar já apareceu.
    if(player->dragonSlayer) {
        if(player->mapY == 14 && player->mapX == 17) {
            player->posX = 224;
            player->posY = 160;
            player->mapY = 5;
            player->mapX = 7;
        }
    }
            

//Objetivo (Altar)
    if(player->mapY == 14 && player->mapX == 22) {
        if (!player->dragonSlayer) {
            player->dragonSlayer = 1;
            mapBackground[14][16] = 0;
            mapCollision[14][17] = 0;
            mapObjects[14][17] = 6;

            mapObjects[14][23] = 71;
            mapObjects[2][7] = 3;   //A porta do dragão abre
            mapCollision[2][7] = 0;
         }
    }

//Game Win
    //O jogador só ganhará se ele matar o dragão COM o buff do Dragon Slayer, ou seja, se ele não fizer isso é pq roubou,rsrsrsrrs.
    if(player->mapY == 0 && player->mapX == 7) {
        if(mapMonsters[1][7] == 0) {
            if(player->dragonSlayer) {
                printf("You win !\n");
                changeState(&gameState, GAME_WIN);
                
            }
        }
    }

//FOG (FOG LIKE A PRO)
    if(!mapExploration[player->mapY][player->mapX+1])
        mapExploration[player->mapY][player->mapX+1] = 1;
    if(!mapExploration[player->mapY][player->mapX-1])
        mapExploration[player->mapY][player->mapX-1] = 1;
    if(!mapExploration[player->mapY+1][player->mapX])
        mapExploration[player->mapY+1][player->mapX] = 1;
    if(!mapExploration[player->mapY-1][player->mapX])
        mapExploration[player->mapY-1][player->mapX] = 1;
    if(!mapExploration[player->mapY][player->mapX])
        mapExploration[player->mapY][player->mapX] = 1;

/*
    if(!mapExploration[player->mapY][player->mapX+2])
        mapExploration[player->mapY][player->mapX+2] = 1;
    if(!mapExploration[player->mapY][player->mapX-2])
        mapExploration[player->mapY][player->mapX-2] = 1;
    if(!mapExploration[player->mapY+2][player->mapX])
        mapExploration[player->mapY+2][player->mapX] = 1;
    if(!mapExploration[player->mapY-2][player->mapX])
        mapExploration[player->mapY-2][player->mapX] = 1;
    if(!mapExploration[player->mapY][player->mapX])
        mapExploration[player->mapY][player->mapX] = 1;
*/   
}
