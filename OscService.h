#ifndef OSCSERVICE_H
#define OSCSERVICE_H

#include <QWidget>
#include <QDateTime>
#include <QStyledItemDelegate>
#include <QStandardItem>
#include <QtGui>
#include <QLabel>
#include "xWidgets.h"

class QModelIndex;
class QPainter;
class QStyleOptionViewItem;
class OscList_ItemDelegate : public QStyledItemDelegate
	{

	public:
	explicit OscList_ItemDelegate(QObject *parent=0) : QStyledItemDelegate(parent) {}

	void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const;
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	};


//Столбцы таблицы осциллограмм
typedef enum  {OSC_INDEX_COL=0, OSC_DATETIME_COL, OSC_EVENT_COL, OSC_STATE_COL} TOscListColumn;

#define SW_ON_MODEFALGS1_BIT		0x0001
#define REG_SELECTED_MODEFALGS1_BIT	0x0002

#define OKTSERV_DATA_PACKETSIZE		32
#define OKTSERV_FULL_PACKETSIZE		(OKTSERV_DATA_PACKETSIZE+4)
#define OKTSERV_PACKETS_NUM		16
#define OKTSERV_IOPACKET_INDEX		0x0F
#define OKTSERV_DIAGPACKET_INDEX	0x0E

#pragma pack(push,1)
typedef struct
	{
	unsigned short Ustat_1V;
	unsigned short Istat_0_1A;
	signed short Uvozb_0_1V;
	signed short Ivozb_0_1A;
	signed short Istat_react_0_1A;
	signed short Istat_act_0_1A;
	unsigned short OutCode;
	unsigned short ModeFlags1;
	unsigned short ModeFlags2;
	unsigned short Freq_0_01Hz;
	} TOktServNativeData_Packet0;
#define PACKET0_WORDS_COUNT		(sizeof(TOktServNativeData_Packet0)/2)

typedef struct
	{
	unsigned short Uip1_0_01V;
	unsigned short Uip2_0_01V;
	unsigned short Unakopit_0_01V;
	unsigned short U5V_0_01V;
	unsigned short U24V_0_01V;
	unsigned short I24V_0_001A;
	signed short T_0_1C;
	unsigned short Config;
	} TOktServNativeData_Packet2;
#define PACKET2_WORDS_COUNT		(sizeof(TOktServNativeData_Packet2)/2)

typedef struct
	{
	unsigned short KeysState;
	unsigned short U24V1_0_01V;
	unsigned short U24V2_0_01V;
	unsigned short U5V_0_01V;
	unsigned short Ublokv_0_01V;
	unsigned short Iip_0_001A;
	unsigned short Iblokv_0_001A;
	} TOktServNativeData_Packet3;
#define PACKET3_WORDS_COUNT		(sizeof(TOktServNativeData_Packet3)/2)

typedef struct
	{
	unsigned short Ustat_code;
	unsigned short Istat_code;
	signed short Uvozb_code;
	signed short Ivozb_code;
	signed short Istat_react_code;
	signed short Istat_act_code;
	unsigned short Freq;
	unsigned short Irot_f150_300;
	unsigned short Urot_f50;
	} TOktServNativeData_Packet4;
#define PACKET4_WORDS_COUNT		(sizeof(TOktServNativeData_Packet4)/2)

typedef struct
	{
	union
		{
		unsigned short uint16;
		signed short int16;
		} vals[OKTSERV_DATA_PACKETSIZE/2];
	} TOktServNativeData_Packet14;
#define PACKET14_WORDS_COUNT		(sizeof(TOktServNativeData_Packet14)/2)

typedef union
	{
	unsigned char data[OKTSERV_DATA_PACKETSIZE];
	signed short data_w[OKTSERV_DATA_PACKETSIZE/2];
	} TUnstructuredPacket;

