#include "DuelPage.hpp"

#include "entities/CodingTask.hpp"
#include "services/DuelManager.hpp"

#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QStyle>
#include <QVBoxLayout>

DuelPage::DuelPage(QWidget *parent) : QWidget(parent)
{
    m_duelManager = std::make_unique<cppforge::services::DuelManager>("Player", this);

    setupUI();
    applyStyles();

    connect(m_duelManager.get(), &cppforge::services::DuelManager::opponentConnected, this,
            &DuelPage::handleOpponentConnected);
    connect(m_duelManager.get(), &cppforge::services::DuelManager::taskReceived, this, &DuelPage::handleTaskReceived);
    connect(m_duelManager.get(), &cppforge::services::DuelManager::connectionError, this,
            &DuelPage::handleConnectionError);
}

DuelPage::~DuelPage() = default;

void DuelPage::setCircularAvatar(const QString &path)
{
    QPixmap pixmap(path);
    if (pixmap.isNull())
        return;

    QPixmap scaled = pixmap.scaled(55, 55, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QPixmap out(55, 55);
    out.fill(Qt::transparent);

    QPainter painter(&out);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(scaled));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 55, 55);
    painter.end();

    m_lblAvatar->setPixmap(out);
}

void DuelPage::clearLobbyList()
{
    QLayoutItem *child;
    while ((child = m_leaderListLayout->takeAt(0)) != nullptr)
    {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }
}

void DuelPage::setupUI()
{
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    mainLayout->setSpacing(30);

    auto leftColumn = new QVBoxLayout();
    leftColumn->setSpacing(25);

    auto header = createProfileHeader();
    auto actions = createActionPanel();

    auto applyShadow = [](QWidget *w)
    {
        auto s = new QGraphicsDropShadowEffect(w);
        s->setBlurRadius(15);
        s->setOffset(0, 4);
        s->setColor(QColor(0, 0, 0, 30));
        w->setGraphicsEffect(s);
    };

    applyShadow(header);
    applyShadow(actions);

    leftColumn->addWidget(header);
    leftColumn->addWidget(actions, 1);

    mainLayout->addLayout(leftColumn, 2);

    m_rightPanel = createLeaderboard();
    applyShadow(m_rightPanel);
    mainLayout->addWidget(m_rightPanel, 1);
}

QFrame *DuelPage::createProfileHeader()
{
    auto frame = new QFrame();
    frame->setObjectName("card");
    frame->setFixedHeight(110);
    auto layout = new QHBoxLayout(frame);
    layout->setContentsMargins(25, 0, 25, 0);

    m_lblAvatar = new QLabel();
    m_lblAvatar->setFixedSize(55, 55);
    m_lblAvatar->setStyleSheet("background-color: #2E8B57; border-radius: 27px;");

    m_lblUsername = new QLabel("Загрузка...");
    m_lblUsername->setFont(QFont("Roboto", 16, QFont::Bold));

    m_lblRating = new QLabel("0");
    m_lblRating->setFont(QFont("Roboto", 14, QFont::Bold));
    m_lblRating->setStyleSheet("color: #444;");

    layout->addWidget(m_lblAvatar);
    layout->addWidget(m_lblUsername);
    layout->addStretch();
    layout->addWidget(m_lblRating);

    return frame;
}

