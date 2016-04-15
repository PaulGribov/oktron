#ifndef XWIDGETS_H
#define XWIDGETS_H

#include <QtGui>
#include <QWidget>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	#include <QtWidgets/QMainWindow>
	#include <QtWidgets/QComboBox>
	#include <QtWidgets/QApplication>
#endif
#include <QToolButton>
#include <QTableView>
#include <QTabBar>
#include <QStylePainter>
#include <QStyleOptionTabV3>
#include <QPainter>
#include <QIcon>
#include <QString>

extern QString xTableViewStyleSheet;
extern QString xButtonNormalStyleSheet;
extern QString xButtonSelectedStyleSheet;
extern QString xTabStyleSheet;
extern QString xTabWidgetStyleSheet;

enum xButType {
	GenBut,
	TableBut,
	};

class xButton : public QToolButton
	{
		Q_OBJECT
	public:
		explicit xButton(int v, const QIcon &icon, int icon_size=48, Qt::ToolButtonStyle style=Qt::ToolButtonTextUnderIcon, QWidget *parent=0);
		bool eventFilter(QObject *object, QEvent *e);
	private:
		int v;
	};

class xTabWidget : public QTabWidget
	{
		Q_OBJECT
	public:
		explicit xTabWidget(QWidget *parent=0);
		bool eventFilter(QObject *object, QEvent *e);
	};


class xComboBox : public QComboBox
	{
	public:
		xComboBox(QWidget *parent=NULL) : QComboBox(parent)
			{
			installEventFilter(this);
			}

		bool eventFilter(/*QObject *object, */QEvent *e)
			{
			//object;
			if((e->type() == QEvent::KeyPress))
				{
				QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);

				switch(keyEvent->key())
					{
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
			return false;
			}

/*
		void keyPressEvent(QKeyEvent *e)
			{
			switch(e->key())
				{
				case Qt::Key_Left:
					QComboBox::keyPressEvent(
						new QKeyEvent(QEvent::KeyPress,
						Qt::Key_Tab,
						Qt::NoModifier));
					break;

				case Qt::Key_Right:
					QComboBox::keyPressEvent(
						new QKeyEvent(QEvent::KeyPress,
						Qt::Key_Backtab,
						Qt::NoModifier));
					break;

				case Qt::Key_Up:
					e->ignore();
					previousInFocusChain();
					break;

				case Qt::Key_Right:
					e->ignore();
					nextInFocusChain();
					break;
				default:
					QComboBox::keyPressEvent(e);
				}
			}
*/
	};


#endif // XWIDGETS_H
