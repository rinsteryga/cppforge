#pragma once

#include <cstdint>
#include <QString>

namespace cppforge
{
    namespace entities
    {
        class Module
        {
        public:
            Module(uint64_t id, const QString &name, const QString &description);

            uint64_t getId() const;
            QString getName() const;
            QString getDescription() const;

        private:
            uint64_t id_;
            QString name_;
            QString description_;
        };
    } // namespace entities
} // namespace cppforge
