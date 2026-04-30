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

/**
 * @class DuelPage
 * @brief GUI class for organizing and searching for network duels.
 *
 * DuelPage provides an interface for creating a lobby (hosting),
 * connecting to existing rooms, and displaying duel participants.
 * The class interacts with DuelManager to handle network events.
 */
class DuelPage : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief DuelPage constructor.
     * @param parent Parent widget.
     */
    explicit DuelPage(QWidget *parent = nullptr);

    /**
     * @brief DuelPage destructor.
     */
    ~DuelPage();

    /**
     * @brief Updates current user profile stats.
     * @param username User name.
     * @param rating Current rating.
     * @param avatarPath Path to the avatar image file.
     */
    void updateUserStats(const QString &username, int rating, const QString &avatarPath);

    /**
     * @brief Returns a pointer to the network interaction manager.
     * @return Pointer to DuelManager object.
     */
    cppforge::services::DuelManager *getDuelManager() const { return m_duelManager.get(); }

signals:
    /**
     * @brief Signals the need to switch to task solving mode (duel start).
     * @param task Task object received from the host or selected by self.
     */
    void startDuelSession(const cppforge::entities::CodingTask &task);

private slots:
    /**
     * @brief "Create Lobby" button click handler. Starts the TCP server.
     */
    void onCreateLobbyClicked();

    /**
     * @brief "Join Lobby" button click handler. Invokes IP input dialog.
     */
    void onJoinLobbyClicked();

    /**
     * @brief Handles successful opponent connection event.
     * @param ip Connected user's IP address.
     */
    void handleOpponentConnected(const QString &ip);

    /**
     * @brief Handles receiving task data over the network.
     * @param task Received task.
     */
    void handleTaskReceived(const cppforge::entities::CodingTask &task);

    /**
     * @brief Displays an error message upon network connection failure.
     * @param error Error text.
     */
    void handleConnectionError(const QString &error);

private:
    /** @brief Initializes and arranges UI controls. */
    void setupUI();

    /** @brief Applies QSS styles to page elements. */
    void applyStyles();

    /** @brief Clears the participant list in the visual lobby list. */
    void clearLobbyList();

    /**
     * @brief Sets the avatar image applying a circular mask.
     * @param path Path to the image.
     */
    void setCircularAvatar(const QString &path);

    /**
     * @brief Adds a player info entry to the participant list.
     * @param name Player name.
     * @param isHost Flag indicating if the player is the room creator.
     */
    void addLeaderboardEntry(const QString &name, bool isHost = false);

    /** @brief Creates the top panel with the user profile. */
    QFrame *createProfileHeader();

    /** @brief Creates the action control panel (Create/Join). */
    QFrame *createActionPanel();

    /** @brief Creates the container for the participant list (Leaderboard/Lobby). */
    QFrame *createLeaderboard();

    QLabel *m_lblAvatar{nullptr};
    QLabel *m_lblUsername{nullptr};
    QLabel *m_lblRating{nullptr};

    QPushButton *m_btnCreateLobby{nullptr};
    QPushButton *m_btnStartDuel = nullptr;
    QPushButton *m_btnJoinLobby{nullptr};
    QPushButton *m_btnStartGame{nullptr};

    QVBoxLayout *m_leaderListLayout{nullptr};
    QFrame *m_rightPanel{nullptr};

    std::unique_ptr<cppforge::services::DuelManager> m_duelManager;
    bool m_isHosting{false};
    uint64_t m_currentUserId{0};
};