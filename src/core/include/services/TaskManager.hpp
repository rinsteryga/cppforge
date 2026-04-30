#pragma once

#include "../entities/CodingTask.hpp"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

/**
 * @brief Представляет раздел теоретического материала внутри модуля.
 */
struct TheorySection
{
    QString id;
    QString title;
    QString content;
    QString format;

    /**
     * @brief Проверяет, заполнены ли основные поля раздела.
     * @return true, если id и заголовок не пусты.
     */
    bool isValid() const { return !id.isEmpty() && !title.isEmpty(); }
};

/**
 * @brief Хранит данные отдельного тестового случая для проверки кода.
 */
struct TestCase
{
    QString input;
    QString expectedOutput;

    /**
     * @brief Проверяет валидность тестового случая.
     * @return true, если поля ввода и ожидаемого вывода заполнены.
     */
    bool isValid() const { return !input.isEmpty() && !expectedOutput.isEmpty(); }
};

/**
 * @brief Описывает практическую задачу по программированию.
 */
struct Task
{
    int id = 0;
    QString title;
    QString description;
    QString difficulty;
    QString initialCode;
    QVector<TestCase> testCases;

    /**
     * @brief Проверяет валидность данных задачи.
     * @return true, если ID задан и название не пустое.
     */
    bool isValid() const { return id > 0 && !title.isEmpty(); }
};

/**
 * @brief Представляет учебный модуль, содержащий теорию и практические задания.
 */
struct Module
{
    int id = 0;
    QString title;
    QString authorText;
    QVector<TheorySection> theory;
    QVector<Task> tasks;

    /**
     * @brief Проверяет валидность данных модуля.
     * @return true, если ID корректен и название заполнено.
     */
    bool isValid() const { return id >= 0 && !title.isEmpty(); }
};

/**
 * @class TaskManager
 * @brief Менеджер для управления учебным контентом: модулями, теорией и задачами.
 *
 * Класс реализован как Singleton и отвечает за загрузку данных из внешних источников (JSON)
 * и навигацию по ним.
 */
class TaskManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Возвращает единственный экземпляр TaskManager.
     * @return Ссылка на экземпляр менеджера.
     */
    static TaskManager &instance();

    /**
     * @brief Деструктор по умолчанию.
     */
    ~TaskManager() = default;

    /**
     * @brief Загружает структуру задач и модулей из JSON-файла.
     * @param filePath Путь к файлу конфигурации.
     * @return true, если загрузка и парсинг прошли успешно.
     */
    bool loadTasks(const QString &filePath);

    /**
     * @brief Очищает все загруженные модули и сбрасывает состояние менеджера.
     */
    void reset();

    /**
     * @brief Находит и возвращает модуль по его ID.
     * @param moduleId Идентификатор искомого модуля.
     * @return Объект модуля (валидность проверяется через Module::isValid()).
     */
    Module getModule(int moduleId) const;

    /**
     * @brief Возвращает конкретную задачу.
     * @param moduleId ID модуля, которому принадлежит задача.
     * @param taskId ID искомой задачи.
     * @return Объект задачи.
     */
    Task getTask(int moduleId, int taskId) const;

    /**
     * @brief Возвращает список всех задач указанного модуля.
     * @param moduleId ID модуля.
     */
    QVector<Task> getTasksForModule(int moduleId) const;

    /**
     * @brief Возвращает список всех разделов теории указанного модуля.
     * @param moduleId ID модуля.
     */
    QVector<TheorySection> getTheoryForModule(int moduleId) const;

    /**
     * @brief Выбирает случайную задачу из базы данных для режима дуэли.
     * @return Сущность CodingTask, готовую для передачи в DuelManager.
     */
    cppforge::entities::CodingTask getRandomDuelTaskFromDb();

    /** @brief Возвращает ID текущего активного модуля. */
    int getCurrentModule() const { return currentModule_; }

    /** @brief Возвращает ID текущей активной задачи. */
    int getCurrentTask() const { return currentTask_; }

    /**
     * @brief Устанавливает текущую активную задачу и модуль.
     * @param moduleId ID нового активного модуля.
     * @param taskId ID новой активной задачи.
     */
    void setCurrentTask(int moduleId, int taskId);

    /**
     * @brief Проверяет, существует ли модуль с данным ID.
     */
    bool isModuleAvailable(int moduleId) const;

    /**
     * @brief Проверяет, существует ли задача в конкретном модуле.
     */
    bool isTaskAvailable(int moduleId, int taskId) const;

    /**
     * @brief Возвращает список названий всех загруженных модулей.
     */
    QStringList getModuleTitles() const;

    /**
     * @brief Возвращает общее количество загруженных модулей.
     */
    int getModuleCount() const;

    /**
     * @brief Возвращает количество задач в конкретном модуле.
     */
    int getTaskCount(int moduleId) const;

    /**
     * @brief Возвращает количество разделов теории в конкретном модуле.
     */
    int getTheoryCount(int moduleId) const;

    /**
     * @brief Выводит отладочную информацию о модуле в консоль.
     */
    void printModuleInfo(int moduleId) const;

    /**
     * @brief Выводит структуру всех загруженных модулей в консоль.
     */
    void printAllModules() const;

signals:
    /**
     * @brief Генерируется при изменении активной задачи.
     * @param moduleId Новый ID модуля.
     * @param taskId Новый ID задачи.
     */
    void taskChanged(int moduleId, int taskId);

    /**
     * @brief Генерируется после успешного завершения метода loadTasks().
     */
    void tasksLoaded();

    /**
     * @brief Генерируется при обновлении прогресса прохождения модуля.
     * @param moduleId ID модуля.
     * @param progress Процент выполнения (0-100).
     */
    void moduleProgressUpdated(int moduleId, int progress);

private:
    /** @brief Приватный конструктор (Singleton). */
    TaskManager() = default;
    TaskManager(const TaskManager &) = delete;
    TaskManager &operator=(const TaskManager &) = delete;

    QVector<Module> modules_;
    int currentModule_ = -1;
    int currentTask_ = -1;

    /** @brief Разбирает JSON-объект в структуру TheorySection. */
    TheorySection parseTheorySection(const QJsonObject &obj);

    /** @brief Разбирает JSON-объект в структуру Task. */
    Task parseTask(const QJsonObject &obj);

    /** @brief Разбирает JSON-объект в структуру Module. */
    Module parseModule(const QJsonObject &obj);
};