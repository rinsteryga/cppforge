#include "../../include/repositories/PgModuleRepository.hpp"

#include "../../../core/include/entities/Module.hpp"

#include <QDebug>
#include <QVariant>
#include <QtSql/QSqlQuery>

#include <stdexcept>

namespace cppforge::repositories
{
    PgModuleRepository::PgModuleRepository(QSqlDatabase &database) : database_(database) {}

    std::vector<entities::Module> PgModuleRepository::getAllModules() const
    {
        std::vector<entities::Module> modules;

        if (!database_.isOpen())
        {
            throw std::runtime_error("Module repository database is unavailable");
        }

        QSqlQuery query(database_);
        if (query.prepare("SELECT id, name, description FROM modules ORDER BY id ASC") && query.exec())
        {
            while (query.next())
            {
                const std::uint64_t moduleId = query.value("id").toULongLong();
                const QString name = query.value("name").toString();
                const QString description = query.value("description").toString();

                modules.emplace_back(moduleId, name, description);
            }
        }
        else
        {
            qWarning() << "Module list query failed";
            throw std::runtime_error("Module list query failed");
        }

        return modules;
    }

    std::optional<entities::Module> PgModuleRepository::getModuleById(std::uint64_t moduleId) const
    {
        if (!database_.isOpen())
        {
            throw std::runtime_error("Module repository database is unavailable");
        }

        QSqlQuery query(database_);
        if (!query.prepare("SELECT id, name, description FROM modules WHERE id = :id"))
        {
            qWarning() << "Module lookup query failed";
            throw std::runtime_error("Module lookup query failed");
        }
        query.bindValue(":id", QVariant::fromValue(moduleId));

        if (!query.exec())
        {
            qWarning() << "Module lookup query failed";
            throw std::runtime_error("Module lookup query failed");
        }

        if (query.next())
        {
            const std::uint64_t foundId = query.value("id").toULongLong();
            const QString name = query.value("name").toString();
            const QString description = query.value("description").toString();

            return entities::Module(foundId, name, description);
        }

        return std::nullopt;
    }
} // namespace cppforge::repositories
