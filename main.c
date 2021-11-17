#include "utils.h"
#include "BASICSTRUCT.h"
#include "simulateFunc.h"
#include <conio.h>
#include "windows.h"

int main(){
        int T=1000;
        printf("输入模拟时间:\n");
        scanf("%d", &T);
        Simulator(T);
        int Time;
        Time = 1;
        char c;
        while (c = _getch()) {
            system("cls");
            switch (c) {
                case 'a':
                    if (Time == 1) {
                        Time = T;
                    } else {
                        Time--;
                    }
                    break;
                case 'd':
                    if (Time == T) {
                        Time = 1;
                    } else {
                        Time++;
                    }
                    break;
                case '0':
                    return 0;
            }
            printf("时间:%d\n", Time);
            printf("%s", VisualLogger[Time]);
            printf("%s", EleLogger[Time]);
            printf("%s", PersonLogger[Time]);
        }
//    while(Time!=T){
//                    printf("时间:%d\n", Time);
//            printf("%s", VisualLogger[Time]);
//            printf("%s", EleLogger[Time]);
//            printf("%s", PersonLogger[Time]);
//        Time++;
//    }
}