#include "widgetKeyBoard.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QLayout>
#include <QScreen>
#include <QKeyEvent>
#include <QDir>


#define ZOOMED_WIDGET_STYLESHEET    "border-radius:8px;font:bold 16px;color:white;"


widgetKeyBoard::widgetKeyBoard(bool embeddedKeyboard, QWidget *activeForm,  bool onlyNumericPad, QWidget *parent) :
        QWidget(parent), m_nextInput(NULL),
        m_activeWindow(activeForm),
        m_currentTextBox(NULL),
        m_numericPad(onlyNumericPad),
        m_embeddedKeyboard(embeddedKeyboard),
        m_echoMode(false), m_zoomFacilityEmbedded(false),
        m_enablePasswordEcho(false),
        m_clipboard(QApplication::clipboard())
{
    this->m_zoomedKey = NULL;
    this->m_clipboard->clear();

}

widgetKeyBoard::~widgetKeyBoard()
{

}

void widgetKeyBoard::enableSwitchingEcho(bool status)
{
    this->m_enablePasswordEcho = status;
}

bool widgetKeyBoard::isEnabledSwitchingEcho(void)
{
    return (this->m_enablePasswordEcho);
}

bool widgetKeyBoard::isEmbeddedKeyboard(void)
{
    return (this->m_embeddedKeyboard);
}

QKeyPushButton * widgetKeyBoard::createNewKey(QString keyValue)
{
    QKeyPushButton *tmp = new QKeyPushButton(this);
    QString        style = QString(DEFAULT_STYLE_BUTTON) + QString(DEFAULT_BACKGROUND_BUTTON);
    QSize          imageDim;
    int            width = 0, height = 0;

    if (this->isEmbeddedKeyboard() == true) {
        imageDim.setWidth(16);
        imageDim.setHeight(16);
    }
    else {
        imageDim.setWidth(32);
        imageDim.setHeight(32);
    }
    if (IS_PASSWORD(keyValue) == true || IS_PASSWORD_EMB(keyValue) == true) {
        tmp->setIconSize(imageDim);

    }
    else if (IS_PASTE(keyValue) == true) {
        tmp->setIconSize(imageDim);
        tmp->setEnabled(false);

    }
    else if (IS_COPY(keyValue) == true) {
        tmp->setIconSize(imageDim);

    }
    else if (IS_CUT_LEFT(keyValue) == true) {
        tmp->setIconSize(imageDim);

    }    
    tmp->setText(keyValue);
    if (this->isEmbeddedKeyboard() == true) {
        width = KEY_WIDTH_EMBEDDED;
        height = KEY_HEIGHT_EMBEDDED;
        style += QString(EMBEDDED_KEYBOARD);
    }
    else {
        width = 54;
        height = 40;
    }
    tmp->setObjectName(keyValue);
    tmp->setMinimumSize(width, height);
    tmp->setMaximumSize(width, height);
    tmp->setStyleSheet(style);
    tmp->setVisible(true);
    return (tmp);
}

