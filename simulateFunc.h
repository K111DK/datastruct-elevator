//
// Created by 35802 on 2021/10/31.
//
#ifndef MAIN_C_SIMULATEFUNC_H
#define MAIN_C_SIMULATEFUNC_H
#include "BASICSTRUCT.h"
int IfOverCross(Button *B,Elevator *E);
Person *PersonRandGenAdd(Queue **W,Button *button,Elevator **E,TimeLine *To,const int * Time,char *logger){//（伪）随机地生成一个人加入到队列
    Person *a;
    if(*Time==0&&EXAMPLE){
        a=(Person*)malloc(sizeof (Person));
        a->GivenUpTime= GenRand(100)*t+40*t;
        a->InFloor = 1;
        a->OutFloor = 4;
        a->InterTime = 1000*t;
        a->code=*Time;
        a->flag[0]=0;
        a->flag[1]=-1;
        InsertTime(To,a->InterTime);
    }else{
        a=(Person*)malloc(sizeof (Person));
        a->GivenUpTime= GenRand(100)*t+40*t;
        a->InFloor = GenRand(FloorNum)-1;
        a->OutFloor = GenRand(FloorNum)-1;
        a->InterTime = GenRand(MaxInterTime)*t;
        a->code=*Time;
        a->flag[0]=0;
        a->flag[1]=-1;
        InsertTime(To,a->InterTime);
    }
    while (a->OutFloor==a->InFloor){
        a->OutFloor = GenRand(FloorNum)-1;
    }
//    sprintf(logger,"%sINFO:\nGUtime:%f\nITtime:%f\n",logger,a->GivenUpTime,a->InterTime);
    enQueue(W[a->InFloor],a);
    //将随机生成的人加入等待队列
    sprintf(logger,"%s新的人No:%d进入第%d层排队队列.\n",logger, a->code, a->InFloor);
    if(GenDetail){
        sprintf(logger,"%s-----------------\n人物信息:\n最大等待时间:%d\n起始层:%d\n目的层:%d\n下一个人到达时间:%d\n序号:%d\n-----------------\n",logger,
                a->GivenUpTime, a->InFloor, a->OutFloor, a->InterTime, a->code);
    }
    if(a->InFloor==E[0]->Floor&&(E[0]->Action[0]==3||E[0]->Action[0]==4)||a->InFloor==E[1]->Floor&&(E[1]->Action[0]==3||E[1]->Action[0]==4)){
        a->GivenUpTime=0;
        sprintf(logger,"%s电梯就在本层且未离开,No:%d 准备进入电梯.\n",logger,a->code);
    }
    E[0]->CallCar[a->InFloor]=1;
    E[1]->CallCar[a->InFloor]=1;
    if(a->InFloor>a->OutFloor){
        button->CallDown[a->InFloor]=1;
    }else{
        button->CallUp[a->InFloor]=1;
    }
    sprintf(logger,"%sNo:%d 按下按钮并在队列中等待.\n",logger,a->code);
    return a;
}

