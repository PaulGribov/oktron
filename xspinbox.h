#ifndef xSPINBOX_H
#define xSPINBOX_H

#include <QSpinBox>

class HexSpinBox : public QSpinBox
	{
	Q_OBJECT
	public:
		explicit HexSpinBox(QWidget *parent = 0, bool char_short=true);
	protected:
		QValidator::State validate(QString &text, int &pos) const;
		int valueFromText(const QString &text) const;
		QString textFromValue(int value) const;
		void keyPressEvent(QKeyEvent *e);

	private:
		QRegExpValidator *validator;
		bool char_short;

	signals:

	public slots:

	};

class DoubleSpinBox : public QDoubleSpinBox
	{
	Q_OBJECT
	public:
		explicit DoubleSpinBox(QWidget *parent = 0, int Order=0);
		virtual QString textFromValue(double value) const;
		virtual double valueFromText(const QString &text) const;

	private:
		int Order;

	protected:
		void keyPressEvent(QKeyEvent *e);
	};

#endif // xSPINBOX_H
