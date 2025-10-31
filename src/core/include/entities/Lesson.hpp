#pragma once

#include <cstdint>
#include <QString>

namespace cppforge
{
    namespace entities
    {
        class Module;

        class Lesson
        {
        public:
            Lesson(uint64_t id, uint64_t moduleId, const QString &title, const QString &content);

            uint64_t getId() const;
            uint64_t getModuleId() const;
            QString getTitle() const;
            QString getContent() const;

        private:
            uint64_t id_;
            uint64_t moduleId_;
            QString title_;
            QString content_;
        };
    } // namespace entities
} // namespace cppforge
