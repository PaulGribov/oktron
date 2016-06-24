#include "xWidgets.h"
#include "mainwindow.h"

QString xButtonSelectedStyleSheet="\
	:enabled {\
	border-image: url(:/images/border_but0_enabled.png)10;\
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

QString xTableButtonSelectedStyleSheet="\
	:enabled {\
	background: rgb(105,169,216);\
	border-width: 5px;\
	color: white;\
	font: 24pt; }\
	:disabled {\
	background: rgb(235,236,236);\
	border-width: 5px;\
	color: white;\
	font: 24pt; }\
	:hover {\
	background: rgb(144,215,253);\
	border-width: 5px;\
	color: white;\
	font: 24pt; }\
	:pressed {\
	background: rgb(83,147,195);\
	border-width: 5px;\
	color: white;\
	font: 24pt; }\
	";

QString xTableButtonNormalStyleSheet="\
	:enabled {\
	background: rgb(243,243,243);\
	border-width: 1px;\
	color: rgb(5,116,174);\
	font: 24pt; }\
	:disabled {\
	background: rgb(239,239,239);\
	border-width: 1px;\
	color: rgb(154,154,154);\
	font: 24pt; }\
	:hover {\
	background: rgb(255,255,255);\
	border-width: 1px;\
	color: rgb(5,116,174);\
	font: 24pt; }\
	:pressed {\
	background: rgb(217,217,217);\
	border-width: 1px;\
	color: rgb(5,116,174);\
	font: 24pt; }\
	";

QString xTabWidgetStyleSheet="\
	QTabBar::tab {\
		border-width: 10px;\
		min-width: %2px;\
		min-height: %3px;\
		font: %1pt; }\
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
		gridline-color: rgb(154,154,154);\
		background: rgb(235,236,236);\
		font: 20pt; }\
	::item:focus {\
		border-image: url(:/images/border_but0_enabled.png) 10;\
		border-width: 1px;\
		color: rgb(5,116,174); }\
	::item:!focus {\
		background: rgb(235,236,236);\
		border-width: 1px;\
		color: rgb(5,116,174); }\
	::item:disabled {\
		background: rgb(235,236,236);\
		border-width: 1px;\
		color: rgb(154,154,154);}\
	QHeaderView {\
		background-color: rgb(235,236,236);\
		}\
	QHeaderView::section  {\
		background-color: rgb(235,236,236);\
		color: rgb(154,154,154);\
		}\
	QTableView QTableCornerButton::section {\
		background: rgb(235,236,236);\
		border: 0px outset rgb(235,236,236);\
		}\
	QScrollBar:vertical {\
		border: 2px solid grey;\
		background: rgb(235,236,236);\
		width: 45px;\
		}\
	QScrollBar:up-arrow:vertical {\
		image: url(:/images/spin_up_enabled.png);\
		border: 0px;\
		background: rgb(235,236,236);\
		}\
	QScrollBar::down-arrow:vertical {\
		image: url(:/images/spin_down_enabled.png);\
		border: 0px;\
		background: rgb(235,236,236);\
		}\
	/*QScrollBar::handle:vertical {\
		background: rgb(5,116,174);\
		min-width: 45px;\
		}*/\
	";

QString xTabStyleSheet="";

xButton::xButton(int v, const QIcon &icon, int icon_size, Qt::ToolButtonStyle style, QWidget *parent) : QToolButton(parent)
	{
	this->v=v;
	setIcon(icon);
	setIconSize(QSize(icon_size,icon_size));
	setToolButtonStyle(style);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	if(v==TableBut)
		{
		//setMinimumSize(QSize(icon_size,icon_size));
		setStyleSheet(xTableButtonNormalStyleSheet);
		}
	else
		{
		setStyleSheet(xButtonNormalStyleSheet);
		}
	installEventFilter(this);
	}

bool xButton::eventFilter(QObject *obj, QEvent *e)
	{
	switch(e->type())
		{
		case QEvent::FocusIn:
			if(v==TableBut)
				{
				setStyleSheet(xTableButtonSelectedStyleSheet);
				}
			else
				{
				setStyleSheet(xButtonSelectedStyleSheet);
				}
			break;
		case QEvent::FocusOut:
			if(v==TableBut)
				{
				setStyleSheet(xTableButtonNormalStyleSheet);
				}
			else
				{
				setStyleSheet(xButtonNormalStyleSheet);
				}
			break;
		case QEvent::KeyPress:
			break;
		case QEvent::MouseMove:
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonDblClick:
			{
			MainWindow::IdleTimeout=0;
			}
			break;
		default:
			break;
		}
	return QObject::eventFilter(obj, e);
	}

xTabWidget::xTabWidget(QWidget *parent) : QTabWidget(parent)
	{
	installEventFilter(this);
	}

bool xTabWidget::eventFilter(QObject *obj, QEvent *e)
	{
	switch(e->type())
		{
		case QEvent::KeyPress:
			{
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
			int k=keyEvent->key();
			if((k==Qt::Key_Return)||(k==Qt::Key_Space))
				{
				QApplication::postEvent(this,
							new QKeyEvent(QEvent::KeyPress,
							Qt::Key_Tab,
							Qt::NoModifier));
				return true;
				}
			else if(obj->inherits("QTabWidget")==true)
				{
				switch(k)
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
			break;
		case QEvent::MouseMove:
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonDblClick:
			{
			MainWindow::IdleTimeout=0;
			}
			break;

		default:
			break;
		}

	return QObject::eventFilter(obj, e);
	}


bool xComboBox::eventFilter(QObject *obj, QEvent *e)
	{
	/*
	if((e->type() == QEvent::KeyPress))
		{
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);

		switch(keyEvent->key())
			{
			case Qt::Key_Escape:
				{
				QWidget *w=parentWidget();
				while(w->inherits("QMainWindow")==false)
					{
					w=w->parentWidget();
					}
				w->close();
				return true;
				}

			case Qt::Key_Left:
				QApplication::postEvent(this,
							new QKeyEvent(QEvent::KeyPress,
							Qt::Key_Backtab,
							Qt::NoModifier));
				return true;
			case Qt::Key_Right:
				QApplication::postEvent(this,
							new QKeyEvent(QEvent::KeyPress,
							Qt::Key_Tab,
							Qt::NoModifier));
				return true;

			case Qt::Key_Space:
				QApplication::postEvent(this,
							new QKeyEvent(QEvent::KeyPress,
							Qt::Key_Enter,
							Qt::NoModifier));
				return true;

			default:
				break;
			}
		}
		*/
	return QObject::eventFilter(obj, e);
	}

void xTableView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
	{
	QTableView::currentChanged(current, previous);
	MainWindow::IdleTimeout=0;
	}

bool xTableView::eventFilter(QObject *obj, QEvent *e)
	{
	switch(e->type())
		{
		case QEvent::KeyPress:
			{
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
			switch(keyEvent->key())
				{
				default:
					break;
				}
			}
			break;
		case QEvent::MouseMove:
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonDblClick:
			{
			MainWindow::IdleTimeout=0;
			}
			break;
		default:
			break;
		}
	return QObject::eventFilter(obj, e);
	}
