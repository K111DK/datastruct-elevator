//
// Created by 35802 on 2021/10/31.
//
#ifndef MAIN_C_BASICSTRUCT_H
#define MAIN_C_BASICSTRUCT_H
#include <stdio.h>
#include <stdlib.h>
#include "windows.h"
#include "time.h"
#include "string.h"
#define t 1//单位时间
//#define T 5000//总模拟时间
enum {GoingUp=1,GoingDown=2,Idle=3,GoingBack=4};//电梯的三种状态
#define Maxsize 500//电梯最大载客量及各层最大排队人数
#define FloorNum 5//楼层数
#define MaxInterTime 80
#define DoorOperTime 20
#define InOutTime 25
#define AccerlerTime 20
#define deccerlerTime 20
#define UpingTime 15
#define DowningTime 15
#define IdleTime 300
#define DetectTime 40
#define GenDetail 0
#define EleDetail 1
#define VisuaLize 1
#define EXAMPLE 0
#define DoubleEle 1
char EleLogger[10003][500];
char PersonLogger[10003][1000];
char VisualLogger[10003][1000];


//void LogInfo(Logger*logger,const char *info){
//
//}
//void save(Logger*logger)//保存学生信息
//{
//    FILE*fp;
//    if((fp=fopen("info.txt","wb"))==NULL)exit(0);
//        fwrite(logger,sizeof(Logger),1,fp);
//    fclose(fp);
//}
//Logger *LoadLog(void)
//{
//    Logger *head,*temp,*temp1;
//    temp=(Logger *)malloc(sizeof(Logger));
//    head=temp;
//    temp1=temp;
//    FILE*fp;
//    if((fp=fopen("info.txt","rb"))==NULL)exit(0);
//    for(;!feof(fp);temp=temp->next)
//    {
//        fread(temp,sizeof(Logger),1,fp);
//        temp->next=(Logger *)malloc(sizeof(Logger));
//        temp1=temp;
//    }
//    temp1->next=NULL;
//    for(temp=head;temp->next!=NULL;temp=temp->next)temp1=temp;
//    temp1->next=NULL;
//    fclose(fp);
//    return head;
//}

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
    int flag[2];
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

void TimeLinePrint(TimeLine* To,char *logger){
    if(To->next==NULL&&To->time==-1){
        return;
    }else{
        TimeLine * Node=To;
        while(Node){
            sprintf(logger,"%s下一个人将在:%d个单位时间后加入排队队列\n",logger,Node->time);
            Node=Node->next;
        }
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
        queue->rear=queue->front;
    }
    return p;
}
void DeletQueNode(Queue*queue,QNode*node){
    if(queue->rear==queue->front||node==NULL){//队列为空
        exit(0);
    }
    QNode *temp;
    temp=queue->front->next;
    while((temp->next!=node)&&!temp){
        temp=temp->next;
    }
    if(node->next==NULL){//只有1个结点
        queue->rear=queue->front;
        queue->rear->next=NULL;
        return;
    }
    temp->next=temp->next->next;
}
int QueueEmpty(Queue*queue){
    if(queue->rear==queue->front||queue->front->next==NULL){
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
#endif //MAIN_C_BASICSTRUCT_H
