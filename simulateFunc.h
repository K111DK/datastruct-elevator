//
// Created by 35802 on 2021/10/31.
//
#ifndef MAIN_C_SIMULATEFUNC_H
#define MAIN_C_SIMULATEFUNC_H
#include "BASICSTRUCT.h"

Person *PersonRandGenAdd(Queue **W,Button *button,Elevator *E,TimeLine *To,const int * Time){//（伪）随机地生成一个人加入到队列
    Person *a;
    a=(Person*)malloc(sizeof (Person));
    a->GivenUpTime= GenRand(100)*t;
    a->InFloor = GenRand(FloorNum)-1;
    a->OutFloor = GenRand(FloorNum)-1;
    a->InterTime = GenRand(MaxInterTime)*t;
    InsertTime(To,a->InterTime);
    while (a->OutFloor==a->InFloor){
        a->OutFloor = GenRand(FloorNum)-1;
    }
//    printf("INFO:\nGUtime:%f\nITtime:%f\n",a->GivenUpTime,a->InterTime);
    enQueue(W[a->InFloor],a);
    //将随机生成的人加入等待队列
    printf("people come in to queue.Time:%d\n", *Time);
    if(a->InFloor==E->Floor&&(E->Action[0]==3||E->Action[0]==4)){
        a->GivenUpTime=0;

        printf("ready to get in the elevator.Time:%d\n", *Time);
    }else{
        E->CallCar[a->InFloor]=1;
        if(a->InFloor>a->OutFloor){
            button->CallDown[a->OutFloor]=1;
        }else{
            button->CallUp[a->OutFloor]=1;
        }
        printf("press the button and wait.Time:%d\n", *Time);
    }
    return a;
}

int Controller(Queue **W,Elevator *E,Button *But,int flag,int *Time){
    int i=0;
    for(i=0;i<FloorNum;i++){
        if((But->CallDown[i]||But->CallUp[i]||E->CallCar[i])&&i!=E->Floor){
            return i;
        }
    }
    return 1;
}

