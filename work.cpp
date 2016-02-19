#include <QtGui>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	#include <QtWidgets/QMainWindow>
#else
	#include <QtGui/QMainWindow>
#endif
#include "mainwindow.h"
#include "work.h"
#include "OscService.h"
#ifdef __linux__
	#include <sys/mount.h>
	#include <sys/types.h>
	#include <sys/wait.h>
	#include <sys/ioctl.h>
	#include <linux/rtc.h>
	#include <fcntl.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <unistd.h>
	#include <errno.h>
	#include <sys/ioctl.h>
	#include <linux/usbdevice_fs.h>
#endif

void cp1251_to_utf8(char *out, char *in)
	{
	static const int table[128] = {
		0x82D0,0x83D0,0x9A80E2,0x93D1,0x9E80E2,0xA680E2,0xA080E2,0xA180E2,
		0xAC82E2,0xB080E2,0x89D0,0xB980E2,0x8AD0,0x8CD0,0x8BD0,0x8FD0,
		0x92D1,0x9880E2,0x9980E2,0x9C80E2,0x9D80E2,0xA280E2,0x9380E2,0x9480E2,
		0,0xA284E2,0x99D1,0xBA80E2,0x9AD1,0x9CD1,0x9BD1,0x9FD1,
		0xA0C2,0x8ED0,0x9ED1,0x88D0,0xA4C2,0x90D2,0xA6C2,0xA7C2,
		0x81D0,0xA9C2,0x84D0,0xABC2,0xACC2,0xADC2,0xAEC2,0x87D0,
		0xB0C2,0xB1C2,0x86D0,0x96D1,0x91D2,0xB5C2,0xB6C2,0xB7C2,
		0x91D1,0x9684E2,0x94D1,0xBBC2,0x98D1,0x85D0,0x95D1,0x97D1,
		0x90D0,0x91D0,0x92D0,0x93D0,0x94D0,0x95D0,0x96D0,0x97D0,
		0x98D0,0x99D0,0x9AD0,0x9BD0,0x9CD0,0x9DD0,0x9ED0,0x9FD0,
		0xA0D0,0xA1D0,0xA2D0,0xA3D0,0xA4D0,0xA5D0,0xA6D0,0xA7D0,
		0xA8D0,0xA9D0,0xAAD0,0xABD0,0xACD0,0xADD0,0xAED0,0xAFD0,
		0xB0D0,0xB1D0,0xB2D0,0xB3D0,0xB4D0,0xB5D0,0xB6D0,0xB7D0,
		0xB8D0,0xB9D0,0xBAD0,0xBBD0,0xBCD0,0xBDD0,0xBED0,0xBFD0,
		0x80D1,0x81D1,0x82D1,0x83D1,0x84D1,0x85D1,0x86D1,0x87D1,
		0x88D1,0x89D1,0x8AD1,0x8BD1,0x8CD1,0x8DD1,0x8ED1,0x8FD1
		};
	volatile int c=0;
	while (*in)
		{
		c++;
		if (*in & 0x80)
			{
			int v = table[(int)(0x7f & *in++)];
			if (!v) continue;
			*out++ = (char)v;
			*out++ = (char)(v >> 8);
			if (v >>= 16) *out++ = (char)v;
			}
		else
			*out++ = *in++;

		}
	*out = 0;
	}


typedef struct ConvLetter
	{
	int	win1251;
	int	unicode;
	} Letter;

