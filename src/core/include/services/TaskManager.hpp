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

namespace cppforge::repositories
{
    class ICodingTaskRepository;
}

/**
 * @brief Represents a theoretical material section within a module.
 */
struct TheorySection
{
    QString id;
    QString title;
    QString content;
    QString format;

    /**
     * @brief Checks if the essential fields of the section are filled.
     * @return true if id and title are not empty.
     */
    bool isValid() const { return !id.isEmpty() && !title.isEmpty(); }
};

/**
 * @brief Stores data for an individual test case used for code verification.
 */
struct TestCase
{
    QString input;
    QString expectedOutput;

    /**
     * @brief Checks test case validity.
     * @return true if input and expected output fields are filled.
     */
    bool isValid() const { return !input.isEmpty() && !expectedOutput.isEmpty(); }
};

/**
 * @brief Describes a practical programming task.
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
     * @brief Checks task data validity.
     * @return true if ID is set and title is not empty.
     */
    bool isValid() const { return id > 0 && !title.isEmpty(); }
};

/**
 * @brief Represents a learning module containing theory and practical tasks.
 */
struct Module
{
    int id = 0;
    QString title;
    QString authorText;
    QVector<TheorySection> theory;
    QVector<Task> tasks;

    /**
     * @brief Checks module data validity.
     * @return true if ID is correct and title is filled.
     */
    bool isValid() const { return id >= 0 && !title.isEmpty(); }
};

namespace cppforge::services
{
    /**
     * @class TaskManager
     * @brief Manager for handling learning content: modules, theory, and tasks.
     *
     * The class is implemented as a Singleton and is responsible for loading data
     * from external sources (JSON) and providing navigation.
     */
    class TaskManager : public QObject
    {
        Q_OBJECT

    public:
        /**
         * @brief Returns the single instance of TaskManager.
         * @return Reference to the manager instance.
         */
        static TaskManager &instance();

        /**
         * @brief Default destructor.
         */
        ~TaskManager() = default;

        /**
         * @brief Loads task and module structure from a JSON file.
         * @param filePath Path to the configuration file.
         * @return true if loading and parsing succeeded.
         */
        bool loadTasks(const QString &filePath);

        /**
         * @brief Clears all loaded modules and resets the manager's state.
         */
        void reset();

        /**
         * @brief Finds and returns a module by its ID.
         * @param moduleId Identifier of the module to find.
         * @return Module object (validity checked via Module::isValid()).
         */
        Module getModule(int moduleId) const;

        /**
         * @brief Returns a specific task.
         * @param moduleId ID of the module the task belongs to.
         * @param taskId ID of the task to find.
         * @return Task object.
         */
        Task getTask(int moduleId, int taskId) const;

        /**
         * @brief Returns a list of all tasks for a specified module.
         * @param moduleId Module ID.
         */
        QVector<Task> getTasksForModule(int moduleId) const;

        /**
         * @brief Returns a list of all theory sections for a specified module.
         * @param moduleId Module ID.
         */
        QVector<TheorySection> getTheoryForModule(int moduleId) const;

        /**
         * @brief Selects a random task from the database for duel mode.
         * @return CodingTask entity ready to be passed to DuelManager.
         */
        cppforge::entities::CodingTask getRandomDuelTaskFromDb();

        /**
         * @brief Injects the task repository for database operations.
         * @param repo Pointer to the repository implementation.
         */
        void setTaskRepository(cppforge::repositories::ICodingTaskRepository *repo);

        /** @brief Returns the ID of the currently active module. */
        int getCurrentModule() const { return currentModule_; }

        /** @brief Returns the ID of the currently active task. */
        int getCurrentTask() const { return currentTask_; }

        /**
         * @brief Sets the currently active task and module.
         * @param moduleId ID of the new active module.
         * @param taskId ID of the new active task.
         */
        void setCurrentTask(int moduleId, int taskId);

        /**
         * @brief Checks if a module with given ID exists.
         */
        bool isModuleAvailable(int moduleId) const;

        /**
         * @brief Checks if a task exists within a specific module.
         */
        bool isTaskAvailable(int moduleId, int taskId) const;

        /**
         * @brief Returns a list of titles for all loaded modules.
         */
        QStringList getModuleTitles() const;

        /**
         * @brief Returns the total count of loaded modules.
         */
        int getModuleCount() const;

        /**
         * @brief Returns the task count in a specific module.
         */
        int getTaskCount(int moduleId) const;

        /**
         * @brief Returns the theory section count in a specific module.
         */
        int getTheoryCount(int moduleId) const;

        /**
         * @brief Prints debug information about a module to the console.
         */
        void printModuleInfo(int moduleId) const;

        /**
         * @brief Prints the structure of all loaded modules to the console.
         */
        void printAllModules() const;

    signals:
        /**
         * @brief Triggered when the active task changes.
         * @param moduleId New module ID.
         * @param taskId New task ID.
         */
        void taskChanged(int moduleId, int taskId);

        /**
         * @brief Triggered after successful loadTasks() completion.
         */
        void tasksLoaded();

        /**
         * @brief Triggered when module progress is updated.
         * @param moduleId Module ID.
         * @param progress Completion percentage (0-100).
         */
        void moduleProgressUpdated(int moduleId, int progress);

    private:
        /** @brief Private constructor (Singleton). */
        TaskManager() = default;
        TaskManager(const TaskManager &) = delete;
        TaskManager &operator=(const TaskManager &) = delete;

        QVector<Module> modules_;
        int currentModule_ = -1;
        int currentTask_ = -1;

        cppforge::repositories::ICodingTaskRepository *m_taskRepo = nullptr;

        /** @brief Parses a JSON object into a TheorySection structure. */
        TheorySection parseTheorySection(const QJsonObject &obj);

        /** @brief Parses a JSON object into a Task structure. */
        Task parseTask(const QJsonObject &obj);

        /** @brief Parses a JSON object into a Module structure. */
        Module parseModule(const QJsonObject &obj);
    };
} // namespace cppforge::services