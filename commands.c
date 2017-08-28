/*
    Arquivo responsável por processar uma String a procura de comandos e seus respectivos valores.
    O processa começa separando a String em diversas linhas (terminando com ';') e depois
    pra cada comando identificado (através de uma lista de comandos possiveis) ele vai adicionar os comandos
    a uma fila.
*/
#include "commands.h"

//Objetivo: Separar as linhas (tudo que terminar com ";")
void parseTextInput(char text[]) {
    ALLEGRO_THREAD *commandsThread = NULL;
    int linePos = 0;
    int i = 0, len = strlen(text);
    char cmd[len+1];
    int count = 0;
    if (len > 0) {
        while(text[i] == ' ' || text[i] == '\n') {
            i++;
        }
        
        //Vou ler char-por-char do Texto e pra cada um lido irei colocar na string cmd[], até encontrar um ';'
        for (;i < len; i++) {
            cmd[linePos] = text[i]; 
            linePos++;
            
            //Cheguei no fim do primeiro comando, delimitado por um ';'
            if(text[i] == ';') {    
                count++;                        //Aumento número de comandos encontrados (linhas)
                cmd[linePos] = '\0';            //Medida de segurança, seto um '\0' logo depois do ';' 
                linePos = 0;                    //Volto para o inicio da string cmd[]
                parseCommandText(cmd);          //Envio o comando para ser processado
                memset(cmd,'\0',strlen(cmd));   //Medida de segurança: Reseto a string cmd[]
                while(text[i+1] == ' ' || text[i+1] == '\n') {       //Removo os espaços vazios antes do próximo comando (se tiver)                   
                    i++;
                }
            }    
        }
        if (!count)
            printf("Nenhum comando identificado.\n");
        else {
            printf("Identificado %d comando(s)...\n\n",count);
            commandsThread = al_create_thread(interpretCommands, NULL);
            al_start_thread(commandsThread);
            //interpretCommands();
        }
    }
}
//Objetivo: Descobrir o tipo de comando da linha (Analisando o comando no inicio da linha)
void parseCommandText(char *commandString) { //s = move 5;\0
    int i, cmdType;
    int fim = 0;
    //int j;
    //int parametersRead = 0;
    //int cmdParameters;
    int len = strlen(commandString);
    char cmd[len];
    TCommand *instruction;
    for (i = 0;i < len; i++) {              //Percorro a string do comando e guardo em cmd[] os chars lidos
        cmd[i] = commandString[i];
        if(commandString[i] == ' ' || commandString[i] == ';') {       //encontrei uma palavra (Comando ou parametro)
            fim = 1;            
            cmd[i] = '\0';                  //"Fecho" a string cmd[] com a nova palavra encontrada
            cmdType = getCommandType(cmd);  //Obtenho o tipo de comando (Já sei o que fazer com ele)
            switch(cmdType) {
                case CMD_HELP:
                case CMD_RESET:
                case CMD_JUMP:
                case CMD_ATK:
                    processCommandType0(cmdType,commandString);
                    break;
                case CMD_MOVE:
                case CMD_ROTATE:
                case CMD_CAST:
                case CMD_USE:
                case CMD_GET:
                case CMD_CHEAT:
                    processCommandType1(cmdType,commandString,i);
                    break;
                default:
                    printf("[parseCommandText]: (Type: %d) Comando desconhecido recebido.\n",cmdType);
            }
        }
        if (fim)
            break;
    }
            /*cmdParameters = getCommandParameters(cmdType);
            instruction = (TCommand*)malloc(sizeof(TCommand));
            instruction->type = cmdType;
            break;
        }
        
    }

    i+=1;
    j = 0;
    while(parametersRead < cmdParameters) {
        if(s[i] == ' ' || s[i] == ';') {
            parametersRead++;
            cmd[i] = '\0';
            instruction->val = atoi(cmd);
        }
        cmd[j] = s[i];
        i++;
        j++;
    }
    addCommand(instruction);*/
}

