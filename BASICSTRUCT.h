//
// Created by 35802 on 2021/10/31.
//
#ifndef MAIN_C_BASICSTRUCT_H
#define MAIN_C_BASICSTRUCT_H
#include <stdio.h>
#include <stdlib.h>
#define t 1//单位时间
#define T 5000//总模拟时间
enum {GoingUp=1,GoingDown=2,Idle=3,GoingBack=4};//电梯的三种状态
#define Maxsize 500//电梯最大载客量及各层最大排队人数
#define FloorNum 5//楼层数
#define MaxInterTime 80
#define DoorOperTime 20
#define InOutTime 25
#define AccerlerTime 14
#define deccerlerTime 14
#define UpingTime 50
#define DowningTime 60
#define IdleTime 300
#define DetectTime 40
#define WaitingTime 20
typedef struct TimeLine{
    struct TimeLine *next;
    int time;
}TimeLine;

typedef struct Button{
    int CallUp[FloorNum];
    int CallDown[FloorNum];
}Button;//各楼层按钮情况

typedef struct  Person{
    int InFloor;
    int OutFloor;
    int GivenUpTime;
   int InterTime;
   int code;
}Person;

typedef  struct StackNode{
    Person *data;
    struct StackNode *next;
}StackNode;

typedef struct Stack//电梯栈，储存各目标楼层下的人
{
    StackNode *top;
    StackNode *bottom;
    int Size;
}Stack;

typedef struct Elevator{//电梯
    Stack *ElePeople[FloorNum];//电梯前往各目标楼层人
    int CallCar[FloorNum];
    int State;//电梯运行状态
    int Floor;//电梯所在楼层
    int D1;//有人出入为1
    int D2;//关门
    int D3;//无人进出且门开为1
    int Action[4];//代表电梯各个活动及活动计时器
}Elevator;

typedef struct QNode{
    Person *data;
    struct QNode * next;
}QNode;
typedef struct Queue{
    QNode *rear,*front;
}Queue;



TimeLine *TimeLineInit(){
    TimeLine *time=(TimeLine*)malloc(sizeof (TimeLine));
    time->time=-1;
    time->next=NULL;
    return time;
}

void InsertTime(TimeLine *time,int tic){
    if(!time)return;
    if(time->next==NULL&&time->time==-1){
        time->time=tic*t;
    } else{
        TimeLine *s= (TimeLine*)malloc(sizeof (TimeLine));
        s->time=tic*t;
        while (time->next){
            time=time->next;
        }
        time->next=s;
        s->next=NULL;
    }
}

void TimeLinePrint(TimeLine* To){
    if(To->next==NULL&&To->time==-1){
        return;
    }else{
        TimeLine * Node=To;
        while(Node){
            printf("下一个人将在:%d个单位时间后加入排队队列\n",Node->time);
            Node=Node->next;
        }
        printf("\n");
        return;
    }
}
int DeletTime(TimeLine *time){//对要加入的人进行计时，并得到当前时刻的加入人数
    TimeLine *pre,*front;
    int num=0;
    if(!time){
        return 0;
    }
    if((time->next==NULL&&time->time==-1)){
        return 0;
    }
    pre=time;
    pre->time-=t;
    front=pre->next;
    if(front==NULL){
        num = pre->time==0?1:0;
        if(num==1){
            pre->time=-1;
        }
        return num;
    }
    while(front->next!=NULL){
        {
            front->time-=t;
            if(front->time==0){
                num++;
                pre->next=front->next;
                front=front->next;
                if(front==NULL){
                    break;
                }
                continue;
            }
            front=front->next;
            pre=pre->next;
        }
    }
    return num;
}



Stack * InitStack(){
    Stack *p=(Stack*) malloc(sizeof (Stack));
    p->bottom=p->top=(StackNode*) malloc(sizeof (StackNode));
    p->bottom->next=NULL;
    p->bottom->data=NULL;
    p->Size = Maxsize;
    return p;
}
int StackEmpty(Stack *p)
{
    if(!p)return 0;
    if (p->top == p->bottom){
        return 1;
    }
    else{
        return 0;
    }
}
int StackSize(Stack *p)
{
    if(p->top==p->bottom)return 0;
    int num=0;
    StackNode *node= (StackNode*) malloc(sizeof (StackNode));
    node=p->top;
    while(node!=p->bottom){
        num++;
        node=node->next;
    }
    return num;
}
int Push(Stack *p,Person *a) {
    if(p==NULL){
        return 0;
    }
    StackNode * newNode=(StackNode*) malloc(sizeof (StackNode));
        newNode->data=a;
        newNode->next=p->top;
        p->top=newNode;
        return 1;
}

