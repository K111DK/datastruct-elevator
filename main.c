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
    float M[7][2];
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
    p=(Stack*) malloc(sizeof (Stack));
    p->bottom = (Person *)malloc(Maxsize * sizeof(Person));
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
    float E[10][2];//代表电梯各个活动及活动计时器
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

Person *PersonRandGenAdd(Queue **W,Button *button,Elevator *E){//（伪）随机地生成一个人加入到队列
    Person *a;
    a=(Person*)malloc(sizeof (Person));
    a->GivenUpTime= GenRand(100)*t;
    a->InFloor = GenRand(FloorNum)-1;
    a->OutFloor = GenRand(FloorNum)-1;
    while (a->OutFloor==a->InFloor){
        a->OutFloor = GenRand(FloorNum)-1;
    }
    a->InterTime = GenRand(MaxInterTime)*t;
    enQueue(W[a->InFloor],a);
    a->M[1][0]=1;
    //将随机生成的人加入等待队列
    if(a->InFloor==E->Floor) {//电梯就在本层
        if (E->E[6][1] > 0 && E->E[6][0] == 0) {//电梯正在关门
            a->M[5][0] = 1;
        } else if(E->D3==1){//正有人出入电梯
            a->M[3][0] = 1;
        } else if(E->D2==0){//电梯处于停留态
            a->M[5][0] = 1;
        }
    }
        else{
            a->M[2][0] = 1;
            if (a->InFloor > a->OutFloor) {//进入队列时就按下按钮
                button->CallDown[a->OutFloor] = 1;
            } else {
                button->CallUp[a->OutFloor] = 1;
            }
            a->M[3][0] = 1;
        }

    }


int Controller(Queue **W,Elevator *E,Button *But,int flag){
    if(E->State!=Idle) {//c1
        return 0;
    }
    int j=0;
    for ( j = 0; j < FloorNum; ++j) {//c3 找出j
        if(But->CallUp[j]||But->CallDown[j]||E->CallCar[j]){
            break;
        }
    }  
    if(j!=E->Floor){//c4 改变状态
        E->State=j>E->Floor?GoingDown:GoingUp;
    }
    else{
        if(flag==1){
            j=1;
        }
        else{
            return -1;
        }
    }
    if(E->Floor==1){//c5
        if(E->Floor==1&&(But->CallDown[1]||But->CallUp[1]||E->CallCar[1]!=0)){
        E->E[3][0]=20*t;
        return 1;
        }
        else{
            if(j!=1) {
            E->E[6][1]=20*t;
            return 1;
            }
        }
    }
    return 0;
}

void ElevatorProcess(Queue **W,Elevator *E,Button *But){
    int i=0;
    int j=0;
    for(i=0;i<=9;i++){
        if(E->E[i][0]==1||E->E[i][1]!=0){//所有计时状态计时流动1t

            if(E->E[i][0]==0&&E->E[i][1]==t){//当未触发状态计时到0时，触发事件，并将现有事件置0
                E->E[i][0]=1;
                E->E[i][1]=0;
                for(j=0;j<=9;j++) {
                if(j!=i&&E->E[j][0]==1){
                    E->E[j][0]=0;
                }
                }
            }
            E->E[i][1]-=t;
        }
    }//时间流动
    for(i=0;i<=9;i++) {
        if(E->E[i][0]==1){
            break;
        }
    }
    switch (i) {
        case 1:
            Controller(W,E,But,0);break;
        case 2:    break;
        case 3:E->D1=E->D2=1;
        E->E[9][1]=300*t;
        E->E[5][1]=76*t;
        E->E[4][1]=20*t;
            break;
        case 4:if(!StackEmpty(E->ElePeople[E->Floor])){
            if(E->E[4][1]==0){

            }
        }
            else{

            }break;
        case 5:if(E->E[5][1]==0) {
            if(E->D1==1){
                E->E[5][1]=40*t;
                if(E->E[6][1]!=0){
                    E->E[6][1]=0;
                }
            }
            else{
                if(E->D3!=0){
                    E->D3=0;
                    E->E[6][1]=20*t;
                }
            }
            }break;
        case 6:break;
        case 7:
        case 8:if(E->E[7][1]==0){
                E->E[7][1]=(i==7?51:61)*t;
                E->Floor++;
            }
            int c=0;
            for(c=E->Floor+1;c<FloorNum;c++){
                if(c>FloorNum){
                    c=-1;
                    break;
                } else if(But->CallDown[c]==But->CallUp[c]==E->CallCar[c]==0) {
                    break;
                } else{
                    if(c==FloorNum){
                        c=-1;
                    }
                }

            }
            if(E->CallCar[E->Floor]==1||But->CallUp[E->Floor]==1||(E->Floor==1||But->CallDown[E->Floor]==1)&&c!=-1){
                E->E[2][1]=(i==7?14:23)*t;
            }break;
        case 9:E->D2=0;
            Controller(W,E,But,0);break;
    }




}