QFrame *DuelPage::createActionPanel()
{
    auto frame = new QFrame();
    frame->setObjectName("card");
    auto layout = new QVBoxLayout(frame);
    layout->setContentsMargins(50, 40, 50, 40);
    layout->setSpacing(20);

    auto title = new QLabel("ЛИГА");
    title->setAlignment(Qt::AlignCenter);
    title->setObjectName("sectionTitle");

    m_btnCreateLobby = new QPushButton("Создать лобби");
    m_btnCreateLobby->setObjectName("btnCreate");
    m_btnCreateLobby->setProperty("state", "default");
    m_btnCreateLobby->setFixedHeight(65);
    m_btnCreateLobby->setCursor(Qt::PointingHandCursor);

    m_btnJoinLobby = new QPushButton("Присоединиться");
    m_btnJoinLobby->setFixedHeight(65);
    m_btnJoinLobby->setCursor(Qt::PointingHandCursor);

    m_btnStartDuel = new QPushButton("НАЧАТЬ ДУЭЛЬ");
    m_btnStartDuel->setObjectName("btnStart");
    m_btnStartDuel->setFixedHeight(65);
    m_btnStartDuel->setCursor(Qt::PointingHandCursor);
    m_btnStartDuel->setVisible(false);

    layout->addStretch();
    layout->addWidget(title);
    layout->addSpacing(10);
    layout->addWidget(m_btnCreateLobby);
    layout->addWidget(m_btnJoinLobby);
    layout->addWidget(m_btnStartDuel);
    layout->addStretch();

    connect(m_btnCreateLobby, &QPushButton::clicked, this, &DuelPage::onCreateLobbyClicked);

    // 2. Кнопка присоединения к лобби
    connect(m_btnJoinLobby, &QPushButton::clicked, this, &DuelPage::onJoinLobbyClicked);

    // 3. Кнопка СТАРТ (исправленная лямбда с защитой от null и логами)
    connect(m_btnStartDuel, &QPushButton::clicked, this,
            [this]()
            {
                qDebug() << "[UI] Нажата кнопка запуска дуэли";

                if (m_duelManager)
                {
                    qDebug() << "[UI] Запрос случайной задачи через DuelManager...";
                    m_duelManager->startRandomDuel();
                }
                else
                {
                    qCritical() << "[UI] КРИТИЧЕСКАЯ ОШИБКА: m_duelManager не инициализирован!";
                    QMessageBox::warning(this, "Ошибка", "Связь с менеджером дуэлей потеряна. Пересоздайте лобби.");
                }
            });

    return frame;
}

QFrame *DuelPage::createLeaderboard()
{
    auto frame = new QFrame();
    frame->setObjectName("card");
    auto layout = new QVBoxLayout(frame);
    layout->setContentsMargins(15, 20, 15, 20);

    auto title = new QLabel("УЧАСТНИКИ ЛОББИ");
    title->setAlignment(Qt::AlignCenter);
    title->setObjectName("leaderboardTitle");
    layout->addWidget(title);

    auto scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameStyle(QFrame::NoFrame);
    auto container = new QWidget();
    m_leaderListLayout = new QVBoxLayout(container);
    m_leaderListLayout->setAlignment(Qt::AlignTop);
    m_leaderListLayout->setSpacing(10);
    scroll->setWidget(container);
    layout->addWidget(scroll);

    return frame;
}

void DuelPage::addLeaderboardEntry(const QString &name, bool isHost)
{
    auto item = new QFrame();
    item->setFixedHeight(55);
    item->setObjectName("lobbyItem");
    item->setStyleSheet(isHost ? "background: #F0FFF0; border: 1px solid #7FFF00; border-radius: 12px;"
                               : "background: #F8F9FA; border: 1px solid #EAECEF; border-radius: 12px;");

    auto layout = new QHBoxLayout(item);
    auto avatar = new QLabel();
    avatar->setFixedSize(32, 32);
    avatar->setStyleSheet(QString("background: %1; border-radius: 16px;").arg(isHost ? "#00FA9A" : "#2E8B57"));

    auto lblName = new QLabel(name + (isHost ? " (Вы)" : ""));
    lblName->setFont(QFont("Roboto", 10, isHost ? QFont::Bold : QFont::Normal));

    layout->addWidget(avatar);
    layout->addWidget(lblName);
    layout->addStretch();

    if (!isHost)
    {
        auto status = new QLabel("READY");
        status->setStyleSheet("color: #2E8B57; font-weight: bold; font-size: 9px;");
        layout->addWidget(status);
    }
    m_leaderListLayout->addWidget(item);
}

