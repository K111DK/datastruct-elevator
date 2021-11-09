//
// Created by 35802 on 2021/10/31.
//
#ifndef MAIN_C_SIMULATEFUNC_H
#define MAIN_C_SIMULATEFUNC_H
#include "BASICSTRUCT.h"

Person *PersonRandGenAdd(Queue **W,Button *button,Elevator *E,TimeLine *To,const int * Time){//（伪）随机地生成一个人加入到队列
    Person *a;
    a=(Person*)malloc(sizeof (Person));
    a->GivenUpTime= GenRand(100)*t+40*t;
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
    printf("People info:\nGivenUp Time:%d\nInFloor:%d\nOutFloor:%d\nInterTime:%d\n",a->GivenUpTime,a->InFloor,a->OutFloor,a->InterTime);
    if(a->InFloor==E->Floor&&(E->Action[0]==3||E->Action[0]==4)){
        a->GivenUpTime=0;
        printf("ready to get in the elevator.Time:%d\n", *Time);
    }
        E->CallCar[a->InFloor]=1;
        if(a->InFloor>a->OutFloor){
            button->CallDown[a->InFloor]=1;
        }else{
            button->CallUp[a->InFloor]=1;
        }
        printf("press the button and wait.Time:%d\n", *Time);
    return a;
}

int Controller(Queue **W,Elevator *E,Button *But,int flag,int *Time){
    int i=0;
    if(E->State==GoingBack){
        E->CallCar[1]=1;
        return 1;
    }
//        while(i<FloorNum){
//            if(!((But->CallDown[i]||But->CallUp[i]||E->CallCar[i])&&i!=E->Floor)&&i!=E->Floor){
//                E->State=Idle;
//                return 1;
//            }
//            i++;
//        }
    if(E->State==GoingUp){
        for(i=E->Floor+1;i<FloorNum;i++){
            if((But->CallDown[i]||But->CallUp[i]||E->CallCar[i])&&i!=E->Floor){
                return i;
            }
        }
        E->State=GoingDown;
    }
    if(E->State==GoingDown){
        for(i=E->Floor-1;i>=0;i--){
            if((But->CallDown[i]||But->CallUp[i]||E->CallCar[i])&&i!=E->Floor){
                return i;
            }
        }
        E->State=GoingUp;
        return Controller(W,E,But,flag,Time);
    }
}

