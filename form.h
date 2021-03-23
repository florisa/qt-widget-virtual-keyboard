#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "keyboard/widgetKeyBoard.h"


namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form();

void setExternalLineEdit(QLineEdit* myExtLineEdit);
QLineEdit* externalLineEditPointer; // external pointer to the main APP

public slots:
    void returnWidgetSignalReceived();

private:
    Ui::Form *ui;
    widgetKeyBoard  *_keyboard;
};

#endif // FORM_H