const Letter g_letters[] = {
	{0x82, 0x201A}, // SINGLE LOW-9 QUOTATION MARK
	{0x83, 0x0453}, // CYRILLIC SMALL LETTER GJE
	{0x84, 0x201E}, // DOUBLE LOW-9 QUOTATION MARK
	{0x85, 0x2026}, // HORIZONTAL ELLIPSIS
	{0x86, 0x2020}, // DAGGER
	{0x87, 0x2021}, // DOUBLE DAGGER
	{0x88, 0x20AC}, // EURO SIGN
	{0x89, 0x2030}, // PER MILLE SIGN
	{0x8A, 0x0409}, // CYRILLIC CAPITAL LETTER LJE
	{0x8B, 0x2039}, // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
	{0x8C, 0x040A}, // CYRILLIC CAPITAL LETTER NJE
	{0x8D, 0x040C}, // CYRILLIC CAPITAL LETTER KJE
	{0x8E, 0x040B}, // CYRILLIC CAPITAL LETTER TSHE
	{0x8F, 0x040F}, // CYRILLIC CAPITAL LETTER DZHE
	{0x90, 0x0452}, // CYRILLIC SMALL LETTER DJE
	{0x91, 0x2018}, // LEFT SINGLE QUOTATION MARK
	{0x92, 0x2019}, // RIGHT SINGLE QUOTATION MARK
	{0x93, 0x201C}, // LEFT DOUBLE QUOTATION MARK
	{0x94, 0x201D}, // RIGHT DOUBLE QUOTATION MARK
	{0x95, 0x2022}, // BULLET
	{0x96, 0x2013}, // EN DASH
	{0x97, 0x2014}, // EM DASH
	{0x99, 0x2122}, // TRADE MARK SIGN
	{0x9A, 0x0459}, // CYRILLIC SMALL LETTER LJE
	{0x9B, 0x203A}, // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
	{0x9C, 0x045A}, // CYRILLIC SMALL LETTER NJE
	{0x9D, 0x045C}, // CYRILLIC SMALL LETTER KJE
	{0x9E, 0x045B}, // CYRILLIC SMALL LETTER TSHE
	{0x9F, 0x045F}, // CYRILLIC SMALL LETTER DZHE
	{0xA0, 0x00A0}, // NO-BREAK SPACE
	{0xA1, 0x040E}, // CYRILLIC CAPITAL LETTER SHORT U
	{0xA2, 0x045E}, // CYRILLIC SMALL LETTER SHORT U
	{0xA3, 0x0408}, // CYRILLIC CAPITAL LETTER JE
	{0xA4, 0x00A4}, // CURRENCY SIGN
	{0xA5, 0x0490}, // CYRILLIC CAPITAL LETTER GHE WITH UPTURN
	{0xA6, 0x00A6}, // BROKEN BAR
	{0xA7, 0x00A7}, // SECTION SIGN
	{0xA8, 0x0401}, // CYRILLIC CAPITAL LETTER IO
	{0xA9, 0x00A9}, // COPYRIGHT SIGN
	{0xAA, 0x0404}, // CYRILLIC CAPITAL LETTER UKRAINIAN IE
	{0xAB, 0x00AB}, // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
	{0xAC, 0x00AC}, // NOT SIGN
	{0xAD, 0x00AD}, // SOFT HYPHEN
	{0xAE, 0x00AE}, // REGISTERED SIGN
	{0xAF, 0x0407}, // CYRILLIC CAPITAL LETTER YI
	{0xB0, 0x00B0}, // DEGREE SIGN
	{0xB1, 0x00B1}, // PLUS-MINUS SIGN
	{0xB2, 0x0406}, // CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
	{0xB3, 0x0456}, // CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I
	{0xB4, 0x0491}, // CYRILLIC SMALL LETTER GHE WITH UPTURN
	{0xB5, 0x00B5}, // MICRO SIGN
	{0xB6, 0x00B6}, // PILCROW SIGN
	{0xB7, 0x00B7}, // MIDDLE DOT
	{0xB8, 0x0451}, // CYRILLIC SMALL LETTER IO
	{0xB9, 0x2116}, // NUMERO SIGN
	{0xBA, 0x0454}, // CYRILLIC SMALL LETTER UKRAINIAN IE
	{0xBB, 0x00BB}, // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
	{0xBC, 0x0458}, // CYRILLIC SMALL LETTER JE
	{0xBD, 0x0405}, // CYRILLIC CAPITAL LETTER DZE
	{0xBE, 0x0455}, // CYRILLIC SMALL LETTER DZE
	{0xBF, 0x0457} // CYRILLIC SMALL LETTER YI
};


