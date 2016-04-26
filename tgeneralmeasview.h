#ifndef TGENERALMEASVIEW_H
#define TGENERALMEASVIEW_H

#include <QtGui>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	#include <QtWidgets/QMainWindow>
#else
	#include <QtGui/QMainWindow>
#endif

#include "tprogsettings.h"
#include "tregsetup.h"
#include "teventslog.h"
#include "toktservext.h"


class TGeneralMeasView : public QWidget
	{
		Q_OBJECT
	public:
		explicit TGeneralMeasView(QWidget *parent = 0);
#define MW_DECL_LABEL_PARAMETER(a)	QLabel *a##0_Label, *a##1_Label, *a##2_Label;
		MW_DECL_LABEL_PARAMETER(Reg)
		MW_DECL_LABEL_PARAMETER(Mode)
		MW_DECL_LABEL_PARAMETER(Ustat)
		MW_DECL_LABEL_PARAMETER(Istat)
		MW_DECL_LABEL_PARAMETER(Uvozb)
		MW_DECL_LABEL_PARAMETER(Ivozb)
		MW_DECL_LABEL_PARAMETER(Istat_react)
		MW_DECL_LABEL_PARAMETER(Istat_act)
		MW_DECL_LABEL_PARAMETER(LastEvent)

		bool PrintDataEnabled;
		void PrintData(TOscDataWithIndic &, QString);
		void PrintDataDisabled();

	signals:

	public slots:
	};

#endif // TGENERALMEASVIEW_H
