#include "../../include/repositories/PgLessonRepository.hpp"

#include "../../../core/include/entities/Lesson.hpp"

#include <QDebug>
#include <QVariant>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

namespace cppforge
{
    namespace repositories
    {
        PgLessonRepository::PgLessonRepository(QSqlDatabase &database) : database_(database) {}

        QString PgLessonRepository::getRandomTip() const
        {
            if (!database_.isOpen())
                return "Keep coding!";

            QSqlQuery query(database_);
            query.prepare("SELECT content FROM tips ORDER BY RANDOM() LIMIT 1");

            if (query.exec() && query.next())
            {
                return query.value(0).toString();
            }

            return "Stay curious!";
        }

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

        std::optional<uint64_t> PgLessonRepository::getNextLessonId(uint64_t currentLessonId) const
        {
            if (!database_.isOpen())
                return std::nullopt;

            QSqlQuery query(database_);
            query.prepare(R"(
                SELECT id FROM lessons 
                WHERE (module_id, order_index) > (
                    SELECT module_id, order_index FROM lessons WHERE id = :id
                )
                ORDER BY module_id ASC, order_index ASC LIMIT 1
            )");
            query.bindValue(":id", static_cast<qulonglong>(currentLessonId));

            if (query.exec() && query.next())
            {
                return query.value(0).toULongLong();
            }
            return std::nullopt;
        }

        std::optional<uint64_t> PgLessonRepository::getPrevLessonId(uint64_t currentLessonId) const
        {
            if (!database_.isOpen())
                return std::nullopt;

            QSqlQuery query(database_);
            query.prepare(R"(
                SELECT id FROM lessons 
                WHERE (module_id, order_index) < (
                    SELECT module_id, order_index FROM lessons WHERE id = :id
                )
                ORDER BY module_id DESC, order_index DESC LIMIT 1
            )");
            query.bindValue(":id", static_cast<qulonglong>(currentLessonId));

            if (query.exec() && query.next())
            {
                return query.value(0).toULongLong();
            }
            return std::nullopt;
        }
    } // namespace repositories
} // namespace cppforge
