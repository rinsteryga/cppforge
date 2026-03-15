#include "../../include/entities/Achievement.hpp"

#include <utility>

namespace cppforge::entities
{
    Achievement::Achievement(int64_t achievementId, QString name, QString description, QString iconPath,
                             const std::chrono::system_clock::time_point &dateEarned)
        : id_(achievementId), name_(std::move(name)), description_(std::move(description)), iconPath_(std::move(iconPath)),
          dateEarned_(dateEarned)
    {
    }

    uint64_t Achievement::getId() const
    {
        return id_;
    }

    const QString &Achievement::getName() const
    {
        return name_;
    }

    const QString &Achievement::getDescription() const
    {
        return description_;
    }

    const QString &Achievement::getIconPath() const
    {
        return iconPath_;
    }

    std::chrono::system_clock::time_point Achievement::getDateEarned() const
    {
        return dateEarned_;
    }

    bool Achievement::operator<(const Achievement &other) const
    {
        return id_ < other.id_;
    }

    bool Achievement::operator==(const Achievement &other) const
    {
        return id_ == other.id_;
    }
} // namespace cppforge::entities
