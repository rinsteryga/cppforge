#include "../../src/gui/include/TaskManager.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QString>
#include <QTemporaryDir>
#include <QTest>

#include <memory>

class TestTaskManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        // Initialize temporary directory for test files
        tempDir = std::make_unique<QTemporaryDir>();
        QVERIFY(tempDir->isValid());
    }

    void cleanupTestCase()
    {
        tempDir.reset();
        TaskManager::instance().reset();
    }

    void init() { TaskManager::instance().reset(); }

    void cleanup() { TaskManager::instance().reset(); }

    void test_SingletonInstance()
    {
        TaskManager &instance1 = TaskManager::instance();
        TaskManager &instance2 = TaskManager::instance();

        QVERIFY(&instance1 == &instance2);
    }

    void test_LoadTasksInvalidPath()
    {
        TaskManager &manager = TaskManager::instance();
        bool result = manager.loadTasks("/invalid/path/tasks.json");

        QCOMPARE(result, false);
    }

    void test_CreateValidTasksJson()
    {
        QJsonArray theory;
        QJsonObject theorySection;
        theorySection["id"] = "theory_1";
        theorySection["title"] = "Introduction to Variables";
        theorySection["content"] = "Variables are used to store data...";
        theorySection["format"] = "text";
        theory.append(theorySection);

        QJsonArray testCases;
        QJsonObject testCase;
        testCase["input"] = "test";
        testCase["expectedOutput"] = "result";
        testCases.append(testCase);

        QJsonArray tasks;
        QJsonObject task1;
        task1["id"] = 1;
        task1["title"] = "Print Hello World";
        task1["description"] = "Write a program that prints Hello World";
        task1["difficulty"] = "easy";
        task1["initialCode"] = "int main() {\n    // Your code here\n    return 0;\n}";
        task1["testCases"] = testCases;
        tasks.append(task1);

        QJsonArray modules;
        QJsonObject module1;
        module1["id"] = 1;
        module1["title"] = "C++ Basics";
        module1["authorText"] = "Learn basic C++ concepts";
        module1["theory"] = theory;
        module1["tasks"] = tasks;
        modules.append(module1);

        QJsonObject root;
        root["modules"] = modules;

        QJsonDocument doc(root);
        QString filePath = tempDir->path() + "/tasks.json";

        QFile file(filePath);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write(doc.toJson());
        file.close();

        // Now test loading
        TaskManager &manager = TaskManager::instance();
        bool result = manager.loadTasks(filePath);
        QCOMPARE(result, true);

        QCOMPARE(manager.getModuleCount(), 1);
    }

    void test_GetModuleCount()
    {
        createAndLoadValidTasksJson();

        TaskManager &manager = TaskManager::instance();
        int count = manager.getModuleCount();

        QVERIFY(count > 0);
    }

    void test_GetModule()
    {
        createAndLoadValidTasksJson();

        TaskManager &manager = TaskManager::instance();
        Module module = manager.getModule(1);

        QVERIFY(module.isValid());
        QCOMPARE(module.id, 1);
    }

    void test_GetModuleTitles()
    {
        createAndLoadValidTasksJson();

        TaskManager &manager = TaskManager::instance();
        QStringList titles = manager.getModuleTitles();

        QVERIFY(!titles.isEmpty());
        QVERIFY(titles.contains("C++ Basics"));
    }

    void test_SetCurrentTask()
    {
        createAndLoadValidTasksJson();

        TaskManager &manager = TaskManager::instance();
        manager.setCurrentTask(1, 1);

        QCOMPARE(manager.getCurrentModule(), 1);
        QCOMPARE(manager.getCurrentTask(), 1);
    }

    void test_GetTasksForModule()
    {
        createAndLoadValidTasksJson();

        TaskManager &manager = TaskManager::instance();
        QVector<Task> tasks = manager.getTasksForModule(1);

        QVERIFY(!tasks.isEmpty());
        QCOMPARE(tasks.size(), 1);
    }

    void test_GetTheoryForModule()
    {
        createAndLoadValidTasksJson();

        TaskManager &manager = TaskManager::instance();
        QVector<TheorySection> theory = manager.getTheoryForModule(1);

        QVERIFY(!theory.isEmpty());
    }

    void test_GetTaskCount()
    {
        createAndLoadValidTasksJson();

        TaskManager &manager = TaskManager::instance();
        int count = manager.getTaskCount(1);

        QVERIFY(count > 0);
    }

    void test_GetTheoryCount()
    {
        createAndLoadValidTasksJson();

        TaskManager &manager = TaskManager::instance();
        int count = manager.getTheoryCount(1);

        QVERIFY(count > 0);
    }

    void test_TaskValidity()
    {
        Task task;
        task.id = 0;
        task.title = "";

        QCOMPARE(task.isValid(), false);

        task.id = 1;
        task.title = "Valid Task";

        QCOMPARE(task.isValid(), true);
    }

    void test_TheorySectionValidity()
    {
        TheorySection section;
        section.id = "";
        section.title = "";

        QCOMPARE(section.isValid(), false);

        section.id = "theory_1";
        section.title = "Valid Section";

        QCOMPARE(section.isValid(), true);
    }

    void test_TestCaseValidity()
    {
        TestCase testCase;
        testCase.input = "";
        testCase.expectedOutput = "";

        QCOMPARE(testCase.isValid(), false);

        testCase.input = "input";
        testCase.expectedOutput = "output";

        QCOMPARE(testCase.isValid(), true);
    }

    void test_ModuleValidity()
    {
        Module module;
        module.id = -1;
        module.title = "";

        QCOMPARE(module.isValid(), false);

        module.id = 0;
        module.title = "Valid Module";

        QCOMPARE(module.isValid(), true);
    }

    void test_Reset()
    {
        createAndLoadValidTasksJson();

        TaskManager &manager = TaskManager::instance();
        QVERIFY(manager.getModuleCount() > 0);

        manager.reset();

        QCOMPARE(manager.getModuleCount(), 0);
        QCOMPARE(manager.getCurrentModule(), -1);
        QCOMPARE(manager.getCurrentTask(), -1);
    }

