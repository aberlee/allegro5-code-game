#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

typedef enum GAME_STATE {GAME_TITLE,GAME_PLAYING, GAME_WIN, GAME_LOST} TGameState;
typedef enum PLAYER_DIRECTIONS {DIR_RIGHT, DIR_UP, DIR_LEFT, DIR_DOWN} TPlayerDirection;
typedef enum PLAYER_STATES {PLAYER_IDLE, PLAYER_WALK, PLAYER_DEAD} TPlayerState;
typedef enum PLAYER_CHARACTER {CHAR_GALINHA, CHAR_GIRL, CHAR_BOY} TPlayerCharacter;
typedef enum MONSTERS {MONS_KOBOLD, MONS_DRAGON, MONS_SKELETON_BAT,MONS_EYE, MONS_TRAP, MONS_SKELETON} TMonsters;
typedef enum GAME_OBJECTS {OBJ_KEY, OBJ_TRAP, OBJ_CHEST, OBJ_PORTAL, OBJ_DOOR, OBJ_OPEN_DOOR, OBJ_ALTAR,OBJ_ALTAR2} TGameObject;
//typedef enum CHARACTER_STATE {STATE_IDLE = 4} TCharacterState;

typedef struct {
    int mapX;
    int mapY;

    int posX;
    int posY;
    int speed;
    int spellType;
    int isActive;
    int spellLeft;
    

    /* Animações */
    int maxFrame;
	int curFrame;
	int frameCount;
	int frameDelay;
	int frameWidth;
	int frameHeight;
	int animationColumn;
	int animationDirection[4];
    int animationIdleFrame;
    int isAnimate;
    
    ALLEGRO_BITMAP *spellSprite[4];
}TSpell;

typedef struct { 
    /* map matrix */
    int mapX;
    int mapY;

    /* Desenho */
    int posX;           //Posição X do Personagem
    int posY;           //Posição Y do Personagem
    int moveToX;
    int moveToY;
    int speed;          //Velocidade com qual o Personagem se movimenta
    int direction;      //Direção para onde o Personagem está olhando (Modificado através de comandos)
    int steps;          //Quantos "passos" o Personagem irá dar (Setado através de comandos)
    int charType;       //Personagem escolhido
    
    /* Game Logic */
    int isDeadYet;      // :)
    TPlayerState state; //Estado atual do personagem. 
    int pickUpItem[2];  //Se deseja pegar um item do chão
    int useItem[2];     //Se deseja usar um item do inventorio
    int inventory[5];   //Inventorio
    int inventoryCount; //Quantidad de itens no inventorio
    int attack;         //Se deseja atacar na direção que está olhando
    int jump;           //Se deseja tentar pular
    int dragonSlayer;   //Não deixa morrer para o dragão (objetivo do jogo)

    /* LÁ MALICIAS MALICIOSAS (Cheating) */
    int cheater;

    /* Animações */
    int maxFrame;                  //Quantidade máxima de frame POR COLUNA (Caso tenha mais de uma  coluna)
	int curFrame;                  //Número do frame atual na coluna atual
	int frameCount;                //Contagem do frame atual. Sprite sheets utilizam mais de um frame para se movimentar
	int frameDelay;                //Tempo de descanço entre frames
	int frameWidth;                //Tamanho de cada frame do sprite sheet em largura
	int frameHeight;               //Tamanho de cada frame do sprite sheet em altura
	int animationColumn;           //Um sprite sheet pode ter mais de uma coluna, qual a coluna atual?
	int animationDirection[4];     //Frames de cada direção do personagem (A coluna de cada direção)
    int animationIdleFrame;        //Frame de animação ociosa
    int isAnimate;                 //Tô animando? (Trocando de frame)

    /* Spells */
    TSpell spellBook[3];

    /* Sprite Sheet */
    ALLEGRO_BITMAP *sprite;

}TPlayer;


typedef struct {
    int row;
    int col;

    ALLEGRO_BITMAP *tileSprite;

}Tile;

typedef struct {
    int width;
    int height;
    int posX;
    int posY;
    int curFrame;
    int frameCount;
    int frameDelay;
    int curColumn;
    int maxColumn;

    ALLEGRO_BITMAP *bloodSprite[11];
} TBloodEffect;
#endif