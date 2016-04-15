#ifndef TOKTSERVREGSETUP_H
#define TOKTSERVREGSETUP_H

#include <QWidget>
#include <QStyledItemDelegate>
#include <QStandardItem>
#include <QTableView>
#include <QProgressBar>
#include <QFrame>
#include <QPushButton>
#include <QItemDelegate>
#include <QLayout>
#include <QLabel>
#include <QHeaderView>
#include "toktserv.h"
#include "xWidgets.h"

typedef enum
	{
	CMD_ERROR=0,
	CMD_IDLE,
	CMD_GET_VAL,
	CMD_GET_NAME,
	CMD_READ_SETTINGS_DESC,
	CMD_INC_VAL,
	CMD_DEC_VAL,
	CMD_SET_VAL,
	CMD_SAVE_VAL,
	CMD_SETTINGS_APPLY,
	CMD_BLOCK_IS_EXISTS,
	CMD_GET_BLOCK_ID,
	CMD_PUT_CODE_BLOCK
	} TRegSetupCmd;

#define REG_SETUP_PARS_NUM	256
#define GETBLOCKSID_PARS_NUM	16

#define REG_SETUP_NAME_LEN	16
#define GETBLOCKSID_NAME_LEN	16

//Тип группы параметра. Определяет к какой из 4-х групп относится параметр.
typedef enum { REGSETUP_GROUPS_MOTOR=0,		//уставки агрегата
	       REGSETUP_GROUPS_PROTECT,		//уставки алгоритмов и защит
	       REGSETUP_GROUPS_WORK,		//уставки текущей работы
	       REGSETUP_GROUPS_COEF,		//индивидуальные коэффициенты
	       REGSETUP_GROUPS_EMPTY,		//не используется
	       REGSETUP_GROUPS_NUM,
	       } TRegSetupParGroup;

//Влияние параметра на другие параметры
typedef enum { afNot=0,			//Не влияет на другие параметры
	       afOnValInOwnGroup,	//Влияет на значение параметров в своей группе отображения
	       afOnValInOtherGroups,	//Влияет на значения параметров в других группах отображения
	       afOnOwnName,		//Влияет только на свое собственное название
	       afOnValInAllGroups,	//Влияет на значения параметров в своей и других группах отображения
	       afOnNamesInOwnGroup,	//Влияет на названия параметров в своей группе отображения
	       afOnNamesInOtherGroups,	//Влияет на названия параметров только в других группах отображения
	       afGlobalPar		//Глобальный параметр. Изменение его может привести к значительным
	       } TRegSetupParAffect;

//Размерность параметра и тип отображения
typedef enum { tyName=0,		//Название
		tyTitle,		//Заголовок
		tyYesNo,		//Двоичный формат. Если параметр равен "0" то индицировать "НЕТ", иначе индицировать "ДА "
		tyButton,		//Параметр "Кнопка"
		tyHexChar,		//HEX формат отображения 1 байт
		tyHexShort,		//HEX формат отображения 2 байта
		tyUnsigned,		//Десятичный, беззнаковый формат
		tySigned		//Десятичный, знаковый формат
		} TRegSetupParType;

typedef enum { db1_0=0, db1_28, db2_56, db5_12 } TRegSetupParDivBy;
typedef enum { odb1_0=0, odb0_1, odb0_01, odb0_001 } TRegSetupParOrder;


//Столбцы таблицы
typedef enum  {REGSETUPLIST_NAME_COL=0, REGSETUPLIST_VAL_COL, REGSETUPLIST_PLUSBUT_COL, REGSETUPLIST_MINUSBUT_COL, REGSETUPLIST_READBUT_COL, REGSETUPLIST_WRITEBUT_COL, REGSETUPLIST_STATUS_COL, REGSETUPLIST_COLS_NUM } TRegSetupListColumn;

class TRegSetupPar : public QWidget
	{
		Q_OBJECT
	public:
		explicit TRegSetupPar(QWidget *parent = 0, int index=0) : QWidget(parent)
			{
			Index=index;
			OSRS_parent=parent;
			for(int i=0;i<REGSETUPLIST_COLS_NUM;i++)
				{
				pCell[i]=NULL;
				}
			Enabled=false;
			}
		int Value;
		QString Name;
		bool Enabled;
		bool Writable;
		TRegSetupParAffect Affect;	//Влияние параметра на другие параметры
		bool TakenImmediately;		/*Параметр принимается к исполнению немедленно.
						Если бит "1", то параметр сразу же начинает влиять
						на работу изделия. Если бит равен "0" - то его влияние
						проявится только после "сохранения".*/
		TRegSetupParType ParType;	//Размерность параметра и тип отображения
		float DivBy;
		int Order;
		bool OnlyDecInc;		//Параметр меняется только инкрементно.
		QStandardItem *pCell[REGSETUPLIST_COLS_NUM];	//Указатель на ячейку

		bool IsEditable[REGSETUPLIST_COLS_NUM]; //Для хранения знаачения из поля StandartItem

	private:
		int Index;
		static int IndexCounter;
		void *OSRS_parent;

	public slots:
		void ButClick();
		void PlusButClick();
		void MinusButClick();
		void ReadButClick();
		void WriteButClick();

	};


