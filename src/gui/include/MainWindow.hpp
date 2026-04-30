#pragma once

#include "DuelPage.hpp"
#include "DuelTaskWindow.hpp"
#include "ModuleRoadmapWidget.hpp"
#include "ProfilePage.hpp"
#include "TaskWindow.hpp"

#include <QEvent>
#include <QList>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QString>
#include <QWidget>
#include <QtSql/QSqlQuery>

#include <memory>
#include <vector>

namespace cppforge::services
{
    class UserService;
    class AchievementService;
} // namespace cppforge::services

class QFrame;
class QLabel;
class QPushButton;
class QProgressBar;
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
class CustomTitleBar;

/**
 * @class MainWindow
 * @brief Главное окно приложения, координирующее навигацию между основными разделами.
 *
 * MainWindow является центральным компонентом графического интерфейса (GUI).
 * Оно управляет переключением страниц (обучение, профиль, дуэли, дорожные карты модулей),
 * обеспечивает интеграцию с бизнес-логикой через сервисы (UserService, AchievementService)
 * и отвечает за визуальные эффекты переходов (анимации плавного появления/исчезновения).
 */
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор главного окна.
     * @param parent Родительский виджет.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Деструктор. Гарантирует корректное освобождение ресурсов.
     */
    ~MainWindow();

    /**
     * @brief Устанавливает имя текущего авторизованного пользователя для отображения в UI.
     * @param username Имя пользователя.
     */
    void setCurrentUser(const QString &username) { m_currentUsername = username; }

    /**
     * @brief Устанавливает идентификатор пользователя в системе.
     * @param id Числовой ID пользователя из базы данных.
     */
    void setUserId(int id);

    /**
     * @brief Внедряет зависимость сервиса пользователей.
     * @param service Указатель на UserService.
     */
    void setUserService(cppforge::services::UserService *service);

    /**
     * @brief Внедряет зависимость сервиса достижений.
     * @param service Указатель на AchievementService.
     */
    void setAchievementService(cppforge::services::AchievementService *service);

protected:
    /**
     * @brief Переопределенный обработчик отрисовки для реализации кастомных фонов или эффектов.
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief Обработчик события отображения окна. Используется для инициализации стартовых анимаций.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * @brief Обрабатывает изменение состояния окна (например, смену языка или системной темы).
     */
    void changeEvent(QEvent *event) override;

public slots:
    /**
     * @brief Запускает анимацию плавного появления окна (из прозрачности).
     */
    void fadeIn();

    /**
     * @brief Запускает анимацию плавного исчезновения окна.
     */
    void fadeOut();

    /**
     * @brief Открывает специализированное окно для выполнения конкретной задачи.
     * @param lessonId Идентификатор выбранного урока/задачи.
     */
    void openTaskWindow(int lessonId);

private slots:
    /** @brief Обработчик клика по карточке модуля. */
    void onModuleButtonClicked();

    /** @brief Переключает интерфейс на главную страницу обучения. */
    void onLearnButtonClicked();

    /** @brief Переключает интерфейс на страницу профиля пользователя. */
    void onProfileButtonClicked();

    /** @brief Вызывается при закрытии окна задачи для возврата к основному интерфейсу. */
    void onTaskWindowClosed();

    /** @brief Обрабатывает выход пользователя из учетной записи. */
    void onLogoutClicked();

    /**
     * @brief Обновляет визуальные индикаторы прогресса для конкретного модуля.
     * @param moduleId ID модуля.
     * @param progress Процент выполнения (0-100).
     */
    void updateModuleProgress(int moduleId, int progress);

    /** @brief Возвращает пользователя из детального просмотра модуля к списку модулей. */
    void onBackToModulesClicked();

    /**
     * @brief Обрабатывает событие разблокировки нового достижения.
     * @param achievement Объект полученного достижения.
     */
    void onAchievementUnlocked(cppforge::entities::Achievement achievement);

