#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>

namespace SwiftCast {

class ExtensionLoader;

struct ResultItem {
    QString title;
    QString subtitle;
    QString category;
    QString iconName;
    QString actionData;
    int     score = 0;
};

class SearchBar : public QLineEdit {
    Q_OBJECT
public:
    explicit SearchBar(QWidget* parent = nullptr);
protected:
    void paintEvent(QPaintEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
signals:
    void escapePressed();
    void arrowDown();
    void arrowUp();
};

class ResultDelegate : public QAbstractItemDelegate {
    Q_OBJECT
public:
    explicit ResultDelegate(QObject* parent = nullptr);
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;
};

class LauncherWindow : public QWidget {
    Q_OBJECT

public:
    explicit LauncherWindow(ExtensionLoader* extensions, QWidget* parent = nullptr);

    void popup();

signals:
    void settingsRequested();

protected:
    void keyPressEvent(QKeyEvent* e) override;
    void focusOutEvent(QFocusEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* e) override;
    void paintEvent(QPaintEvent* e) override;
    void showEvent(QShowEvent* e) override;

private slots:
    void onSearchChanged(const QString& text);
    void onItemActivated(QListWidgetItem* item);
    void activateSelected();

private:
    void buildUi();
    void applyWindowEffect();
    void centerOnScreen();
    void populateResults(const QString& query);
    void executeItem(const ResultItem& item);
    void setResults(const QList<ResultItem>& items);

    SearchBar*       m_searchBar  = nullptr;
    QListWidget*     m_resultList = nullptr;
    QLabel*          m_statusBar  = nullptr;
    QLabel*          m_searchIcon = nullptr;
    ExtensionLoader* m_extensions = nullptr;
    QPropertyAnimation* m_showAnim = nullptr;

    QList<ResultItem> m_currentResults;
};

} // namespace SwiftCast
