#ifndef WORK_H
#define WORK_H

void cp1251_to_utf8(char *out, char *in);
int utf8_to_cp1251(char *out, char *in);
char *utf8_to_cp1251_qstr(QString in);
#ifdef __linux__
char *LastConnectedDiskFind();
bool GetDestDiskState(char *dev_name);
bool CopyToDestDisk(char *dev_name);
bool SetSystemClock(QDateTime &date);
#endif

#endif // WORK_H