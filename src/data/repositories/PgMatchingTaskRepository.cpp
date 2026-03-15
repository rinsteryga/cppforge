#include "PgMatchingTaskRepository.hpp"

#include "../../core/include/entities/MatchingPair.hpp"
#include "../../core/include/entities/MatchingTask.hpp"

#include <QDebug>
#include <QVariant>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

namespace cppforge
{
    namespace repositories
    {
        PgMatchingTaskRepository::PgMatchingTaskRepository(QSqlDatabase &database) : database_(database) {}

        std::vector<entities::MatchingTask>
        PgMatchingTaskRepository::getMatchingTasksByLessonId(uint64_t lessonId) const
        {
            std::vector<entities::MatchingTask> tasks;

            if (!database_.isOpen())
                return tasks;

            QSqlQuery query(database_);
            query.prepare(
                "SELECT id, title, description FROM matching_tasks WHERE lesson_id = :lesson_id ORDER BY id ASC");
            query.bindValue(":lesson_id", QVariant::fromValue(lessonId));

            if (query.exec())
            {
                while (query.next())
                {
                    uint64_t taskId = query.value("id").toULongLong();
                    QString title = query.value("title").toString();
                    QString description = query.value("description").toString();

                    std::set<entities::MatchingPair> pairs;
                    QSqlQuery pairQuery(database_);
                    pairQuery.prepare("SELECT id, left_item, right_item FROM matching_pairs WHERE matching_task_id = "
                                      ":task_id ORDER BY id ASC");
                    pairQuery.bindValue(":task_id", QVariant::fromValue(taskId));

                    if (pairQuery.exec())
                    {
                        while (pairQuery.next())
                        {
                            uint64_t pairId = pairQuery.value("id").toULongLong();
                            QString left = pairQuery.value("left_item").toString();
                            QString right = pairQuery.value("right_item").toString();
                            pairs.emplace(pairId, left, right);
                        }
                    }

                    tasks.emplace_back(taskId, lessonId, title, description, pairs);
                }
            }
            else
            {
                qDebug() << "Failed to fetch matching tasks:" << query.lastError().text();
            }

            return tasks;
        }

        std::optional<entities::MatchingTask> PgMatchingTaskRepository::getMatchingTaskById(uint64_t id) const
        {
            if (!database_.isOpen())
                return std::nullopt;

            QSqlQuery query(database_);
            query.prepare("SELECT lesson_id, title, description FROM matching_tasks WHERE id = :id");
            query.bindValue(":id", QVariant::fromValue(id));

            if (query.exec() && query.next())
            {
                uint64_t lessonId = query.value("lesson_id").toULongLong();
                QString title = query.value("title").toString();
                QString description = query.value("description").toString();

                std::set<entities::MatchingPair> pairs;
                QSqlQuery pairQuery(database_);
                pairQuery.prepare("SELECT id, left_item, right_item FROM matching_pairs WHERE matching_task_id = "
                                  ":task_id ORDER BY id ASC");
                pairQuery.bindValue(":task_id", QVariant::fromValue(id));

                if (pairQuery.exec())
                {
                    while (pairQuery.next())
                    {
                        uint64_t pairId = pairQuery.value("id").toULongLong();
                        QString left = pairQuery.value("left_item").toString();
                        QString right = pairQuery.value("right_item").toString();
                        pairs.emplace(pairId, left, right);
                    }
                }

                return entities::MatchingTask(id, lessonId, title, description, pairs);
            }

            return std::nullopt;
        }
    } // namespace repositories
} // namespace cppforge
