#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    setWindowTitle("Virtual Keyboard");

    _keyboard = new widgetKeyBoard(false, this, false, ui->frame);
    _keyboard->enableSwitchingEcho(true);
    _keyboard->createKeyboard();
    _keyboard->show(this);

    connect(_keyboard,SIGNAL(keySignalReceived()),this,SLOT(returnWidgetSignalReceived()));
    externalLineEditPointer = nullptr;
}

Form::~Form()
{
    delete ui;
}

void Form::returnWidgetSignalReceived()
{
   QLineEdit* currentQLineEdit = _keyboard->currentTextBox();
   QString currentText = currentQLineEdit->text();
   if(externalLineEditPointer != nullptr)
   {
        externalLineEditPointer->setText(currentText);
   }
}

void Form::setExternalLineEdit(QLineEdit* myExtLineEdit)
{
    externalLineEditPointer = myExtLineEdit;
    QLineEdit* currentQLineEdit = _keyboard->currentTextBox();
    currentQLineEdit->setText((myExtLineEdit->text()));
}
