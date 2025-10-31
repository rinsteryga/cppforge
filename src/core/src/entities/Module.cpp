#include "../../include/entities/Module.hpp"

namespace cppforge::entities
{
    Module::Module(uint64_t id, const QString &name, const QString &description)
        : id_(id), name_(name), description_(description) {}

    uint64_t Module::getId() const
    {
        return id_;
    }

    QString Module::getName() const
    {
        return name_;
    }

    QString Module::getDescription() const
    {
        return description_;
    }
}
