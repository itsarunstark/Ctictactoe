#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void printArr(int size,int *data);

int fillChoice(Matrix *matrix,int player, int pos){
    if(-1<pos && pos<9 && matrix->available[pos]){
        matrix->available[pos] = 0;
        matrix->data[pos] = player;
        matrix->free--;
        return 1;
    }
    return 0;
}

int rmChoice(Matrix *matrix, int pos){
    if(!matrix->available[pos]){
        matrix->available[pos] = 1;
        matrix->data[pos] = 0;
        matrix->free++;
    }
    return 1;
}

int checkWinner(Matrix *matrix){
    for(int i=0;i<3;i++){
        if(matrix->data[3*i] == matrix->data[3*i+1] && matrix->data[3*i + 1] == matrix->data[3*i + 2] && matrix->data[3*i] != 0){
            return matrix->data[3*i];
        }
        if(matrix->data[i] == matrix->data[i+3] && matrix->data[i+3] == matrix->data[i+6] && matrix->data[i] != 0){
            return matrix->data[i];
        }
    }
    if(matrix->data[0] == matrix->data[4] && matrix->data[4] == matrix->data[8] && matrix->data[0] != 0) {
        return matrix->data[0];
    }
    else if(matrix->data[2] == matrix->data[4] && matrix->data[4] == matrix->data[6] && matrix->data[2] != 0) {
        return matrix->data[2];
    }
    if(!matrix->free)
        return 2;
    return 0;
}

void printMatrix(Matrix *matrix){
    for(int i=0;i<9;i++){
        switch (matrix->data[i]) {
            case 0:printf("* ");break;
            case 1:printf("X ");break;
            case -1:printf("O ");break;
        }
        // printf("%d ",matrix->data[i]);
        if((i+1)%3 == 0)
            printf("\n");
    }
}

void findBestMove(Matrix *matrix, Move *move, int player, int level){
    int rounds = matrix->free;
    int results = matrix->free;
    int iter = 0;
    int status = 0;
    int data[matrix->free];
    while(rounds>0){
        if(matrix->available[iter]){

            fillChoice(matrix, player, iter);
            status = checkWinner(matrix);
            if(level<=0){
                data[results-rounds] = rand()%3 - 1;
                // printf("Random init.\n");
            }else if(status){
                data[results-rounds] = (status == 2)?0:status;
            }else{
                // printf("Working.\n");
                findBestMove(matrix, move, -player, level-1);
                data[results-rounds] = move->value;
            }
            rmChoice(matrix,iter);
            rounds--;
        }
        iter++;
    }
    int value = (player>0)?-2:2;
    iter = 0;
    int mov = -1;
    while(rounds<results){
        if(matrix->available[iter]){
            if(mov<0)
                mov = iter;
            if(player>0){
                if(value<data[rounds]){
                    mov = iter;
                    value = data[rounds];
                }else if(value== data[rounds] && rand()%2){
                    mov = iter;
                }
            }else{
                if(value>data[rounds]){
                    mov = iter;
                    value = data[rounds];
                }else if(value== data[rounds] && rand()%2){
                    mov = iter;
                }
            }
            rounds++;
        }
        iter++;
    }
    move->move = mov;
    move->value = value;
    // printArr(results,data);

}

/*int player1(Matrix *matrix){
    int choice;
    printf("Player 1's Turn: ");
    scanf("%d",&choice);
    return fillChoice(matrix, 1, choice-1);
    // Move move;
    // printf("AI 1's Turn : ");
    // findBestMove(matrix, &move, 1, 6);
    // printf("I Go for [Move: ] %d , [Weight: ] %d\n",move.move,move.value);
    // return fillChoice(matrix, 1, move.move);
    
}
*/
void printArr(int size,int *data){
    printf("Arr = {");
    while(size-- > 0){
        printf("%d ",*(data++));
    }
    printf("}\n");
}

/*int player2(Matrix *matrix){
    Move move;
    printf("AI 2's Turn : ");
    findBestMove(matrix, &move, -1, 9);
    printf("I Go for [Move: ] %d , [Weight: ] %d\n",move.move,move.value);
    return fillChoice(matrix, -1, move.move);

}

int main(){
    srand(time(NULL));
    Matrix matrix;
    matrix.free = 9;
    for(int i=0;i<9;i++){
        matrix.available[i] = 1;
        matrix.data[i] = 0;
    }
    int exit_game = 0;
    int choice = 0;
    printMatrix(&matrix);
    int status;
    while(!exit_game){
        if(choice){
            status = player1(&matrix);
        }else{
            status = player2(&matrix);
        }
        if(status)
            choice = !choice;

        printMatrix(&matrix);
        status = checkWinner(&matrix);
        // printf("%d\n",status);
        if(status){
            switch (status) {
                case 1:
                    printf("Player 1 wins.\n");break;
                case -1:
                    printf("Player 2 wins.\n");break;
                default:
                    printf("A Nice Draw.\n");
            }
            exit_game = 1;
        }

    }
}
*/