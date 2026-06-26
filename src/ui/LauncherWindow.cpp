#include "LauncherWindow.h"
#include "../extensions/ExtensionLoader.h"

#include <QApplication>
#include <QClipboard>
#include <QProcess>
#include <QDesktopServices>
#include <QRegularExpression>
#include <QScreen>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QCloseEvent>
#include <QFile>
#include <QStyle>
#include <QStyleOption>
#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>

#ifdef Q_OS_WIN
#  include <windows.h>
#  include <dwmapi.h>
#endif

namespace SwiftCast {

// ── SearchBar ────────────────────────────────────────────────────────────────

SearchBar::SearchBar(QWidget* parent)
    : QLineEdit(parent)
{
    setObjectName("SearchInput");
    setPlaceholderText("Search apps, commands, extensions…");
    setClearButtonEnabled(false);
    setFrame(false);
}

void SearchBar::paintEvent(QPaintEvent* e)
{
    QLineEdit::paintEvent(e);
}

void SearchBar::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape) { emit escapePressed(); return; }
    if (e->key() == Qt::Key_Down)   { emit arrowDown();     return; }
    if (e->key() == Qt::Key_Up)     { emit arrowUp();       return; }
    QLineEdit::keyPressEvent(e);
}

// ── ResultDelegate ───────────────────────────────────────────────────────────

ResultDelegate::ResultDelegate(QObject* parent)
    : QAbstractItemDelegate(parent)
{}

void ResultDelegate::paint(QPainter* painter,
                           const QStyleOptionViewItem& option,
                           const QModelIndex& index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    const bool selected = option.state & QStyle::State_Selected;
    const bool hovered  = option.state & QStyle::State_MouseOver;

    // Background
    if (selected) {
        painter->setBrush(QColor("#3B1E8A"));
    } else if (hovered) {
        painter->setBrush(QColor("#1E1B2E"));
    } else {
        painter->setBrush(Qt::transparent);
    }
    painter->setPen(Qt::NoPen);

    QRectF r = option.rect.adjusted(4, 1, -4, -1);
    painter->drawRoundedRect(r, 8, 8);

    // Icon placeholder area
    QRectF iconRect(r.left() + 10, r.top() + 9, 22, 22);
    painter->setBrush(selected ? QColor("#5B3CB0") : QColor("#2D2640"));
    painter->drawRoundedRect(iconRect, 5, 5);

    // Title
    QString title = index.data(Qt::UserRole).toString();
    QString sub   = index.data(Qt::UserRole + 1).toString();
    QString cat   = index.data(Qt::UserRole + 2).toString();

    QFont titleFont = option.font;
    titleFont.setPointSizeF(10.5);
    titleFont.setWeight(QFont::Medium);
    painter->setFont(titleFont);
    painter->setPen(selected ? QColor("#F0EAFF") : QColor("#D4C8F0"));

    QRectF titleRect(r.left() + 42, r.top() + 7, r.width() - 52 - 60, 18);
    painter->drawText(titleRect, Qt::AlignVCenter | Qt::AlignLeft,
                      option.fontMetrics.elidedText(title, Qt::ElideRight,
                                                    static_cast<int>(titleRect.width())));

    // Subtitle
    if (!sub.isEmpty()) {
        QFont subFont = option.font;
        subFont.setPointSizeF(9.0);
        painter->setFont(subFont);
        painter->setPen(selected ? QColor("#A89AC8") : QColor("#6D5FA0"));

        QRectF subRect(r.left() + 42, r.top() + 23, r.width() - 52 - 60, 14);
        painter->drawText(subRect, Qt::AlignVCenter | Qt::AlignLeft,
                          option.fontMetrics.elidedText(sub, Qt::ElideRight,
                                                        static_cast<int>(subRect.width())));
    }

    // Category badge (right side)
    if (!cat.isEmpty()) {
        QFont catFont = option.font;
        catFont.setPointSizeF(8.5);
        painter->setFont(catFont);
        painter->setPen(selected ? QColor("#C084FC") : QColor("#4A3F6B"));

        QFontMetrics fm(catFont);
        int tw = fm.horizontalAdvance(cat) + 16;
        QRectF catRect(r.right() - tw - 6, r.top() + 10, tw, 16);
        painter->setBrush(selected ? QColor("#2D0D6E") : QColor("#1A1628"));
        painter->setPen(selected ? QColor("#5B3CB0") : QColor("#2D2640"));
        painter->drawRoundedRect(catRect, 4, 4);
        painter->setPen(selected ? QColor("#C084FC") : QColor("#4A3F6B"));
        painter->drawText(catRect, Qt::AlignCenter, cat);
    }

    painter->restore();
}

