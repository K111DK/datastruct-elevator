//
// Created by 35802 on 2021/11/16.
//

#ifndef ELEVATOR_LOL_H
#define ELEVATOR_LOL_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
        namespace
Ui {
class lol;
}
QT_END_NAMESPACE

        class
lol :
public QMainWindow{
        Q_OBJECT

        public:
        explicit lol(QWidget *parent = nullptr);
        ~lol() override;

        private:
        Ui::lol *ui;
};


#endif //ELEVATOR_LOL_H