private:
    void createAndLoadValidTasksJson()
    {
        QJsonArray theory;
        QJsonObject theorySection;
        theorySection["id"] = "theory_1";
        theorySection["title"] = "Introduction to Variables";
        theorySection["content"] = "Variables are used to store data...";
        theorySection["format"] = "text";
        theory.append(theorySection);

        QJsonArray testCases;
        QJsonObject testCase;
        testCase["input"] = "test";
        testCase["expectedOutput"] = "result";
        testCases.append(testCase);

        QJsonArray tasks;
        QJsonObject task1;
        task1["id"] = 1;
        task1["title"] = "Print Hello World";
        task1["description"] = "Write a program that prints Hello World";
        task1["difficulty"] = "easy";
        task1["initialCode"] = "int main() {\n    // Your code here\n    return 0;\n}";
        task1["testCases"] = testCases;
        tasks.append(task1);

        QJsonArray modules;
        QJsonObject module1;
        module1["id"] = 1;
        module1["title"] = "C++ Basics";
        module1["authorText"] = "Learn basic C++ concepts";
        module1["theory"] = theory;
        module1["tasks"] = tasks;
        modules.append(module1);

        QJsonObject root;
        root["modules"] = modules;

        QJsonDocument doc(root);
        QString filePath = tempDir->path() + "/tasks.json";

        QFile file(filePath);
        file.open(QIODevice::WriteOnly);
        file.write(doc.toJson());
        file.close();

        TaskManager &manager = TaskManager::instance();
        manager.loadTasks(filePath);
    }

    std::unique_ptr<QTemporaryDir> tempDir;
};

QTEST_MAIN(TestTaskManager)
#include "test_TaskManager.moc"
