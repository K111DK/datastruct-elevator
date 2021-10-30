#include <stdio.h>
#include "stdlib.h"
#define t 0.1//单位时间
#define T 10000//总模拟时间
enum {GoingUp,GoingDown,Idle};//电梯的三种状态
#define Maxsize 50//电梯最大载客量及各层最大排队人数
#define FloorNum 5//楼层数
#define MaxInterTime 100

typedef struct Button{
    int CallUp[FloorNum];
    int CallDown[FloorNum];
}Button;//各楼层按钮情况

typedef struct  Person{
    int InFloor;
    int OutFloor;
    float GivenUpTime;
    float InterTime;
}Person;

int GenRand(int a){//产生范围为0~a的随机整数
    return rand()%a+1;
}

typedef struct stack//电梯栈，储存各目标楼层下的人
{
    Person *top;
    Person *bottom;
    int Size;
}Stack;
void InitStack(Stack *p){
    p->bottom = (int *)malloc(Maxsize * sizeof(int));
     if (p->bottom == NULL)
    {
        return;
    }
     p->top = p->bottom;
     p->Size = Maxsize;
 }
int StackEmpty(Stack *p)
{
    if (p->top == p->bottom){
        return 1;
    }
    else{
        return 0;
    }
}
int StackFull(Stack *p)
{
    if (p->top - p->bottom == p->Size){
        return 1;
    }
    else{
        return 0;
    }
}

int Push(Stack *p,Person *a) {
    if(StackFull(p)==1){
        return 0;
    }
    p->top = a;
    p->top++;
}
int Pop(Stack *p)
{
    if (StackEmpty(p) == 1)
    {
        return 0;
    }
    p->top--;
    return 1;
}


typedef struct Elevator{//电梯
    Stack *ElePeople[FloorNum];//电梯前往各目标楼层人
    int State;//电梯运行状态
    int Floor;//电梯所在楼层
    int D1;//有人出入为1
    int D2;//停留不超过300t为1
    int D3;//无人进出且门开为1
    float E[9][1];//代表电梯各个活动及活动计时器
}Elevator;
//定义队列的结构体

//typedef struct Squeue{//各楼层的排队队列
//    Person *WaitingPeople[Maxsize];
//    int front;
//    int rear;
//}Squeue;
//void InitQueue(Squeue *qu){
//    qu->front = qu->rear = 0;
//}
//int isQueueEmpty(Squeue *qu){
//    if(qu->front == qu->rear)
//    {
//        return 1;
//    }
//    else
//    {
//        return 0;
//    }
//}
//int inQueue(Squeue *qu,Person *x){
//    if((qu->rear + 1) % Maxsize == qu->front)
//    {
//        return 0;
//    }
//    qu->rear = (qu->rear + 1) % Maxsize;
//    qu->WaitingPeople[qu->rear] = x;
//    return 1;
//}
//int deQueue(Squeue *qu){
//    if(qu->front == qu->rear){
//    return 0;
//    }
//    qu->front = (qu->front + 1) % Maxsize;
//    return 1;
//}
typedef struct QNode{
    Person *data;
    struct QNode * next;
}QNode,Queue;
QNode * initQueue(){
    QNode * queue=(QNode*)malloc(sizeof(QNode));
    queue->next=NULL;
    return queue;
}
QNode* enQueue(QNode * rear,Person *data){
    QNode * enElem=(QNode*)malloc(sizeof(QNode));
    enElem->data=data;
    enElem->next=NULL;
    rear->next=enElem;
    rear=enElem;
    return rear;
}
void DeQueue(QNode * top,QNode * rear){
    if (top->next==NULL) {
        printf("队列为空");
        return ;
    }
    QNode * p=top->next;
    top->next=p->next;
    if (rear==p) {
        rear=top;
    }
    free(p);
}

void Controller(){




};


void PeopleProcess(){//判断是否要往各楼层等待队列加人,并根据各楼层等待人状态调整楼层按钮，调整人的出入队状态








}

void ElevatorProcess(){








}
Person *PersonGen(){//（伪）随机地生成一个人加入到队列
    Person *a;
    a=(Person*)malloc(sizeof (Person));
    a->GivenUpTime= GenRand(100)*t;
    a->InFloor = GenRand(FloorNum)-1;
    a->OutFloor = GenRand(FloorNum)-1;
    while (a->OutFloor==a->InFloor){
        a->OutFloor = GenRand(FloorNum)-1;
    }
    a->InterTime = GenRand(MaxInterTime)*t;
    return a;
}

void Init(Button *But,Queue ** W,Elevator *E){
    int i=0;
    for(i=0;i<FloorNum;i++){
        But->CallDown[i]=0;
        But->CallUp[i]=0;
        W[i]=initQueue();
        InitStack(E->ElePeople[i]);
    }
    E->D1=E->D2=E->D3=0;
    E->Floor=1;
    E->State=Idle;
    for(i=0;i<9;i++){
        E->E[i][0]=0;
    }
}

int main(){
    int Time=0;
    Button *But=(Button*) malloc(sizeof (Button));
    Queue *WaitingQue[FloorNum];
    Elevator *E=(Elevator*) malloc(sizeof (Elevator));
    Init(But,WaitingQue,E);
    while(Time!=T){





        Time++;
    }
}