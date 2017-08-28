#include "commandQueue.h"
#include "commands.h"

//Cria uma Queue normal para armazenar os comandos digitados.
//Um comando é descrito como tcommand = { tipo(INT), valor(INT) }
Queue* createQueue() {
    Queue *commandQueue = (Queue*) malloc(sizeof (Queue));
    if (commandQueue == NULL) {
        return NULL;
    }
  
    commandQueue->size = 0;
    commandQueue->head = NULL;
    commandQueue->tail = NULL;

    return commandQueue;
}

//Destroy a Queue
void destroyQueue(Queue *commandQueue) {
    TCommand *cmd;
    while (!isEmpty(commandQueue)) {
        cmd = Dequeue(commandQueue);
        free(cmd);
    }
    free(commandQueue);
}

//Adiciona um novo comando a Queue
int Enqueue(Queue *commandQueue, TCommand *cmd) {
    if (commandQueue == NULL || cmd == NULL) {
        return 0;
    }

    cmd->next = NULL;
    if (isEmpty(commandQueue)) {
        commandQueue->head = cmd;
        commandQueue->tail = cmd;

    } else {
        commandQueue->tail->next = cmd;
        commandQueue->tail = cmd;
    }
    commandQueue->size++;
    return 1;
}

//Retira o primeiro commando da Queue. (Ordem de prioridade, o primeiro que entrar é o primeiro a ser processado)
TCommand* Dequeue(Queue *commandQueue) {
    TCommand *cmd;
    if (isEmpty(commandQueue))
        return NULL;

    cmd = commandQueue->head;
    commandQueue->head = commandQueue->head->next;
    commandQueue->size--;
    return cmd;
}

//Verifica se a Queue está vazia (True: 1)
int isEmpty(Queue* commandQueue) {
    if (commandQueue == NULL)
        return 1;
    else if (commandQueue->size == 0)
        return 1; 
    else 
      return 0;
}
