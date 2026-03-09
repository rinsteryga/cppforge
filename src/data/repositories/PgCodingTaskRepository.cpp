#include "PgCodingTaskRepository.hpp"

#include "../../core/include/entities/CodingTask.hpp"
#include "../../core/include/entities/TestCase.hpp"

#include <QDebug>
#include <QVariant>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include <set>

namespace cppforge
{
    namespace repositories
    {
        PgCodingTaskRepository::PgCodingTaskRepository(QSqlDatabase &database) : database_(database) {}

        std::vector<entities::CodingTask> PgCodingTaskRepository::getTasksByLessonId(uint64_t lessonId) const
        {
            std::vector<entities::CodingTask> tasks;

            if (!database_.isOpen())
                return tasks;

            QSqlQuery query(database_);
            query.prepare(
                "SELECT id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit FROM coding_tasks WHERE "
                "lesson_id = :lesson_id ORDER BY id ASC");
            query.bindValue(":lesson_id", QVariant::fromValue(lessonId));

            if (query.exec())
            {
                while (query.next())
                {
                    uint64_t taskId = query.value("id").toULongLong();
                    QString title = query.value("title").toString();
                    QString description = query.value("description").toString();
                    QString initialCode = query.value("initial_code").toString();
                    QString whiteStr = query.value("whitelist").toString();
                    QString blackStr = query.value("blacklist").toString();
                    int32_t timeLimit = query.value("time_limit").toInt();
                    int32_t memoryLimit = query.value("memory_limit").toInt();

                    std::set<entities::TestCase> testCases;
                    QSqlQuery testQuery(database_);
                    testQuery.prepare(
                        "SELECT id, input, expected_output, is_public FROM test_cases WHERE coding_task_id = "
                        ":task_id ORDER BY id ASC");
                    testQuery.bindValue(":task_id", QVariant::fromValue(taskId));

                    if (testQuery.exec())
                    {
                        while (testQuery.next())
                        {
                            uint64_t testId = testQuery.value("id").toULongLong();
                            QString input = testQuery.value("input").toString();
                            QString output = testQuery.value("expected_output").toString();
                            bool isPublic = testQuery.value("is_public").toBool();

                            testCases.emplace(testId, input, output, isPublic);
                        }
                    }

                    std::optional<std::set<QString>> whitelist;
                    std::optional<std::set<QString>> blacklist;

                    if (!whiteStr.isEmpty())
                    {
                        std::set<QString> white;
                        for (const auto &kw : whiteStr.split(",", Qt::SkipEmptyParts))
                            white.insert(kw.trimmed());
                        if (!white.empty()) whitelist = std::move(white);
                    }

                    if (!blackStr.isEmpty())
                    {
                        std::set<QString> black;
                        for (const auto &kw : blackStr.split(",", Qt::SkipEmptyParts))
                            black.insert(kw.trimmed());
                        if (!black.empty()) blacklist = std::move(black);
                    }

                    tasks.emplace_back(taskId, lessonId, title, description, initialCode, testCases, timeLimit,
                                       memoryLimit, whitelist, blacklist);
                }
            }
            else
            {
                qDebug() << "Failed to fetch coding tasks:" << query.lastError().text();
            }

            return tasks;
        }

        std::optional<entities::CodingTask> PgCodingTaskRepository::getTaskById(uint64_t id) const
        {
            if (!database_.isOpen())
                return std::nullopt;

            QSqlQuery query(database_);
            query.prepare(
                "SELECT lesson_id, title, description, initial_code, whitelist, blacklist, time_limit, memory_limit FROM coding_tasks "
                "WHERE id = :id");
            query.bindValue(":id", QVariant::fromValue(id));

            if (query.exec() && query.next())
            {
                uint64_t lessonId = query.value("lesson_id").toULongLong();
                QString title = query.value("title").toString();
                QString description = query.value("description").toString();
                QString initialCode = query.value("initial_code").toString();
                QString whiteStr = query.value("whitelist").toString();
                QString blackStr = query.value("blacklist").toString();
                int32_t timeLimit = query.value("time_limit").toInt();
                int32_t memoryLimit = query.value("memory_limit").toInt();

                std::set<entities::TestCase> testCases;
                QSqlQuery testQuery(database_);
                testQuery.prepare("SELECT id, input, expected_output, is_public FROM test_cases WHERE coding_task_id = "
                                  ":task_id ORDER BY id ASC");
                testQuery.bindValue(":task_id", QVariant::fromValue(id));

                if (testQuery.exec())
                {
                    while (testQuery.next())
                    {
                        uint64_t testId = testQuery.value("id").toULongLong();
                        QString input = testQuery.value("input").toString();
                        QString output = testQuery.value("expected_output").toString();
                        bool isPublic = testQuery.value("is_public").toBool();

                        testCases.emplace(testId, input, output, isPublic);
                    }
                }

                std::optional<std::set<QString>> whitelist;
                std::optional<std::set<QString>> blacklist;

                if (!whiteStr.isEmpty())
                {
                    std::set<QString> white;
                    for (const auto &kw : whiteStr.split(",", Qt::SkipEmptyParts))
                        white.insert(kw.trimmed());
                    if (!white.empty()) whitelist = std::move(white);
                }

                if (!blackStr.isEmpty())
                {
                    std::set<QString> black;
                    for (const auto &kw : blackStr.split(",", Qt::SkipEmptyParts))
                        black.insert(kw.trimmed());
                    if (!black.empty()) blacklist = std::move(black);
                }

                return entities::CodingTask(id, lessonId, title, description, initialCode, testCases, timeLimit,
                                            memoryLimit, whitelist, blacklist);
            }

            return std::nullopt;
        }
    } // namespace repositories
} // namespace cppforge