QSize ResultDelegate::sizeHint(const QStyleOptionViewItem&,
                                const QModelIndex&) const
{
    return {400, 42};
}

// ── LauncherWindow ───────────────────────────────────────────────────────────

LauncherWindow::LauncherWindow(ExtensionLoader* extensions, QWidget* parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool)
    , m_extensions(extensions)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating, false);
    setObjectName("LauncherWindow");

    buildUi();
    applyWindowEffect();

    // Load stylesheet
    QFile qss(":/styles/dark.qss");
    if (qss.open(QIODevice::ReadOnly))
        setStyleSheet(qss.readAll());
}

void LauncherWindow::buildUi()
{
    setFixedWidth(660);
    setMinimumHeight(60);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Search row ──
    auto* searchRow = new QHBoxLayout();
    searchRow->setContentsMargins(16, 0, 16, 0);
    searchRow->setSpacing(0);

    m_searchIcon = new QLabel("⌕", this);
    m_searchIcon->setFixedSize(36, 52);
    m_searchIcon->setAlignment(Qt::AlignCenter);
    m_searchIcon->setStyleSheet("color: #6D5FA0; font-size: 20px;");

    m_searchBar = new SearchBar(this);
    m_searchBar->setFixedHeight(52);

    searchRow->addWidget(m_searchIcon);
    searchRow->addWidget(m_searchBar);
    root->addLayout(searchRow);

    // ── Results list ──
    m_resultList = new QListWidget(this);
    m_resultList->setObjectName("ResultsList");
    m_resultList->setItemDelegate(new ResultDelegate(m_resultList));
    m_resultList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_resultList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_resultList->setMouseTracking(true);
    m_resultList->hide();
    root->addWidget(m_resultList);

    // ── Status bar ──
    m_statusBar = new QLabel("  Ctrl+Enter to open · F12 for settings", this);
    m_statusBar->setObjectName("StatusBar");
    m_statusBar->setFixedHeight(26);
    m_statusBar->hide();
    root->addWidget(m_statusBar);

    // Connections
    connect(m_searchBar, &QLineEdit::textChanged,
            this, &LauncherWindow::onSearchChanged);
    connect(m_searchBar, &SearchBar::escapePressed,
            this, &LauncherWindow::hide);
    connect(m_searchBar, &SearchBar::arrowDown, this, [this]{
        int next = m_resultList->currentRow() + 1;
        if (next < m_resultList->count())
            m_resultList->setCurrentRow(next);
    });
    connect(m_searchBar, &SearchBar::arrowUp, this, [this]{
        int prev = m_resultList->currentRow() - 1;
        if (prev >= 0)
            m_resultList->setCurrentRow(prev);
    });
    connect(m_resultList, &QListWidget::itemActivated,
            this, &LauncherWindow::onItemActivated);

    m_resultList->installEventFilter(this);
}

void LauncherWindow::applyWindowEffect()
{
#ifdef Q_OS_WIN
    // Windows 11: rounded corners + dark titlebar
    HWND hwnd = reinterpret_cast<HWND>(winId());

    DWM_WINDOW_CORNER_PREFERENCE cornerPref = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE,
                          &cornerPref, sizeof(cornerPref));

    BOOL darkMode = TRUE;
    DwmSetWindowAttribute(hwnd, 20 /*DWMWA_USE_IMMERSIVE_DARK_MODE*/,
                          &darkMode, sizeof(darkMode));
#endif
}

void LauncherWindow::centerOnScreen()
{
    QScreen* screen = QApplication::primaryScreen();
    if (!screen) return;

    QRect sg = screen->availableGeometry();
    int x = sg.left() + (sg.width()  - width())  / 2;
    int y = sg.top()  + (sg.height() / 3)         - height() / 2;
    move(x, y);
}

void LauncherWindow::popup()
{
    m_searchBar->clear();
    m_resultList->clear();
    m_resultList->hide();
    m_statusBar->hide();
    adjustSize();
    centerOnScreen();
    show();
    raise();
    activateWindow();
    m_searchBar->setFocus();
}

void LauncherWindow::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);
    applyWindowEffect();
}