void widgetKeyBoard::controlKeyEcho(QLineEdit *control)
{
    QString         textToSearch;
    QKeyPushButton  *echoControlKey;
    QString         tmpStyle;
    QPushButton     *pasteButton = this->findChild<QPushButton *>(KEY_PASTE);
    QPushButton     *cutButton = this->findChild<QPushButton *>(KEY_CUT_LEFT);
    QPushButton     *copyButton = this->findChild<QPushButton *>(KEY_COPY);

    if (this->isEmbeddedKeyboard()) {
        textToSearch = KEY_HIDECHAR_EMBEDDED;
        tmpStyle = QString(EMBEDDED_KEYBOARD);
    }
    else
        textToSearch = KEY_HIDECHAR;
    if (pasteButton != NULL && copyButton != NULL) { // support for copy/paste functionality
        if (control->echoMode() == QLineEdit::Normal) {
            cutButton->setEnabled(true);
            copyButton->setEnabled(true);
            if (this->m_clipboard->text().length() > 0)
                pasteButton->setEnabled(true);
            else
                pasteButton->setEnabled(false);
        }
        else if (control->echoMode() == QLineEdit::Password) {
            copyButton->setEnabled(false);
            cutButton->setEnabled(false);
            pasteButton->setEnabled(false);
        }
    }
    echoControlKey = this->findChild<QKeyPushButton *>(textToSearch);
    if (echoControlKey != NULL) { // support for password echo functionality            
        if (control->echoMode() == QLineEdit::Normal) {            
            echoControlKey->setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(DEFAULT_BACKGROUND_BUTTON) +  tmpStyle); // reported as active

        }
        else if (control->echoMode() == QLineEdit::Password) {            
            echoControlKey->setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(CHANGED_BACKGROUND_BUTTON) + tmpStyle); // reported as active

        }
        echoControlKey->repaint();
        QApplication::processEvents();
    }
}

//
// receives the characters that have been typed:
void widgetKeyBoard::receiptChildKey(QKeyEvent *event, QLineEdit *focusThisControl, bool reset)
{
    static QLineEdit    *nextInput = NULL;

    if (reset == true) { // reinitializes the control
        nextInput = this->getNextTextbox(focusThisControl, true);
        return;
}
    if (nextInput == NULL)
        return;
    //
    // begins analyzing the character received:
    QString newKey = event->text();
    QString tmpReceiptString = nextInput->text();
    int     tmpPos = nextInput->cursorPosition();

    if (this->isNumericPad())
        newKey = newKey.trimmed();
    if (NO_SPECIAL_KEY(newKey) == false) {
        if (IS_RETURN(newKey) == true || IS_TAB(newKey) == true) { // in case of TAB, it moves to the next text
            nextInput = this->setDefaultTextStyle(nextInput);
            nextInput->deselect();
            nextInput = this->getNextTextbox();
            this->controlKeyEcho(nextInput); // status of key echo here
            if (nextInput != NULL) {
                nextInput->setCursorPosition(nextInput->text().length()); // comment this line if you want caret position at current char inserted
                nextInput->setFocus(Qt::TabFocusReason);
            }
        }
        else if (IS_BACK(newKey) == true || IS_BACK_EMB(newKey) == true) { // in case of DELETE character, it eliminates the character on the left
            if (tmpPos-1 >= 0) {
                tmpReceiptString = tmpReceiptString.remove(tmpPos-1, 1);
                nextInput->setText(tmpReceiptString);
                nextInput->setCursorPosition(tmpPos-1);
                nextInput->setSelection(tmpPos-2, 1);
            }
        }
        else if (IS_CANC(newKey) == true) { // it is a DEL character, it deletes the character on the right
             tmpReceiptString = tmpReceiptString.remove(tmpPos, 1);
             nextInput->setText(tmpReceiptString);
             nextInput->setCursorPosition(tmpPos);
             nextInput->setSelection(tmpPos-1, 1);
        }
        else if (IS_COPY(newKey) == true || IS_CUT_LEFT(newKey) == true) {
            QPushButton *button = this->findChild<QPushButton *>(KEY_PASTE);

            if (button != NULL) {
                if (nextInput->text().length() != 0) {
                    this->m_clipboard->setText(nextInput->text());
                    if (IS_CUT_LEFT(newKey) == true)
                        nextInput->setText("");
                    button->setEnabled(true);
                }
                else
                    button->setEnabled(false);
            }
        }
        else if (IS_PASTE(newKey) == true)
            nextInput->setText(this->m_clipboard->text());
        else if (IS_ALT(newKey) == true || IS_CTRL_LEFT(newKey) == true) {
            ; // it does not perform any operation
        }
    }
    else { // it is a character to be displayed in the current text box
        tmpReceiptString = tmpReceiptString.insert(tmpPos, newKey);
        nextInput->setText(tmpReceiptString);
        nextInput->setCursorPosition(tmpPos+1);
        nextInput->setSelection(tmpPos, 1);
    }
}

