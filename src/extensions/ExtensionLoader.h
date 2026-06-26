#pragma once
#include <QObject>
#include <QList>
#include <QString>
#include <memory>
#include "../ui/LauncherWindow.h"

namespace SwiftCast {

class ExtensionBase;

class ExtensionLoader : public QObject {
    Q_OBJECT

public:
    explicit ExtensionLoader(QObject* parent = nullptr);
    ~ExtensionLoader();

    // Loads all built-in extensions + any DLLs in the given directory
    void loadAll(const QString& extensionsDir);

    // Query all loaded extensions and merge results (sorted by score)
    QList<ResultItem> query(const QString& text);

    // Dispatch execute to the right extension
    void execute(const QString& actionData);

    QList<ExtensionBase*> loaded() const;

private:
    void registerBuiltIns();
    void loadFromDirectory(const QString& dir);

    QList<std::shared_ptr<ExtensionBase>> m_extensions;
};

} // namespace SwiftCast
