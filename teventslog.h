#ifndef TEVENTSLOG_H
#define TEVENTSLOG_H

#include <QtGui>
#include <QWidget>
#include "OscService.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QPushButton>
#endif

#define EVLOG_NUM_MAX 100
#define EVLOG_FILENAME "EvLog.txt"

//Столбцы таблицы протокола
typedef enum  {EVLOG_DATETIME_COL=0, EVLOG_TEXT_COL, EVLOG_OSCINDEX_COL} TEventsListColumn;

class EventsList_ItemDelegate : public QStyledItemDelegate
	{

	public:
	explicit EventsList_ItemDelegate(QObject *parent=0) : QStyledItemDelegate(parent) {}

	void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const;
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	};

class TEventsLog : public QWidget
	{
		Q_OBJECT
	public:
		explicit TEventsLog(QWidget *obj_MainWindow);
		QList<TEventExt> EventsList;
		TEventExt *MakeEvent(QString, bool);
		TEventExt *CheckDataEvent(TOscDataWithIndic &, TOscDataWithIndic &);
		QStandardItemModel EventsList_Model;
		xTableView *EventsList_TableView;

		void AddNewEvent(TEventExt *);
		void Save();
		void Load();
		void Retranslate();
		void GotoLastEvent();

	signals:

	public slots:
		void Close();

	private:
		QWidget *obj_MainWindow;
		int OscIndex;
	};


#endif // TEVENTSLOG_H
