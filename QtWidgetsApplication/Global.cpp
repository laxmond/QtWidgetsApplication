#include "Global.h"
#include <QDateTime>

QString txt(const char* s) {
	return QObject::tr("[%1]# %2\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"), QString::fromLocal8Bit(s));
}

QString txt(const QString& s) {
	return QObject::tr("[%1]# %2\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"), s);
}