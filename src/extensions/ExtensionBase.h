#pragma once
#include <QString>
#include <QList>
#include <QJsonObject>

namespace SwiftCast {

struct ResultItem;

// Every extension implements this interface.
// Extensions can be built-in (compiled in) or external (loaded as DLL).
class ExtensionBase {
public:
    virtual ~ExtensionBase() = default;

    // Unique identifier, e.g. "com.swiftcast.app-launcher"
    virtual QString id() const = 0;

    // Display name shown in the extension store
    virtual QString name() const = 0;

    // Short description
    virtual QString description() const = 0;

    // Version string "major.minor.patch"
    virtual QString version() const = 0;

    // Called once after the extension is loaded.
    // config is the extension's personal JSON config from settings.
    virtual void initialize(const QJsonObject& config) { Q_UNUSED(config) }

    // Return results relevant to the query. Called on every keystroke.
    virtual QList<ResultItem> query(const QString& text) = 0;

    // Execute the action for the given actionData string.
    virtual void execute(const QString& actionData) { Q_UNUSED(actionData) }
};

} // namespace SwiftCast
