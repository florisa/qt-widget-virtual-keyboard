#ifndef TESTER_H
#define TESTER_H

#include <QWidget>
#include "keyboard/widgetKeyBoard.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QTranslator>


namespace Ui
{
    class Tester;
}

class Tester : public QWidget
{
    Q_OBJECT

public:
    Tester(QWidget *parent = 0);
    ~Tester();

    widgetKeyBoard *    getKeyboard();

private:
    Ui::Tester      *ui;
    widgetKeyBoard  *myKeyboard;

private slots:

    void on_pushButton_3_clicked();
};

#endif // TESTER_H

