#ifndef _WIDGETKEYBOARD_H
	
    #define _WIDGETKEYBOARD_H

    #include "QKeyPushButton.h"
    #include <QSound>
    #include <QLineEdit>
    #include <QClipboard>
    #include <QLabel>
    //
    //
    // class that creates and controls the keyboard:
    class widgetKeyBoard : public QWidget {
        Q_OBJECT

        public:
            widgetKeyBoard(bool embeddedKeyboard = false, QWidget *activeForm = NULL, bool onlyNumericPad = false, QWidget *parent = NULL);
            ~widgetKeyBoard();

            void            createKeyboard();
            bool            isEmbeddedKeyboard(void);
            bool            statusEchoMode(void){return (this->m_echoMode);}
            void            setStatusEchoMode(bool echo) {this->m_echoMode = echo; }
            void            receiptChildKey(QKeyEvent *event, QLineEdit *focusThisControl, bool reset = false); // welcomes the keys pressed
            QLineEdit *     currentTextBox(void) { return (this->m_currentTextBox);}
            void            switchKeyEchoMode(QLineEdit *control);
            void            enableSwitchingEcho(bool status); // if you don't want control echo from keyboard
            bool            isEnabledSwitchingEcho(void); // current status
            void            borderFrame(bool visible = true);
            bool            isNumericPad();

            QKeyPushButton*  returnPushButton;

        public slots:
            void            show(QWidget *activeForm, QLineEdit *first = NULL, bool frameless = false);
            void            hide(bool noChangeColor);
            void            focusThis(QLineEdit *control);

            void            returnKeySignalReceived();

        signals:
            void keySignalReceived();

        protected:
            virtual void    closeEvent(QCloseEvent * event);

        private:
            widgetKeyBoard(const widgetKeyBoard&);
            widgetKeyBoard& operator=(const widgetKeyBoard&);

            void            init_keyboard(QLineEdit *focusThisControl); // reinitializes the basic functions of the keyboard
            QLineEdit *     setCurrentTextStyle(QLineEdit *control);
            QLineEdit *     setDefaultTextStyle(QLineEdit *control);
            QKeyPushButton *createNewKey(QString keyValue);
            QLineEdit *		getNextTextbox(QLineEdit *thisControl = NULL, bool reset = false);
            void            controlKeyEcho(QLineEdit *control);

        private:
            QLineEdit		m_noFocusPalette; // used to restore unfocused linetext
            QWidget         *m_nextInput; // points to the textbox currently in focus
            QWidget         *m_activeWindow;
            QLineEdit		*m_currentTextBox; // It maintains the reference to the currently in use box
            QLabel          *m_zoomedKey; // used for zoomed pressed key
            bool            m_embeddedKeyboard;
            bool            m_echoMode; // status of current text object for echo
            bool            m_zoomFacilityEmbedded;
            bool            m_enablePasswordEcho; // controls the possibility to change among normal/password echo
            bool            m_numericPad;
            QClipboard      *m_clipboard;
    };

#endif // _WIDGETKEYBOARD_H
