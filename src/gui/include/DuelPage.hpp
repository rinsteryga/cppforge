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
 * @brief Класс графического интерфейса для организации и поиска сетевых дуэлей.
 *
 * DuelPage предоставляет пользователю интерфейс для создания лобби (хостинга),
 * подключения к существующим комнатам и отображения участников дуэли.
 * Класс взаимодействует с DuelManager для обработки сетевых событий.
 */
class DuelPage : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор страницы дуэлей.
     * @param parent Родительский виджет.
     */
    explicit DuelPage(QWidget *parent = nullptr);

    /**
     * @brief Деструктор DuelPage.
     */
    ~DuelPage();

    /**
     * @brief Обновляет отображение данных профиля текущего пользователя.
     * @param username Имя пользователя.
     * @param rating Текущий рейтинг.
     * @param avatarPath Путь к файлу изображения аватара.
     */
    void updateUserStats(const QString &username, int rating, const QString &avatarPath);

    /**
     * @brief Возвращает указатель на менеджер сетевого взаимодействия.
     * @return Указатель на объект DuelManager.
     */
    cppforge::services::DuelManager *getDuelManager() const { return m_duelManager.get(); }

signals:
    /**
     * @brief Сигнализирует о необходимости перехода в режим решения задачи (старт дуэли).
     * @param task Объект задачи, полученный от хоста или выбранный самостоятельно.
     */
    void startDuelSession(const cppforge::entities::CodingTask &task);

private slots:
    /**
     * @brief Обработчик нажатия кнопки "Создать лобби". Запускает TCP-сервер.
     */
    void onCreateLobbyClicked();

    /**
     * @brief Обработчик нажатия кнопки "Присоединиться". Вызывает диалог ввода IP.
     */
    void onJoinLobbyClicked();

    /**
     * @brief Обрабатывает событие успешного подключения оппонента.
     * @param ip IP-адрес подключившегося пользователя.
     */
    void handleOpponentConnected(const QString &ip);

    /**
     * @brief Обрабатывает получение данных задачи по сети.
     * @param task Полученная задача.
     */
    void handleTaskReceived(const cppforge::entities::CodingTask &task);

    /**
     * @brief Отображает сообщение об ошибке при сбое сетевого соединения.
     * @param error Текст ошибки.
     */
    void handleConnectionError(const QString &error);

private:
    /** @brief Инициализирует и размещает элементы управления UI. */
    void setupUI();

    /** @brief Применяет QSS стили к элементам страницы. */
    void applyStyles();

    /** @brief Очищает список участников в визуальном списке лобби. */
    void clearLobbyList();

    /**
     * @brief Устанавливает изображение аватара с применением маски круга.
     * @param path Путь к изображению.
     */
    void setCircularAvatar(const QString &path);

    /**
     * @brief Добавляет строку с информацией об игроке в список участников.
     * @param name Имя игрока.
     * @param isHost Флаг, указывающий, является ли игрок создателем комнаты.
     */
    void addLeaderboardEntry(const QString &name, bool isHost = false);

    /** @brief Создает верхнюю панель с профилем пользователя. */
    QFrame *createProfileHeader();

    /** @brief Создает панель кнопок управления (Создать/Присоединиться). */
    QFrame *createActionPanel();

    /** @brief Создает контейнер для списка участников (Leaderboard/Lobby). */
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