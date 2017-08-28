#ifndef COMMAND_QUEUE_H
#define COMMAND_QUEUE_H

typedef struct Command_t {
    int type;
    int val;
    struct Command_t *next;
}TCommand;

typedef struct Queue {
    TCommand *head;
    TCommand *tail;
    int size;
} Queue;

Queue* createQueue();
void destroyQueue(Queue *commandQueue);
int Enqueue(Queue *commandQueue, TCommand *cmd);
TCommand* Dequeue(Queue *commandQueue);
int isEmpty(Queue* commandQueue);

#endif