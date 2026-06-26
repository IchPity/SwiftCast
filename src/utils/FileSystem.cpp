#include "FileSystem.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDirIterator>

namespace SwiftCast::FileSystem {

bool ensureDir(const QString& path)
{
    return QDir().mkpath(path);
}

bool copyFile(const QString& src, const QString& dst, bool overwrite)
{
    if (overwrite && QFile::exists(dst))
        QFile::remove(dst);
    return QFile::copy(src, dst);
}

bool removeDir(const QString& path)
{
    QDir dir(path);
    return dir.removeRecursively();
}

QString readText(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return {};
    return QTextStream(&f).readAll();
}

bool writeText(const QString& path, const QString& text)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream(&f) << text;
    return true;
}

QStringList listFiles(const QString& dir, const QStringList& filters)
{
    QStringList result;
    QDirIterator it(dir,
                    filters.isEmpty() ? QStringList{"*"} : filters,
                    QDir::Files,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) result << it.next();
    return result;
}

} // namespace SwiftCast::FileSystem
