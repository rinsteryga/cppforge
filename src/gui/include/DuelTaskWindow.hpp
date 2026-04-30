#pragma once

#include "../../core/include/entities/CodingTask.hpp"
#include "../../core/include/services/CodeRunner.hpp"
#include "../../core/include/services/StaticAnalyzer.hpp"
#include "CppHighlighter.hpp"
#include "CustomTitleBar.hpp"

#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QString>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>

namespace cppforge
{
    namespace services
    {
        class DuelManager;
    }
} // namespace cppforge

/**
 * @class DuelTaskWindow
 * @brief Окно активной сессии дуэли, объединяющее редактор кода, таймер и систему тестирования.
 *
 * Данный класс является основным интерфейсом игрока во время дуэли. Он отвечает за:
 * - Отображение условия задачи и написание кода.
 * - Индикацию оставшегося времени и текущего потенциального счета.
 * - Запуск тестов решения и статический анализ кода.
 * - Взаимодействие с DuelManager для синхронизации прогресса с оппонентом.
 */
class DuelTaskWindow : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор окна дуэли.
     * @param manager Указатель на существующий DuelManager для обмена данными по сети.
     * @param parent Родительский виджет.
     */
    explicit DuelTaskWindow(cppforge::services::DuelManager *manager, QWidget *parent = nullptr);

    /**
     * @brief Инициализирует окно данными конкретной задачи.
     * @param task Объект задачи (текст, начальный код, тесты).
     */
    void setTask(const cppforge::entities::CodingTask &task);

    /**
     * @brief Устанавливает имя локального игрока для отображения в интерфейсе.
     * @param name Никнейм пользователя.
     */
    void setLocalNickname(const QString &name);

    /**
     * @brief Отображает модальное окно или панель с итогами завершенной дуэли.
     * @param winnerName Имя победителя.
     * @param score Финальный счет победителя.
     */
    void showFinalResult(const QString &winnerName, int score);

protected:
    /**
     * @brief Переопределенное событие появления окна. Используется для запуска анимаций или таймера.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * @brief Фильтр событий для обработки специфических нажатий клавиш в редакторе (например, Tab).
     */
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    /**
     * @brief Вызывается каждую секунду. Обновляет таймер и динамически уменьшает текущий счет.
     */
    void onTick();

    /**
     * @brief Выполняет запуск текущего кода на наборе тестов задачи.
     */
    void onRunClicked();

    /**
     * @brief Финализирует решение. Проверяет все тесты и, при успехе, отправляет сигнал о завершении дуэли.
     */
    void onSubmitClicked();

private:
    /** @brief Инициализация графических компонентов и их компоновка. */
    void setupUI();

    /** @brief Настройка визуального стиля окна, шрифтов и цветовой схемы. */
    void setupStyles();

    QString m_localNickname; ///< Никнейм текущего пользователя.

    /** @brief Указатель на менеджер сетевого взаимодействия. Не владеет объектом. */
    cppforge::services::DuelManager *duelManager_{nullptr};

    std::unique_ptr<CustomTitleBar> customTitleBar_; ///< Кастомная панель заголовка окна.
    QTextEdit *codeEditor_{nullptr};                 ///< Поле ввода исходного кода.
    QTextEdit *testOutput_{nullptr};                 ///< Панель отображения результатов тестов.
    QTextEdit *practiceEdit_{nullptr};               ///< Окно отображения описания задачи.

    QLabel *labelTimer_{nullptr}; ///< Виджет отображения оставшегося времени.
    QLabel *labelScore_{nullptr}; ///< Виджет отображения динамического счета.
    QTimer *duelTimer_{nullptr};  ///< Таймер обратного отсчета.

    int timeLeft_{600};      ///< Оставшееся время в секундах (по умолчанию 10 мин).
    int currentScore_{1000}; ///< Текущий счет, уменьшающийся со временем.

    cppforge::entities::CodingTask currentTask_;                   ///< Данные текущей задачи.
    std::unique_ptr<cppforge::services::CodeRunner> runner_;       ///< Сервис для компиляции и запуска кода.
    std::unique_ptr<cppforge::services::StaticAnalyzer> analyzer_; ///< Сервис для проверки качества кода.
};