void LauncherWindow::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Drop shadow (drawn outside widget bounds via transparent bg)
    QPainterPath path;
    path.addRoundedRect(rect().adjusted(1,1,-1,-1), 14, 14);

    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#12101A"));
    p.drawPath(path);

    // Border
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(QColor("#2D2640"), 1));
    p.drawRoundedRect(rect().adjusted(0,0,-1,-1), 14, 14);
}

void LauncherWindow::focusOutEvent(QFocusEvent* e)
{
    QWidget::focusOutEvent(e);
}

void LauncherWindow::closeEvent(QCloseEvent* e)
{
    e->ignore();
    hide();
}

bool LauncherWindow::eventFilter(QObject* obj, QEvent* e)
{
    if (obj == m_resultList && e->type() == QEvent::KeyPress) {
        auto* ke = static_cast<QKeyEvent*>(e);
        if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) {
            activateSelected();
            return true;
        }
        if (ke->key() == Qt::Key_Escape) {
            hide();
            return true;
        }
        if (ke->key() == Qt::Key_F12) {
            hide();
            emit settingsRequested();
            return true;
        }
    }
    return QWidget::eventFilter(obj, e);
}

void LauncherWindow::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape) { hide(); return; }
    if (e->key() == Qt::Key_F12)    { hide(); emit settingsRequested(); return; }
    if (e->key() == Qt::Key_Return) { activateSelected(); return; }
    QWidget::keyPressEvent(e);
}

void LauncherWindow::onSearchChanged(const QString& text)
{
    if (text.trimmed().isEmpty()) {
        m_resultList->clear();
        m_resultList->hide();
        m_statusBar->hide();
        adjustSize();
        return;
    }
    populateResults(text);
}

void LauncherWindow::populateResults(const QString& query)
{
    QList<ResultItem> results;

    // Ask each extension for results
    if (m_extensions) {
        results = m_extensions->query(query);
    }

    // Built-in: simple app search fallback
    if (results.isEmpty()) {
        ResultItem item;
        item.title      = query;
        item.subtitle   = "Search the web for \"" + query + "\"";
        item.category   = "Web";
        item.actionData = "web:" + query;
        results.append(item);
    }

    setResults(results);
}

void LauncherWindow::setResults(const QList<ResultItem>& items)
{
    m_currentResults = items;
    m_resultList->clear();

    const int maxVisible = qMin(items.size(), 8);

    for (const auto& r : items) {
        auto* item = new QListWidgetItem(m_resultList);
        item->setData(Qt::UserRole,     r.title);
        item->setData(Qt::UserRole + 1, r.subtitle);
        item->setData(Qt::UserRole + 2, r.category);
        item->setData(Qt::UserRole + 3, r.actionData);
        item->setSizeHint({660, 42});
    }

    if (!items.isEmpty()) {
        m_resultList->setCurrentRow(0);
        m_resultList->setFixedHeight(maxVisible * 42 + 12);
        m_resultList->show();
        m_statusBar->setText(
            QString("  %1 result%2  ·  ↑↓ navigate  ·  Enter to open  ·  F12 settings")
            .arg(items.size()).arg(items.size() == 1 ? "" : "s"));
        m_statusBar->show();
    } else {
        m_resultList->hide();
        m_statusBar->hide();
    }

    adjustSize();
    centerOnScreen();
}

void LauncherWindow::activateSelected()
{
    QListWidgetItem* item = m_resultList->currentItem();
    if (!item) return;
    onItemActivated(item);
}

void LauncherWindow::onItemActivated(QListWidgetItem* item)
{
    if (!item) return;
    int row = m_resultList->row(item);
    if (row < 0 || row >= m_currentResults.size()) return;

    executeItem(m_currentResults[row]);
    hide();
}

void LauncherWindow::executeItem(const ResultItem& item)
{
    if (item.actionData.startsWith("web:")) {
        QString q = item.actionData.mid(4);
        QDesktopServices::openUrl(
            QUrl("https://www.google.com/search?q=" +
                 QUrl::toPercentEncoding(q)));
        return;
    }

    if (item.actionData.startsWith("exec:")) {
        QString path = item.actionData.mid(5);
        QProcess::startDetached(path, {});
        return;
    }

    if (item.actionData.startsWith("url:")) {
        QDesktopServices::openUrl(QUrl(item.actionData.mid(4)));
        return;
    }

    if (m_extensions)
        m_extensions->execute(item.actionData);
}

} // namespace SwiftCast
