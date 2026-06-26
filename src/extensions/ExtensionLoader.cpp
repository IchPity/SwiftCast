#include "ExtensionLoader.h"
#include "ExtensionBase.h"
#include "../ui/LauncherWindow.h"

#include <QDir>
#include <QLibrary>
#include <QDirIterator>
#include <QStandardPaths>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include <QClipboard>
#include <QRegularExpression>
#include <algorithm>

// ── Built-in: App Launcher ───────────────────────────────────────────────────

#ifdef Q_OS_WIN
#  include <windows.h>
#  include <shlobj.h>
#  include <QSettings>
#endif

namespace SwiftCast {

// Simple built-in app launcher extension
class AppLauncherExtension : public ExtensionBase {
public:
    QString id()          const override { return "com.swiftcast.app-launcher"; }
    QString name()        const override { return "App Launcher"; }
    QString description() const override { return "Search and open installed applications"; }
    QString version()     const override { return "1.0.0"; }

    void initialize(const QJsonObject&) override
    {
        indexApps();
    }

    QList<ResultItem> query(const QString& text) override
    {
        if (text.trimmed().length() < 2) return {};

        QList<ResultItem> results;
        const QString lower = text.toLower();

        for (const AppEntry& app : m_apps) {
            if (app.name.toLower().contains(lower)) {
                ResultItem r;
                r.title      = app.name;
                r.subtitle   = app.path;
                r.category   = "App";
                r.actionData = "exec:" + app.path;
                r.score      = app.name.toLower().startsWith(lower) ? 100 : 50;
                results.append(r);
            }
        }

        std::sort(results.begin(), results.end(),
                  [](const ResultItem& a, const ResultItem& b){
                      return a.score > b.score;
                  });

        return results.mid(0, 6);
    }

private:
    struct AppEntry { QString name; QString path; };
    QList<AppEntry> m_apps;

    void indexApps()
    {
#ifdef Q_OS_WIN
        // Common locations for shortcuts
        QStringList searchDirs;
        wchar_t buf[MAX_PATH];

        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_COMMON_PROGRAMS, nullptr, 0, buf)))
            searchDirs << QString::fromWCharArray(buf);
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_PROGRAMS, nullptr, 0, buf)))
            searchDirs << QString::fromWCharArray(buf);
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr, 0, buf)))
            searchDirs << QString::fromWCharArray(buf);
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_COMMON_DESKTOPDIRECTORY, nullptr, 0, buf)))
            searchDirs << QString::fromWCharArray(buf);

        for (const QString& dir : searchDirs) {
            QDirIterator it(dir, {"*.lnk", "*.exe"},
                            QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                it.next();
                AppEntry e;
                e.name = it.fileInfo().baseName();
                e.path = it.filePath();
                m_apps.append(e);
            }
        }
#endif
    }
};

// ── Built-in: Calculator ─────────────────────────────────────────────────────

class CalculatorExtension : public ExtensionBase {
public:
    QString id()          const override { return "com.swiftcast.calculator"; }
    QString name()        const override { return "Calculator"; }
    QString description() const override { return "Evaluate math expressions instantly"; }
    QString version()     const override { return "1.0.0"; }

    QList<ResultItem> query(const QString& text) override
    {
        QString t = text.trimmed();
        if (t.isEmpty()) return {};

        // Only trigger if text looks like a math expression
        static QRegularExpression mathRe(R"(^[\d\s\+\-\*\/\.\(\)\^%]+$)");
        if (!mathRe.match(t).hasMatch()) return {};

        // Simple evaluation using QJSEngine would be ideal,
        // but to avoid Qt Quick dependency we do basic ops:
        bool ok = false;
        double result = evaluate(t, ok);
        if (!ok) return {};

        ResultItem r;
        r.title      = QString::number(result, 'g', 10);
        r.subtitle   = t + " = " + r.title;
        r.category   = "Calculator";
        r.actionData = "copy:" + r.title;
        r.score      = 200;
        return {r};
    }

    void execute(const QString& actionData) override
    {
        if (actionData.startsWith("copy:")) {
            QApplication::clipboard()->setText(actionData.mid(5));
        }
    }

private:
    double evaluate(const QString&, bool& ok) {
        // Placeholder — real implementation uses a proper expression parser
        ok = false;
        return 0.0;
    }
};

// ── ExtensionLoader ──────────────────────────────────────────────────────────

ExtensionLoader::ExtensionLoader(QObject* parent)
    : QObject(parent)
{}

ExtensionLoader::~ExtensionLoader() = default;

void ExtensionLoader::loadAll(const QString& extensionsDir)
{
    registerBuiltIns();
    loadFromDirectory(extensionsDir);
}

void ExtensionLoader::registerBuiltIns()
{
    auto appLauncher = std::make_shared<AppLauncherExtension>();
    appLauncher->initialize({});
    m_extensions.append(appLauncher);

    auto calc = std::make_shared<CalculatorExtension>();
    calc->initialize({});
    m_extensions.append(calc);
}

void ExtensionLoader::loadFromDirectory(const QString& dir)
{
    QDir d(dir);
    if (!d.exists()) return;

    // Future: load DLL-based extensions via QLibrary
    // Each DLL exports: ExtensionBase* swiftcast_create_extension();
    const auto entries = d.entryList({"*.dll", "*.so", "*.dylib"}, QDir::Files);
    for (const QString& file : entries) {
        QLibrary lib(d.filePath(file));
        if (!lib.load()) continue;

        using CreateFn = ExtensionBase*(*)();
        auto create = reinterpret_cast<CreateFn>(lib.resolve("swiftcast_create_extension"));
        if (!create) continue;

        ExtensionBase* ext = create();
        if (ext) {
            m_extensions.append(std::shared_ptr<ExtensionBase>(ext));
            ext->initialize({});
        }
    }
}

QList<ResultItem> ExtensionLoader::query(const QString& text)
{
    QList<ResultItem> all;
    for (const auto& ext : m_extensions) {
        all.append(ext->query(text));
    }
    std::sort(all.begin(), all.end(),
              [](const ResultItem& a, const ResultItem& b){
                  return a.score > b.score;
              });
    return all.mid(0, 10);
}

void ExtensionLoader::execute(const QString& actionData)
{
    for (const auto& ext : m_extensions) {
        ext->execute(actionData);
    }
}

QList<ExtensionBase*> ExtensionLoader::loaded() const
{
    QList<ExtensionBase*> list;
    for (const auto& e : m_extensions) list.append(e.get());
    return list;
}

} // namespace SwiftCast
