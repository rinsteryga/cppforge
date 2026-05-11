#include "../../include/services/CourseService.hpp"

#include "../../include/entities/CodingTask.hpp"
#include "../../include/entities/Lesson.hpp"
#include "../../include/repositories/ICodingTaskRepository.hpp"
#include "../../include/repositories/ILessonRepository.hpp"
#include "../../include/repositories/IUserRepository.hpp"

#include <QVariant>

#include <algorithm>

namespace cppforge::services
{
    CourseService::CourseService(repositories::ILessonRepository *lessonRepo,
                                 repositories::ICodingTaskRepository *taskRepo, repositories::IUserRepository *userRepo)
        : m_lessonRepo(lessonRepo), m_taskRepo(taskRepo), m_userRepo(userRepo)
    {
    }

    std::vector<entities::RoadmapNodeData> CourseService::getModuleRoadmap(uint64_t userId, uint64_t moduleId) const
    {
        std::vector<entities::RoadmapNodeData> nodes;

        auto lessons = m_lessonRepo->getLessonsByModuleId(moduleId);
        auto completedIds = m_userRepo->getCompletedLessonIds(userId, moduleId);

        for (const auto &lesson : lessons)
        {
            entities::RoadmapNodeData node;
            node.lessonId = lesson.getId();
            node.title = lesson.getTitle();
            node.isCompleted =
                std::find(completedIds.begin(), completedIds.end(), lesson.getId()) != completedIds.end();

            bool isLocked = false;
            if (!nodes.empty())
            {
                isLocked = !nodes.back().isCompleted;
            }
            node.isLocked = isLocked;
            nodes.push_back(node);
        }

        return nodes;
    }

    std::vector<int> CourseService::getAllModulesProgress(uint64_t userId) const
    {
        std::vector<int> progresses;

        for (int i = 1; i <= 14; ++i)
        {
            progresses.push_back(m_userRepo->getModuleProgress(userId, i));
        }
        return progresses;
    }

    int CourseService::getModuleProgress(uint64_t userId, uint64_t moduleId) const
    {
        return m_userRepo->getModuleProgress(userId, moduleId);
    }

    std::optional<entities::TaskWindowData> CourseService::getTaskWindowData(uint64_t userId, uint64_t lessonId) const
    {
        auto lessonOpt = m_lessonRepo->getLessonById(lessonId);
        if (!lessonOpt)
        {
            return std::nullopt;
        }

        const auto &lesson = lessonOpt.value();
        entities::TaskWindowData data;
        data.title = lesson.getTitle();
        data.theoryContent = lesson.getContent();
        data.moduleId = lesson.getModuleId();
        data.isCompleted = m_userRepo->isLessonCompleted(userId, lessonId);

        auto tasks = m_taskRepo->getTasksByLessonId(lessonId);
        if (!tasks.empty())
        {
            const auto &task = tasks.front();
            data.taskId = task.getId();
            data.practiceDescription = task.getDescription();
            data.initialCode = task.getInitialCode();
            data.testCases = task.getTestCases();
            data.previousCode = m_userRepo->getLastSubmission(userId, task.getId());
            data.whitelist = task.getWhitelist();
            data.blacklist = task.getBlacklist();
        }

        return data;
    }

    std::optional<uint64_t> CourseService::getNextLessonId(uint64_t currentLessonId) const
    {
        return m_lessonRepo->getNextLessonId(currentLessonId);
    }

    std::optional<uint64_t> CourseService::getPrevLessonId(uint64_t currentLessonId) const
    {
        return m_lessonRepo->getPrevLessonId(currentLessonId);
    }
} // namespace cppforge::services