private:
    /** @brief Инициализирует структуру интерфейса и создает страницы. */
    void setupUI();

    /** @brief Настраивает флаги окна (рамки, прозрачность, кастомный заголовок). */
    void setupWindowProperties();

    /** @brief Настраивает кастомную панель заголовка окна. */
    void setupTitleBar();

    /** @brief Инициализирует левую навигационную панель. */
    void setupLeftPanel();

    /** @brief Инициализирует центральную область с контентом (QStackedWidget). */
    void setupCenterPanel();

    /** @brief Инициализирует правую информационную панель. */
    void setupRightPanel();

    /** @brief Применяет глобальные QSS стили ко всем элементам окна. */
    void setupStyles();

    /** @brief Центрирует окно приложения относительно экрана. */
    void centerWindow();

    /**
     * @brief Запускает анимационный переход к дорожной карте конкретного модуля.
     * @param moduleId ID целевого модуля.
     */
    void animateToTaskWindow(int moduleId);

    /** @brief Проверяет наличие данных пользователя в БД перед инициализацией страниц. */
    bool validateUserExists();

    /** @brief Загружает прогресс выполнения по всем доступным модулям из БД. */
    void loadAllModulesProgress();

    /**
     * @brief Формирует визуальную "дорожную карту" (roadmap) для выбранного модуля.
     * @param moduleId ID модуля для загрузки.
     */
    void loadRoadmapForModule(int moduleId);

    // Управление интерфейсом и анимацией
    std::unique_ptr<CustomTitleBar> customTitleBar_;          ///< Кастомная полоса заголовка.
    std::unique_ptr<QPropertyAnimation> transitionAnimation_; ///< Анимация переходов между страницами.
    std::unique_ptr<QVBoxLayout> centerPanelLayout_;          ///< Основной компоновщик центральной области.
    std::unique_ptr<TaskWindow> taskWindow_;                  ///< Отдельное окно для решения задач.

    std::unique_ptr<QStackedWidget> contentStack; ///< Стек страниц для навигации.

    // Страницы приложения
    ProfilePage *profilePage{nullptr};          ///< Страница профиля.
    QWidget *learningPage{nullptr};             ///< Главная страница обучения.
    QWidget *roadmapPage{nullptr};              ///< Страница детального просмотра модуля.
    DuelPage *duelPage{nullptr};                ///< Страница сетевых дуэлей.
    DuelTaskWindow *m_duelTaskWindow = nullptr; ///< Окно активной дуэли.

    // Кнопки навигации
    QPushButton *learnBtn{nullptr};   ///< Кнопка "Обучение".
    QPushButton *ratingBtn{nullptr};  ///< Кнопка "Дуэли/Рейтинг".
    QPushButton *profileBtn{nullptr}; ///< Кнопка "Профиль".
    QPushButton *logoutBtn{nullptr};  ///< Кнопка "Выход".

    ModuleRoadmapWidget *roadmapWidget{nullptr}; ///< Виджет отображения прогресса модуля.

    // Состояние
    QString m_currentUsername;     ///< Имя текущего пользователя.
    int m_currentUserId{-1};       ///< ID текущего пользователя.
    int m_currentOpenModuleId{-1}; ///< ID открытого в данный момент модуля.

    // Сервисы (не владеет указателями)
    cppforge::services::UserService *m_userService{nullptr};
    cppforge::services::AchievementService *m_achievementService{nullptr};

    // UI компоненты панелей
    std::unique_ptr<QFrame> sideBar;                ///< Боковая панель.
    std::unique_ptr<QFrame> eventCard;              ///< Виджет событий.
    std::unique_ptr<QFrame> dailyTaskCard;          ///< Виджет ежедневного задания.
    std::unique_ptr<QScrollArea> modulesScrollArea; ///< Область прокрутки списка модулей.
    std::unique_ptr<QWidget> modulesContainer;      ///< Контейнер для карточек модулей.
    std::unique_ptr<QVBoxLayout> modulesLayout;     ///< Компоновщик карточек модулей.

    std::vector<std::unique_ptr<QFrame>> moduleCards; ///< Список визуальных карточек модулей.
    QList<QLabel *> moduleProgressLabels;             ///< Метки процентов прогресса.
    QList<QProgressBar *> moduleProgressBars;         ///< Полосы прогресса.
    QList<QPushButton *> moduleButtons;               ///< Кнопки перехода к модулям.

    bool isTransitioning_{false}; ///< Флаг активной анимации перехода.
    int pendingModuleId_{-1};     ///< ID модуля, ожидающего открытия после анимации.
};