int utf8_to_cp1251(char *out, char *in)
	{
	int i = 0;
	int j = 0;
	for(; in[i] != 0; ++i)
		{
		char prefix = in[i];
		char suffix = in[i+1];
		if ((prefix & 0x80) == 0)
			{
			out[j] = (char)prefix;
			++j;
			}
		else if ((~prefix) & 0x20)
			{
			int first5bit = prefix & 0x1F;
			first5bit <<= 6;
			int sec6bit = suffix & 0x3F;
			int unicode_char = first5bit + sec6bit;

			if ( unicode_char >= 0x410 && unicode_char <= 0x44F )
				{
				out[j] = (char)(unicode_char - 0x350);
				}
			else if (unicode_char >= 0x80 && unicode_char <= 0xFF)
				{
				out[j] = (char)(unicode_char);
				}
			else if (unicode_char >= 0x402 && unicode_char <= 0x403)
				{
				out[j] = (char)(unicode_char - 0x382);
				}
			else
				{
				int count = sizeof(g_letters) / sizeof(Letter);
				for (int k = 0; k < count; ++k)
					{
					if (unicode_char == g_letters[k].unicode)
						{
						out[j] = g_letters[k].win1251;
						goto NEXT_LETTER;
						}
					}
				// can't convert this char
				return 0;
				}
NEXT_LETTER:
			++i;
			++j;
			}
		else
			{
			// can't convert this chars
			return 0;
			}
		}
	out[j] = 0;
	return 1;
	}

char *utf8_to_cp1251_qstr(QString in)
	{
	char *buf=new char[in.length()+1];
	utf8_to_cp1251(buf, in.toUtf8().data());
	return buf;
	}


#ifdef __linux__
char *LastConnectedDiskFind()
	{
	char dev_name[256];
	static char last_dev_name[256];
	strcpy(dev_name, "sd");
	last_dev_name[0]=0;

	for(int i=0;i<4;i++)
		{
#ifndef __i386__
		dev_name[2]='a'+i;
#else
		dev_name[2]='b'+i;
#endif
		dev_name[3]=0;
		if(GetDestDiskState(dev_name))
			{
			strcpy(last_dev_name, dev_name);
			strcat(dev_name, "/");
			strcat(dev_name, last_dev_name);
			int k=strlen(dev_name);
			dev_name[k+1]=0;
			for(int j=0;j<4;j++)
				{
				dev_name[k]='1'+j;
				if(GetDestDiskState(dev_name))
					{
					last_dev_name[3]='1'+j;
					last_dev_name[4]=0;
					}
				}
			}
		}
	return last_dev_name;
	}


bool GetDestDiskState(char *dev_name)
	{
	FILE *fp;
	char command[256];
	char str0[256];
	bool result=false;
	strcpy(command, "udevadm info -a -p /sys/block/");
	strcat(command, dev_name);
	fp = popen(command, "r");
	if(fp)
		{
		str0[255]=0;
		while(fgets(str0, 255, fp) != NULL)
			{
			if(strstr(str0, "SUBSYSTEM==\"block\"")) result=true;
			}

		pclose(fp);
		}
	return result;
	}

bool usbreset(const char *dev_name)
	{
	int fd;
	int rc;

	fd = open(dev_name, O_WRONLY);
	if (fd < 0)
		{
		qDebug() << "usbreset: Error opening output file";
		return false;
		}

	rc = ioctl(fd, USBDEVFS_RESET, 0);
	/*
	if (rc < 0)
		{
		qDebug() << "usbreset: Error in ioctl";
		return false;
		}
	*/
	printf("usbreset: Reset successful");
	close(fd);
	return true;
	}


