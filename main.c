#include "utils.h"
#include "BASICSTRUCT.h"
#include "simulateFunc.h"
#include "time.h"


void Init(Button *But,Queue **W,Elevator **E){
    int i=0;
    int j=0;
    for(i=0;i<FloorNum;i++){
        But->CallDown[i]=0;
        But->CallUp[i]=0;
        W[i]=initQueue();
    }
    for(j=0;j<2;j++){
        E[j]=(Elevator*) malloc(sizeof (Elevator));
        for(i=0;i<FloorNum;i++){
            E[j]->ElePeople[i]=InitStack();
            E[j]->CallCar[i]=0;
        }
        E[j]->D1=-1;
        E[j]->D2=E[j]->D3=0;
        E[j]->Floor=1;
        E[j]->State=Idle;
        E[j]->Action[0]=1;
        E[j]->Action[1]=-1;
        E[j]->Action[2]=-1;
        E[j]->Action[3]=-1;
    }
}

int main(){
    int *Time=(int*) malloc(sizeof (int));
    *Time=0;
    TimeLine *To=(TimeLine*) malloc(sizeof (TimeLine));
    To=TimeLineInit();
    Button *But=(Button*) malloc(sizeof (Button));
    Queue *WaitingQue[FloorNum];
    Elevator *E[2];
    Init(But,WaitingQue,E);//初始化系统
    PersonRandGenAdd(WaitingQue,But,E,To,Time);//随机加入第一个人
    while(*Time!=T){
        *Time+=1;
        printf("\n--------------------------\n时间:%d\n",*Time);
        srand((unsigned int)*(Time));//*time(NULL)
        ElevatorProcess(WaitingQue,E,But,Time,0);
        ElevatorProcess(WaitingQue,E,But,Time,1);
        ElePrint(E,WaitingQue,But,Time);
        PeopleProcess(WaitingQue,E,But,To,Time);
        QueuePrint(WaitingQue,But,E);
        TimeLinePrint(To);
        printf("--------------------------\n");
    }
}