void ElevatorProcess(Queue **W,Elevator *E,Button *But,int *Time){
    int f=0;
    int i=0;
    while(1) {
        printf("case :%d triggered\n",E->Action[0]);
        switch (E->Action[0]) {
            case 1://一楼等待
            if(E->Floor==1){
                printf("Elevator wait in first floor.Time:%d\n", *Time);}
                for (f=0; f < FloorNum; f++) {//遍历是否若有人按按钮
                    if (E->CallCar[f] || But->CallDown[f] || But->CallUp[f]) {//当有按钮被按下时
                        if (E->Floor == f) {//若在第一层则直接开门
                            E->Action[0] = 3;
                            printf("ready to open the door.Time:%d\n", *Time);
                            break;
                        } else {//否则开始判断目标层
                            E->Action[0] = 6;
                            printf("ready to move\n:%d", *Time);
                            break;
                        }
                    }
                }
                if(f==FloorNum)return;
                break;
            case 2:
                if(E->State==GoingUp){//到达最高层换方向
                    if(E->Floor==FloorNum-1){
                        E->State==GoingDown;
                    }
                }
                if(E->State==GoingDown){//换方向
                    if(E->Floor==0){
                        E->State==GoingUp;
                    }
                }
                if(E->Floor==1&&E->State==GoingBack){
                    E->Action[0]=1;
                    E->Action[1]=-1;
                    E->Action[2]=-1;
                    E->State=Idle;
                }else{
                    E->Action[0]=3;
                }
                break;
            case 3://开门
                if (E->Action[2] == -1) {//第一次进入开门状态
                    E->Action[2] = DoorOperTime * t;
                    E->Action[1] = 4;//次态为4
                    printf("door is openning.Time:%d\n", *Time);
                    return;
                }
                else{//开门中间态
                    if (E->Action[2] == t) {//门完全打开
                    printf("door is openned.Time:%d\n", *Time);
                    E->Action[0] = 4;//让人进出
                    E->Action[1] = -1;
                    E->Action[2] = -1;
                        return;
                    } else {//门正在打开
                    printf("door is openning.Time:%d\n", *Time);
                    E->Action[2] -= t;
                        return;
                    }
                }
                break;
            case 4://让人进出
//                if (StackEmpty(E->ElePeople[E->Floor])&& QueueEmpty(W[E->Floor])) {//判断是否有人整蛊，按了当层按钮但当层没人且没人要下这层
//                    E->D2 = 1;
//                } else {
//                    E->D2 = 0;
//                }
                if (E->D2 == 1) {//有人整蛊时，等待70t后关门
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
                            return;
                        } else {
                            E->Action[2] -= t;//计时
                            return;
                        }
                    }
                } else {//无人整蛊，正常出入
                    if (StackEmpty(E->ElePeople[E->Floor])) {//电梯人出来完了
                        printf("out done\n");
                        if (QueueEmpty(W[E->Floor])) {//门外人进来完了
                            if (E->Action[2] == -1) {//此时无人进出，准备关门
                                E->Action[1] = 5;
                                E->Action[2] = DoorOperTime * t;
                                E->Action[2] -= t;
                                printf("door is closing.Time:%d\n", *Time);
                                E->CallCar[E->Floor]=0;
                                But->CallDown[E->Floor]=0;
                                But->CallUp[E->Floor]=0;
                                return;
                            } else {
                                if (E->Action[2] == t) {//门已关上
                                    E->Action[0] = 5;
                                    E->Action[2] = -1;
                                    E->CallCar[E->Floor]=0;
                                    But->CallDown[E->Floor]=0;
                                    But->CallDown[E->Floor]=0;
                                    printf("door is closed.Time:%d\n", *Time);
                                    return;
                                } else {
                                    E->Action[2] -= t;
                                    printf("door is closing.Time:%d\n", *Time);
                                    return;
                                }
                            }
                        } else {//门外还有人进
                            if (E->Action[2] != -1) {
                                if (E->Action[2] == t) {//计时完毕
                                    QNode *p;
                                    p = DeQueue(W[E->Floor]);//当层人出队
                                    E->CallCar[p->data->OutFloor]=1;//电梯内目标楼层按钮按下
                                    Push(E->ElePeople[p->data->OutFloor], p->data);//把人压入电梯中
                                    printf("people entered the elevator.Time:%d\n", *Time);
                                    E->Action[2] = -1;
                                    return;
                                } else {
                                    E->Action[2] -= t;//继续计时
                                    printf("people entering.Time:%d\n", *Time);
                                    return;
                                }
                            } else {//进门初态25t为周期开始计时
                                printf("people entering.Time:%d\n", *Time);
                                E->Action[2] = InOutTime * t;
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
                                printf("people outing\n");
                                E->Action[2] -= t;//计时
                                return;
                            }
                        } else {
                            printf("people outing\n");
                            E->Action[2] = InOutTime * t;//开始计时
                            E->Action[2] -= t;
                            return;
                        }
                    }
                }
                break;
            case 5:
                    for(i=0;i<FloorNum;i++){
                        if((But->CallDown[i]||But->CallUp[i]||E->CallCar[i])){
                            if(i==E->Floor){
                                E->Action[2] = -1;
                                E->Action[1] = -1;
                                E->Action[0] = 3;
                                break;
                            }
                            else{
                                E->Action[2] = -1;
                                E->Action[1] = -1;
                                E->Action[0] = 6;
                                printf("elevator ready to move.Time:%d\n", *Time);
                                break;
                            }
                        }
                    }
                if(i!=FloorNum)break;
                    if(E->Action[2]==-1){
                        E->Action[2]=IdleTime*t;
                        E->Action[2]-=t;
                        printf("the elevator is idling\n");
                    }else{
                        if(E->Action[2]==t){
                            E->Action[2]=-1;
                            E->Action[1]=-1;
                            E->Action[0]=6;
                            E->State=GoingBack;
                            printf("ready to reaturn to the first floor\n");
                        }else{
                            E->Action[2]-=t;
                        }
                    }
                break;
            case 6:
                if(E->State==Idle&&E->Floor==1){
                    E->State=GoingUp;
                }
                if(E->D2==1&&E->D3!=1){//判断是否有人整蛊
                    printf("the elevator is idling.Time:%d\n", *Time);
                if(E->Action[2]==-1){//开始静候
                    E->Action[2]=IdleTime*t;
                    E->Action[1]=1;
                    E->Action[2]-=t;
                    return;
                }
                else{
                    if (!QueueEmpty(W[E->Floor])) {
                        E->Action[0]=3;
                        E->Action[1]=-1;
                        E->Action[2]=-1;
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
                }
                //准备移动
                        E->CallCar[Controller(W, E, But, 0, Time)] = 1;//找到目标层
                        if(Controller(W,E,But,0,Time)>E->Floor){//往上
                            if(E->Action[2]==-1) {//加速态
                                if(E->State==Idle){
                                    E->State=GoingUp;
                                }
                                E->Action[1] = 7;
                                E->Action[2] = AccerlerTime * t;
                                E->Action[2] -= t;
                                printf("the elevator is ready to move up.Time:%d\n", *Time);
                                return;
                            }
                            else{
                                if(E->Action[2]==t){//加速完毕开始进入上升循环
                                    E->Action[0]=7;
                                    E->Action[1]=-1;
                                    E->Action[2]=-2;//置1是保证到达下一状态时层数不会直接变换
                                    return;
                                }
                                else{
                                    E->Action[2]-=t;
                                    return;
                                }
                            }
                        }else{//往下
                            if(E->Action[2]==-1) {
                                if(E->State==Idle){
                                    E->State=GoingDown;
                                }
                                E->Action[1] = 8;
                                E->Action[2] = AccerlerTime * t;
                                E->Action[2] -= t;
                                return;
                            }
                            else{
                                if(E->Action[2]==t){//加速完毕进入下降循环
                                    printf("the elevator is ready to move down.Time:%d\n", *Time);
                                    E->Action[0]=8;
                                    E->Action[1]=-1;
                                    E->Action[2]=-2;//置1是保证到达下一状态时层数不会直接变换
                                    return;
                                }
                                else{
                                    E->Action[2]-=t;
                                    return;
                                }
                            }
                        }
            case 7://上升循环
                if(E->Action[2]==-1){//初态表示已进行了依次循环，floor++
                    E->Floor++;}
                    if(E->CallCar[E->Floor+1]||But->CallUp[E->Floor+1]||But->CallDown[E->Floor+1]){//此时(在本层和下一层间运动)若下一层要进出，则开始减速
                        if(E->Action[2]==-1||E->Action[2]==-2) {//减速初态
                            E->Action[1] = 2;
                            E->Action[2] = deccerlerTime * t;
                            E->Action[2] -= t;
                            printf("the elevator is going to stop.Time:%d\n", *Time);
                            return;
                        }
                        else{//减速完毕，回到状态2，即停顿判断态
                            if(E->Action[2]==t){
                                E->Action[0]=2;
                                E->Action[1]=-1;
                                E->Action[2]=-1;
                                E->Floor+=1;
                                printf("the elevator is stopped.Time:%d\n", *Time);
                                return;
                            }
                            else{
                                E->Action[2]-=t;
                                return;
                            }
                        }
                    }
                    else{//否则继续加速至下一层（到达下一层的时候停不下来）
                        if(E->Action[2]==-1||E->Action[2]==-2) {
                            E->Action[1] = 7;
                            E->Action[2] = UpingTime * t;
                            E->Action[2] -= t;
                            printf("the elevator is moving up.Time:%d\n", *Time);
                            return;
                        }
                        else{
                            if(E->Action[2]==t){
                                E->Action[0]=7;
                                E->Action[1]=-1;
                                E->Action[2]=-1;//调回初态
                                return;
                            }
                            else{
                                E->Action[2]-=t;
                                return;
                            }
                        }
                    }
            case 8:
                if(E->Action[2]==-1){//同上
                    E->Floor--;}
                if(E->CallCar[E->Floor-1]||But->CallUp[E->Floor-1]||But->CallDown[E->Floor-1]){
                    if(E->Action[2]==-1||E->Action[2]==-2) {
                        E->Action[1] = 2;
                        E->Action[2] = deccerlerTime * t;
                        E->Action[2] -= t;
                        printf("the elevator is going to stop.Time:%d\n", *Time);
                        return;
                    }
                    else{
                        if(E->Action[2]==t){
                            E->Action[0]=2;
                            E->Action[1]=-1;
                            E->Action[2]=-1;
                            printf("the elevator is stopped\n");
                            E->Floor-=1;
                            return;
                        }
                        else{
                            E->Action[2]-=t;
                            return;
                        }
                    }
                }
                else{
                    if(E->Action[2]==-1||E->Action[2]==-2) {
                        E->Action[1] = 8;
                        E->Action[2] = DowningTime * t;
                        E->Action[2] -= t;
                        printf("the elevator is moving down.Time:%d\n", *Time);
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
        printf("1 person gen\n");
    PersonRandGenAdd(W,But,E,To,Time);
    i--;
    }
}

#endif //MAIN_C_SIMULATEFUNC_H
