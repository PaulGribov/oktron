#include "xWidgets.h"

QString xButtonSelectedStyleSheet="\
	:enabled {\
	border-image: url(:/images/border_but0_enabled.png) 10;\
	border-width: 10px;\
	color: white;\
	font: 24pt; }\
	:disabled {\
	border-image: url(:/images/border_but0_disabled.png) 10;\
	border-width: 10px;\
	color: white;\
	font: 24pt; }\
	:hover {\
	border-image: url(:/images/border_but0_hover.png) 10;\
	border-width: 10px;\
	color: white;\
	font: 24pt; }\
	:pressed {\
	border-image: url(:/images/border_but0_pressed.png) 10;\
	border-width: 10px;\
	color: white;\
	font: 24pt; }\
	";

QString xButtonNormalStyleSheet="\
	:enabled {\
	border-image: url(:/images/border_but3_enabled.png) 10;\
	border-width: 10px;\
	color: rgb(5,116,174);\
	font: 24pt; }\
	:disabled {\
	border-image: url(:/images/border_but3_disabled.png) 10;\
	border-width: 10px;\
	color: rgb(154,154,154);\
	font: 24pt; }\
	:hover {\
	border-image: url(:/images/border_but3_hover.png) 10;\
	border-width: 10px;\
	color: rgb(5,116,174);\
	font: 24pt; }\
	:pressed {\
	border-image: url(:/images/border_but3_pressed.png) 10;\
	border-width: 10px;\
	color: rgb(5,116,174);\
	font: 24pt; }\
	";

QString xTabWidgetStyleSheet="\
	QTabBar::tab {\
		border-width: 10px;\
		min-width: %1px;\
		min-height: %2px;\
		font: 24pt; }\
	QTabBar::tab:selected, QTabBar::tab:enabled {\
		border-image: url(:/images/border_but3_enabled.png) 10;\
		color: rgb(154,154,154);\
		}\
	QTabBar::tab:selected, QTabBar::tab:hover{\
		border-image: url(:/images/border_but3_hover.png) 10;\
		color: rgb(154,154,154);\
		}\
	QTabBar::tab:selected {\
		border-image: url(:/images/border_but3_pressed.png) 10;\
		color: rgb(5,116,174);\
		}\
	QTabBar::tab:focus {\
		border-image: url(:/images/border_but0_enabled.png) 10;\
		color: white;\
		}\
	";

QString xTableViewStyleSheet="\
	:enabled {\
		gridline-color: rgb(235,236,236);\
		background: rgb(235,236,236);\
		font: 18pt; }\
	::item:focus {\
		border-image: url(:/images/border_but0_enabled.png) 10;\
		border-width: 2px;\
		color: rgb(5,116,174); }\
	::item:!focus {\
		border-image: url(:/images/border_but1_enabled.png) 10;\
		border-width: 2px;\
		color: rgb(5,116,174); }\
	::item:disabled {\
		border-image: url(:/images/border_but1_disabled.png) 10;\
		border-width: 2px;\
		color: rgb(154,154,154);}\
	QHeaderView {\
		background-color: rgb(235,236,236);\
		}\
	QHeaderView::section  {\
		background-color: rgb(235,236,236);\
		color: rgb(154,154,154);\
		border-image: url(:/images/border_but1_hover.png) 10;\
		border-width: 2px;\
		}\
	QTableView QTableCornerButton::section {\
		background: rgb(235,236,236);\
		border: 0px outset rgb(235,236,236);\
		}\
	";

QString xTabStyleSheet="\
		";
		//:enabled {\
			background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E1E1E1, stop: 0.4 #DDDDDD, stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);\
			border: 2px solid #C4C4C3;\
			border-bottom-color: #C2C7CB;\
			border-top-left-radius: 4px;\
			border-top-right-radius: 4px;\
			min-width: 8ex;\
			padding: 2px; }\
		:selected, :hover {\
			background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #fafafa, stop: 0.4 #f4f4f4, stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);\
			}\
		:selected {\
			border-color: #9B9B9B;\
			border-bottom-color: #C2C7CB; /* same as pane color */\
			}\
		:!selected {\
			margin-top: 2px; /* make non-selected tabs look smaller */\
			}\
		";

xButton::xButton(const QIcon &icon, int icon_size, Qt::ToolButtonStyle style, QWidget *parent) : QToolButton(parent)
	{
	setIcon(icon);
	setIconSize(QSize(icon_size,icon_size));
	setToolButtonStyle(style);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setStyleSheet(xButtonNormalStyleSheet);
	installEventFilter(this);
	}

bool xButton::eventFilter(QObject *object, QEvent *e)
	{
	(void)object;
	switch(e->type())
		{
		case QEvent::FocusIn:
			setStyleSheet(xButtonSelectedStyleSheet);
			break;
		case QEvent::FocusOut:
			setStyleSheet(xButtonNormalStyleSheet);
			break;
		default:
			break;
		}
	return false;
	}

xTabWidget::xTabWidget(QWidget *parent) : QTabWidget(parent)
	{
	installEventFilter(this);
	}

bool xTabWidget::eventFilter(QObject *object, QEvent *e)
	{
	if(e->type() == QEvent::KeyPress)
		{
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
		if(keyEvent->key()==Qt::Key_Escape)
			{
			QWidget *w=parentWidget();
			while(w->inherits("QMainWindow")==false)
				{
				w=w->parentWidget();
				}
			w->close();
			return true;
			}
		else if(object->inherits("QTabWidget")==true)
			{

			switch(keyEvent->key())
				{
				case Qt::Key_Up:
					QApplication::postEvent(this,
								new QKeyEvent(QEvent::KeyPress,
								Qt::Key_Backtab,
								Qt::NoModifier));
					return true;
				case Qt::Key_Down:
					QApplication::postEvent(this,
								new QKeyEvent(QEvent::KeyPress,
								Qt::Key_Tab,
								Qt::NoModifier));
					return true;
				case Qt::Key_Space:
				default:
					break;
				}
			}
		}
	return false;
	}