Person *Pop(Stack *p)
{
    if (StackEmpty(p) == 1||p==NULL){
        return 0;
    }
    StackNode *node;
    node=p->top;
    p->top=p->top->next;
    return node->data;
}






Queue * initQueue(){
    Queue * queue=(Queue*) malloc(sizeof (Queue));
    queue->front=queue->rear=(QNode*)malloc(sizeof(QNode));
    queue->rear->next=NULL;
    return queue;
}
void enQueue(Queue*queue,Person *data){
    QNode * enElem;
    enElem=(QNode*)malloc(sizeof(QNode));
    enElem->data=data;
    enElem->next=NULL;
    queue->rear->next=enElem;
    queue->rear=enElem;
}
QNode *DeQueue(Queue*queue){
    if (queue->front->next==NULL) {
        return NULL;
    }
    QNode * p=queue->front->next;
    queue->front->next=p->next;
    if (queue->rear==p) {
        queue->front=queue->rear;
    }
    return p;
}
void DeletQueNode(Queue*queue,QNode*node){
    if(queue->rear==queue->front){//队列为空
        exit(0);
    }
    QNode *temp;
    temp=queue->front->next;
    while((temp->next!=node)&&!temp){
        temp=temp->next;
    }
    if(node->next==NULL){//只有1个结点
        queue->front=queue->rear;
        queue->rear->next=NULL;
        return;
    }
    temp->next=temp->next->next;
}
int QueueEmpty(Queue*queue){
    if(queue->rear==queue->front){
        return 1;
    }else{
        return 0;
    }
}
int QueueSize(Queue*queue){
    int i=0;
    if(queue==NULL){
        return -1;
    }
    if(queue->rear==queue->front){
        return 0;
    }else{
        QNode *p;
        p=queue->front->next;
        while(p){
            p=p->next;
            i++;
        }
        return i;
    }
}
void ElePrint(Elevator*E){
    int i=0;
    StackNode *p;
    printf("-------------\n");
    for(i=0;i<FloorNum;i++){
        printf("要去第%d层的人有%d个\n", i,StackSize(E->ElePeople[i]));
        if(!StackEmpty(E->ElePeople[i])){
            p=E->ElePeople[i]->top;
            while(p->next) {
                printf("No:%d   ", p->data->code);
                p = p->next;
            }

        }
        printf("\n-------------\n");
    }
    printf("----------\n");
    printf("各层呼叫电梯情况\n");
    for(i=0;i<FloorNum;i++){
        printf("第%d层:%d\n",i,E->CallCar[i]);
    }
    printf("----------\n");
}

void QueuePrint(Queue**W,Button *button,Elevator *E){
    int i=0;
    if(W==NULL){
        return;
    }
    int floor=E->Floor;
    printf("\n------------------------------------\n");
    printf("层数  电梯位置  按钮情况  排队情况(从左到右为队头到队尾)\n");
    printf("------------------------------------\n");
    for(i=FloorNum-1;i>=0;i--){
        if(floor==i){
            printf("%d   | ***** |按 |上:%d |第%d层的排队队列有:%d人\n    | ***** |钮 |下:%d |",i,button->CallUp[i],i, QueueSize(W[i]),button->CallDown[i]);
        }else{
            printf("%d   |       |按 |上:%d |第%d层的排队队列有:%d人\n    |       |钮 |下:%d |",i,button->CallUp[i],i, QueueSize(W[i]),button->CallDown[i]);
        }
        QNode *p=W[i]->front->next;
        if(p){
        while(p){
            printf("No:%d   ", p->data->code);
            p = p->next;
        }
        }
        printf("\n------------------------------------\n");
    }
}

#endif //MAIN_C_BASICSTRUCT_H
