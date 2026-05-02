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
    connect(m_duelManager.get(), &cppforge::services::DuelManager::opponentIdentified, this,
            &DuelPage::handleOpponentIdentified);
    connect(m_duelManager.get(), &cppforge::services::DuelManager::taskReceived, this, &DuelPage::handleTaskReceived);
    connect(m_duelManager.get(), &cppforge::services::DuelManager::connectionError, this,
            &DuelPage::handleConnectionError);
    connect(m_duelManager.get(), &cppforge::services::DuelManager::duelFinished, this, &DuelPage::handleDuelFinished);
}

DuelPage::~DuelPage() = default;

void DuelPage::setUserId(uint64_t id)
{
    m_currentUserId = id;
}

void DuelPage::setUserService(cppforge::services::UserService *service)
{
    m_userService = service;
}

void DuelPage::handleDuelFinished(const QString &winner, int score)
{
    if (!m_userService || m_currentUserId == 0)
    {
        return;
    }

    bool isMe = (winner == m_lblUsername->text());
    m_userService->recordDuelResult(m_currentUserId, isMe, score);

    auto userOpt = m_userService->findById(m_currentUserId);
    if (userOpt)
    {
        double winrate = 0.0;
        int total = userOpt->getDuelWins() + userOpt->getDuelLosses();
        if (total > 0)
        {
            winrate = (static_cast<double>(userOpt->getDuelWins()) / total) * 100.0;
        }
        updateUserStats(userOpt->getUsername(), userOpt->getDuelPoints(), winrate, userOpt->getAvatarPath());
    }

    resetLobby();
}

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
    m_lblAvatar->setStyleSheet("background-color: palette(button); border-radius: 27px;");

    m_lblUsername = new QLabel("Loading...");
    m_lblUsername->setFont(QFont("Roboto", 16, QFont::Bold));

    m_lblRating = new QLabel("PTS: 0");
    m_lblRating->setFont(QFont("Roboto", 14, QFont::Bold));
    m_lblRating->setStyleSheet("color: #3b82f6;");

    m_lblWinrate = new QLabel("WR: 0%");
    m_lblWinrate->setFont(QFont("Roboto", 14, QFont::Bold));
    m_lblWinrate->setStyleSheet("color: #10b981;");

    layout->addWidget(m_lblAvatar);
    layout->addWidget(m_lblUsername);
    layout->addStretch();
    layout->addWidget(m_lblRating);
    layout->addSpacing(20);
    layout->addWidget(m_lblWinrate);

    return frame;
}