typedef union
	{
	struct
		{
		TOktServNativeData_Packet0 Packet0;
		unsigned short reserved0[OKTSERV_DATA_PACKETSIZE/2-PACKET0_WORDS_COUNT];
		unsigned short reserved1[OKTSERV_DATA_PACKETSIZE/2];
		TOktServNativeData_Packet2 Packet2;
		unsigned short reserved2[OKTSERV_DATA_PACKETSIZE/2-PACKET2_WORDS_COUNT];
		TOktServNativeData_Packet3 Packet3;
		unsigned short reserved3[OKTSERV_DATA_PACKETSIZE/2-PACKET3_WORDS_COUNT];
		TOktServNativeData_Packet4 Packet4;
		unsigned short reserved4[OKTSERV_DATA_PACKETSIZE/2-PACKET4_WORDS_COUNT];
		TUnstructuredPacket EndPackets0[OKTSERV_PACKETS_NUM-5/*стоят перед*/-2/*идут далее*/];
		TOktServNativeData_Packet14 Packet14;
		TUnstructuredPacket EndPackets1;
		} Structured;
	TUnstructuredPacket UnstructuredPacket[OKTSERV_PACKETS_NUM];
	} TOktServNativeData;
#pragma pack(pop)


//Структура одной записи данных осциллограмм
typedef struct
	{
	TOktServNativeData_Packet0 Packet0;
	} TOscData;

#define OKTSERV_PACKETS_TO_OSC_NUM		1

typedef struct
	{
	TOscData OscData;
	TOktServNativeData_Packet2 Packet2;
	TOktServNativeData_Packet3 Packet3;
	TOktServNativeData_Packet4 Packet4;
	TOktServNativeData_Packet14 Packet14;
	} TOscDataWithIndic;


//Тип: событие с временем
typedef struct
	{
	//Дата и время события
	QDateTime DateTime;
	//Событие
	QString Text;
	} TEvent;

typedef struct
	{
	//Событие
	TEvent Event;
	//Индекс осциллограммы
	int OscIndex;
	} TEventExt;


typedef struct
	{
	//Индекс начала данных в массиве OscData
	int DataIndex;
	//Смещение записи события относительно начала
	int EventOffset;
	//Событие
	TEventExt EventExt;
	//Указатель на ячейку со статусом осц. в таблице
	QStandardItem *pOscStateCell;
	} TOscProcess;

class TParsTableView : public QTableView
	{
		Q_OBJECT
	public:
		TParsTableView(QWidget *parent) : QTableView(parent)
			{
			installEventFilter(this);
			}
		bool eventFilter(QObject *obj, QEvent *e);
		void currentChanged(const QModelIndex &current, const QModelIndex &previous);

	};

class TOscService : public QWidget
	{
		Q_OBJECT
	public:
		explicit TOscService(QWidget *OscList_parent=0, QWidget *ParametersView_parent=0, bool Base_Reserv=true);

		QList<TOscData> OscData;
		QList<TOscProcess> OscProcess;
		void Reset();
		void AddOscRecord(TOscDataWithIndic &data, bool print);
		int OscStart(TEventExt *);
		int OscCounter;
		QStandardItemModel OscList_Model;
		QTableView *OscList_TableView;
		xTabWidget *ParametersView_TabWidget;
		xButton *OscList_CloseButton;
		xButton *ParametersView_CloseButton;
		QStringList TabNames_StringList;
		struct BitsDescStruct {
			QStringList StringList;
			QList<QStandardItem *> Items[16];
			} ModeFlags1, ModeFlags2, Config, KeysState;

		struct PacketDescStruct {
			QStringList StringList;
			QWidget *Tab;
			TParsTableView *TableView;
			QStandardItemModel Model;
			QList<QStandardItem *> Items[16];
			} Packet[OKTSERV_PACKETS_NUM];
		QTimer *tester_QTimer;
		bool SaveOscillogram(TOscProcess &op);
		void ResetOscProcess();
		void Retranslate();

	signals:

	public slots:
		void ParametersView_CloseWindow();
		void OscList_CloseWindow();

	private:
		int OscHalfSize;
		bool Base_Reserv;
		QWidget *OscList_parent;
		QWidget *ParametersView_parent;
		void BitsDescWriteToOscFile(QFile &, struct BitsDescStruct &);
		void BitsDescFillToModel(struct PacketDescStruct &, struct BitsDescStruct &);
		void BitsStateFillToModel(struct BitsDescStruct &, unsigned short);
	};


//Число записанных осциллограмм
#define OSC_NUM_MAX			100
#define OscFileNamePrefix		"OSC_N"
#define OSC_PROCESS_NUM_MAX		10

//Столбцы таблицы осциллограмм
typedef enum  {PARAMETERSVIEWTAB_ADDR_COL=0, PARAMETERSVIEWTAB_NAME_COL, PARAMETERSVIEWTAB_VAL_COL} TParametersViewTabsColumn;

#endif // OSCSERVICE_H