void widgetKeyBoard::switchKeyEchoMode(QLineEdit *control)
{
    this->setStatusEchoMode(!this->statusEchoMode());
    if (this->statusEchoMode() == true)
        control->setEchoMode(QLineEdit::Password);
    else
        control->setEchoMode(QLineEdit::Normal);
    this->controlKeyEcho(control);
}


QLineEdit * widgetKeyBoard::getNextTextbox(QLineEdit *thisControl, bool reset)
{
    QLineEdit	*tmpInputFound = NULL;

    this->m_currentTextBox = NULL;
    if (this->m_activeWindow == NULL) { // no main window to operate on
        m_nextInput = NULL;
        return (NULL);
    }
    if (thisControl != NULL) {
        if (this->m_activeWindow->findChild<QLineEdit *>(thisControl->objectName()) == 0) // verifies that thisControl is a child of m_activeWindow
            return (NULL);
        else // it's true!
            return (this->setCurrentTextStyle(thisControl));
    }
    //
    // loop through the chain of controls in order of focus to get the QEditLine
    if (m_nextInput == NULL || reset == true)
        m_nextInput = this->m_activeWindow->nextInFocusChain();
    else
        m_nextInput = m_nextInput->nextInFocusChain();
    do {
        if (m_nextInput->inherits("QLineEdit") == true) { // found a text box to use
            tmpInputFound = dynamic_cast<QLineEdit *>(m_nextInput);
            return (this->setCurrentTextStyle(tmpInputFound));
        }
        m_nextInput = m_nextInput->nextInFocusChain();
    } while (m_nextInput != NULL);
    return (NULL);
}


QLineEdit * widgetKeyBoard::setCurrentTextStyle(QLineEdit *control)
{
    this->m_currentTextBox = control;
    control->setStyleSheet(STYLE_FOCUS_TEXT);
    return (control);
}

QLineEdit * widgetKeyBoard::setDefaultTextStyle(QLineEdit *control)
{
    control->setStyleSheet(this->m_noFocusPalette.styleSheet());
    return (control);
}

void widgetKeyBoard::init_keyboard(QLineEdit *focusThisControl)
{
    this->m_currentTextBox = NULL;
    this->m_nextInput = focusThisControl;
    this->receiptChildKey(NULL, focusThisControl, true);
}


void widgetKeyBoard::show(QWidget *activeForm, QLineEdit *first, bool frameless)
{
    this->m_activeWindow = activeForm;
    this->init_keyboard(first);
    if (this->parent() == 0)
    {
        if (this->windowState() == Qt::WindowMinimized)
            this->setWindowState(Qt::WindowNoState);
        this->setStatusEchoMode(false);
        this->m_clipboard->clear();
        //this->move(20, QApplication::desktop()->screenGeometry().height() - this->height() - 200);
        this->borderFrame(frameless);
        QWidget::show();
    }
    else
    {
        qobject_cast<QWidget*>(this->parent())->setVisible(true);
    }
}

void widgetKeyBoard::borderFrame(bool visible)
{
    if (visible == true)
        this->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::MSWindowsFixedSizeDialogHint);
    else
        this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
}

bool widgetKeyBoard::isNumericPad()
{
    return (m_numericPad);
}

void widgetKeyBoard::focusThis(QLineEdit *control)
{
    if (this->m_activeWindow != NULL)
    {
        if (this->m_currentTextBox != NULL)
            this->m_currentTextBox = this->setDefaultTextStyle(this->m_currentTextBox);
        this->init_keyboard(control);
        this->controlKeyEcho(control);
    }
    else
    {
        ;// There is no active window to operate on
    }
}

void widgetKeyBoard::closeEvent(QCloseEvent * event)
{
    event->ignore();
}

