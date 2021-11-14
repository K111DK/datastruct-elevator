//
// Created by 35802 on 2021/10/31.
//
#ifndef MAIN_C_SIMULATEFUNC_H
#define MAIN_C_SIMULATEFUNC_H
#include "BASICSTRUCT.h"
int IfOverCross(Button *B,Elevator *E);
Person *PersonRandGenAdd(Queue **W,Button *button,Elevator **E,TimeLine *To,const int * Time){//（伪）随机地生成一个人加入到队列
    Person *a;
    a=(Person*)malloc(sizeof (Person));
    a->GivenUpTime= GenRand(100)*t+40*t;
    a->InFloor = GenRand(FloorNum)-1;
    a->OutFloor = GenRand(FloorNum)-1;
    a->InterTime = GenRand(MaxInterTime)*t;
    a->code=*Time;
    a->flag[0]=0;
    a->flag[1]=-1;
    InsertTime(To,a->InterTime);
    while (a->OutFloor==a->InFloor){
        a->OutFloor = GenRand(FloorNum)-1;
    }
//    printf("INFO:\nGUtime:%f\nITtime:%f\n",a->GivenUpTime,a->InterTime);
    enQueue(W[a->InFloor],a);
    //将随机生成的人加入等待队列
    printf("新的人No:%d进入第%d层排队队列.\n", a->code, a->InFloor);
    if(GenDetail){
        printf("-----------------\n人物信息:\n最大等待时间:%d\n起始层:%d\n目的层:%d\n下一个人到达时间:%d\n序号:%d\n-----------------\n",
               a->GivenUpTime, a->InFloor, a->OutFloor, a->InterTime, a->code);
    }
    if(a->InFloor==E[0]->Floor&&(E[0]->Action[0]==3||E[0]->Action[0]==4)||a->InFloor==E[1]->Floor&&(E[1]->Action[0]==3||E[1]->Action[0]==4)){
        a->GivenUpTime=0;
        printf("电梯就在本层且未离开,No:%d 准备进入电梯.\n",a->code);
    }
        E[0]->CallCar[a->InFloor]=1;
        E[1]->CallCar[a->InFloor]=1;
        if(a->InFloor>a->OutFloor){
            button->CallDown[a->InFloor]=1;
        }else{
            button->CallUp[a->InFloor]=1;
        }
        printf("No:%d 按下按钮并在队列中等待.\n",a->code);
    return a;
}

int Controller(Queue **W,Elevator **Ele,Button *But,int ele,int *Time){
    Elevator *E;
    E=Ele[ele];
    int i=0;
        for(i=0;i<FloorNum;i++){
            if((But->CallDown[i]||But->CallUp[i]||E->CallCar[i])&&i!=E->Floor){
                if(E->State==Idle){
                if(i>E->Floor){
                    E->State=GoingUp;
                }else{
                    E->State=GoingDown;
                }
                return i;
                }
                else {
                    break;
                }
            }
        }
        if(i==FloorNum)E->State=GoingBack;
    if(E->State==GoingBack){
        for(i=0;i<FloorNum;i++){
            if((But->CallDown[i]||But->CallUp[i]||E->CallCar[i])&&i!=E->Floor){
                break;
            }
        }
        if(i==FloorNum) {
            E->CallCar[1] = 1;
            return 1;
        }else{
            if(i>E->Floor){
                E->State=GoingUp;
                return i;
            }else{
                E->State=GoingDown;
            }
        }
    }
    if(E->State==GoingUp){
        if(E->Floor==4){
            E->State=GoingDown;
        }else {
            for (i = E->Floor + 1; i < FloorNum; i++) {
                if ((But->CallDown[i] || But->CallUp[i] || E->CallCar[i]) && i != E->Floor) {
                    return i;
                }
            }
            E->State = GoingDown;
        }
    }
    if(E->State==GoingDown){
        if(E->Floor==0){
            E->State=GoingUp;
        }
        else{
        for(i=E->Floor-1;i>=0;i--){
            if((But->CallDown[i]||But->CallUp[i]||E->CallCar[i])&&i!=E->Floor){
                return i;
            }
        }
        }
        E->State=GoingUp;
        return Controller(W,Ele,But,ele,Time);
    }
    return -1;
}

int TotalCall(Button*But){
    int num=0;
    int i=0;
    for(i=0;i<FloorNum;i++){
        if(But->CallUp[i]||But->CallDown[i]){
            num++;
        }
    }
    return num;
}

