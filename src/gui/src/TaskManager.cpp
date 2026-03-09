#include "TaskManager.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTextCodec>

TaskManager &TaskManager::instance()
{
    static TaskManager instance;
    return instance;
}

bool TaskManager::loadTasks(const QString &filePath)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    qDebug() << "=== TaskManager: Loading tasks from ===";
    qDebug() << "Path:" << filePath;

    bool fileExists = QFile::exists(filePath);
    qDebug() << "File exists:" << fileExists;

    if (!fileExists)
    {
        QStringList alternativePaths;
        alternativePaths << ":/tasks/tasks.json"
                         << ":/resources/tasks.json"
                         << ":/tasks.json" << QCoreApplication::applicationDirPath() + "/tasks/tasks.json"
                         << QCoreApplication::applicationDirPath() + "/tasks.json"
                         << QDir::currentPath() + "/tasks/tasks.json" << QDir::currentPath() + "/tasks.json";

        for (const QString &altPath : alternativePaths)
        {
            if (QFile::exists(altPath))
            {
                qDebug() << "Found alternative path:" << altPath;
                return loadTasks(altPath);
            }
        }
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open tasks file:" << filePath;
        qDebug() << "Error:" << file.errorString();
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    qDebug() << "File size:" << data.size() << "bytes";

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString textData = codec->toUnicode(data);
    QByteArray utf8Data = textData.toUtf8();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(utf8Data, &parseError);

    if (doc.isNull())
    {
        qDebug() << "Failed to parse JSON - invalid format";
        qDebug() << "JSON parse error:" << parseError.errorString();
        qDebug() << "Error offset:" << parseError.offset;
        return false;
    }

    if (!doc.isObject())
    {
        qDebug() << "JSON root is not an object";
        return false;
    }

    modules_.clear();

    QJsonObject root = doc.object();
    qDebug() << "JSON root keys:" << root.keys();

    if (!root.contains("modules"))
    {
        qDebug() << "JSON missing 'modules' key";
        return false;
    }

    if (!root["modules"].isArray())
    {
        qDebug() << "'modules' is not an array";
        return false;
    }

    QJsonArray modulesArray = root["modules"].toArray();
    qDebug() << "Found" << modulesArray.size() << "modules in JSON";

    for (int i = 0; i < modulesArray.size(); ++i)
    {
        const QJsonValue &moduleVal = modulesArray[i];
        if (!moduleVal.isObject())
        {
            qDebug() << "Module" << i << "is not an object, skipping";
            continue;
        }

        QJsonObject moduleObj = moduleVal.toObject();
        qDebug() << "Parsing module" << i << "with keys:" << moduleObj.keys();

        Module module = parseModule(moduleObj);
        if (module.isValid())
        {
            modules_.append(module);
            qDebug() << "Successfully parsed module" << module.id << ":" << module.title;
        }
        else
        {
            qDebug() << "Module" << i << "is invalid";
        }
    }

    emit tasksLoaded();
    qDebug() << "=== TaskManager: Successfully loaded" << modules_.size() << "modules ===";
    printAllModules();
    return true;
}

Module TaskManager::parseModule(const QJsonObject &obj)
{
    Module module;
    module.id = obj["id"].toInt(-1);
    module.title = obj["title"].toString();

    qDebug() << "  Parsing module ID:" << module.id << "Title:" << module.title;

    if (obj.contains("author_text"))
    {
        module.authorText = obj["author_text"].toString();
        qDebug() << "    Found author text, length:" << module.authorText.length();
    }

    if (obj.contains("theory") && obj["theory"].isArray())
    {
        QJsonArray theoryArray = obj["theory"].toArray();
        qDebug() << "    Theory array size:" << theoryArray.size();

        for (const QJsonValue &theoryVal : theoryArray)
        {
            if (theoryVal.isObject())
            {
                TheorySection theory = parseTheorySection(theoryVal.toObject());
                if (theory.isValid())
                {
                    module.theory.append(theory);
                }
            }
        }
        qDebug() << "    Loaded" << module.theory.size() << "theory sections";
    }
    else
    {
        qDebug() << "    No theory array found";
    }

    if (obj.contains("tasks") && obj["tasks"].isArray())
    {
        QJsonArray tasksArray = obj["tasks"].toArray();
        qDebug() << "    Tasks array size:" << tasksArray.size();

        for (const QJsonValue &taskVal : tasksArray)
        {
            if (taskVal.isObject())
            {
                Task task = parseTask(taskVal.toObject());
                if (task.isValid())
                {
                    module.tasks.append(task);
                }
            }
        }
        qDebug() << "    Loaded" << module.tasks.size() << "tasks";
    }
    else
    {
        qDebug() << "    No tasks array found";
    }

    return module;
}

TheorySection TaskManager::parseTheorySection(const QJsonObject &obj)
{
    TheorySection section;
    section.id = obj["id"].toString();
    section.title = obj["title"].toString();
    section.content = obj["content"].toString();
    section.format = obj["format"].toString("text");

    qDebug() << "      Theory section:" << section.id << "-" << section.title;

    return section;
}

