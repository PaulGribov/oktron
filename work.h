#ifndef WORK_H
#define WORK_H

void cp1251_to_utf8(char *out, char *in);
int utf8_to_cp1251(char *out, char *in);
char *utf8_to_cp1251_qstr(QString in);
#if !defined(Q_OS_WIN)
	char *LastConnectedDiskFind();
	bool GetDestDiskState(char *dev_name);
	bool CopyToDestDisk(char *dev_name, bool &hex_exists);
	bool SetSystemClock(QDateTime &date);
#endif

#endif // WORK_H