/* =======================[ PROCESS COMAND TYPES ]=======================
 * Os tipos de comando (0,1,2,...) são baseados na quantidade de parâmetros
 * As "Instruções" são montadas e adicionadas a fila aqui.
 *
 */

//Comandos Type 0 não possuem parâmetros
void processCommandType0(int cmdType, char *cmd) {
    TCommand *c;
    
    c = (TCommand*)malloc(sizeof(TCommand));
    c->type = cmdType;
    c->val = 0;
    c->next = NULL;
    
    addCommand(c);
    
}

//Comandos Type 1 possuem 1 parâmetro
void processCommandType1(int cmdType, char *cmd, int startParams) {
    int fim = 0;    
    int j = 0,i = startParams;
    int len = strlen(cmd);
    int paramsLen = (len-startParams)+1; //O tamanho MAX de um param é igual: [Tamanho do seu comando] - [Nome do comando]
    char params[paramsLen]; 
    TCommand *c;
    while(cmd[i] == ' ') {
        i++;
    }
    memset(params,'\0',paramsLen);
    for(;i < len; i++) {
        if(cmd[i] == ' ' || cmd[i] == ';') {
            params[i+1] = '\0';
            c = (TCommand*)malloc(sizeof(TCommand));
            c->type = cmdType;
            if (cmdType == CMD_CAST)
                c->val = getSpellType(params);
            else if (cmdType == CMD_GET || cmdType == CMD_USE)
                c->val = getObjectType(params);
            else if (cmdType == CMD_CHEAT)
                c->val = getCheatType(params);
            else
                c->val = atoi(params);

            c->next = NULL;
            addCommand(c);
            fim = 1;
            break;          //Type 1 só tem 1 parâmetro, logo ao encontrar eu encerro.
        }
        if(fim)
            break;
        else {
            params[j] = cmd[i]; 
            j++;
        }
    }    
}

int getCommandType(char *cmdString) {
    int type = -1;
    int i;
    int slen = strlen(cmdString);
    for (i = 0; i < slen; i++) {
        cmdString[i] = tolower(cmdString[i]);
    }
    if(strcmp(cmdString,"move") == 0)
        type = CMD_MOVE;
    else if(strcmp(cmdString,"rotate") == 0)
        type = CMD_ROTATE;
    else if(strcmp(cmdString,"reset") == 0)
        type = CMD_RESET;
    else if(strcmp(cmdString,"jump") == 0)
        type = CMD_JUMP;
    else if(strcmp(cmdString,"cast") == 0)
        type = CMD_CAST;
    else if(strcmp(cmdString,"use") == 0)
        type = CMD_USE;
    else if(strcmp(cmdString,"get") == 0)
        type = CMD_GET;
    else if(strcmp(cmdString,"help") == 0)
        type = CMD_HELP;
    else if(strcmp(cmdString,"cheat") == 0)
        type = CMD_CHEAT;
    else if(strcmp(cmdString,"attack") == 0 || strcmp(cmdString,"atk") == 0)
        type = CMD_ATK;
    return type;
}

int getCheatType(char *param) {
    int type = -1;
    int i;
    int slen = strlen(param);

    for (i = 0; i < slen; i++) {
        param[i] = tolower(param[i]);
    }

    if(strcmp(param,"iddqd") == 0 || strcmp(param,"idkfa") == 0) //map reveal
        type = CHEAT_IDKFA;
    else if(strcmp(param,"upupdowndownleftrightleftrightbastart") == 0) //dragon slayer, door open (ou adicionar chave ao inventorio), teleport to dragon
        type = CHEAT_KONAMI;
    else if(strcmp(param,"justinbailey") == 0)
        type = CHEAT_JUSTINBAILEY; //No collision
    else if(strcmp(param,"allyourbasearebelongtous") == 0 || strcmp(param,"thereisnocowlevel") == 0 || strcmp(param,"bewareoblivionisathand") == 0)
        type = CHEAT_OTHERS; //suicide

    return type;
}

