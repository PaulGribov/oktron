#include "teventslog.h"
#include "work.h"
#include <QHeaderView>
#include "mainwindow.h"

TEventsLog::TEventsLog(QWidget *obj_MainWindow) : QWidget(obj_MainWindow)
	{
	this->obj_MainWindow=obj_MainWindow;

	QVBoxLayout *EventsList_IntLayout = new QVBoxLayout();
	setLayout(EventsList_IntLayout);

	EventsList_TableView=new xTableView(this);
#ifdef __i386__
	EventsList_TableView->setMinimumHeight(300);
	EventsList_TableView->setMinimumWidth(600);
#endif
	EventsList_IntLayout->addWidget(EventsList_TableView);
	EventsList_TableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	EventsList_TableView->setStyleSheet(xTableViewStyleSheet);
	EventsList_TableView->verticalHeader()->setVisible(false);
	EventsList_TableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	//EventsList_TableView->setShowGrid(false);
	QHeaderView *verticalHeader = EventsList_TableView->verticalHeader();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	verticalHeader->sectionResizeMode(QHeaderView::Fixed);
#else
	verticalHeader->setResizeMode(QHeaderView::Fixed);
#endif
	verticalHeader->setDefaultSectionSize(60);

	EventsList_TableView->setModel(&EventsList_Model);
	EventsList_TableView->setItemDelegate(new EventsList_ItemDelegate());
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
	EventsList_TableView->verticalHeader()->setDefaultAlignment(Qt::AlignVCenter|Qt::AlignRight);
#endif

	Retranslate();
	OscIndex=0;
	Load();
	}

void TEventsLog::Retranslate()
	{
	EventsList_Model.clear();
	EventsList_Model.setHorizontalHeaderLabels(QStringList() << tr("Время") << tr("Событие") << tr("Осц.") );

	EventsList_TableView->setColumnWidth(EVLOG_DATETIME_COL, 130);
	EventsList_TableView->setColumnWidth(EVLOG_TEXT_COL, 335);
	EventsList_TableView->setColumnWidth(EVLOG_OSCINDEX_COL, 80);
	}

void TEventsLog::Close()
	{
	close();
	}

void TEventsLog::AddNewEvent(TEventExt *e)
	{
	qDebug() << tr("New event: ");
	QFile EvLogFile(EVLOG_FILENAME);
	if (!EvLogFile.open(QIODevice::Append | QIODevice::Text)) qDebug() << tr("File open error!");

	QList<QStandardItem*> items;
	int column=0;
	foreach(const QString &text, QStringList() << e->Event.DateTime.toString(tr("dd.MM.yyyy hh:mm:ss")) << e->Event.Text << ((e->OscIndex>=0)?QString().setNum(e->OscIndex):"") )
		{
		qDebug() << text.toUtf8() << ';';
		items << new QStandardItem(text);
		items.back()->setEditable(false);
		if(EvLogFile.isOpen())
			{
			char *buf=utf8_to_cp1251_qstr(text);
			EvLogFile.write(buf);
			delete buf;
			if((++column)<EventsList_Model.columnCount()) EvLogFile.write(";");
			else EvLogFile.write("\r\n");
			}
		}
	qDebug() << "\r\n";

	EventsList_Model.appendRow(items);
	if(EventsList_Model.rowCount()>EVLOG_NUM_MAX)
		{
		EventsList_Model.removeRow(0);
		}

	GotoLastEvent();
	}

void TEventsLog::GotoLastEvent()
	{
	QModelIndex newIndex = EventsList_Model.index(EventsList_Model.rowCount()-1, 0);
	//selectionModel()->select(newIndex, QItemSelectionModel::Select);
	EventsList_TableView->setCurrentIndex(newIndex);
	}

void TEventsLog::Save()
	{
#ifdef __linux__
	QDir::setCurrent("/mnt/localdisk/oscs");
#endif
	qDebug() << tr("Save EvLog-file...");
	QFile EvLogFile(EVLOG_FILENAME);
	if (!EvLogFile.open(QIODevice::WriteOnly | QIODevice::Text)) qDebug() << tr("File open error!");
	else
		{
		for(int row=0;row<EventsList_Model.rowCount();row++)
			{
			for(int column=0; column<EventsList_Model.columnCount(); column++)
				{
				char *buf=utf8_to_cp1251_qstr(EventsList_Model.item(row,column)->text());
				EvLogFile.write(buf);
				delete buf;
				if((column+1)<EventsList_Model.columnCount()) EvLogFile.write(";");
				}
			EvLogFile.write("\r\n");
			}
		EvLogFile.close();
		qDebug() << tr("Ok");
		}
	}

