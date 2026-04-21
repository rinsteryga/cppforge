#include "../../include/entities/Achievement.hpp"

#include <utility>

namespace cppforge::entities
{
    Achievement::Achievement(int64_t achievementId, QString name, QString description, QString iconPath,
                             ConditionType conditionType, uint32_t conditionValue,
                             const std::chrono::system_clock::time_point &dateEarned)
        : id_(achievementId), name_(std::move(name)), description_(std::move(description)),
          iconPath_(std::move(iconPath)), conditionType_(conditionType), conditionValue_(conditionValue),
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

    ConditionType Achievement::getConditionType() const
    {
        return conditionType_;
    }

    uint32_t Achievement::getConditionValue() const
    {
        return conditionValue_;
    }

    std::chrono::system_clock::time_point Achievement::getDateEarned() const
    {
        return dateEarned_;
    }

    void Achievement::setDateEarned(const std::chrono::system_clock::time_point &date)
    {
        dateEarned_ = date;
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