int IfOverCross(Button*But,Elevator*E){
    int i=0;
    if(E->State==GoingUp){
        for(i=E->Floor;i>=0;i--){
            if((But->CallDown[i]||But->CallUp[i])){
                if(i==E->Floor&&(E->Action[0]<=5)){
                    break;
                }else{
                    return 1;
                }
            }
        }
    }else if(E->State==GoingDown){
        for(i=E->Floor;i<FloorNum;i++){
            if((But->CallDown[i]||But->CallUp[i])){
                if(i==E->Floor&&(E->Action[0]<=5)){
                    break;
                }else{
                    return 1;
                }
            }
        }
    }else{

    }
    return 0;
}


void ElevatorProcess(Queue **W,Elevator **Ele,Button *But,int *Time,int ele){
    int f=0;
    int i=0;
    Elevator *E;
    E=Ele[ele];
    printf("ele:%d   totalcall:%d   ifovercross:%d\n",ele,TotalCall(But),IfOverCross(But,Ele[1-ele]));
    while(1) {
        switch (E->Action[0]) {
            case 1://一楼等待
            if(ele==1){
                if(TotalCall(But)<=1||!IfOverCross(But,Ele[1-ele])){
                    printf("%d",TotalCall(But)<=1||!IfOverCross(But,Ele[1-ele]));
                    return;
                }
                }
                printf("电梯在1楼等待.Time:%d\n", *Time);
                f= Controller(W,Ele,But,ele,Time);
                if(f!=-1) {
                    if (E->Floor == f) {//若在第一层则直接开门
                        E->Action[0] = 3;
                        printf("准备开门.Time:%d\n", *Time);
                        break;
                    } else {//否则开始判断目标层
                        E->Action[0] = 6;
                        printf("准备移动.Time:%d\n", *Time);
                        break;
                    }
                }
                return;
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
                    E->Action[2] -= t;
                    E->Action[1] = 4;//次态为4
                    printf("开始开门.Time:%d\n", *Time);
                    return;
                }
                else{//开门中间态
                    if (E->Action[2] == t) {//门完全打开
                    printf("门已打开.Time:%d\n", *Time);
                    E->Action[0] = 4;//让人进出
                    E->Action[1] = -1;
                    E->Action[2] = -1;
                        return;
                    } else {//门正在打开
                    printf("正在开门.Time:%d\n", *Time);
                    E->Action[2] -= t;
                        return;
                    }
                }
                break;
            case 4://让人进出
                if (!StackEmpty(E->ElePeople[E->Floor])||!QueueEmpty(W[E->Floor])||(E->Action[2]!=-1&&E->D1==0)) {
                    if((E->Action[2]!=-1&&E->D1==0)){
                        E->Action[3]=E->Action[2];
                    }
                    E->D1=1;//有人进出时,进行40t计时,当进出状态保持，40t会不断刷新，进出完毕时，40t会直接进入以下倒数状态
                    if(E->Action[3]==-1||E->Action[3]==t){
                        E->Action[3]=DetectTime*t;
                    }else{
                        E->Action[3]-=t;
                    }
                }
                //无人整蛊，正常出入
                    if (StackEmpty(E->ElePeople[E->Floor])) {//电梯人出来完了
                        printf("电梯内要出来的人已全部出来\n");
                        if (QueueEmpty(W[E->Floor])|| (QueueSize(W[E->Floor])==1&&W[E->Floor]->front->next->data->flag[1]==1-ele)){//门外人进来完了
                            E->D2=1;
                            if (E->Action[2] == -1&&E->Action[3]==-1) {//此时无人进出，准备关门 有人进出的40t计时的剩余部分会直接在这部分继续
                                E->Action[1] = 5;
                                E->Action[2] = DetectTime * t;
                                E->Action[2] -= t;
                                printf("电梯准备关门\n");
                                return;
                            } else {
                                if(E->Action[3]!=-1&&E->D1==1){
                                    E->Action[2]=E->Action[3];
                                    E->D1=0;
                                }
                                if (E->Action[2] == t) {//门已关上
                                    E->Action[0] = 5;
                                    E->Action[2] = -1;
                                    E->Action[3] = -1;
                                    E->CallCar[E->Floor]=0;
                                    But->CallDown[E->Floor]=0;
                                    But->CallUp[E->Floor]=0;
                                    E->D2==0;
                                    printf("电梯准备关门\n");
                                    return;
                                } else {
                                    E->Action[2] -= t;
                                    printf("电梯准备关门\n");
                                    return;
                                }
                            }
                        } else {//门外还有人进
                            if(E->D2==1&&E->Action[2]!=-1){
                                E->Action[3]=E->Action[2]-t;
                                E->Action[2]=-1;
                                E->D2=0;
                            }
                            if (E->Action[2] != -1) {
                                if (E->Action[2] == t) {//计时完毕
                                    QNode *p;
                                    //当层人出队
                                    printf("队头:No:%d",W[E->Floor]->front->next->data->code);
                                    p = DeQueue(W[E->Floor]);
                                    E->CallCar[p->data->OutFloor]=1;//电梯内目标楼层按钮按下
                                    Push(E->ElePeople[p->data->OutFloor], p->data);//把人压入电梯中
                                    printf("No:%d已进入电梯%d.Time:%d\n",p->data->code,ele,*Time);
                                    p->data->flag[0]=-1;
                                    E->Action[2] = -1;
                                    return;
                                } else {
                                    QNode *q=W[E->Floor]->front->next->data->flag[1]==ele?W[E->Floor]->front->next:W[E->Floor]->front->next->next;
                                    E->Action[2] -= t;//继续计时
                                    printf("No:%d正在进入电梯%d.Time:%d\n",q->data->code,ele,*Time);
                                    return;
                                }
                            } else {//进门初态25t为周期开始计时
                                QueueEmpty(W[E->Floor]);
                                QNode *Wnode;
                                Wnode=W[E->Floor]->front->next;
                                if(Wnode->data->flag[1]!=1-ele){
                                Wnode->data->flag[0]=1;
                                Wnode->data->flag[1]=ele;
                                printf("No:%d正在进入电梯%d.Time:%d\n",Wnode->data->code,ele,*Time);
                                E->Action[2] = InOutTime * t;
                                E->Action[2] -= t;
                                return;}
                                }
                        }
                    } else {//有人出
                        if (E->Action[2] != -1) {//非计时状态，对下一个人进行计时
                            if (E->Action[2] == t) {//计时完毕，将人从电梯栈中弹出
                                E->ElePeople[E->Floor]->top->data->flag[0]=-2;
                                printf("No:%d已离开电梯.Time:%d\n",Pop(E->ElePeople[E->Floor])->code,*Time);
                                E->Action[2] = -1;//计时器归位
                                return;
                            } else {
                                printf("No:%d正在离开电梯\n",E->ElePeople[E->Floor]->top->data->code);
                                E->Action[2] -= t;//计时
                                return;
                            }
                        } else {
                            printf("No:%d正在离开电梯\n",E->ElePeople[E->Floor]->top->data->code);
                            E->ElePeople[E->Floor]->top->data->flag[0]=2;
                            E->Action[2] = InOutTime * t;//开始计时
                            E->Action[2] -= t;
                            return;
                        }
                    }
                break;
            case 5:
                if (StackEmpty(E->ElePeople[E->Floor])&&QueueEmpty(W[E->Floor])){
                    if(E->Action[2]==-1){
                        E->Action[2]=DoorOperTime*t;
                        E->Action[2]-=t;
                        printf("正在关门.Time:%d\n", *Time);
                        return;
                    }else if(E->Action[2]==0){
                        printf("门已关上\n");
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
                                    printf("电梯准备移动.Time:%d\n", *Time);
                                    break;
                                }
                            }
                        }
                        if(i!=FloorNum)break;
                        if(E->Action[2]==-1){
                            E->Action[2]=IdleTime*t;
                            E->Action[2]-=t;
                            E->State=Idle;
                            printf("电梯在静置等待\n");
                        }else{
                            if(E->Action[2]==t){
                                E->Action[2]=-1;
                                E->Action[1]=-1;
                                E->Action[0]=6;
                                E->State=GoingBack;
                                printf("准备返回第一层\n");
                            }else{
                                E->Action[2]-=t;
                            }
                        }break;
                    }else{
                        E->Action[2]-=t;
                        printf("正在关门.Time:%d\n", *Time);
                        return;
                    }
                }else{
                        E->Action[0]=3;
                        break;
                }
                break;
            case 6:
                //准备移动
                        E->CallCar[Controller(W, Ele, But, ele, Time)] = 1;//找到目标层
                        if(Controller(W,Ele,But,ele,Time)>E->Floor){//往上
                            if(E->Action[2]==-1) {//加速态
                                E->Action[1] = 7;
                                E->Action[2] = AccerlerTime * t;
                                E->Action[2] -= t;
                                printf("电梯开始加速.Time:%d\n", *Time);
                                return;
                            }
                            else{
                                if(E->Action[2]==t){//加速完毕开始进入上升循环
                                    E->Action[0]=7;
                                    E->Action[1]=-1;
                                    E->Action[2]=-2;//置1是保证到达下一状态时层数不会直接变换
                                    printf("加速完毕,准备上升\n");
                                    return;
                                }
                                else{
                                    E->Action[2]-=t;
                                    printf("电梯正在加速\n");
                                    return;
                                }
                            }
                        }else{//往下
                            if(E->Action[2]==-1) {
                                E->Action[1] = 8;
                                E->Action[2] = AccerlerTime * t;
                                E->Action[2] -= t;
                                printf("电梯开始加速.Time:%d\n", *Time);
                                return;
                            }
                            else{
                                if(E->Action[2]==t){//加速完毕进入下降循环
                                    printf("电梯加速完毕,准备下降:%d\n", *Time);
                                    E->Action[0]=8;
                                    E->Action[1]=-1;
                                    E->Action[2]=-2;//置1是保证到达下一状态时层数不会直接变换
                                    return;
                                }
                                else{
                                    E->Action[2]-=t;
                                    printf("电梯正在加速\n");
                                    return;
                                }
                            }
                        }
            case 7://上升循环
                    if(E->CallCar[E->Floor+1]||But->CallUp[E->Floor+1]||But->CallDown[E->Floor+1]){//此时(在本层和下一层间运动)若下一层要进出，则开始减速
                        if(E->Action[2]==-1||E->Action[2]==-2) {//减速初态
                            E->Action[1] = 2;
                            E->Action[2] = deccerlerTime * t;
                            E->Action[2] -= t;
                            printf("电梯开始减速:%d\n", *Time);
                            return;
                        }
                        else{//减速完毕，回到状态2，即停顿判断态
                            if(E->Action[2]==t){
                                E->Action[0]=2;
                                E->Action[1]=-1;
                                E->Action[2]=-1;
                                E->Floor++;
                                printf("电梯已停止.Time:%d\n", *Time);
                                return;
                            }
                            else{
                                E->Action[2]-=t;
                                printf("电梯正在减速:%d\n", *Time);
                                return;
                            }
                        }
                    }
                    else{//否则继续加速至下一层（到达下一层的时候停不下来）
                        if(E->Action[2]==-1||E->Action[2]==-2) {
                            E->Action[1] = 7;
                            E->Action[2] = UpingTime * t;
                            E->Action[2] -= t;
                            printf("电梯正在上升.Time:%d\n", *Time);
                            return;
                        }
                        else{
                            if(E->Action[2]==t){
                                E->Action[0]=7;
                                E->Action[1]=-1;
                                E->Action[2]=-1;//调回初态
                                printf("电梯上升了一层.Time:%d\n", *Time);
                                E->Floor++;
                                return;
                            }
                            else{
                                E->Action[2]-=t;
                                printf("电梯正在上升.Time:%d\n", *Time);
                                return;
                            }
                        }
                    }
            case 8:
                if(E->CallCar[E->Floor-1]||But->CallUp[E->Floor-1]||But->CallDown[E->Floor-1]){//此时(在本层和下一层间运动)若下一层要进出，则开始减速
                    if(E->Action[2]==-1||E->Action[2]==-2) {//减速初态
                        E->Action[1] = 2;
                        E->Action[2] = deccerlerTime * t;
                        E->Action[2] -= t;
                        printf("电梯开始减速:%d\n", *Time);
                        return;
                    }
                    else{//减速完毕，回到状态2，即停顿判断态
                        if(E->Action[2]==t){
                            E->Action[0]=2;
                            E->Action[1]=-1;
                            E->Action[2]=-1;
                            E->Floor--;
                            printf("电梯已停止.Time:%d\n", *Time);
                            return;
                        }
                        else{
                            E->Action[2]-=t;
                            printf("电梯正在减速:%d\n", *Time);
                            return;
                        }
                    }
                }
                else{//否则继续加速至下一层（到达下一层的时候停不下来）
                    if(E->Action[2]==-1||E->Action[2]==-2) {
                        E->Action[1] = 8;
                        E->Action[2] = DowningTime * t;
                        E->Action[2] -= t;
                        printf("电梯正在下降.Time:%d\n", *Time);
                        return;
                    }
                    else{
                        if(E->Action[2]==t){
                            E->Action[0]=8;
                            E->Action[1]=-1;
                            E->Action[2]=-1;//调回初态
                            printf("电梯下降了一层.Time:%d\n", *Time);
                            E->Floor--;
                            return;
                        }
                        else{
                            E->Action[2]-=t;
                            printf("电梯正在下降.Time:%d\n", *Time);
                            return;
                        }
                    }
                }
            default:
                return;
        }
    }
}

