#pragma once

#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>

class QLabel;
class QPushButton;
class QFrame;

namespace cppforge
{
    namespace services
    {
        class DuelManager;
    }
    namespace entities
    {
        struct CodingTask;
    }
} // namespace cppforge

class DuelPage : public QWidget
{
    Q_OBJECT

public:
    explicit DuelPage(QWidget *parent = nullptr);
    ~DuelPage();

    void updateUserStats(const QString &username, int rating, const QString &avatarPath);

signals:
    void startDuelSession(const cppforge::entities::CodingTask &task);

private slots:
    void onCreateLobbyClicked();
    void onJoinLobbyClicked();
    void handleOpponentConnected(const QString &ip);
    void handleTaskReceived(const cppforge::entities::CodingTask &task);
    void handleConnectionError(const QString &error);

private:
    void setupUI();
    void applyStyles();
    void clearLobbyList();
    void setCircularAvatar(const QString &path);
    void addLeaderboardEntry(const QString &name, bool isHost = false);

    QFrame *createProfileHeader();
    QFrame *createActionPanel();
    QFrame *createLeaderboard();

    QLabel *m_lblAvatar{nullptr};
    QLabel *m_lblUsername{nullptr};
    QLabel *m_lblRating{nullptr};
    QPushButton *m_btnCreateLobby{nullptr};
    QPushButton *m_btnJoinLobby{nullptr};

    QVBoxLayout *m_leaderListLayout{nullptr};
    QFrame *m_rightPanel{nullptr};

    std::unique_ptr<cppforge::services::DuelManager> m_duelManager;
    bool m_isHosting{false};
};