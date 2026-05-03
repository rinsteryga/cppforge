#pragma once

#include "services/UserService.hpp"

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
        class UserService;
        class ThemeService;
    } // namespace services
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
     * @brief Constructs a DuelPage.
     * @param parent Parent widget.
     */
    explicit DuelPage(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~DuelPage() override;

    /**
     * @brief Sets the unique identifier for the current user.
     * @param id The user's ID.
     */
    void setUserId(uint64_t id);

    /**
     * @brief Injects the user service for database interactions.
     * @param service Pointer to the UserService.
     */
    void setUserService(cppforge::services::UserService *service);

    /**
     * @brief Sets the theme service and connects to its signals.
     * @param service Pointer to the ThemeService.
     */
    void setThemeService(cppforge::services::ThemeService *service);

    /**
     * @brief Updates current user profile stats.
     * @param username User name.
     * @param rating Current rating (points).
     * @param winrate Current winrate percentage.
     * @param avatarPath Path to the avatar image file.
     */
    void updateUserStats(const QString &username, int rating, double winrate, const QString &avatarPath);

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
     * @brief Processes the end of a duel and records results to the database.
     * @param winner The name of the winner.
     * @param score Final score.
     */
    void handleDuelFinished(const QString &winner, int score);

    /**
     * @brief "Create Lobby" button click handler.
     */
    void onCreateLobbyClicked();

    /**
     * @brief "Join Lobby" button click handler.
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

    /**
     * @brief Handles receiving the opponent's nickname.
     * @param name Opponent's name.
     * @param avatarPath Opponent's avatar path.
     */
    void handleOpponentIdentified(const QString &name, const QString &avatarPath);

    /**
     * @brief Resets the lobby state and disconnects.
     */
    void resetLobby();

private:
    /**
     * @brief Initializes and arranges UI controls.
     */
    void setupUI();

    /**
     * @brief Applies QSS styles based on the current theme.
     */
    void applyStyles();

    /**
     * @brief Clears the participant list in the visual lobby list.
     */
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
     * @param avatarPath Player's avatar path.
     */
    void addLeaderboardEntry(const QString &name, bool isHost = false, const QString &avatarPath = "");

    /**
     * @brief Creates the top panel with the user profile.
     * @return Pointer to the frame containing the header.
     */
    QFrame *createProfileHeader();

    /**
     * @brief Creates the action control panel (Create/Join).
     * @return Pointer to the frame containing the panel.
     */
    QFrame *createActionPanel();

    /**
     * @brief Creates the container for the participant list.
     * @return Pointer to the frame containing the leaderboard.
     */
    QFrame *createLeaderboard();

    QLabel *m_lblAvatar{nullptr};                                   ///< Avatar display label.
    QLabel *m_lblUsername{nullptr};                                 ///< Username display label.
    QLabel *m_lblRating{nullptr};                                   ///< Rating display label.
    QLabel *m_lblWinrate{nullptr};                                  ///< Winrate display label.
    QPushButton *m_btnCreateLobby{nullptr};                         ///< Create lobby button.
    QPushButton *m_btnStartDuel = nullptr;                          ///< Start duel button.
    QPushButton *m_btnJoinLobby{nullptr};                           ///< Join lobby button.
    QPushButton *m_btnStartGame{nullptr};                           ///< Start game button (unused).
    QVBoxLayout *m_leaderListLayout{nullptr};                       ///< Layout for the lobby list.
    QFrame *m_rightPanel{nullptr};                                  ///< Right side panel.
    std::unique_ptr<cppforge::services::DuelManager> m_duelManager; ///< Network manager.
    cppforge::services::UserService *m_userService{nullptr};        ///< User database service.
    cppforge::services::ThemeService *m_themeService{nullptr};      ///< Theme service.
    bool m_isHosting{false};                                        ///< Hosting state flag.
    uint64_t m_currentUserId{0};                                    ///< Current user ID.
    QString m_currentAvatarPath;                                    ///< Current user avatar path.
};