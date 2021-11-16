//
// Created by 35802 on 2021/11/16.
//

// You may need to build the project (run Qt uic code generator) to get "ui_lol.h" resolved

#include "lol.h"
#include "ui_lol.h"


lol::lol(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::lol) {
    ui->setupUi(this);
}

lol::~lol() {
    delete ui;
}
