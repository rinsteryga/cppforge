#pragma once

#include <QString>

#include <cstdint>

namespace cppforge
{
    namespace entities
    {
        class Module
        {
        public:
            Module(uint64_t id, const QString &name, const QString &description);

            uint64_t getId() const;
            const QString &getName() const;
            const QString &getDescription() const;

        private:
            uint64_t id_;
            QString name_;
            QString description_;
        };
    } // namespace entities
} // namespace cppforge
