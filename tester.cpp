#include "tester.h"
#include "ui_tester.h"
#include <QLabel>
#include <QDesktopWidget>
#include "form.h"

Tester::Tester(QWidget *parent)
    : QWidget(parent), ui(new Ui::Tester), myKeyboard(NULL)
{

    ui->setupUi(this);
    this->myKeyboard = new widgetKeyBoard(true, 0, true); // false = alpha numeric keyboard, true = numeric keyboard
    this->myKeyboard->enableSwitchingEcho(true); // enable possibility to change echo through keyboard
    this->myKeyboard->createKeyboard(); // only create keyboard

}

Tester::~Tester()
{
    delete ui;
    delete (this->myKeyboard);
}

void Tester::on_pushButton_3_clicked()
{
    Form *emb = new Form();

    emb->show();
}
