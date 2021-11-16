#include "utils.h"
#include "BASICSTRUCT.h"
#include "simulateFunc.h"


int main(){
    Simulator();
    int Time;
    Time=0;
    char c;
    while(Time!=T){
        system("\r");
        printf("时间:%d\n",Time);
        printf("%s",VisualLogger[Time]);
//        printf("%s",EleLogger[Time]);
//      printf("%s",PersonLogger[Time]);
        Time++;
    }
    return 0;
}