class QModelIndex;
class QPainter;
class QStyleOptionViewItem;

class TRegSetupList_ItemDelegate : public QStyledItemDelegate
	{

	private:
		QWidget *OSRS_parent;
	public:
		explicit TRegSetupList_ItemDelegate(QWidget *OSRS_parent=NULL) : QStyledItemDelegate(OSRS_parent)
			{
			this->OSRS_parent=OSRS_parent;
			}
		void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
		void setEditorData(QWidget *editor, const QModelIndex &index) const;
		void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

	};


//Столбцы таблицы
typedef enum  {GETBLOCKSID_NAME_COL=0, GETBLOCKSID_N_COL, GETBLOCKSID_ID_COL, GETBLOCKSID_HEXFILE_COL, GETBLOCKSID_UPDATE_COL, GETBLOCKSID_STATUS_COL, GETBLOCKSID_COLS_NUM } TGetBlocksIDListColumn;

class TGetBlocksIDPar : public QWidget
	{
		Q_OBJECT
	public:
		explicit TGetBlocksIDPar(QWidget *parent = 0, int index=0) : QWidget(parent)
			{
			Index=index;
			OSRS_parent=parent;
			for(int i=0;i<GETBLOCKSID_COLS_NUM;i++)
				{
				pCell[i]=NULL;
				}
			Updatable=false;
			UpdBut=NULL;
			}

		QString Id;
		QString HexId;
		bool Updatable;
		int Addr;
		xButton *UpdBut;
		QStandardItem *pCell[GETBLOCKSID_COLS_NUM];	//Указатель на ячейку


	private:
		int Index;
		static int IndexCounter;
		void *OSRS_parent;

	public slots:
		void ButClick();

	};

class TGetBlocksIDList_ItemDelegate : public QStyledItemDelegate
	{

	private:
		QWidget *OSRS_parent;
	public:
		explicit TGetBlocksIDList_ItemDelegate(QWidget *OSRS_parent=NULL) : QStyledItemDelegate(OSRS_parent)
			{
			this->OSRS_parent=OSRS_parent;
			}
		void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const;
		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
		//void setEditorData(QWidget *editor, const QModelIndex &index) const;
		void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	};


class RegSetupTableView : public QTableView
	{
	public:
		RegSetupTableView(QWidget *NextFocusChain=NULL, TRegSetupPar **pRegSetupPars=NULL, TGetBlocksIDPar **pGetBlocksIDPars=NULL, void *parent=NULL) : QTableView()
			{
			this->NextFocusChain=NextFocusChain;
			this->pRegSetupPars=pRegSetupPars;
			this->pGetBlocksIDPars=pGetBlocksIDPars;
			OSRS_parent=parent;
			setStyleSheet(xTableViewStyleSheet);
			verticalHeader()->setVisible(false);
			installEventFilter(this);
			}

		QWidget *NextFocusChain;
		TRegSetupPar **pRegSetupPars;
		TGetBlocksIDPar **pGetBlocksIDPars;
		void *OSRS_parent;
		void OpenEditor4Index(QModelIndex current);
		void CloseEditor4Index(QModelIndex previous);
		void keyPressEvent(QKeyEvent *event);
		void currentChanged(const QModelIndex &current, const QModelIndex &previous);
		bool eventFilter(QObject *object, QEvent *e);
	};

typedef struct
	{
	RegSetupTableView *TableView;
	QStandardItemModel Model;
	QVBoxLayout *Layout;
	struct
		{
		QFrame *Frame;
		QVBoxLayout *Layout;
		QLabel *Label;
		QProgressBar *ProgressBar;
		} LoadPars;
	struct
		{
		QHBoxLayout *Layout;
		QSpacerItem *Spacer;
		xButton *Reload_Button;
		xButton *Close_Button;
		} ButtonsBar;
	QWidget *Focused;
	} TRegSetupWidgets;


