#include <QPushButton>

#ifndef _QKEYPUSHBUTTON_H

    // colors and style to use for the keyboard and keys:
	#define _QKEYPUSHBUTTON_H

        #define DEFAULT_BACKGROUND_BUTTON       "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #8C8F8C, stop: 1 #000000);"
        #define CHANGED_BACKGROUND_BUTTON       "background:lightgray;color:darkred;"

        #define DEFAULT_STYLE_BUTTON            "color:white;border: 1px solid #000000;border-radius:6px;"
        #define STYLE_FOCUS_TEXT                "border: 1px solid red"

        #define EMBEDDED_KEYBOARD               "font-size:9px"
        #define WIDTH_ZOOM_KEY                  20
        #define HEIGHT_ZOOM_KEY                 WIDTH_ZOOM_KEY
        //
        // special keys used within classes:
        #define KEY_TAB                         tr("TAB")
        #define KEY_ALT                         tr("ALT")
        #define KEY_CAPS                        tr("CAPS")
        #define KEY_CTRL_LEFT                   tr("CTRL")
        #define KEY_CUT_LEFT                    tr("Cut")
        #define KEY_COPY                        tr("Copy")
        #define KEY_PASTE                       tr("Paste")
        #define KEY_BACKSPACE                   tr("BACKSPACE")
        #define KEY_BACKSPACE_EMBEDDED          "<---"
        #define KEY_HIDECHAR                    tr("echo")
        #define KEY_HIDECHAR_EMBEDDED           tr("Echo")
        #define KEY_CANC                        tr("DEL")
        #define KEY_RETURN                      tr("RETURN")
        #define KEY_SPACE                       " "
        #define KEY_WIDTH_EMBEDDED              30 //26
        #define KEY_HEIGHT_EMBEDDED             30 //22
        //
        // parameterized macro to reduce the code:
        #define IS_KEY(keyTextPressed, keyValCompare)   (keyTextPressed).contains((keyValCompare), Qt::CaseInsensitive)

        #define IS_TAB(text)                    IS_KEY(text, KEY_TAB)
        #define IS_ALT(text)                    IS_KEY(text, KEY_ALT)
        #define IS_CAPS(text)                   IS_KEY(text, KEY_CAPS)
        #define IS_CTRL_LEFT(text)              IS_KEY(text, KEY_CTRL_LEFT)
        #define IS_CUT_LEFT(text)               IS_KEY(text, KEY_CUT_LEFT)
        #define IS_BACK(text)                   IS_KEY(text, KEY_BACKSPACE)
        #define IS_BACK_EMB(text)               IS_KEY(text, KEY_BACKSPACE_EMBEDDED)
        #define IS_CANC(text)                   IS_KEY(text, KEY_CANC)
        #define IS_RETURN(text)                 IS_KEY(text, KEY_RETURN)
        #define IS_SPACE(text)                  IS_KEY(text, KEY_SPACE)
        #define IS_PASSWORD(text)               IS_KEY(text, KEY_HIDECHAR)
        #define IS_PASSWORD_EMB(text)           IS_KEY(text, KEY_HIDECHAR_EMBEDDED)
        #define IS_COPY(text)                   IS_KEY(text, KEY_COPY)
        #define IS_PASTE(text)                  IS_KEY(text, KEY_PASTE)
        #define NO_SPECIAL_KEY(text)            ((text).length() == 1)

    // class definition representing a key:
	class QKeyPushButton : public QPushButton {
		Q_OBJECT
	
		public:
            //QKeyPushButton(QString notTranslatedText, QWidget *parent = 0);
            QKeyPushButton(QWidget *parent = 0);
	
		private slots:
            void getKeyPress(bool statusCaps);
		
		signals:
            void pressedKey(bool statusCaps);
            void returnKeyPressed();

		private:
			QWidget		*m_parent;
            QString     style_embedded;
            int         m_oldYKey;

		protected:
			void 		mousePressEvent(QMouseEvent *event);
			void 		mouseReleaseEvent(QMouseEvent *event);
	};

#endif // _QKEYPUSHBUTTON_H