bool CopyToDestDisk(char *dev_name)
	{
	FILE *fp;
	char command[256];
	char str0[256];
	int errorlevel=1;
	int status;
#ifndef __i386__
	//Открыть доступ для записи в /dev
	strcpy(command, "mount -no remount,rw /dev");
	fp = popen(command, "r");
	if(fp)
		{
		memset(str0, 0, 256);
		while(fgets(str0, 255, fp) != NULL)
			{
			qDebug() << str0;
			}
		status=pclose(fp);
		errorlevel=WEXITSTATUS(status);
		if(errorlevel)
			{
remdeverr_loc:
			qDebug() << "Remount /dev error!";
			return false;
			}
		}
	else
		goto remdeverr_loc;

	//Сброс USB-устройства - для обновления записи в /dev
	usbreset("/dev/bus/usb/002/001");

	sleep(5);
#endif

	//Монтирование USB-диска
	strcpy(command, "mount -n -t vfat ");
	strcat(command, "/dev/");
	strcat(command, dev_name);
	strcat(command, " /mnt/destdisk");
	fp = popen(command, "r");
	if(fp)
		{
		memset(str0, 0, 256);
		while(fgets(str0, 255, fp) != NULL)
			{
			qDebug() << str0;
			}
		status=pclose(fp);
		errorlevel=WEXITSTATUS(status);
		qDebug() << QString("Mount errorlevel: %1").arg(errorlevel);

		if(errorlevel==0)
			{

			sleep(5);

			//Копирование содержимого каталога
			strcpy(command, "cp -v -f -r /mnt/localdisk/oscs /mnt/destdisk/osc_from__");
			strcat(command, QDateTime::currentDateTime().toString("dd_MM_yyyy__hh_mm_ss").toAscii().data());
			fp = popen(command, "r");
			if(fp)
				{
				memset(str0, 0, 256);
				while(fgets(str0, 255, fp) != NULL)
					{
					qDebug() << str0;
					}
				status=pclose(fp);
				errorlevel=WEXITSTATUS(status);
				qDebug() << QString("cp localdisk/oscs->destdisk: %1").arg(errorlevel);
				}
#ifndef __i386__
			//Обратное копирование hex-файлов
			QDir dir("/mnt/destdisk");
			QFileInfoList listFiles = dir.entryInfoList(QStringList("*.hex"), QDir::Files);
			if((errorlevel==0)&&(listFiles.count()>0))
				{
				strcpy(command, "cp -v -f /mnt/destdisk/*.hex /mnt/localdisk");
				fp = popen(command, "r");
				if(fp)
					{
					memset(str0, 0, 256);
					while(fgets(str0, 255, fp) != NULL)
						{
						qDebug() << str0;
						}
					status=pclose(fp);
					errorlevel=WEXITSTATUS(status);
					qDebug() << QString("cp destdisk->localdisk: %1").arg(errorlevel);
					}
				}
#endif
			//Размонтирование флешки
			umount("/mnt/destdisk");
			}
		}
#ifndef __i386__
	//Закрыть доступ для записи в /dev
	strcpy(command, "mount -no remount,ro /dev");
	fp = popen(command, "r");
	if(fp)
		{
		memset(str0, 0, 256);
		while(fgets(str0, 255, fp) != NULL)
			{
			qDebug() << str0;
			}
		status=pclose(fp);
		if(WEXITSTATUS(status)) goto remdeverr_loc;
		}
	else
		goto remdeverr_loc;
#endif
	return (errorlevel==0)?true:false;
	}

bool SetSystemClock(QDateTime &date)
	{
	bool result=false;
	FILE *fp;
	char command[256];
	char str0[256];

	//Установка системного времени
	strcpy(command, "date ");
#ifndef __i386__
	strcat(command, date.toString("yyyyMMddhhmm.ss").toAscii().data());
#else
	strcat(command, date.toString("MMddhhmmyyyy.ss").toAscii().data());
#endif
	qDebug() << command;
	fp = popen(command, "r");
	if(fp)
		{
		str0[255]=0;
		while(fgets(str0, 255, fp) != NULL)
			{
			qDebug() << str0;
			}
		result=true;
		pclose(fp);
		}
#ifndef __i386__
	//Синхронизация аппаратного времени с системным
	if(result)
		{
		strcpy(command, "hwclock --systohc");
		qDebug() << command;
		fp = popen(command, "r");
		if(fp)
			{
			str0[255]=0;
			while(fgets(str0, 255, fp) != NULL)
				{
				qDebug() << str0;
				}
			pclose(fp);
			}
		}
#endif
	return result;
	}
#endif
