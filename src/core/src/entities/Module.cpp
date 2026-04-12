#include "../../include/entities/Module.hpp"

#include <utility>

namespace cppforge::entities
{
    Module::Module(uint64_t moduleId, QString name, QString description)
        : id_(moduleId), name_(std::move(name)), description_(std::move(description))
    {
    }

    uint64_t Module::getId() const
    {
        return id_;
    }

    const QString &Module::getName() const
    {
        return name_;
    }

    const QString &Module::getDescription() const
    {
        return description_;
    }
} // namespace cppforge::entities
