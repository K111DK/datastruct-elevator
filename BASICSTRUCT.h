//
// Created by 35802 on 2021/10/31.
//
#ifndef MAIN_C_BASICSTRUCT_H
#define MAIN_C_BASICSTRUCT_H
#include <stdio.h>
#include <stdlib.h>
#define t 1//单位时间
#define T 1000//总模拟时间
enum {GoingUp,GoingDown,Idle,GoingBack};//电梯的三种状态
#define Maxsize 50//电梯最大载客量及各层最大排队人数
#define FloorNum 5//楼层数
#define MaxInterTime 100


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
}Person;

typedef struct stack//电梯栈，储存各目标楼层下的人
{
    Person *top;
    Person *bottom;
    int Size;
}Stack;

typedef struct Elevator{//电梯
    Stack *ElePeople[FloorNum];//电梯前往各目标楼层人
    int CallCar[FloorNum];
    int State;//电梯运行状态
    int Floor;//电梯所在楼层
    int D1;//有人出入为1
    int D2;//滞留
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
    printf("t malloc\n");
    time->time=0;
    time->next=NULL;
    return time;
}

void InsertTime(TimeLine *time,int tic){
    if(!time)return;
    if(time->next==NULL&&time->time==0){
        time->time=tic*t;
    } else{
        TimeLine *s= (TimeLine*)malloc(sizeof (TimeLine));
        printf("t malloc 2\n");
        s->time=tic*t;
        while (time->next){
            time=time->next;
        }
        time->next=s;
        s->next=NULL;
    }
}

int DeletTime(TimeLine *time){//对要加入的人进行计时，并得到当前时刻的加入人数
    TimeLine *pre,*front;
    int num=0;
    if(!time||(time->next==NULL&&time->time==0)){
        return 0;
    }
    pre=time;
    pre->time-=t;
    front=pre->next;
    if(front==NULL){
//        printf("hell");
//        printf("%f\n",pre->time);
        num = pre->time==0?1:0;
        if(num==1){
            pre->time=0;
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
    printf("stack malloc\n");
    Stack *p=(Stack*) malloc(sizeof (Stack));
    p->bottom = (Person *)malloc(Maxsize * sizeof(Person));
    p->top = p->bottom;
    p->Size = Maxsize;
    return p;
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
    return 1;
}
Person *Pop(Stack *p)
{
    if (StackEmpty(p) == 1){
        return 0;
    }
    Person *q=p->top;
    p->top--;
    return q;
}

Queue * initQueue(){
    Queue * queue=(Queue*) malloc(sizeof (Queue));
    queue->front=queue->rear=(QNode*)malloc(sizeof(QNode));
    printf("queue malloc\n");
    queue->rear->next=NULL;
    return queue;
}
void enQueue(Queue*queue,Person *data){
    printf("111111111\n");
    QNode * enElem;
    printf("qnode malloc\n");
    enElem=(QNode*)malloc(sizeof(QNode));
    enElem->data=(Person*) malloc(sizeof (Person));
    enElem->data=data;
    enElem->next=NULL;
    queue->rear->next=enElem;
    queue->rear=enElem;
}
QNode *DeQueue(Queue*queue){
    if (queue->front->next==NULL) {
        printf("队列为空");
        return NULL;
    }
    QNode * p=queue->front->next;
    queue->front->next=p->next;
    if (queue->rear==p) {
        queue->rear=queue->front;
    }
    return p;
}
void DeletQueNode(Queue*queue,QNode*node){
    if(queue->rear==queue->front){//队列为空
        exit(0);
    }
    QNode *temp=queue->front;
    while(temp->next!=node){
        temp=temp->next;
    }
    if(node->next==NULL){//只有1个结点
        free(queue->front);
        queue->front=queue->rear;
        queue->rear->next=NULL;
    }
    temp->next=temp->next->next;
}
int QueueEmpty(Queue*queue){
    return queue->rear==queue->front?1:0;
}


#endif //MAIN_C_BASICSTRUCT_H