void PeopleProcess(Queue **W,Elevator **Ele,Button *But,TimeLine *To,int* Time){
    int i;
    int j;
    Elevator *E;
    for(j=0;j<2;j++){
        E=Ele[j];
    printf("各楼层人群:\n");
    for(i=0;i<FloorNum;i++){
        QNode *node;
        node=W[i]->front->next;
        while(node!=NULL){
            node->data->InFloor;
            if(node->data==NULL)break;
            switch (node->data->flag[0]) {
                case 1:
                    printf("No:%d正在进入电梯.Time:%d\n",node->data->code ,*Time);break;
                case -1:
                    printf("No:%d已经进入电梯.Time:%d\n",node->data->code ,*Time);break;
                default:
                    if(node->data->InFloor==E->Floor&&(E->Action[0]==4||E->Action[0]==3||E->Action[0]==1)){
                        node->data->GivenUpTime=0;
                        printf("No:%d准备进入电梯.Time:%d\n",node->data->code ,*Time);
                        break;
                    }else{
                        if(node->data->GivenUpTime==t){
                            DeletQueNode(W[i],node);
                            printf("No:%d离开排队队列.Time:%d\n",node->data->code,*Time);
                        }
                        else{
                            node->data->GivenUpTime-=t;
                            printf("No:%d正在排队等待,剩余等待时间:%d.Time:%d\n",node->data->code,node->data->GivenUpTime,*Time);
                        }
                    }
            }
            node=node->next;
        }
    }
    if(DeletTime(To)==1){
        PersonRandGenAdd(W,But,Ele,To,Time);
    }
    printf("电梯内人群情况:\n");
    StackNode *p;
    for(i=0;i<FloorNum;i++){
        if(!StackEmpty(E->ElePeople[i])){
            p=E->ElePeople[i]->top;
            while(p->next) {
                printf("No:%d 目标层:%d ", p->data->code,p->data->OutFloor);
                if(p->data->flag[0]==-1){
                    printf("状态:电梯内等待\n");
                }else if(p->data->flag[0]==2){
                    printf("状态:正在离开电梯\n");
                }else{
                }
                p = p->next;
            }

        }
    }
    }
}
void ElePrint(Elevator**E,Queue **W,Button *But,int *Time){
    if(EleDetail==0){
        return;
    }
    int i=0;
    int ele=0;
    StackNode *p;
//    printf("各层呼叫电梯情况\n");
//    for(i=0;i<FloorNum;i++){
//        printf("第%d层:%d\n",i,E->CallCar[i]);
//    }
    printf("运行状态:");
    for(ele=0;ele<2;ele++){
    switch (E[ele]->State) {
        case 1:
            printf("向上");
            break;
        case 2:
            printf("向下");
            break;
        case 3:
            printf("静候");
            break;
        case 4:
            printf("返回1层");
            break;
    }
    printf("  Case:%d  现在楼层:%d 目标楼层:%d\n",E[ele]->Action[0],E[ele]->Floor ,Controller(W,E,But,ele,Time));
    }
}

