#include <QtGui>
#include "xspinbox.h"

HexSpinBox::HexSpinBox(QWidget *parent, bool char_short)
    : QSpinBox(parent)
	{
	if((this->char_short=char_short)==true)
		{
		setRange(0,255);
		validator = new QRegExpValidator(QRegExp("[0-9A-Fa-f]{1,2}"), this);
		}
	else
		{
		setRange(0,65535);
		validator = new QRegExpValidator(QRegExp("[0-9A-Fa-f]{1,4}"), this);
		}
	}

QValidator::State HexSpinBox::validate(QString &text, int &pos) const
	{
	return (validator->validate(text,pos));
	}

int HexSpinBox::valueFromText(const QString &text) const
	{
	bool ok;
	return text.toInt(&ok, 16);
	}

QString HexSpinBox::textFromValue(int value) const
	{
	if(char_short)
		{
		return QString("%1").arg(value, 2, 16, QLatin1Char('0')).toUpper();
		}
	else
		{
		return QString("%1").arg(value, 4, 16, QLatin1Char('0')).toUpper();
		}
	}

void HexSpinBox::keyPressEvent(QKeyEvent *e)
	{
	switch (e->key())
		{
		case Qt::Key_Left:
			QSpinBox::keyPressEvent(
				new QKeyEvent(QEvent::KeyPress,
				Qt::Key_Backtab,
				Qt::NoModifier));
			break;

		case Qt::Key_Right:
			QSpinBox::keyPressEvent(
				new QKeyEvent(QEvent::KeyPress,
				Qt::Key_Tab,
				Qt::NoModifier));
			break;

		default:
			QSpinBox::keyPressEvent(e);
		}
	}


DoubleSpinBox::DoubleSpinBox(QWidget *parent, int Order) : QDoubleSpinBox(parent)
	{
	this->Order=Order;
	}

QString DoubleSpinBox::textFromValue(double value) const
	{
	QString text = QString("%L1").arg(value, 0, 'f', Order);
	return text.replace(QLatin1Char('.'), QLocale().decimalPoint());
	}

double DoubleSpinBox::valueFromText(const QString &text) const
	{
	return QString(text).replace(QLocale().decimalPoint(), QLatin1Char('.')).toDouble();
	}

void DoubleSpinBox::keyPressEvent(QKeyEvent *e)
	{
	switch (e->key())
		{
		case Qt::Key_Comma:
		case Qt::Key_Period:
			{
			QKeyEvent *myKeyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Comma,
				e->modifiers(), QLocale().decimalPoint());
			QDoubleSpinBox::keyPressEvent(myKeyEvent);
			break;
			}

		case Qt::Key_Left:
			QDoubleSpinBox::keyPressEvent(
				new QKeyEvent(QEvent::KeyPress,
				Qt::Key_Backtab,
				Qt::NoModifier));
			break;

		case Qt::Key_Right:
			QDoubleSpinBox::keyPressEvent(
				new QKeyEvent(QEvent::KeyPress,
				Qt::Key_Tab,
				Qt::NoModifier));
			break;

		default:
			QDoubleSpinBox::keyPressEvent(e);
		}
	}
