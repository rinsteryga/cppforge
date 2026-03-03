#pragma once

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QDebug>

struct TheorySection {
    QString id;
    QString title;
    QString content;
    QString format; 
    
    bool isValid() const { return !id.isEmpty() && !title.isEmpty(); }
};

struct TestCase {
    QString input;
    QString expectedOutput;
    
    bool isValid() const { return !input.isEmpty() && !expectedOutput.isEmpty(); }
};

struct Task {
    int id = 0;
    QString title;
    QString description;
    QString difficulty;
    QString initialCode;
    QVector<TestCase> testCases;
    
    bool isValid() const { return id > 0 && !title.isEmpty(); }
};

struct Module {
    int id = 0;
    QString title;
    QString authorText; 
    QVector<TheorySection> theory;
    QVector<Task> tasks;
    
    bool isValid() const { return id >= 0 && !title.isEmpty(); }
};

class TaskManager : public QObject {
    Q_OBJECT

public:
    static TaskManager& instance();
    ~TaskManager() = default;
    
    bool loadTasks(const QString& filePath);
    void reset();
    
    Module getModule(int moduleId) const;
    Task getTask(int moduleId, int taskId) const;
    QVector<Task> getTasksForModule(int moduleId) const;
    QVector<TheorySection> getTheoryForModule(int moduleId) const;
    
    int getCurrentModule() const { return currentModule_; }
    int getCurrentTask() const { return currentTask_; }
    void setCurrentTask(int moduleId, int taskId);
    
    bool isModuleAvailable(int moduleId) const;
    bool isTaskAvailable(int moduleId, int taskId) const;
    
    QStringList getModuleTitles() const;
    int getModuleCount() const;
    int getTaskCount(int moduleId) const;
    int getTheoryCount(int moduleId) const;
    
    void printModuleInfo(int moduleId) const;
    void printAllModules() const;

signals:
    void taskChanged(int moduleId, int taskId);
    void tasksLoaded();
    void moduleProgressUpdated(int moduleId, int progress);

private:
    TaskManager() = default;
    TaskManager(const TaskManager&) = delete;
    TaskManager& operator=(const TaskManager&) = delete;

    QVector<Module> modules_;
    int currentModule_ = -1;
    int currentTask_ = -1;
    
    TheorySection parseTheorySection(const QJsonObject& obj);
    Task parseTask(const QJsonObject& obj);
    Module parseModule(const QJsonObject& obj);
};