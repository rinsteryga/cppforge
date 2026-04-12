#pragma once

#include <QString>

#include <cstdint>

namespace cppforge::entities
{
    /**
     * @brief Represents a top-level organizational unit in the learning curriculum.
     *
     * A Module acts as a generic container that aggregates related Lessons
     * under a single thematic topic or phase of learning.
     */
    class Module
    {
    public:
        Module() = default;

        /**
         * @brief Constructs a new Module instance.
         *
         * @param moduleId The unique identifier of the module.
         * @param name The overarching thematic name of the module.
         * @param description A brief overview of what this module covers.
         */
        Module(uint64_t moduleId, QString name, QString description);

        /**
         * @brief Retrieves the module's unique identifier.
         * @return The 64-bit unsigned integer representing the module ID.
         */
        uint64_t getId() const;

        /**
         * @brief Retrieves the display name of the module.
         * @return A constant reference to the QString containing the name.
         */
        const QString &getName() const;

        /**
         * @brief Retrieves the syllabus description of the module.
         * @return A constant reference to the QString containing the description.
         */
        const QString &getDescription() const;

    private:
        uint64_t id_;
        QString name_;
        QString description_;
    };
} // namespace cppforge::entities

#include <QMetaType>
Q_DECLARE_METATYPE(cppforge::entities::Module)