class TOktServRegSetup : public QWidget
	{
		Q_OBJECT
	public:
		explicit TOktServRegSetup(QWidget *RegSetupParent=NULL, QWidget *GetBlocksIDParent=NULL, TOktServ *OktServ=NULL);


		xButton *SettingsApply_Button;
		xButton *SaveSettings_Button;

#define	REQ_SEQ_LEN	300
		struct {
			TRegSetupCmd c;
			int index;
			} ReqSeq[REQ_SEQ_LEN];
		int ReqSeqIndex;

		TOktServ *OktServ;
		QTimer *RegSetup_QTimer;

		TRegSetupWidgets RegSetup;
		TRegSetupList_ItemDelegate *RegSetupList_ItemDelegate;

		TRegSetupWidgets GetBlocksID;
		TGetBlocksIDList_ItemDelegate *GetBlocksIDList_ItemDelegate;

		void RegSetupWidgetsCreate(TRegSetupWidgets &, QWidget *NextFocusChain, TRegSetupPar **pRegSetupPars, TGetBlocksIDPar **pGetBlocksIDPars);
		void WidgetsEnabled(bool);

		TRegSetupPar **pRegSetupPars;
		TGetBlocksIDPar **pGetBlocksIDPars;
		float CalcValue(int index);
		void StringValue(int index, QString &str);
		static QStringList YESNO_sl;

		QMutex ReqSeqLock;

		bool PostReq(TRegSetupCmd c, int index)
			{
			if(ReqSeqIndex<REQ_SEQ_LEN-1)
				{
				QMutexLocker locker(&ReqSeqLock);
				ReqSeq[ReqSeqIndex].c=c;
				ReqSeq[ReqSeqIndex].index=index;
				ReqSeqIndex++;
				return true;
				}
			return false;
			}

		bool PostReqWithStatusClear(TRegSetupCmd c, int index)
			{
			if(!pRegSetupPars[index]->Enabled) return false;
			for(int i=0;i<REG_SETUP_PARS_NUM;i++)
				{
				if(pRegSetupPars[i]->pCell[REGSETUPLIST_STATUS_COL])
					{
					pRegSetupPars[i]->pCell[REGSETUPLIST_STATUS_COL]->setIcon(QIcon());
					pRegSetupPars[i]->pCell[REGSETUPLIST_STATUS_COL]->setText(tr(""));
					}
				}
			return PostReq(c, index);
			}



		void RegSetupParSetFlags(int);
		QStringList GetBlocksID_StringList;
		void FindHEXs_GetBlocksID();
		bool LoadHEX_GetBlocksID(QString);
		bool IsBusy();
		void Start_Service();
		void StopService();
		void Retranslate();

typedef enum	{errIOPacketReceiveTimeout=0,
		errIOPacketDeliveredTimeout,
		errAnswerTimeout,
		errBlocksExistsInvalidAnswer,
		errPutCodeBlock_DeviceBusyTimeout,
		errPutCodeBlock_DeviceNotExists,
		errProcessAborted,
		errHexFileNotLoaded, } TRegSetupErrors;

		int LastErrorCode;
		QStringList RegSetupErrors_StringList;
		int StartupDelay;


	private:
		int Hex2Byte_LoadHEX_GetBlocksID(QString HexLine, int &p, int &CRC, bool &res);

		QWidget *PrevWidget;
		TRegSetupCmd Cmd, prevCmd;
		int ParameterIndex;
		int HexLineIndex, HexLinesNumber;
		QStringList HexFile_StringList;
		int HexUploadTimeout;

		volatile bool Wait4Parameter;
		int TimeoutCnt, IOPacketDeliveredTimeoutCnt, IOPacketReceiveTimeoutCnt;
		int Wait4Parameter_PacketID;
		int wait_qp_cnt;
		void AddRow2Table_RegSetup();
		void AddRow2Table_GetBlocksID(QString, bool, bool);
		unsigned short BlocksExistsBits;

		QModelIndex GetBlocksID_SavedIndex;
		QModelIndex RegSetup_SavedIndex;

	signals:
		void RegSetup_CloseSignal();
		void GetBlocksID_CloseSignal();

	public slots:
		void ReadSettingsDesc();
		void RegSetupReq();
		void Stop();
		void RegSetup_CloseSlotInternal();
		void SettingsApply();
		void SaveSettings();
		void GetBlocksID_CloseSlot();
		void GetBlocksIDSlot();

	};

#define REGSETUPDBG

#endif // TOKTSERVREGSETUP_H