void DuelPage::onCreateLobbyClicked()
{
    if (m_isHosting)
    {
        m_duelManager = std::make_unique<cppforge::services::DuelManager>("Player", this);
        connect(m_duelManager.get(), &cppforge::services::DuelManager::opponentConnected, this,
                &DuelPage::handleOpponentConnected);
        connect(m_duelManager.get(), &cppforge::services::DuelManager::taskReceived, this,
                &DuelPage::handleTaskReceived);
        connect(m_duelManager.get(), &cppforge::services::DuelManager::connectionError, this,
                &DuelPage::handleConnectionError);

        m_isHosting = false;
        m_btnCreateLobby->setText("Создать лобби");
        m_btnCreateLobby->setProperty("state", "default");
        m_btnJoinLobby->setEnabled(true);
        m_btnStartDuel->setVisible(false);
        clearLobbyList();
    }
    else if (m_duelManager->hostRoom(4242))
    {
        m_isHosting = true;
        clearLobbyList();
        addLeaderboardEntry(m_lblUsername->text(), true);
        m_btnCreateLobby->setText("Отменить лобби");
        m_btnCreateLobby->setProperty("state", "active");
        m_btnJoinLobby->setEnabled(false);
    }

    m_btnCreateLobby->style()->unpolish(m_btnCreateLobby);
    m_btnCreateLobby->style()->polish(m_btnCreateLobby);
}

void DuelPage::onJoinLobbyClicked()
{
    bool ok;
    QString ip = QInputDialog::getText(this, "Поиск", "IP Хоста:", QLineEdit::Normal, "127.0.0.1", &ok);
    if (ok && !ip.isEmpty())
    {
        m_duelManager->joinRoom(ip, 4242);
        clearLobbyList();
        addLeaderboardEntry(m_lblUsername->text(), false);
    }
}

void DuelPage::handleOpponentConnected(const QString &ip)
{
    if (m_leaderListLayout->count() >= 2)
        return;

    addLeaderboardEntry("Соперник (" + ip + ")", false);

    if (m_isHosting)
    {
        m_btnStartDuel->setVisible(true);
    }

    QMessageBox::information(this, "Дуэль", "Противник подключился!");
}

void DuelPage::handleTaskReceived(const cppforge::entities::CodingTask &task)
{
    emit startDuelSession(task);
}

void DuelPage::handleConnectionError(const QString &error)
{
    QMessageBox::critical(this, "Ошибка", error);
    m_isHosting = false;
    m_btnCreateLobby->setEnabled(true);
    m_btnCreateLobby->setText("Создать лобби");
    m_btnCreateLobby->setProperty("state", "default");
    m_btnJoinLobby->setEnabled(true);
    m_btnStartDuel->setVisible(false);

    m_btnCreateLobby->style()->unpolish(m_btnCreateLobby);
    m_btnCreateLobby->style()->polish(m_btnCreateLobby);

    clearLobbyList();
}

void DuelPage::applyStyles()
{
    setStyleSheet(R"(
        DuelPage { background-color: #F8F9FA; }
        #card { background-color: white; border-radius: 20px; border: 1px solid #EDF0F2; }
        #sectionTitle { font-size: 22px; font-weight: 900; color: #444; letter-spacing: 2px; }
        #leaderboardTitle { font-size: 11px; font-weight: bold; color: #A0AEC0; }
        
        QPushButton {
            background-color: #FFFFFF;
            border: 2px solid #EAECEF;
            border-radius: 15px;
            color: #2D3436;
            font-weight: bold;
        }

        #btnCreate[state="default"]:hover {
            border-color: #2E8B57;
            color: #2E8B57;
            background-color: #F0FFF4;
        }

        #btnCreate[state="active"] {
            background-color: #FEF2F2;
            border-color: #E74C3C;
            color: #991B1B;
        }

        #btnCreate:pressed {
            background-color: #FDEDEC;
        }
        
        QPushButton:hover:!#btnCreate:!#btnStart {
            border-color: #3498DB;
        }
    )");
}

void DuelPage::updateUserStats(const QString &username, int rating, const QString &avatarPath)
{
    m_lblUsername->setText(username);
    m_lblRating->setText(QString::number(rating));
    if (!avatarPath.isEmpty())
        setCircularAvatar(avatarPath);
}