void widgetKeyBoard::hide(bool changeColor)
{
    try {
        if (changeColor == true)
            if (this->m_activeWindow != NULL && this->m_currentTextBox != NULL)
                this->m_currentTextBox = this->setDefaultTextStyle(this->m_currentTextBox);
        this->m_currentTextBox = NULL;
        this->m_activeWindow = NULL;
        if (this->parent() != 0)
            qobject_cast<QWidget*>(this->parent())->setVisible(false);
        else
            setVisible(false);
    }
    catch (...) {
    }
}

void widgetKeyBoard::createKeyboard()
{
	QKeyPushButton	*tmp = NULL;	
    QVBoxLayout     *tmpVLayout = new QVBoxLayout;
    QHBoxLayout     *tmpLayout = new QHBoxLayout;
    QGridLayout     *gridLayout = NULL;
    //QString         tmpStyle = QString::null;
    QString         tmpStyle = QString();
    QString         backspace;

    if (this->isNumericPad() == true)
    {
        gridLayout = new QGridLayout;
        for (short i = 49; i <= 51; i++)
            tmpLayout->addWidget(createNewKey(QChar(i)));

        gridLayout->addLayout(tmpLayout, 2, 0);
        tmpLayout = new QHBoxLayout;

        for (short i = 52; i <= 54; i++)
            tmpLayout->addWidget(createNewKey(QChar(i)));

        gridLayout->addLayout(tmpLayout, 1, 0);
        tmpLayout = new QHBoxLayout;

        for (short i = 55; i <= 57; i++)
            tmpLayout->addWidget(createNewKey(QChar(i)));

        gridLayout->addLayout(tmpLayout, 0, 0);
        tmpLayout = new QHBoxLayout;
        tmp = createNewKey(tr("0"));
        tmp->setMinimumSize(tmp->minimumSize().width() * 2, tmp->minimumSize().height());
        tmpLayout->addWidget(tmp);

        tmpLayout->addWidget(createNewKey(".")); // to avoid language translation
        gridLayout->addLayout(tmpLayout, 3, 0);
        tmpLayout = new QHBoxLayout;

        tmpLayout->addWidget(createNewKey(",")); // to avoid language translation
        if (this->isEmbeddedKeyboard() == true)
        {
            backspace = KEY_BACKSPACE_EMBEDDED;
            tmpStyle = QString(KEY_BACKSPACE_EMBEDDED);
        }
        else
        {
            backspace = "BACKSPACE";
            tmpStyle = QString(KEY_BACKSPACE);
        }

        tmp = createNewKey(tmpStyle);
        tmp->setMaximumWidth(tmp->maximumWidth() * 2);
        tmp->setMinimumWidth(tmp->minimumWidth() * 2);
        tmpLayout->addWidget(tmp);
        gridLayout->addLayout(tmpLayout, 4, 0);
        this->setLayout(gridLayout);
    }
    else {
        //
        // numeric key printing:
        tmpLayout->addWidget(createNewKey("\\"));
        for (short i = 49; i <= 57; i++)
        {
            tmpLayout->addWidget(createNewKey(QChar(i)));
        }

        tmpLayout->addWidget(createNewKey(tr("0")));
        tmpLayout->addWidget(createNewKey(tr("/")));
        tmpLayout->addWidget(createNewKey(tr("?")));
        tmpLayout->addWidget(createNewKey(tr("'")));

        if (this->isEmbeddedKeyboard() == true)
        {
            backspace = KEY_BACKSPACE_EMBEDDED;
            tmpStyle = QString(KEY_BACKSPACE_EMBEDDED);
        }
        else
        {
            backspace = "BACKSPACE";
            tmpStyle = QString(KEY_BACKSPACE);
        }

        tmp = createNewKey(tmpStyle);
        tmp->setMaximumWidth(tmp->maximumWidth() * 2);
        tmp->setMinimumWidth(tmp->minimumWidth() * 2);
        tmpLayout->addWidget(tmp);
        tmpStyle = QString(KEY_CANC);

        if (this->isEmbeddedKeyboard() == true)
            tmpStyle = tmpStyle.toLower();

        tmp = createNewKey(tmpStyle);
        tmp->setMaximumWidth(tmp->maximumWidth());
        tmp->setMinimumWidth(tmp->minimumWidth());
        tmpLayout->addWidget(tmp);

        tmpVLayout->insertLayout(0, tmpLayout);
        //
        // Print "Q" line:
        tmpLayout = new QHBoxLayout;
        QVBoxLayout *layoutReturn = new QVBoxLayout;

        tmp = createNewKey(KEY_TAB);
        tmp->setMaximumWidth(tmp->maximumWidth() * 2 - 5);
        tmp->setMinimumWidth(tmp->minimumWidth() * 2 - 5);
        tmpLayout->addWidget(tmp);

        tmpLayout->addWidget(createNewKey(tr("Q")));
        tmpLayout->addWidget(createNewKey(tr("W")));
        tmpLayout->addWidget(createNewKey(tr("E")));
        tmpLayout->addWidget(createNewKey(tr("R")));
        tmpLayout->addWidget(createNewKey(tr("T")));
        tmpLayout->addWidget(createNewKey(tr("Y")));
        tmpLayout->addWidget(createNewKey(tr("U")));
        tmpLayout->addWidget(createNewKey(tr("I")));
        tmpLayout->addWidget(createNewKey(tr("O")));
        tmpLayout->addWidget(createNewKey(tr("P")));
        tmpLayout->addWidget(createNewKey(tr("+")));
        tmpLayout->addWidget(createNewKey(tr("*")));
        tmpLayout->addWidget(createNewKey(tr("-")));

        layoutReturn->insertLayout(0, tmpLayout, 1); // inserts the line of the "Q"
        //
        // Print "A" line:
        tmpLayout = new QHBoxLayout;

        tmp = createNewKey(KEY_CAPS);
        tmp->setMaximumWidth(tmp->maximumWidth() * 2 + 5);
        tmp->setMinimumWidth(tmp->minimumWidth() * 2 + 5);

        if (this->isEmbeddedKeyboard() == true)
            tmpStyle = QString(EMBEDDED_KEYBOARD);
        tmp->setStyleSheet(QString(DEFAULT_STYLE_BUTTON) + QString(CHANGED_BACKGROUND_BUTTON) + tmpStyle); // reported as active
        tmpLayout->addWidget(tmp);

        tmpLayout->addWidget(createNewKey(tr("A")));
        tmpLayout->addWidget(createNewKey(tr("S")));
        tmpLayout->addWidget(createNewKey(tr("D")));
        tmpLayout->addWidget(createNewKey(tr("F")));
        tmpLayout->addWidget(createNewKey(tr("G")));
        tmpLayout->addWidget(createNewKey(tr("H")));
        tmpLayout->addWidget(createNewKey(tr("J")));
        tmpLayout->addWidget(createNewKey(tr("K")));
        tmpLayout->addWidget(createNewKey(tr("L")));
        tmpLayout->addWidget(createNewKey(tr(QString::fromLocal8Bit("ò").toUtf8())));
        tmpLayout->addWidget(createNewKey(tr(QString::fromLocal8Bit("à").toUtf8())));
        tmpLayout->addWidget(createNewKey(tr(QString::fromLocal8Bit("ù").toUtf8())));
        tmpLayout->addWidget(createNewKey(tr("@")));
        tmpLayout->insertStretch(-1, 1);

        layoutReturn->insertLayout(1, tmpLayout, 1); // inserts the line of the "A"
        //
        // inserts the vertical layout within a horizontal:
        tmpLayout = new QHBoxLayout;
        tmpLayout->insertLayout(0, layoutReturn, 1);
        //
        // also inserts the enter key:

        tmp = createNewKey(KEY_RETURN);
        returnPushButton = tmp;
        connect(returnPushButton,SIGNAL(returnKeyPressed()),this,SLOT(returnKeySignalReceived()));
        tmp->setMaximumWidth(tmp->maximumWidth() * 2);
        tmp->setMinimumWidth(tmp->minimumWidth() * 2);
        tmp->setMinimumHeight(tmp->minimumHeight() * 2);
        tmp->setMaximumHeight(tmp->maximumHeight() * 2);
        tmpLayout->addWidget(tmp);
        //
        // inserts the horizontal inside the main vertical layout:
        tmpVLayout->insertLayout(1, tmpLayout);
        //
        // Print "Z" line:
        tmpLayout = new QHBoxLayout;

        tmp = createNewKey(KEY_CUT_LEFT);
        tmp->setMaximumWidth(tmp->maximumWidth() * 2);
        tmp->setMinimumWidth(tmp->minimumWidth() * 2);
        tmpLayout->addWidget(tmp);

        tmpLayout->addWidget(createNewKey(tr("_")));
        tmpLayout->addWidget(createNewKey(tr("Z")));
        tmpLayout->addWidget(createNewKey(tr("X")));
        tmpLayout->addWidget(createNewKey(tr("C")));
        tmpLayout->addWidget(createNewKey(tr("V")));
        tmpLayout->addWidget(createNewKey(tr("B")));
        tmpLayout->addWidget(createNewKey(tr("N")));
        tmpLayout->addWidget(createNewKey(tr("M")));
        tmpLayout->addWidget(createNewKey(tr(",")));
        tmpLayout->addWidget(createNewKey(tr(";")));
        tmpLayout->addWidget(createNewKey(tr(":")));
        tmpLayout->addWidget(createNewKey(tr(".")));
        tmp = createNewKey(KEY_CTRL_LEFT);
        tmp->setMaximumWidth(tmp->maximumWidth() * 3 + 5);
        tmp->setMinimumWidth(tmp->minimumWidth() * 3 + 5);
        tmpLayout->addWidget(tmp);
        tmpVLayout->insertLayout(2, tmpLayout);
        //
        // SPACE line print:
        tmpLayout = new QHBoxLayout;
        tmp = createNewKey(KEY_COPY);
        tmp->setMaximumWidth(tmp->maximumWidth() * 2);
        tmp->setMinimumWidth(tmp->minimumWidth() * 2);
        tmpLayout->addWidget(tmp);

        tmp = createNewKey(KEY_ALT);
        tmp->setMaximumWidth(tmp->maximumWidth() * 2);
        tmp->setMinimumWidth(tmp->minimumWidth() * 2);
        tmpLayout->addWidget(tmp);

        tmp = createNewKey(KEY_SPACE);
        tmp->setMaximumWidth(tmp->maximumWidth() * 10);
        tmp->setMinimumWidth(tmp->minimumWidth() * 10);
        tmpLayout->addWidget(tmp);

        // password echo button:
        if (this->isEmbeddedKeyboard() == true)
            tmpStyle = QString(KEY_HIDECHAR_EMBEDDED);
        else
            tmpStyle = QString(KEY_HIDECHAR);

        tmp = createNewKey(tmpStyle);
        tmp->setMaximumWidth(tmp->maximumWidth() * 2);
        tmp->setMinimumWidth(tmp->minimumWidth() * 2);
        tmpLayout->addWidget(tmp);
        //

        tmp = createNewKey(KEY_PASTE);
        tmp->setMaximumWidth(tmp->maximumWidth() * 2);
        tmp->setMinimumWidth(tmp->minimumWidth() * 2);
        tmpLayout->addWidget(tmp);
        tmpVLayout->insertLayout(3, tmpLayout);

        //
        // hook the layout to the whole form:
        this->setLayout(tmpVLayout);
    }
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

}

void widgetKeyBoard::returnKeySignalReceived()
{
    emit keySignalReceived();
}
