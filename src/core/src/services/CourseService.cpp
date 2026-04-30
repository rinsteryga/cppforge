#include "../../include/services/CourseService.hpp"

#include <QVariant>
#include <QtSql/QSqlQuery>

namespace cppforge::services
{
    std::vector<entities::RoadmapNodeData> CourseService::getModuleRoadmap(uint64_t userId, uint64_t moduleId) const
    {
        std::vector<entities::RoadmapNodeData> nodes;

        QSqlQuery query;
        query.prepare(R"(
            SELECT l.id, l.title, COALESCE(up.is_completed, FALSE) as completed
            FROM lessons l
            LEFT JOIN user_progress up ON l.id = up.lesson_id AND up.user_id = :uid
            WHERE l.module_id = :mid
            ORDER BY l.id ASC
        )");

        query.bindValue(":uid", static_cast<qulonglong>(userId));
        query.bindValue(":mid", static_cast<qulonglong>(moduleId));

        if (query.exec())
        {
            while (query.next())
            {
                entities::RoadmapNodeData node;
                node.lessonId = query.value("id").toULongLong();
                node.title = query.value("title").toString();
                node.isCompleted = query.value("completed").toBool();

                bool isLocked = false;
                if (!nodes.empty())
                {
                    isLocked = !nodes.back().isCompleted;
                }
                node.isLocked = isLocked;
                nodes.push_back(node);
            }
        }

        return nodes;
    }

    std::vector<int> CourseService::getAllModulesProgress(uint64_t userId) const
    {
        std::vector<int> progresses;
        for (int i = 1; i <= 14; ++i)
        {
            QSqlQuery query;
            query.prepare(R"(
                SELECT 
                    (SELECT COUNT(*) FROM user_progress 
                     WHERE user_id = :uid AND module_id = :mid AND is_completed = TRUE) * 100 / 
                    NULLIF((SELECT COUNT(*) FROM lessons WHERE module_id = :mid), 0)
            )");

            query.bindValue(":uid", static_cast<qulonglong>(userId));
            query.bindValue(":mid", i);

            int progressValue = 0;
            if (query.exec() && query.next())
            {
                progressValue = query.value(0).toInt();
            }
            progresses.push_back(progressValue);
        }
        return progresses;
    }

    int CourseService::getModuleProgress(uint64_t userId, uint64_t moduleId) const
    {
        if (moduleId == 0)
        {
            return 0;
        }

        QSqlQuery query;
        query.prepare(R"(
            SELECT 
                (SELECT COUNT(*) FROM lessons WHERE module_id = :mid) as total,
                (SELECT COUNT(*) FROM user_progress 
                 WHERE module_id = :mid AND user_id = :uid AND is_completed = true) as completed
        )");
        query.bindValue(":mid", static_cast<qulonglong>(moduleId));
        query.bindValue(":uid", static_cast<qulonglong>(userId));

        if (query.exec() && query.next())
        {
            int total = query.value("total").toInt();
            int completed = query.value("completed").toInt();

            if (total <= 0)
            {
                return 0;
            }

            int progress = static_cast<int>((static_cast<double>(completed) / total) * 100.0);
            return std::clamp(progress, 0, 100);
        }
        return 0;
    }

    std::optional<entities::TaskWindowData> CourseService::getTaskWindowData(uint64_t userId, uint64_t lessonId) const
    {
        QSqlQuery query;
        query.prepare(R"(
            SELECT 
                l.title, 
                l.content, 
                l.module_id,
                t.id AS task_id, 
                t.description, 
                t.initial_code, 
                (SELECT is_completed FROM user_progress WHERE user_id = :uid AND lesson_id = l.id) as is_done
            FROM lessons l 
            LEFT JOIN coding_tasks t ON l.id = t.lesson_id 
            WHERE l.id = :lessonId
        )");
        query.bindValue(":lessonId", static_cast<qulonglong>(lessonId));
        query.bindValue(":uid", static_cast<qulonglong>(userId));

        if (!query.exec() || !query.next())
        {
            return std::nullopt;
        }

        entities::TaskWindowData data;
        data.title = query.value("title").toString();
        data.theoryContent = query.value("content").toString();
        data.moduleId = query.value("module_id").toULongLong();
        data.isCompleted = query.value("is_done").toBool();

        QVariant taskIdVar = query.value("task_id");
        if (!taskIdVar.isNull())
        {
            uint64_t taskId = taskIdVar.toULongLong();
            data.taskId = taskId;
            data.practiceDescription = query.value("description").toString();
            data.initialCode = query.value("initial_code").toString();

            QSqlQuery loadSubQuery;
            loadSubQuery.prepare(
                "SELECT source_code FROM submissions WHERE user_id = :uid AND coding_task_id = :tid ORDER "
                "BY submitted_at DESC LIMIT 1");
            loadSubQuery.bindValue(":uid", static_cast<qulonglong>(userId));
            loadSubQuery.bindValue(":tid", static_cast<qulonglong>(taskId));

            if (loadSubQuery.exec() && loadSubQuery.next())
            {
                data.previousCode = loadSubQuery.value(0).toString();
            }

            QSqlQuery testQuery;
            testQuery.prepare(
                "SELECT id, input, expected_output, is_public FROM test_cases WHERE coding_task_id = :tid");
            testQuery.bindValue(":tid", static_cast<qulonglong>(taskId));
            if (testQuery.exec())
            {
                while (testQuery.next())
                {
                    data.testCases.emplace(testQuery.value("id").toULongLong(), testQuery.value("input").toString(),
                                           testQuery.value("expected_output").toString(),
                                           testQuery.value("is_public").toBool());
                }
            }
        }

        return data;
    }

    std::optional<uint64_t> CourseService::getNextLessonId(uint64_t currentLessonId) const
    {
        QSqlQuery query;
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

    std::optional<uint64_t> CourseService::getPrevLessonId(uint64_t currentLessonId) const
    {
        QSqlQuery query;
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
} // namespace cppforge::services