//void ClockAdjust(Queue **W,Elevator *E,Button *But){
//
//    for(i=1;i<=9;i++){
//        if(E->E[i][0]==1){
//            E->E[i][1]--;
//        }
//    }
//}
void PeopleProcess(Queue **W,Elevator *E,Button *But){
    //判断是否要往各楼层等待队列加人,并根据各楼层等待人状态调整楼层按钮，调整人的出入队状态
    //1.判断是否要加入人
    //2.遍历所有层的队列，并设置相应的电梯按钮
    int i=0;
    QNode *s;
    for(i=0;i<FloorNum;i++) {
        if(s=W[i]->front->next==W[i]->rear)continue;
        for (s = W[i]->front; s != W[i]->rear; s = s->next) {
            s->data->GivenUpTime-=t;
            s->data->InterTime-=t;//时钟流动
            if (s->data->GivenUpTime == 0) {//等待时间到时
                if (s->data->InFloor != E->Floor || E->D1 == 0) {//离开
                    DeletQueNode(W[i], s);
//                    if (s->data->InFloor > s->data->OutFloor) {//考虑到当离开的恰好是最后一个人时，按钮重新置0
//                        But->CallDown[s->data->OutFloor] = 0;
//                    } else {
//                        But->CallUp[s->data->OutFloor] = 0;
//                    }
                    continue;
                } else if (s->data->InFloor == E->Floor && E->D1 != 0) {//等待进入电梯
                    s->data->GivenUpTime = 0;
                    if (s->next == NULL) {//轮到他时，使其进入电梯
//                        if (s->data->InFloor > s->data->OutFloor) {//离开按钮重新置0
//                            But->CallDown[s->data->OutFloor] = 0;
//                        } else {
//                            But->CallUp[s->data->OutFloor] = 0;
//                        }
                    }
                }
            } else if (s->data->InterTime == 0) {//间隔时间结束，生成下一个人
                PersonRandGenAdd(W, But,E);
            } else {
                continue;
            }
        }
    }
}

//void ElevatorProcess(Queue **W,Elevator *E,Button *But){
//    int i=0;
//    QNode *s;
//    for(i=0;i<FloorNum;i++){//各楼层等待人的忍耐时间自减
//        for(s=W[i]->front;s!=W[i]->rear;s=s->next){
//            s->data->GivenUpTime--;
//            s->data->InterTime--;
//        }
//    }
//
//
//
//
//
//
//
//
//
//}



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
    InitStack(E->ElePeople[1]);
    Init(But,WaitingQue,E);//初始化系统
    PersonRandGenAdd(WaitingQue,But,E);//随机加入第一个人
    while(Time!=T){
        PeopleProcess(WaitingQue,E,But);
        ElevatorProcess(WaitingQue,E,But);
        Time++;
    }
}