int Controller(Queue **W,Elevator **Ele,Button *But,int ele,int *Time){
    Elevator *E;
    E=Ele[ele];
    if(E->State==Idle&&E->Action[0]==1){
        return 1;
    }
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
    if(i==FloorNum){
        E->State=Idle;
        return 1;
    }
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
    return 1;
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


void ElevatorProcess(Queue **W,Elevator **Ele,Button *But,int *Time,int ele,char *logger){
    int f=0;
    int i=0;
    Elevator *E;
    E=Ele[ele];
//    sprintf(logger,"%sele:%d   totalcall:%d   ifovercross:%d\n",logger,ele,TotalCall(But),IfOverCross(But,Ele[1-ele]));
    while(1) {
        switch (E->Action[0]) {
            case 1://一楼等待
                if(ele==1){
                    if((TotalCall(But)<=1||!IfOverCross(But,Ele[1-ele]))||!DoubleEle){
//                    sprintf(logger,"%s%d",logger,TotalCall(But)<=1||!IfOverCross(But,Ele[1-ele]));
                        return;
                    }
                }
                sprintf(logger,"%s电梯%d在1楼等待.Time:%d\n",logger,ele, *Time);
                for(f=0;f<FloorNum;f++){
                    if(But->CallDown[f]||But->CallUp[f]||E->CallCar[f]){
                        break;
                    }
                }
                if(f!=FloorNum) {
                    if (E->Floor == f) {//若在第一层则直接开门
                        E->Action[0] = 3;
                        sprintf(logger,"%s电梯:%d准备开门%d.Time:%d\n",logger,ele,f,*Time);
                        break;
                    } else {//否则开始判断目标层
                        E->Action[0] = 6;
                        sprintf(logger,"%s电梯:%d准备移动.Time:%d\n",logger,ele, *Time);
                        break;
                    }
                }else {
                    return;
                }
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
                E->D3=0;
                if (E->Action[2] == -1) {//第一次进入开门状态
                    E->Action[2] = DoorOperTime * t;
                    E->Action[2] -= t;
                    E->Action[1] = 4;//次态为4
                    sprintf(logger,"%s电梯:%d开始开门.Time:%d\n",logger,ele, *Time);
                    return;
                }
                else{//开门中间态
                    if (E->Action[2] == t) {//门完全打开
                        sprintf(logger,"%s电梯:%d门已打开.Time:%d\n",logger,ele, *Time);
                        E->Action[0] = 4;//让人进出
                        E->Action[1] = -1;
                        E->Action[2] = -1;
                        return;
                    } else {//门正在打开
                        sprintf(logger,"%s电梯:%d正在开门.Time:%d\n",logger,ele, *Time);
                        E->Action[2] -= t;
                        return;
                    }
                }
                break;
            case 4://让人进出
            if(E->D1==1)E->D1=0;
                if(E->Action[3]==-1){
                    E->Action[3]=DetectTime*t;
                    E->Action[3]-=t;
                }else if(E->Action[3]==t){
                    E->Action[3]=DetectTime*t;
                    E->D1=1;
                }else{
                    E->Action[3]-=t;
                }
                if (StackEmpty(E->ElePeople[E->Floor])) {//电梯人出来完了
                    sprintf(logger,"%s电梯:%d内要出来的人已全部出来\n",logger,ele);
                    if (QueueEmpty(W[E->Floor])||((QueueSize(W[E->Floor])==1&&W[E->Floor]->front->next->data->flag[1]==1-ele))&&DoubleEle){//门外人进来完了
                        E->D2=1;
                        E->Action[2]=E->Action[3];
                        if(E->D1==1){
                            E->Action[0] = 5;
                            E->Action[2] = -1;
                            E->Action[3] = -1;
                            E->CallCar[E->Floor]=0;
                            But->CallDown[E->Floor]=0;
                            But->CallUp[E->Floor]=0;
                            E->D2==0;
                            sprintf(logger,"%s电梯:%d准备关门\n",logger,ele);
                            return;
                        }else{
                            sprintf(logger,"%s电梯:%d准备关门\n",logger,ele);
                            return;
                        }
                    } else {//门外还有人进
                        if(E->D2==1&&E->Action[2]!=-1){
                            E->Action[2]=-1;
                            E->D2=0;
                        }
                        if (E->Action[2] != -1) {
                            if (E->Action[2] == t) {//计时完毕
                                QNode *p;
                                //当层人出队
                                sprintf(logger,"%s队头:No:%d",logger,W[E->Floor]->front->next->data->code);
                                p = DeQueue(W[E->Floor]);
                                E->CallCar[p->data->OutFloor]=1;//电梯内目标楼层按钮按下
                                Push(E->ElePeople[p->data->OutFloor], p->data);//把人压入电梯中
                                sprintf(logger,"%sNo:%d已进入电梯%d.Time:%d\n",logger,p->data->code,ele,*Time);
                                p->data->flag[0]=-1;
                                E->Action[2] = -1;
                                return;
                            } else {
                                QNode *q=W[E->Floor]->front->next->data->flag[1]==ele?W[E->Floor]->front->next:W[E->Floor]->front->next->next;
                                E->Action[2] -= t;//继续计时
                                sprintf(logger,"%sNo:%d正在进入电梯%d.Time:%d\n",logger,q->data->code,ele,*Time);
                                return;
                            }
                        } else {//进门初态25t为周期开始计时
                            QueueEmpty(W[E->Floor]);
                            QNode *Wnode;
                            Wnode=W[E->Floor]->front->next;
                            if(Wnode->data->flag[1]!=1-ele){
                                Wnode->data->flag[0]=1;
                                Wnode->data->flag[1]=ele;
                                sprintf(logger,"%sNo:%d正在进入电梯%d.Time:%d\n",logger,Wnode->data->code,ele,*Time);
                                E->Action[2] = InOutTime * t;
                                E->Action[2] -= t;
                                return;}
                            else{
                                Wnode=Wnode->next;
                                Wnode->data->flag[0]=1;
                                Wnode->data->flag[1]=ele;
                                sprintf(logger,"%sNo:%d正在进入电梯%d.Time:%d\n",logger,Wnode->data->code,ele,*Time);
                                E->Action[2] = InOutTime * t;
                                E->Action[2] -= t;
                                return;
                            }
                        }
                    }
                } else {//有人出
                    if (E->Action[2] != -1) {//非计时状态，对下一个人进行计时
                        if (E->Action[2] == t) {//计时完毕，将人从电梯栈中弹出
                            E->ElePeople[E->Floor]->top->data->flag[0]=-2;
                            sprintf(logger,"%sNo:%d已离开电梯.Time:%d\n",logger,Pop(E->ElePeople[E->Floor])->code,*Time);
                            E->Action[2] = -1;//计时器归位
                            return;
                        } else {
                            sprintf(logger,"%sNo:%d正在离开电梯\n",logger,E->ElePeople[E->Floor]->top->data->code);
                            E->Action[2] -= t;//计时
                            return;
                        }
                    } else {
                        sprintf(logger,"%sNo:%d正在离开电梯\n",logger,E->ElePeople[E->Floor]->top->data->code);
                        E->ElePeople[E->Floor]->top->data->flag[0]=2;
                        E->Action[2] = InOutTime * t;//开始计时
                        E->Action[2] -= t;
                        return;
                    }
                }
                break;
            case 5:
                if (StackEmpty(E->ElePeople[E->Floor])&&QueueEmpty(W[E->Floor])){
                    Controller(W,Ele,But,ele,Time);
                    if(E->D3==1){
                        if(E->State==Idle){
                            if(E->Action[2]==-1){
                                E->Action[2]=IdleTime*t;
                                E->Action[2]-=t;
                                E->State=Idle;
                                sprintf(logger,"%s电梯:%d在静置等待\n",logger,ele);
                                return;
                            }else{
                                if(E->Action[2]==t){
                                    E->Action[2]=-1;
                                    E->Action[1]=-1;
                                    E->Action[0]=6;
                                    E->State=GoingBack;
                                    sprintf(logger,"%s电梯:%d准备返回第一层\n",logger,ele);
                                    return;
                                }else{
                                    E->Action[2]-=t;
                                    return;
                                }
                            }
                        }
                    }
                    if(E->Action[2]==-1){
                        E->Action[2]=DoorOperTime*t;
                        E->Action[2]-=t;
                        sprintf(logger,"%s电梯:%d正在关门.Time:%d\n",logger,ele, *Time);
                        return;
                    }
                    else if(E->Action[2]==t){
                        sprintf(logger,"%s电梯:%d门已关上\n",logger,ele);
                        E->D3=1;
                        E->Action[2]=-1;
                        for(i=0;i<FloorNum;i++){
                            if((But->CallDown[i]||But->CallUp[i])||E->CallCar[i]){
                                if(i==E->Floor){
                                    E->Action[2] = -1;
                                    E->Action[1] = -1;
                                    E->Action[0] = 3;
                                    return;
                                }
                                else{
                                    E->Action[2] = -1;
                                    E->Action[1] = -1;
                                    E->Action[0] = 6;
                                    sprintf(logger,"%s电梯:%d准备移动.Time:%d\n",logger,ele, *Time);
                                    return;
                                }
                            }
                        }
                        if(i!=FloorNum)return;
                    }else{
                        E->Action[2]-=t;
                        sprintf(logger,"%s电梯:%d正在关门.Time:%d\n",logger, ele,*Time);
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
                        sprintf(logger,"%s电梯:%d开始加速.Time:%d\n",logger,ele, *Time);
                        return;
                    }
                    else{
                        if(E->Action[2]==t){//加速完毕开始进入上升循环
                            E->Action[0]=7;
                            E->Action[1]=-1;
                            E->Action[2]=-2;//置1是保证到达下一状态时层数不会直接变换
                            sprintf(logger,"%s电梯:%d加速完毕,准备上升\n",logger,ele);
                            return;
                        }
                        else{
                            E->Action[2]-=t;
                            sprintf(logger,"%s电梯:%d正在加速\n",logger,ele);
                            return;
                        }
                    }
                }else{//往下
                    if(E->Action[2]==-1) {
                        E->Action[1] = 8;
                        E->Action[2] = AccerlerTime * t;
                        E->Action[2] -= t;
                        sprintf(logger,"%s电梯:%d开始加速.Time:%d\n",logger,ele, *Time);
                        return;
                    }
                    else{
                        if(E->Action[2]==t){//加速完毕进入下降循环
                            sprintf(logger,"%s电梯:%d加速完毕,准备下降:%d\n",logger,ele, *Time);
                            E->Action[0]=8;
                            E->Action[1]=-1;
                            E->Action[2]=-2;//置1是保证到达下一状态时层数不会直接变换
                            return;
                        }
                        else{
                            E->Action[2]-=t;
                            sprintf(logger,"%s电梯:%d正在加速\n",logger,ele);
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
                        sprintf(logger,"%s电梯:%d开始减速:%d\n",logger,ele, *Time);
                        return;
                    }
                    else{//减速完毕，回到状态2，即停顿判断态
                        if(E->Action[2]==t){
                            E->Action[0]=2;
                            E->Action[1]=-1;
                            E->Action[2]=-1;
                            E->Floor++;
                            sprintf(logger,"%s电梯:%d已停止.Time:%d\n",logger,ele, *Time);
                            return;
                        }
                        else{
                            E->Action[2]-=t;
                            sprintf(logger,"%s电梯:%d正在减速:%d\n",logger,ele, *Time);
                            return;
                        }
                    }
                }
                else{//否则继续加速至下一层（到达下一层的时候停不下来）
                    if(E->Action[2]==-1||E->Action[2]==-2) {
                        E->Action[1] = 7;
                        E->Action[2] = UpingTime * t;
                        E->Action[2] -= t;
                        sprintf(logger,"%s电梯:%d正在上升.Time:%d\n",logger,ele, *Time);
                        return;
                    }
                    else{
                        if(E->Action[2]==t){
                            E->Action[0]=7;
                            E->Action[1]=-1;
                            E->Action[2]=-1;//调回初态
                            sprintf(logger,"%s电梯:%d上升了一层.Time:%d\n",logger,ele, *Time);
                            E->Floor++;
                            return;
                        }
                        else{
                            E->Action[2]-=t;
                            sprintf(logger,"%s电梯:%d正在上升.Time:%d\n",logger,ele, *Time);
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
                        sprintf(logger,"%s电梯:%d开始减速:%d\n",logger,ele, *Time);
                        return;
                    }
                    else{//减速完毕，回到状态2，即停顿判断态
                        if(E->Action[2]==t){
                            E->Action[0]=2;
                            E->Action[1]=-1;
                            E->Action[2]=-1;
                            E->Floor--;
                            sprintf(logger,"%s电梯:%d已停止.Time:%d\n",logger,ele, *Time);
                            return;
                        }
                        else{
                            E->Action[2]-=t;
                            sprintf(logger,"%s电梯:%d正在减速:%d\n",logger,ele, *Time);
                            return;
                        }
                    }
                }
                else{//否则继续加速至下一层（到达下一层的时候停不下来）
                    if(E->Action[2]==-1||E->Action[2]==-2) {
                        E->Action[1] = 8;
                        E->Action[2] = DowningTime * t;
                        E->Action[2] -= t;
                        sprintf(logger,"%s电梯:%d正在下降.Time:%d\n",logger,ele, *Time);
                        return;
                    }
                    else{
                        if(E->Action[2]==t){
                            E->Action[0]=8;
                            E->Action[1]=-1;
                            E->Action[2]=-1;//调回初态
                            sprintf(logger,"%s电梯:%d下降了一层.Time:%d\n",logger,ele, *Time);
                            E->Floor--;
                            return;
                        }
                        else{
                            E->Action[2]-=t;
                            sprintf(logger,"%s电梯:%d正在下降.Time:%d\n",logger, ele,*Time);
                            return;
                        }
                    }
                }
            default:
                return;
        }
    }
}

void PeopleProcess(Queue **W,Elevator **Ele,Button *But,TimeLine *To,int* Time,char*logger){
    int i;
    int j;
    Elevator *E;

    sprintf(logger,"%s各楼层人群:\n",logger);
    for(i=0;i<FloorNum;i++){
        QNode *node;
        node=W[i]->front->next;
        while(node!=NULL){
            node->data->InFloor;
            if(node->data==NULL)break;
            switch (node->data->flag[0]) {
                case 1:
                    sprintf(logger,"%sNo:%d正在进入电梯%d.Time:%d\n",logger,node->data->code,node->data->flag[1] ,*Time);break;
                case -1:
                    sprintf(logger,"%sNo:%d已经进入电梯%d.Time:%d\n",logger,node->data->code,node->data->flag[1],*Time);break;
                default:
                    if(node->data->InFloor==Ele[0]->Floor&&(Ele[0]->Action[0]==4||Ele[0]->Action[0]==3||Ele[0]->Action[0]==1)){
                        node->data->GivenUpTime=0;
                        sprintf(logger,"%sNo:%d准备进入电梯0.Time:%d\n",logger,node->data->code ,*Time);
                        break;
                    }else if(node->data->InFloor==Ele[1]->Floor&&(Ele[1]->Action[0]==4||Ele[1]->Action[0]==3||Ele[1]->Action[0]==1)){
                        node->data->GivenUpTime=0;
                        sprintf(logger,"%sNo:%d准备进入电梯1.Time:%d\n",logger,node->data->code ,*Time);
                        break;
                    }else{
                        if(node->data->GivenUpTime==t){
                            DeletQueNode(W[i],node);
                            sprintf(logger,"%sNo:%d离开排队队列.Time:%d\n",logger,node->data->code,*Time);
                        }
                        else{
                            node->data->GivenUpTime-=t;
                            sprintf(logger,"%sNo:%d正在排队等待,剩余等待时间:%d.Time:%d\n",logger,node->data->code,node->data->GivenUpTime,*Time);
                        }
                    }
            }
            node=node->next;
        }
    }
    if(DeletTime(To)==1){
        PersonRandGenAdd(W,But,Ele,To,Time,logger);
    }
    sprintf(logger,"%s电梯内人群情况:\n",logger);
    for(j=0;j<2;j++){
        sprintf(logger,"%s电梯:%d\n",logger,j);
        E=Ele[j];
        StackNode *p;
        for(i=0;i<FloorNum;i++){
            if(!StackEmpty(E->ElePeople[i])){
                p=E->ElePeople[i]->top;
                while(p->next) {
                    sprintf(logger,"%sNo:%d 目标层:%d ",logger, p->data->code,p->data->OutFloor);
                    if(p->data->flag[0]==-1){
                        sprintf(logger,"%s状态:电梯内等待\n",logger);
                    }else if(p->data->flag[0]==2){
                        sprintf(logger,"%s状态:正在离开电梯\n",logger);
                    }else{
                    }
                    p = p->next;
                }

            }
        }
    }
}

void ElePrint(Elevator**E,Queue **W,Button *But,int *Time,char*logger){
    if(EleDetail==0){
        return;
    }
    int i=0;
    int ele=0;
    StackNode *p;
//    sprintf(logger,"%s各层呼叫电梯情况\n");
//    for(i=0;i<FloorNum;i++){
//        sprintf(logger,"%s第%d层:%d\n",logger,i,E->CallCar[i]);
//    }
    for(ele=0;ele<2;ele++){
        sprintf(logger,"%s电梯%d运行状态:",logger,ele);
        switch (E[ele]->State) {
            case 1:
                sprintf(logger,"%s向上",logger);
                break;
            case 2:
                sprintf(logger,"%s向下",logger);
                break;
            case 3:
                sprintf(logger,"%s静候",logger);
                break;
            case 4:
                sprintf(logger,"%s返回1层",logger);
                break;
        }
        sprintf(logger,"%s  Case:%d  现在楼层:%d 目标楼层:%d 计时器状态:%d\n",logger,E[ele]->Action[0],E[ele]->Floor ,Controller(W,E,But,ele,Time),E[ele]->Action[2]);
    }
}

void QueuePrint(Queue**W,Button *button,Elevator **E,char *logger){
    if(!VisuaLize){
        return;
    }
    int i=0;
    if(W==NULL){
        return;
    }
    int floor0=E[0]->Floor;
    int floor1=E[1]->Floor;
    sprintf(logger,"%s------------------------------------\n",logger);
    sprintf(logger,"%s层数   电梯0    电梯1   按钮情况   排队情况(从左到右为队头到队尾)\n",logger);
    sprintf(logger,"%s------------------------------------\n",logger);
    for(i=FloorNum-1;i>=0;i--){
        if(floor0==i&&floor1==i){
            sprintf(logger,"%s%d   | ***** | ***** |按 |上:%d |第%d层的排队队列有:%d人\n    | ***** | ***** |钮 |下:%d |",logger,i,button->CallUp[i],i, QueueSize(W[i]),button->CallDown[i]);
        }else if(floor1==i){
            sprintf(logger,"%s%d   |       | ***** |按 |上:%d |第%d层的排队队列有:%d人\n    |       | ***** |钮 |下:%d |",logger,i,button->CallUp[i],i, QueueSize(W[i]),button->CallDown[i]);
        }else if(floor0==i){
            sprintf(logger,"%s%d   | ***** |       |按 |上:%d |第%d层的排队队列有:%d人\n    | ***** |       |钮 |下:%d |",logger,i,button->CallUp[i],i, QueueSize(W[i]),button->CallDown[i]);
        }else{
            sprintf(logger,"%s%d   |       |       |按 |上:%d |第%d层的排队队列有:%d人\n    |       |       |钮 |下:%d |",logger,i,button->CallUp[i],i, QueueSize(W[i]),button->CallDown[i]);
        }
        QNode *p=W[i]->front->next;
        if(p){
            while(p){
                sprintf(logger,"%sNo:%d   ",logger, p->data->code);
                p = p->next;
            }
        }
        sprintf(logger,"%s\n------------------------------------\n",logger);
    }
}

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

void Simulator(int T){
    int *Time=(int*) malloc(sizeof (int));
    double i=0;
    *Time=0;
    TimeLine *To=(TimeLine*) malloc(sizeof (TimeLine));
    To=TimeLineInit();
    Button *But=(Button*) malloc(sizeof (Button));
    Queue *WaitingQue[FloorNum];
    Elevator *E[2];
    Init(But,WaitingQue,E);//初始化系统
    PersonRandGenAdd(WaitingQue,But,E,To,Time,PersonLogger[*Time]);//随机加入第一个人
    while(*Time!=T){

        (*Time)+=1;
        i=(*Time);
        printf("Simulate Process:%.1f%c\r",(i/T)*100,37);
        srand((unsigned int)*(Time));//*time(NULL)
        ElevatorProcess(WaitingQue,E,But,Time,0,EleLogger[*Time]);
        ElevatorProcess(WaitingQue,E,But,Time,1,EleLogger[*Time]);
        PeopleProcess(WaitingQue,E,But,To,Time,PersonLogger[*Time]);
        QueuePrint(WaitingQue,But,E,VisualLogger[*Time]);
        TimeLinePrint(To,PersonLogger[*Time]);
        ElePrint(E,WaitingQue,But,Time,EleLogger[*Time]);
    }
}
#endif //MAIN_C_SIMULATEFUNC_H
