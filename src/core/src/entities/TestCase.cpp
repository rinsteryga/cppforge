#include "../../include/entities/TestCase.hpp"

namespace cppforge::entities
{
    TestCase::TestCase(uint64_t id, const QString &input, const QString &expectedOutput, bool isPublic)
        : id_(id), input_(input), expectedOutput_(expectedOutput), isPublic_(isPublic)
    {
    }

    uint64_t TestCase::getId() const
    {
        return id_;
    }

    const QString &TestCase::getInput() const
    {
        return input_;
    }

    const QString &TestCase::getExpectedOutput() const
    {
        return expectedOutput_;
    }

    bool TestCase::isPublic() const
    {
        return isPublic_;
    }

    bool TestCase::operator<(const TestCase &other) const
    {
        return id_ < other.id_;
    }
} // namespace cppforge::entities