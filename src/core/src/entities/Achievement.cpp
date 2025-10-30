#include "Achievement.hpp"

namespace cppforge::entities
{
    Achievement::Achievement(int64_t id, const QString &name, const QString &description, const QString &iconPath,
                             const std::chrono::system_clock::time_point &dateEarned)
        : id_(id), name_(name), description_(description), iconPath_(iconPath), dateEarned_(dateEarned) {}

    uint64_t Achievement::getId() const
    {
        return id_;
    }

    QString Achievement::getName() const
    {
        return name_;
    }

    QString Achievement::getDescription() const
    {
        return description_;
    }

    QString Achievement::getIconPath() const
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
}