void QueuePrint(Queue**W,Button *button,Elevator **E){
    if(!VisuaLize){
        return;
    }
    int i=0;
    if(W==NULL){
        return;
    }
    int floor0=E[0]->Floor;
    int floor1=E[1]->Floor;
    printf("------------------------------------\n");
    printf("层数  电梯位置  按钮情况  排队情况(从左到右为队头到队尾)\n");
    printf("------------------------------------\n");
    for(i=FloorNum-1;i>=0;i--){
        if(floor0==i&&floor1==i){
            printf("%d   | ***** | ***** |按 |上:%d |第%d层的排队队列有:%d人\n    | ***** | ***** |钮 |下:%d |",i,button->CallUp[i],i, QueueSize(W[i]),button->CallDown[i]);
        }else if(floor1==i){
            printf("%d   |       | ***** |按 |上:%d |第%d层的排队队列有:%d人\n    |       | ***** |钮 |下:%d |",i,button->CallUp[i],i, QueueSize(W[i]),button->CallDown[i]);
        }else if(floor0==i){
            printf("%d   | ***** |       |按 |上:%d |第%d层的排队队列有:%d人\n    | ***** |       |钮 |下:%d |",i,button->CallUp[i],i, QueueSize(W[i]),button->CallDown[i]);
        }else{
            printf("%d   |       |       |按 |上:%d |第%d层的排队队列有:%d人\n    |       |       |钮 |下:%d |",i,button->CallUp[i],i, QueueSize(W[i]),button->CallDown[i]);
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
#endif //MAIN_C_SIMULATEFUNC_H
