#pragma once

#include <cstdint>
#include <QString>

namespace cppforge
{
    namespace entities
    {
        class TestCase
        {
        public:
            TestCase(uint64_t id, const QString &input, const QString &expectedOutput, bool isPublic);

            uint64_t getId() const;
            QString getInput() const;
            QString getExpectedOutput() const;
            bool isPublic() const;

        private:
            uint64_t id_;
            QString input_;
            QString expectedOutput_;
            bool isPublic_;
        };
    } // namespace entities
} // namespace cppforge
