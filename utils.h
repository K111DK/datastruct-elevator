//
// Created by 35802 on 2021/10/31.
//

#ifndef MAIN_C_UTILS_H
#define MAIN_C_UTILS_H
#include "stdio.h"
#include "stdlib.h"

int GenRand(int a){//产生范围为0~a的随机整数
    return rand()%a+1;
}

#endif //MAIN_C_UTILS_H
