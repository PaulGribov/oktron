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
		bool eventFilter(QObject *obj, QEvent *e);
	private:
		int v;
	};

class xTabWidget : public QTabWidget
	{
		Q_OBJECT
	public:
		explicit xTabWidget(QWidget *parent=0);
		bool eventFilter(QObject *obj, QEvent *e);
	};


class xComboBox : public QComboBox
	{
	public:
		xComboBox(QWidget *parent=NULL) : QComboBox(parent)
			{
			installEventFilter(this);
			}
		bool eventFilter(QObject *obj, QEvent *e);
	};

class xTableView : public QTableView
	{
		Q_OBJECT
	public:
		xTableView(QWidget *parent=NULL) : QTableView(parent)
			{
			installEventFilter(this);
			}
		bool eventFilter(QObject *obj, QEvent *e);

		void currentChanged(const QModelIndex &current, const QModelIndex &previous);
	};

#endif // XWIDGETS_H
