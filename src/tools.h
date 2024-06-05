#include<stdbool.h>
#ifndef TOOLS_H_
#define TOOLS_H_


typedef struct move{
    int move,value;
}Move;



typedef struct Matrix{
    int free;
    int data[9];
    bool available[9];
}Matrix;

int fillChoice(Matrix *matrix,int player,int pos);
int rmChoice(Matrix *matrix,int pos);
void findBestMove(Matrix *matrix,Move *move,int player,int level);
int checkWinner(Matrix *matrix);
#endif //TOOLS_H_