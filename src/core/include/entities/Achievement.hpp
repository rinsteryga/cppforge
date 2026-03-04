#pragma once

#include <QString>

#include <chrono>
#include <cstdint>

namespace cppforge
{
    namespace entities
    {
        class Achievement
        {
        public:
            Achievement(int64_t id, const QString &name, const QString &description, const QString &iconPath,
                        const std::chrono::system_clock::time_point &dateEarned);

            uint64_t getId() const;
            const QString &getName() const;
            const QString &getDescription() const;
            const QString &getIconPath() const;
            std::chrono::system_clock::time_point getDateEarned() const;

            bool operator<(const Achievement &other) const;
            bool operator==(const Achievement &other) const;

        private:
            uint64_t id_;
            QString name_;
            QString description_;

            QString iconPath_;
            std::chrono::system_clock::time_point dateEarned_;
        };
    } // namespace entities
} // namespace cppforge
