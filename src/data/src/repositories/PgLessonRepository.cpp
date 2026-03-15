#include "PgLessonRepository.hpp"

#include "../../core/include/entities/Lesson.hpp"

#include <QDebug>
#include <QVariant>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

namespace cppforge
{
    namespace repositories
    {
        PgLessonRepository::PgLessonRepository(QSqlDatabase &database) : database_(database) {}

        std::vector<entities::Lesson> PgLessonRepository::getLessonsByModuleId(uint64_t moduleId) const
        {
            std::vector<entities::Lesson> lessons;

            if (!database_.isOpen())
                return lessons;

            QSqlQuery query(database_);
            query.prepare(
                "SELECT id, module_id, title, content FROM lessons WHERE module_id = :module_id ORDER BY id ASC");
            query.bindValue(":module_id", QVariant::fromValue(moduleId));

            if (query.exec())
            {
                while (query.next())
                {
                    uint64_t id = query.value("id").toULongLong();
                    uint64_t mId = query.value("module_id").toULongLong();
                    QString title = query.value("title").toString();
                    QString content = query.value("content").toString();

                    lessons.emplace_back(id, mId, title, content);
                }
            }
            else
            {
                qDebug() << "Failed to fetch lessons for module:" << query.lastError().text();
            }

            return lessons;
        }

        std::optional<entities::Lesson> PgLessonRepository::getLessonById(uint64_t id) const
        {
            if (!database_.isOpen())
                return std::nullopt;

            QSqlQuery query(database_);
            query.prepare("SELECT id, module_id, title, content FROM lessons WHERE id = :id");
            query.bindValue(":id", QVariant::fromValue(id));

            if (query.exec() && query.next())
            {
                uint64_t lessonId = query.value("id").toULongLong();
                uint64_t mId = query.value("module_id").toULongLong();
                QString title = query.value("title").toString();
                QString content = query.value("content").toString();

                return entities::Lesson(lessonId, mId, title, content);
            }

            return std::nullopt;
        }
    } // namespace repositories
} // namespace cppforge
