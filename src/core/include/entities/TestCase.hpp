#pragma once

#include <QString>

#include <cstdint>

namespace cppforge::entities
{
    /**
     * @brief Represents a single validation scenario for a coding task.
     *
     * A TestCase provides the standard input and the expected standard output
     * necessary to evaluate if a user's code submission is logically correct.
     */
    class TestCase
    {
    public:
        /**
         * @brief Constructs a new TestCase instance.
         *
         * @param testCaseId The unique identifier of the test case.
         * @param input The text fed into the standard input (stdin) of the program.
         * @param expectedOutput The exact text expected from the standard output (stdout).
         * @param isPublic Flag indicating if this test case is visible to the user before submission.
         */
        TestCase(uint64_t testCaseId, QString input, QString expectedOutput, bool isPublic);

        /**
         * @brief Retrieves the test case's unique identifier.
         * @return The 64-bit unsigned integer representing the test case ID.
         */
        uint64_t getId() const;

        /**
         * @brief Retrieves the simulated standard input text.
         * @return A constant reference to the QString containing the input parameters.
         */
        const QString &getInput() const;

        /**
         * @brief Retrieves the target standard output text.
         * @return A constant reference to the QString containing the expected result.
         */
        const QString &getExpectedOutput() const;

        /**
         * @brief Indicates whether the test case constraints are visible to the user.
         * @return True if the test case is openly available as an example; otherwise false.
         */
        bool isPublic() const;

        /**
         * @brief Compares this test case against another for strict ordering.
         *
         * Important for storing options inside an ordered std::set or analogous container,
         * sorting uniquely by their ID.
         *
         * @param other The comparable TestCase entity.
         * @return True if this test case's ID is strictly smaller than the other's.
         */
        bool operator<(const TestCase &other) const;

    private:
        uint64_t id_;
        QString input_;
        QString expectedOutput_;
        bool isPublic_;
    };
} // namespace cppforge::entities