void ElevatorProcess(Queue **W,Elevator *E,Button *But,int *Time){
    int f=0;
    while(1) {
        switch (E->Action[0]) {
            case 1://一楼等待
            if(E->Floor==1){
                printf("Elevator wait in first floor.Time:%d\n", *Time);}
                for (f=0; f < FloorNum; f++) {
                    if (E->CallCar[f] || But->CallDown[f] || But->CallUp[f]) {//当有按钮被按下时
                        if (E->Floor == f) {//直接开门
                            E->Action[0] = 3;
                            printf("ready to open the door.Time:%d\n", *Time);
                        } else {
                            E->Action[0] = 6;
                            printf("ready to move\n:%d", *Time);
                        }
                    }
                    else{
                        return;
                    }
                }
                break;
            case 2:
                if(E->Floor==1){
                    E->Action[0]=1;
                }else{
                    E->Action[0]=3;
                }
                break;
            case 3:
                if (E->Action[2] == -1) {//第一次进入开门状态
                    E->Action[2] = 20 * t;
                    E->Action[1] = 4;//次态为4
                    printf("door is openning.Time:%d\n", *Time);
                    return;
                }
                if (E->Action[2] == t) {//门完全打开
                    printf("door is openned.Time:%d\n", *Time);
                    E->Action[0] = 4;
                    E->Action[1] = -1;
                    E->Action[2] = -1;
                } else {//门正在打开
                    printf("door is openning.Time:%d\n", *Time);
                    E->Action[2] -= t;
                    return;
                }
                break;
            case 4:
                if (StackEmpty(E->ElePeople[E->Floor]) && QueueEmpty(W[E->Floor])) {//判断是否有人整蛊
                    E->D2 = 1;
                } else {
                    E->D2 = 0;
                }

                if (E->D2 == 1) {//有人整蛊时
                    printf("the elevator is idle now.Time:%d\n", *Time);
                    if (E->Action[2] == -1) {//初态开始计时
                        E->Action[1] = 5;
                        E->Action[2] = 70 * t;
                        return;
                    } else {
                        if (E->Action[2] == t) {//跳转到5准备回1层
                            E->Action[0] = 5;
                            E->Action[1] = -1;
                            E->Action[2] = -1;
                        } else {//
                            E->Action[2] -= t;
                            return;
                        }
                    }
                } else {
                    if (StackEmpty(E->ElePeople[E->Floor])) {//电梯
                        if (QueueEmpty(W[E->Floor])) {//门外
                            if (E->Action[2] == -1) {//无人进出，准备关门
                                E->Action[1] = 5;
                                E->Action[2] = 40 * t;
                                E->Action[2] -= t;
                                printf("door is closing.Time:%d\n", *Time);
                                return;
                            } else {
                                if (E->Action[2] == t) {
                                    E->Action[0] = 5;
                                    E->Action[2] = -1;
                                    printf("door is closed.Time:%d\n", *Time);
                                } else {
                                    E->Action[2] -= t;
                                    printf("door is closing.Time:%d\n", *Time);
                                    return;
                                }
                            }
                        } else {//有人进
                            if (E->Action[2] != -1) {
                                if (E->Action[2] == t) {//计时完毕
                                    QNode *p;
                                    p = DeQueue(W[E->Floor]);//出队
                                    E->CallCar[p->data->OutFloor]=1;//电梯内按钮按下
                                    Push(E->ElePeople[E->Floor], p->data);//把人压入电梯中
                                    printf("people enter the elevator.Time:%d\n", *Time);
                                    E->Action[2] = -1;
                                    return;
                                } else {
                                    E->Action[2] -= t;//继续计时
                                    return;
                                }
                            } else {//25t为周期开始计时
                                E->Action[2] = 25 * t;
                                E->Action[2] -= t;
                                return;
                            }
                        }
                    } else {//有人出
                        if (E->Action[2] != -1) {//非计时状态，对下一个人进行计时
                            if (E->Action[2] == t) {//计时完毕，将人从电梯栈中弹出
                                printf("people get out of elevator.Time:%d\n", *Time);
                                Pop(E->ElePeople[E->Floor]);
                                E->Action[2] = -1;//计时器归位
                                return;
                            } else {
                                E->Action[2] -= t;//计时
                                return;
                            }
                        } else {
                            E->Action[2] = 25 * t;//开始计时
                            E->Action[2] -= t;
                            return;
                        }
                    }
                }
                break;
            case 5:
                    E->Action[0] = 6;
                    E->Action[1] = -1;
                    E->Action[2] = -1;
                printf("elevator ready to move.Time:%d\n", *Time);
                break;
            case 6:
                if(E->D2==1&&E->D3!=1){//判断是否有人整蛊
                    printf("the elevator is idling.Time:%d\n", *Time);
                if(E->Action[2]==-1){//开始静候
                    E->Action[2]=300*t;
                    E->Action[1]=1;
                    E->Action[2]-=t;
                    return;
                }
                else{
                    if (!QueueEmpty(W[E->Floor])) {
                        E->Action[0]=3;
                        E->Action[1]=4;
                        E->Action[2]=20*t;
                        E->Action[2]-=t;
                        return;
                    }
                    if(E->Action[2]==t){
                        E->CallCar[1]=1;
                        E->Action[2]=-1;
                        E->D3=1;//回1楼
                        return;
                    }
                    else{
                        E->Action[2]-=t;
                        return;
                    }
                }
                }else{//准备移动
                    printf("the elevator is ready to move.Time:%d\n", *Time);
                        E->CallCar[Controller(W, E, But, 0, Time)] = 1;
                        if(Controller(W,E,But,0,Time)>E->Floor){
                            if(E->Action[2]==-1) {
                                E->Action[1] = 7;
                                E->Action[2] = 15 * t;
                                E->Action[2] -= t;
                                return;
                            }
                            else{
                                if(E->Action[2]==t){
                                    E->Action[0]=7;
                                    E->Action[1]=-1;
                                    E->Action[2]=-1;
                                    return;
                                }
                                else{
                                    E->Action[2]-=t;
                                    return;
                                }
                            }
                        }else{
                            if(E->Action[2]==-1) {
                                E->Action[1] = 8;
                                E->Action[2] = 15 * t;
                                E->Action[2] -= t;
                                return;
                            }
                            else{
                                if(E->Action[2]==t){
                                    E->Action[0]=8;
                                    E->Action[1]=-1;
                                    E->Action[2]=-1;
                                    return;
                                }
                                else{
                                    E->Action[2]-=t;
                                    return;
                                }
                            }
                        }
                }
            case 7:
                printf("the elevator is moving up.Time:%d\n", *Time);
                if(E->Action[2]==-1){
                    E->Floor++;}
                    if(E->CallCar[E->Floor+1]||But->CallUp[E->Floor+1]||But->CallDown[E->Floor+1]){
                        printf("the elevator is going to stop.Time:%d\n", *Time);
                        if(E->Action[2]==-1) {
                            E->Action[1] = 2;
                            E->Action[2] = 14 * t;
                            E->Action[2] -= t;
                            return;
                        }
                        else{
                            if(E->Action[2]==t){
                                E->Action[0]=2;
                                E->Action[1]=-1;
                                E->Action[2]=-1;
                                return;
                            }
                            else{
                                E->Action[2]-=t;
                                return;
                            }
                        }
                    }
                    else{
                        if(E->Action[2]==-1) {
                            E->Action[1] = 7;
                            E->Action[2] = 51 * t;
                            E->Action[2] -= t;
                            return;
                        }
                        else{
                            if(E->Action[2]==t){
                                E->Action[0]=7;
                                E->Action[1]=-1;
                                E->Action[2]=-1;
                                return;
                            }
                            else{
                                E->Action[2]-=t;
                                return;
                            }
                        }
                    }
            case 8:
                printf("the elevator is moving down.Time:%d\n", *Time);
                if(E->Action[2]==-1){
                    E->Floor--;}
                if(E->CallCar[E->Floor-1]||But->CallUp[E->Floor-1]||But->CallDown[E->Floor-1]){
                    printf("the elevator is going to stop.Time:%d\n", *Time);
                    if(E->Action[2]==-1) {
                        E->Action[1] = 2;
                        E->Action[2] = 23 * t;
                        E->Action[2] -= t;
                        return;
                    }
                    else{
                        if(E->Action[2]==t){
                            E->Action[0]=2;
                            E->Action[1]=-1;
                            E->Action[2]=-1;
                            return;
                        }
                        else{
                            E->Action[2]-=t;
                            return;
                        }
                    }
                }
                else{
                    if(E->Action[2]==-1) {
                        E->Action[1] = 8;
                        E->Action[2] = 61 * t;
                        E->Action[2] -= t;
                        return;
                    }
                    else{
                        if(E->Action[2]==t){
                            E->Action[0]=8;
                            E->Action[1]=-1;
                            E->Action[2]=-1;
                            return;
                        }
                        else{
                            E->Action[2]-=t;
                            return;
                        }
                    }
                }
            default:
                return;
        }
    }
}

void PeopleProcess(Queue **W,Elevator *E,Button *But,TimeLine *To,int* Time){
    int i;
    for(i=0;i<FloorNum;i++){
        QNode *node;
        node=W[i]->front->next;
        while(node!=NULL){
            node->data->InFloor;
            if(node->data==NULL)break;
            if(node->data->InFloor==E->Floor&&(E->Action[0]==4||E->Action[0]==3)){
                node->data->GivenUpTime=0;
                printf("ready to get in the elevator.Time:%d\n", *Time);
                break;
            }else{
                if(node->data->GivenUpTime==t){
                    DeletQueNode(W[i],node);
                    printf("people leave the queue.Time:%d\n", *Time);
                }
                else{
                    node->data->GivenUpTime-=t;
                    printf("the people is waiting in line.Time:%d\n", *Time);
                }
            }
            node=node->next;
        }
    }
    i= DeletTime(To);
    while(i){
        printf("persongen\n");
    PersonRandGenAdd(W,But,E,To,Time);
    i--;
    }
}

#endif //MAIN_C_SIMULATEFUNC_H