int getSpellType(char *param) {
    int type = -1;
    int i;
    int slen = strlen(param);

    for (i = 0; i < slen; i++) {
        param[i] = tolower(param[i]);
    }
        if(strcmp(param,"fireball") == 0)
            type = SPELL_FIREBALL;   
        else if(strcmp(param,"frostbolt") == 0)
            type = SPELL_FROSTBOLT;   
        else if(strcmp(param,"poisonbolt") == 0)
            type = SPELL_POISONBOLT;   

    return type;
}

int getObjectType(char *param) {
    int type = -1;
    int i;
    int slen = strlen(param);

    for (i = 0; i < slen; i++) {
        param[i] = tolower(param[i]);
    }
        if(strcmp(param,"key") == 0)
            type = OBJ_PICK_KEY;  
        else
            printf("Parâmetro: [%s] é desconhecido ao comando GET !\n",param);    
    return type;
}

void addCommand(TCommand *command) {
    if(!Enqueue(commandList, command)){
        printf("Erro ao adicionar comando na CommandQueue! \n");
    }
    //printf("Comando Adicionado: {Type: %d, val: %d }\n",command->type, command->val);

}

//Identifica os comandos um a um e faz o que pedem
void* interpretCommands(ALLEGRO_THREAD *thread, void *args) {
    printf("Interpretando comandos...\n");
  /*
   * 2 - Função no MAIN onde eu verifico se a commandList NÃO está vazia e se uma flag
   *     que me permite começar a interpretação está setada.
   */
    TCommand *cmd;
    while(!isEmpty(commandList)) {
        if(!(player.isAnimate)) {
            cmd = Dequeue(commandList);
            switch(cmd->type) {
                case CMD_RESET:
                    printf("COMMAND_RESET\n");
                    commandReset(&player);
                    break;
                case CMD_MOVE:
                    printf("COMMAND_MOVE\n");
                    commandMove(&player,(cmd->val));
                    break;
                case CMD_ROTATE:
                    printf("COMMAND_ROTATE\n");
                    commandRotate(&player,cmd->val);
                    break;
                case CMD_JUMP:
                    printf("COMMAND_JUMP\n");
                    commandJump(&player);
                    break;
              case CMD_CAST:
                    printf("COMMAND_CAST\n");
                    commandCast(&player, cmd->val);
                    break;
                case CMD_USE:
                    printf("COMMAND_USE\n");
                    commandUse(&player, cmd->val);
                    break;
                case CMD_GET:
                    printf("COMMAND_GET\n");
                    if(cmd->val > 0)
                        commandGet(&player, cmd->val);
                    break;
                case CMD_ATK:
                    printf("COMMAND_ATTACK\n");
                    commandAttack(&player);
                    break;
                case CMD_HELP:
                    printf("COMMAND_HELP\n");
                    commandHelp();
                    break;
                case CMD_CHEAT:
                    printf("Cheating, huh?\n");
                    commandCheat(&player, cmd->val);
                    break;
                default:
                    printf("Error ao interpretar comando: { type: %d, val:% d }\n",cmd->type, cmd->val);
            }
           al_rest(1); 
        }  
    }
    printf("Fim da interpretação de comandos !\n\n");
    //al_destroy_thread(thread);
    return NULL;
}

//Seta o Player.steps
//Obs.: Poderia fazer sem ter que passar o TPlayer no parâmetro, porém dessa forma faz com que eu possa adicionar mais personagens (TPlayer) futuramente.
void commandMove(TPlayer *player, int steps) {
    if(player->steps > 0)
        player->steps += steps;    //Se eu ainda tiver que movimentar eu adiciono os novos "Steps" ao que já tenho
    else
        player->steps = steps; 
}

void commandAttack(TPlayer *player) {
    player->attack = 1;
}

void commandJump(TPlayer *player) {
    player->jump = 1;
}