Task TaskManager::parseTask(const QJsonObject &obj)
{
    Task task;
    task.id = obj["id"].toInt();
    task.title = obj["title"].toString();
    task.description = obj["description"].toString();
    task.difficulty = obj["difficulty"].toString();
    task.initialCode = obj["initialCode"].toString();

    qDebug() << "      Task" << task.id << ":" << task.title << "(" << task.difficulty << ")";

    if (obj.contains("testCases") && obj["testCases"].isArray())
    {
        QJsonArray testsArray = obj["testCases"].toArray();
        for (const QJsonValue &testVal : testsArray)
        {
            if (testVal.isObject())
            {
                QJsonObject testObj = testVal.toObject();
                TestCase test;
                test.input = testObj["input"].toString();
                test.expectedOutput = testObj["output"].toString();
                task.testCases.append(test);
            }
        }
        qDebug() << "        Loaded" << task.testCases.size() << "test cases";
    }

    return task;
}

Module TaskManager::getModule(int moduleId) const
{
    qDebug() << "TaskManager::getModule(" << moduleId << ")";
    qDebug() << "  Total modules:" << modules_.size();

    for (const Module &m : modules_)
    {
        if (m.id == moduleId)
        {
            qDebug() << "  Found module:" << m.id << m.title;
            qDebug() << "    Theory count:" << m.theory.size();
            qDebug() << "    Tasks count:" << m.tasks.size();
            qDebug() << "    Has author text:" << !m.authorText.isEmpty();
            return m;
        }
    }

    qDebug() << "  Module" << moduleId << "not found";
    return Module();
}

Task TaskManager::getTask(int moduleId, int taskId) const
{
    Module m = getModule(moduleId);

    if (!m.isValid())
    {
        qDebug() << "Module" << moduleId << "not found or invalid";
        return Task();
    }

    for (const Task &t : m.tasks)
    {
        if (t.id == taskId)
        {
            return t;
        }
    }

    qDebug() << "Task" << taskId << "not found in module" << moduleId;
    return Task();
}

QVector<Task> TaskManager::getTasksForModule(int moduleId) const
{
    Module m = getModule(moduleId);
    return m.tasks;
}

QVector<TheorySection> TaskManager::getTheoryForModule(int moduleId) const
{
    Module m = getModule(moduleId);
    return m.theory;
}

void TaskManager::setCurrentTask(int moduleId, int taskId)
{
    Module m = getModule(moduleId);
    bool taskExists = false;

    for (const Task &t : m.tasks)
    {
        if (t.id == taskId)
        {
            taskExists = true;
            break;
        }
    }

    if (!m.isValid())
    {
        qDebug() << "Cannot set current task - module" << moduleId << "not found";
        return;
    }

    if (!taskExists)
    {
        qDebug() << "Cannot set current task - task" << taskId << "not found in module" << moduleId;
        return;
    }

    currentModule_ = moduleId;
    currentTask_ = taskId;

    qDebug() << "Current task set to module" << moduleId << ", task" << taskId;
    emit taskChanged(moduleId, taskId);
}

void TaskManager::reset()
{
    modules_.clear();
    currentModule_ = -1;
    currentTask_ = -1;
    qDebug() << "TaskManager reset";
}

bool TaskManager::isModuleAvailable(int moduleId) const
{
    Module m = getModule(moduleId);
    return m.isValid();
}

bool TaskManager::isTaskAvailable(int moduleId, int taskId) const
{
    Task t = getTask(moduleId, taskId);
    return t.isValid();
}

QStringList TaskManager::getModuleTitles() const
{
    QStringList titles;
    for (const Module &m : modules_)
    {
        titles.append(m.title);
    }
    return titles;
}

int TaskManager::getModuleCount() const
{
    return modules_.size();
}

int TaskManager::getTaskCount(int moduleId) const
{
    Module m = getModule(moduleId);
    return m.tasks.size();
}

int TaskManager::getTheoryCount(int moduleId) const
{
    Module m = getModule(moduleId);
    return m.theory.size();
}

void TaskManager::printModuleInfo(int moduleId) const
{
    Module m = getModule(moduleId);

    if (!m.isValid())
    {
        qDebug() << "Module" << moduleId << "not found";
        return;
    }

    qDebug() << "=== Module" << m.id << ":" << m.title << "===";
    if (!m.authorText.isEmpty())
    {
        qDebug() << "Has author text:" << m.authorText.left(50) << "...";
    }
    qDebug() << "Theory sections:" << m.theory.size();
    for (const TheorySection &t : m.theory)
    {
        qDebug() << "  -" << t.id << ":" << t.title;
    }

    qDebug() << "Tasks:" << m.tasks.size();
    for (const Task &t : m.tasks)
    {
        qDebug() << "  -" << t.id << ":" << t.title << "(" << t.difficulty << ")";
        qDebug() << "    Test cases:" << t.testCases.size();
    }
}

void TaskManager::printAllModules() const
{
    qDebug() << "=== All Modules (" << modules_.size() << ") ===";
    for (const Module &m : modules_)
    {
        qDebug() << "Module" << m.id << ":" << m.title;
        qDebug() << "  Has author text:" << !m.authorText.isEmpty();
        qDebug() << "  Theory:" << m.theory.size();
        for (const TheorySection &t : m.theory)
        {
            qDebug() << "    -" << t.id << ":" << t.title;
        }
        qDebug() << "  Tasks:" << m.tasks.size();
        for (const Task &t : m.tasks)
        {
            qDebug() << "    -" << t.id << ":" << t.title;
        }
    }
}