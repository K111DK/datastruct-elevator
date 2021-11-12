#include "utils.h"
#include "BASICSTRUCT.h"
#include "simulateFunc.h"
#include "time.h"


void Init(Button *But,Queue **W,Elevator *E){
    int i=0;
    for(i=0;i<FloorNum;i++){
        But->CallDown[i]=0;
        But->CallUp[i]=0;
        W[i]=initQueue();
        E->ElePeople[i]=InitStack();
        E->CallCar[i]=0;
    }
    E->D1=E->D2=E->D3=0;
    E->Floor=1;
    E->State=Idle;
    E->Action[0]=1;
    E->Action[1]=-1;
    E->Action[2]=-1;
}

int main(){
    int *Time=(int*) malloc(sizeof (int));
    *Time=0;
    TimeLine *To=(TimeLine*) malloc(sizeof (TimeLine));
    To=TimeLineInit();
    Button *But=(Button*) malloc(sizeof (Button));
    Queue *WaitingQue[FloorNum];
    Elevator *E=(Elevator*) malloc(sizeof (Elevator));
    E->ElePeople[1]=InitStack();
    Init(But,WaitingQue,E);//初始化系统
    PersonRandGenAdd(WaitingQue,But,E,To,Time);//随机加入第一个人
    while(*Time!=T){
        srand((unsigned int)*(Time));//*time(NULL)
        PeopleProcess(WaitingQue,E,But,To,Time);
        ElevatorProcess(WaitingQue,E,But,Time);
        //ElePrint(E);
        printf("目标楼层:%d\n", Controller(WaitingQue,E,But,0,Time));
        printf("E status:%d\n",E->State);
        QueuePrint(WaitingQue,But,E);
        TimeLinePrint(To);
        *Time+=1;
    }
}