void TEventsLog::Load()
	{
#ifdef __linux__
	QDir::setCurrent("/mnt/localdisk/oscs");
#endif
	qDebug() << tr("Read EvLog-file...");
	EventsList_Model.setRowCount(0);
	int row=0;
	QFile EvLogFile(EVLOG_FILENAME);
	if (!EvLogFile.open(QIODevice::ReadOnly | QIODevice::Text)) qDebug() << tr("File open error!");
	else
		{
#define LINE_IN_CP1251_LEN_MAX	1024
		char line_in_cp1251[LINE_IN_CP1251_LEN_MAX];
		while (!EvLogFile.atEnd())
			{
			int len=EvLogFile.readLine(line_in_cp1251, LINE_IN_CP1251_LEN_MAX);
			if(len)
				{
				char *buf=new char[len*4+1];
				cp1251_to_utf8(buf, line_in_cp1251);
				QStringList fields = QString(buf).split(";");
				delete buf;
				if(fields.size() == EventsList_Model.columnCount())
					{
					//Если список полный - удаление превой строки
					if(row==EVLOG_NUM_MAX)
						{
						row--;
						EventsList_Model.removeRow(0);
						}
					for(int column=0; column<EventsList_Model.columnCount(); column++)
						{
						QString text=fields[column];
						/*
						//Дата и время на разных строках...
						if(column==0)
							{
							QStringList datetime = QString(text).split(" ");
							if(datetime.count()>1)
								{
								text=datetime[0]+"\n"+datetime[1];
								}
							}*/
						QStandardItem *item = new QStandardItem(text);
						item->setEditable(false);
						EventsList_Model.setItem(row, column, item);
						}
					row++;
					}
				}
			}
		EvLogFile.close();
		qDebug() << tr("Events count: ") << QString("%1").number(EventsList_Model.rowCount());
		}

	qDebug() << tr("Search for index of last oscillogram...");
	QDir dir;
	//QApplication::processEvents();
	QFileInfoList listFiles = dir.entryInfoList(QStringList("*.OSD"), QDir::Files);
	QDateTime last;
	last.fromMSecsSinceEpoch(0);
	foreach(QFileInfo file, listFiles)
		{
		QString name = QStringList(file.fileName().split(".")).at(0);
		bool ok;
		name.remove(QRegExp("[A-Za-z]"));
		int i=name.toInt(&ok);
		if(ok && (file.lastModified()>last))
			{
			last = file.lastModified();
			OscIndex=i+1;
			}
		}
	qDebug() << tr("Index: ") << QString("%1").number(OscIndex);
	}

TEventExt *TEventsLog::MakeEvent(QString ev_txt, bool osc)
	{
	TEventExt *e;
	e=new TEventExt;
	e->Event.DateTime = QDateTime::currentDateTime();
	e->Event.Text = ev_txt;
	if(osc)
		{
		e->OscIndex = OscIndex;
		//Счётчик осциллограмм
		if((++OscIndex>99)||(OscIndex<0)) OscIndex=0;
		}
	else
		{
		e->OscIndex = -1;
		}

	AddNewEvent(e);
	return e;
	}

TEventExt *TEventsLog::CheckDataEvent(TOscDataWithIndic &od, TOscDataWithIndic &previous_od)
	{
	TEventExt *e=NULL;
	int f;
	//Изменение состояния флага - Вкл/Выкл
	f=(previous_od.OscData.Packet0.ModeFlags1 ^ od.OscData.Packet0.ModeFlags1) & 0x0D;
	if(f)
		{
		QStringList *s=&(((MainWindow *)obj_MainWindow)->OktServExt[0]->OscService->ModeFlags1.StringList);
		int m=0x01;
		for(int i=0;i<s->count();i++)
			{
			if(f&m)
				{
				if(e) delete e;
				e=MakeEvent(s->at(i) + (od.OscData.Packet0.ModeFlags1 & m ? tr(" - Включено") : tr(" - Выключено")), true);
				}
			}

		}
	f=(previous_od.OscData.Packet0.ModeFlags2 ^ od.OscData.Packet0.ModeFlags2) & 0xFF;
	if(f)
		{
		QStringList *s=&(((MainWindow *)obj_MainWindow)->OktServExt[0]->OscService->ModeFlags2.StringList);
		int m=0x01;
		for(int i=0;i<s->count();i++)
			{
			if(f & m & od.OscData.Packet0.ModeFlags2)
				{
				if(e) delete e;
				e=MakeEvent(s->at(i), true);
				}
			}

		}
	return e;
	}


void EventsList_ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
	QStyledItemDelegate::paint(painter, option, index);
	}

QWidget *EventsList_ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
	return QStyledItemDelegate::createEditor(parent, option, index);
	}

void EventsList_ItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
	{
	QStyledItemDelegate::setEditorData(editor, index);
	}

void EventsList_ItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
	{
	QStyledItemDelegate::setModelData(editor, model, index);
	}


