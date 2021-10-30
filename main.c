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
    int CallCar[FloorNum];
    int State;//电梯运行状态
    int Floor;//电梯所在楼层
    int D1;//有人出入为1
    int D2;//停留不超过300t为1
    int D3;//无人进出且门开为1
    float E[9][2];//代表电梯各个活动及活动计时器
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
}QNode;
typedef struct Queue{
    QNode *rear,*front;
}Queue;
Queue * initQueue(){
    Queue * queue=(Queue*) malloc(sizeof (Queue));
    queue->front=queue->rear=(QNode*)malloc(sizeof(QNode));
    queue->rear->next=NULL;
    return queue;
}
void enQueue(Queue*queue,Person *data){
    QNode * enElem=(QNode*)malloc(sizeof(QNode));
    enElem->data=data;
    enElem->next=NULL;
    queue->rear->next=enElem;
    queue->rear=enElem;
}
void DeQueue(Queue*queue){
    if (queue->front->next==NULL) {
        printf("队列为空");
        return ;
    }
    QNode * p=queue->front->next;
    queue->front->next=p->next;
    if (queue->rear==p) {
        queue->rear=queue->front;
    }
    free(p);
}
void DeletQueNode(Queue*queue,QNode*node){

}
int QueueEmpty(Queue*queue){
    return queue->rear==queue->front?1:0;
}

void PersonRandGenAdd(Queue**W,Button *button){//（伪）随机地生成一个人加入到队列
    Person *a;
    a=(Person*)malloc(sizeof (Person));
    a->GivenUpTime= GenRand(100)*t;
    a->InFloor = GenRand(FloorNum)-1;
    a->OutFloor = GenRand(FloorNum)-1;
    while (a->OutFloor==a->InFloor){
        a->OutFloor = GenRand(FloorNum)-1;
    }
    a->InterTime = GenRand(MaxInterTime)*t;
    enQueue(W[a->InFloor],a);//将随机生成的人加入等待队列
    if(a->InFloor>a->OutFloor){//进入队列时就按下按钮
        button->CallDown[a->OutFloor]=1;
    } else{
        button->CallUp[a->OutFloor]=1;
    }
}

void Controller(){




}

void ClockAdjust(Queue **W,Elevator *E,Button *But){
    int i=0;
    QNode *s;
    for(i=0;i<FloorNum;i++){//各楼层等待人的忍耐时间自减
        for(s=W[i]->front;s!=W[i]->rear;s=s->next){
            s->data->GivenUpTime--;
            s->data->InterTime--;
        }
    }
    for(i=1;i<=9;i++){
        if(E->E[i][0]==1){
            E->E[i][1]--;
        }
    }
}
void PeopleProcess(Queue **W,Elevator *E,Button *But){//判断是否要往各楼层等待队列加人,并根据各楼层等待人状态调整楼层按钮，调整人的出入队状态
    //1.判断是否要加入人
    //2.遍历所有层的队列，并设置相应的电梯按钮
    int i=0;
    QNode *s;
    for(i=0;i<FloorNum;i++){
        for(s=W[i]->front;s!=W[i]->rear;s=s->next){
            if(s->data->GivenUpTime==0){//等待时间到时
                if(s->data->InFloor!=E->Floor||E->D1==0){
                    DeletQueNode(W[i],s);
                    if(s->data->InFloor>s->data->OutFloor){//考虑到当离开的恰好是最后一个人时，按钮重新置0
                        But->CallDown[s->data->OutFloor]=0;
                    }else{
                        But->CallUp[s->data->OutFloor]=0;
                    }
                    continue;
                }
                else if(s->data->InFloor==E->Floor&&E->D1!=0){
                    s->data->GivenUpTime=0;
                    if(s->next==NULL){//轮到他时，使其进入电梯
                        DeletQueNode(W[i],s);
                        Push(E->ElePeople[s->data->OutFloor],s->data);
                        if(s->data->InFloor>s->data->OutFloor){//离开按钮重新置0
                            But->CallDown[s->data->OutFloor]=0;
                        }else{
                            But->CallUp[s->data->OutFloor]=0;
                        }
                    }
                }
            }
            else if(s->data->InterTime==0){//间隔时间结束，生成下一个人
                PersonRandGenAdd(W,But);
            }
            else{
                continue;
            }
        }
    }








}

void ElevatorProcess(Queue **W,Elevator *E,Button *But){
    int i=0;
    for(i=1;i<=8;i++){
        if(E->E[i][0]){
            break;}
    }
    switch (i) {
        case 1:Controller();break;
        case 2:Controller();break;
        case 3:E->D1=E->D2=1;

    }







}



void Init(Button *But,Queue **W,Elevator *E){
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
        E->E[i][1]=0;
    }
}

int main(){
    int Time=1;
    Button *But=(Button*) malloc(sizeof (Button));
    Queue *WaitingQue[FloorNum];
    Elevator *E=(Elevator*) malloc(sizeof (Elevator));
    Init(But,WaitingQue,E);//初始化系统
    PersonRandGenAdd(WaitingQue);//随机加入第一个人
    while(Time!=T){
        ClockAdjust(WaitingQue,E,But);





        Time++;
    }
}