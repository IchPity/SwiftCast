#pragma once
#include <QString>
#include <QStringList>

namespace SwiftCast::FileSystem {

bool   ensureDir(const QString& path);
bool   copyFile(const QString& src, const QString& dst, bool overwrite = true);
bool   removeDir(const QString& path);
QString readText(const QString& path);
bool   writeText(const QString& path, const QString& text);
QStringList listFiles(const QString& dir, const QStringList& filters = {});

} // namespace SwiftCast::FileSystem