void commandReset(TPlayer *player) {
    player->posX = 2  * 32;
    player->posY = 14 * 32;
    player->speed = 2;
    player->direction = DIR_RIGHT;
    player->steps = 0;
    player->isAnimate = 0; 
    player->mapY = 14;
    player->mapX = 2;
    player->isDeadYet = 0; 
    player->state = PLAYER_IDLE;
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
    //(...)

    switch(player->charType) {
        case CHAR_GALINHA:
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
            break;
    }
}

void commandRotate(TPlayer *player, int dir) {
    int i;
    for (i = 0; i < dir; i++) { 
        if(player->direction >= 3)
            player->direction = 0;
        else
            player->direction++;
     }
}

void commandUse(TPlayer *player, int obj) {
    int i;

    if(player->inventoryCount > 0) {
        for (i = 0; i < player->inventoryCount; i++) { 
            if(player->inventory[i] == obj){ 
                player->useItem[0] = 1; 
                player->useItem[1] = obj;
            }
        }      
    }
}

void commandGet(TPlayer *player, int obj) {
    player->pickUpItem[0] = 1;      //Quero um item
    player->pickUpItem[1] = obj;    //TIpo do item (Tem que ser igual o do mapa)

}

void commandCast(TPlayer *player, int spell) {
    int i;
    int spellX, spellY;
    switch(player->direction) {
        case DIR_UP:
            spellX = player->posX+10;
            spellY = player->posY;
            break;
        case DIR_DOWN:
            spellX = player->posX+10;
            spellY = player->posY+5;
            break;
        case DIR_LEFT:
            spellX = player->posX-5;
            spellY = player->posY+10;
           break;
        case DIR_RIGHT:
            spellX = player->posX+5;
            spellY = player->posY+10;
           break;
    }

    for (i = 0; i < 5; i++) { 
        if(player->spellBook[i].spellType == spell) {
            if(player->spellBook[i].spellLeft > 0) {
                if(!player->spellBook[i].isActive) {
                    player->spellBook[i].posX = spellX;
                    player->spellBook[i].posY = spellY;
                    player->spellBook[i].mapX = player->mapX;
                    player->spellBook[i].mapY = player->mapY;
                    player->spellBook[i].isActive = 1;
                    player->spellBook[i].spellLeft--;
                }
            }
            break;
        }    
    }
}

void commandCheat(TPlayer *player, int cheatCode) {
    switch(cheatCode) {
        case CHEAT_IDKFA:
            drawMapFog = 0;
            player->cheater = 1;
            break;
        case CHEAT_KONAMI:
            player->inventory[player->inventoryCount] = OBJ_PICK_KEY;
            player->inventoryCount++;
            player->dragonSlayer = 1;
            player->posX = 224;
            player->posY = 160;
            player->mapY = 5;
            player->mapX = 7;
            player->direction = DIR_UP;
            player->cheater = 1;
            break;
        case CHEAT_JUSTINBAILEY:
            break;
        case CHEAT_OTHERS:
            player->isDeadYet = 1;
            player->isAnimate = 1;
            player->cheater = 1;
            break;
    }
}

void commandHelp() {
    al_show_native_message_box(NULL,"Help", "",
    "reset\t\t=> Reseta o personagem para posição inicial\n"
    "move X\t=> Move o personagem X casas para frente\n"
    "rotate X\t=> Rotaciona o personagem mudando a sua direção\n"
    "atk/attack\t=> Ataca o que tiver na sua frente\n"
    "cast [Spell]\t=> Lança uma magia (fireball, frostbolt, poisonbolt)\n"
    "get Item\t=> Tenta pegar o item correspondente no chão(Ex.: get key)\n"
    "use Item\t=> Tenta usar o item correspondente caso esteja no seu inventorio\n"
    "cheat [Code]\t=> Que tal tentar usar um cheat famoso?\n"
    "help\t=> Exibe esta tela\n",

    NULL, 0);
}
