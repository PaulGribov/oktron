#include "tgeneralmeasview.h"

TGeneralMeasView::TGeneralMeasView(QWidget *parent) : QWidget(parent)
	{
	//Текущие измерения
	QVBoxLayout *LeftCol_Layout = new QVBoxLayout();
	QVBoxLayout *RightCol_Layout = new QVBoxLayout();

	QBrush brush(QColor(5,116,174));
	brush.setStyle(Qt::SolidPattern);
	QPalette palette0;
	palette0.setBrush(QPalette::Active, QPalette::WindowText, brush);
	palette0.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
	brush.setColor(QColor(230, 88, 42));
	QPalette palette1;
	palette1.setBrush(QPalette::Active, QPalette::WindowText, brush);
	palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush);

	QFont font0;
	font0.setPointSize(20);
	font0.setBold(false);
	QFont font1;
	font1.setPointSize(58);
	font1.setBold(true);
	QFont font2;
	font2.setPointSize(16);
	font2.setBold(false);
	QFont font3;
	font3.setPointSize(28);
	font3.setBold(true);
	QFont font4;
	font4.setPointSize(12);
	font4.setBold(false);
	//Ustat1_Label->setAlignment(Qt::AlignRight);

#define CREATE_LABEL_PARAMETER(a, c, d, e, g)\
		QHBoxLayout *a##_Layout = new QHBoxLayout();\
		a##0_Label = new QLabel();\
		a##0_Label->setPalette(palette0);\
		a##0_Label->setFont(font##d);\
		a##_Layout->addWidget(a##0_Label, 0, Qt::AlignRight | Qt::AlignVCenter);\
		a##1_Label = new QLabel();\
		a##1_Label->setPalette(palette1);\
		a##1_Label->setFont(font##e);\
		a##1_Label->setFixedWidth(g);\
		a##1_Label->setAlignment(Qt::AlignRight);\
		a##_Layout->addWidget(a##1_Label, 0, Qt::AlignCenter | Qt::AlignVCenter);\
		a##2_Label = new QLabel();\
		a##2_Label->setPalette(palette0);\
		a##2_Label->setFont(font##d);\
		a##_Layout->addWidget(a##2_Label, 0, Qt::AlignLeft | Qt::AlignVCenter);\
		c##_Layout->addLayout(a##_Layout);


#ifndef __i386__
	#define W0 180
	#define W1 105
#else
	#define W0 250
	#define W1 120
#endif
	CREATE_LABEL_PARAMETER(Reg, LeftCol, 2, 3, W0+20)
	CREATE_LABEL_PARAMETER(Ustat, LeftCol, 0, 1, W0)
	CREATE_LABEL_PARAMETER(Istat, LeftCol, 0, 1, W0)
	CREATE_LABEL_PARAMETER(Istat_react, LeftCol, 2, 3, W1)
	CREATE_LABEL_PARAMETER(Mode, RightCol, 2, 3, W0+20)
	CREATE_LABEL_PARAMETER(Uvozb, RightCol, 0, 1, W0)
	CREATE_LABEL_PARAMETER(Ivozb, RightCol, 0, 1, W0)
	CREATE_LABEL_PARAMETER(Istat_act, RightCol, 2, 3, W1)

	QHBoxLayout *Meas_Layout = new QHBoxLayout();
	Meas_Layout->addLayout(LeftCol_Layout);
	Meas_Layout->addLayout(RightCol_Layout);

	setLayout(Meas_Layout);
	/*
	QVBoxLayout *LastEventExt_Layout = new QVBoxLayout();
	CREATE_LABEL_PARAMETER(LastEvent, "", LastEventExt, 4, 4, "", 350)
	ui->LastEvent_GroupBox->setLayout(LastEventExt_Layout);
*/
	Retranslate();
	}

void TGeneralMeasView::Retranslate()
	{
	Reg0_Label->setText(tr("Регулятор:"));
	Mode0_Label->setText(tr("Режим:"));
	Ustat0_Label->setText(tr("U стат:"));
	Istat0_Label->setText(tr("I стат:"));
	Uvozb0_Label->setText(tr("U возб:"));
	Ivozb0_Label->setText(tr("I возб:"));
	Istat_react0_Label->setText(tr("I стат.реакт:"));
	Istat_act0_Label->setText(tr("I стат.акт:"));

	Ustat2_Label->setText(tr("кВ"));
	Istat2_Label->setText(tr("А"));
	Uvozb2_Label->setText(tr("В"));
	Ivozb2_Label->setText(tr("А"));
	Istat_react2_Label->setText(tr("А.реакт."));
	Istat_act2_Label->setText(tr("А.реакт."));
	}


//Печать данных на экран
void TGeneralMeasView::PrintData(TOscDataWithIndic &od, QString name)
	{
	Reg1_Label->setText(name);
	Mode1_Label->setText(((od.OscData.Packet0.ModeFlags1 & SW_ON_MODEFALGS1_BIT)?tr("В РАБОТЕ"):tr("ОСТАНОВ.")));
	Ustat1_Label->setText(tr("%L1").arg((float)od.OscData.Packet0.Ustat_1V/1000, 5, 'f', 2, ' '));
	Istat1_Label->setText(tr("%L1").arg((float)od.OscData.Packet0.Istat_0_1A/10, 5, 'f', 0, ' '));
	Uvozb1_Label->setText(tr("%L1").arg((float)od.OscData.Packet0.Uvozb_0_1V/10, 5, 'f', 1, ' '));
	Ivozb1_Label->setText(tr("%L1").arg((float)od.OscData.Packet0.Ivozb_0_1A/10, 5, 'f', 1, ' '));
	Istat_react1_Label->setText(tr("%L1").arg((float)od.OscData.Packet0.Istat_react_0_1A/10, 5, 'f', 0, ' '));
	Istat_act1_Label->setText(tr("%L1").arg((float)od.OscData.Packet0.Istat_act_0_1A/10, 5, 'f', 0, ' '));
	}

//Печать данных на экран
void TGeneralMeasView::PrintDataDisabled()
	{
	Reg1_Label->setText(tr("---"));
	Mode1_Label->setText(tr("---"));
	Ustat1_Label->setText(tr("---"));
	Istat1_Label->setText(tr("---"));
	Uvozb1_Label->setText(tr("---"));
	Ivozb1_Label->setText(tr("---"));
	Istat_react1_Label->setText(tr("---"));
	Istat_act1_Label->setText(tr("---"));
	}