QFrame *DuelPage::createActionPanel()
{
    auto frame = new QFrame();
    frame->setObjectName("card");
    auto layout = new QVBoxLayout(frame);
    layout->setContentsMargins(50, 40, 50, 40);
    layout->setSpacing(20);

    auto title = new QLabel("LEAGUE");
    title->setAlignment(Qt::AlignCenter);
    title->setObjectName("sectionTitle");

    m_btnCreateLobby = new QPushButton("CREATE LOBBY");
    m_btnCreateLobby->setObjectName("btnCreate");
    m_btnCreateLobby->setProperty("state", "default");
    m_btnCreateLobby->setFixedHeight(70);
    m_btnCreateLobby->setCursor(Qt::PointingHandCursor);

    m_btnJoinLobby = new QPushButton("JOIN LOBBY");
    m_btnJoinLobby->setFixedHeight(70);
    m_btnJoinLobby->setCursor(Qt::PointingHandCursor);

    m_btnStartDuel = new QPushButton("START DUEL");
    m_btnStartDuel->setObjectName("btnStart");
    m_btnStartDuel->setFixedHeight(70);
    m_btnStartDuel->setStyleSheet("background-color: #27ae60; color: white; border: none; font-size: 18px;");
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

    connect(m_btnJoinLobby, &QPushButton::clicked, this, &DuelPage::onJoinLobbyClicked);

    connect(m_btnStartDuel, &QPushButton::clicked, this,
            [this]()
            {
                qDebug() << "[UI] Duel start button clicked";

                if (m_duelManager)
                {
                    qDebug() << "[UI] Requesting random task via DuelManager...";
                    m_duelManager->startRandomDuel();
                }
                else
                {
                    qCritical() << "[UI] CRITICAL ERROR: m_duelManager not initialized!";
                    QMessageBox::warning(this, "Error", "Connection to duel manager lost. Please recreate the lobby.");
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

    auto title = new QLabel("LOBBY PARTICIPANTS");
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

void DuelPage::addLeaderboardEntry(const QString &name, bool isHost, const QString &avatarPath)
{
    auto item = new QFrame();
    item->setFixedHeight(65);
    item->setObjectName("lobbyItem");
    item->setStyleSheet(
        isHost ? "background-color: palette(base); border: 2px solid palette(mid); border-radius: 14px;"
               : "background-color: palette(alternate-base); border: 1px solid palette(mid); border-radius: 14px;");

    auto layout = new QHBoxLayout(item);
    auto avatar = new QLabel();
    avatar->setFixedSize(32, 32);

    if (!avatarPath.isEmpty())
    {
        QPixmap pix(avatarPath);
        if (!pix.isNull())
        {
            QPixmap scaled = pix.scaled(32, 32, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            QPixmap out(32, 32);
            out.fill(Qt::transparent);
            QPainter p(&out);
            p.setRenderHint(QPainter::Antialiasing);
            p.setBrush(QBrush(scaled));
            p.setPen(Qt::NoPen);
            p.drawEllipse(0, 0, 32, 32);
            p.end();
            avatar->setPixmap(out);
        }
        else
        {
            avatar->setStyleSheet("background-color: palette(button); border-radius: 16px;");
        }
    }
    else
    {
        avatar->setStyleSheet("background-color: palette(button); border-radius: 16px;");
    }

    auto lblName = new QLabel(name + (isHost ? " (You)" : ""));
    lblName->setFont(QFont("Roboto", 11, isHost ? QFont::Bold : QFont::Normal));

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

void DuelPage::handleOpponentIdentified(const QString &name, const QString &avatarPath)
{
    clearLobbyList();
    addLeaderboardEntry(m_lblUsername->text(), true, m_currentAvatarPath);
    addLeaderboardEntry(name, false, avatarPath);
}

void DuelPage::onCreateLobbyClicked()
{
    if (m_isHosting)
    {
        resetLobby();
    }
    else if (m_duelManager->hostRoom(4242))
    {
        m_isHosting = true;
        clearLobbyList();
        addLeaderboardEntry(m_lblUsername->text(), true, m_currentAvatarPath);
        m_btnCreateLobby->setText("CANCEL LOBBY");
        m_btnCreateLobby->setProperty("state", "active");
        m_btnJoinLobby->setEnabled(false);
    }

    m_btnCreateLobby->style()->unpolish(m_btnCreateLobby);
    m_btnCreateLobby->style()->polish(m_btnCreateLobby);
}

void DuelPage::resetLobby()
{
    m_duelManager->disconnectAll();
    m_isHosting = false;
    m_btnCreateLobby->setText("CREATE LOBBY");
    m_btnCreateLobby->setProperty("state", "default");
    m_btnJoinLobby->setEnabled(true);
    m_btnJoinLobby->setText("JOIN LOBBY");
    m_btnStartDuel->setVisible(false);
    clearLobbyList();
}

void DuelPage::onJoinLobbyClicked()
{
    if (m_btnJoinLobby->text() == "LEAVE LOBBY")
    {
        resetLobby();
        return;
    }

    bool ok;
    QString ip = QInputDialog::getText(this, "Connect", "Host IP Address:", QLineEdit::Normal, "127.0.0.1", &ok);
    if (ok && !ip.isEmpty())
    {
        m_duelManager->joinRoom(ip, 4242);
        clearLobbyList();
        addLeaderboardEntry(m_lblUsername->text(), false, m_currentAvatarPath);

        m_btnJoinLobby->setText("LEAVE LOBBY");
        m_btnCreateLobby->setEnabled(false);
    }
}

void DuelPage::handleOpponentConnected(const QString &ip)
{
    if (m_leaderListLayout->count() >= 2)
    {
        return;
    }

    addLeaderboardEntry("Opponent (" + ip + ")", false);

    if (m_isHosting)
    {
        m_btnStartDuel->setVisible(true);
    }

    QMessageBox::information(this, "Duel", "Opponent connected!");
}

void DuelPage::handleTaskReceived(const cppforge::entities::CodingTask &task)
{
    emit startDuelSession(task);
}

void DuelPage::handleConnectionError(const QString &error)
{
    QMessageBox::critical(this, "Error", error);
    resetLobby();
}

void DuelPage::applyStyles()
{
    setStyleSheet(R"(
        DuelPage { background-color: palette(alternate-base); }
        #card { 
            background-color: palette(base); 
            border-radius: 24px; 
            border: 1px solid;
            border-color: palette(mid); 
        }
        #sectionTitle { 
            font-size: 26px; 
            font-weight: 900; 
            color: palette(text); 
            letter-spacing: 1px; 
            margin-bottom: 10px;
        }
        #leaderboardTitle { 
            font-size: 14px; 
            font-weight: 800; 
            color: palette(window-text); 
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        
        QPushButton {
            background-color: palette(base);
            border: 2px solid;
            border-color: palette(mid);
            border-radius: 18px;
            color: palette(text);
            font-size: 15px;
            font-weight: 800;
        }

        QPushButton:hover {
            border-color: #3b82f6;
            background-color: palette(alternate-base);
            color: palette(text);
        }

        #btnCreate[state="default"]:hover {
            border-color: #10b981;
            color: #10b981;
            background-color: palette(alternate-base);
        }

        #btnCreate[state="active"] {
            background-color: #fef2f2;
            border-color: #ef4444;
            color: #dc2626;
        }

        #btnStart {
            background-color: #10b981;
            color: white;
            border: none;
        }
        
        #btnStart:hover {
            background-color: #059669;
            color: white;
        }

        #lobbyItem {
            margin: 2px 0;
        }
    )");
}

void DuelPage::updateUserStats(const QString &username, int rating, double winrate, const QString &avatarPath)
{
    m_lblUsername->setText(username);
    m_lblRating->setText(QString("PTS: %1").arg(rating));
    m_lblWinrate->setText(QString("WR: %1%").arg(winrate, 0, 'f', 1));

    m_currentAvatarPath = avatarPath;
    if (m_duelManager)
    {
        m_duelManager->sendIdentity(username, avatarPath);
    }

    if (!avatarPath.isEmpty())
    {
        setCircularAvatar(avatarPath);
    }
}
