#include "PgModuleRepository.hpp"

#include "../../core/include/entities/Module.hpp"

#include <QDebug>
#include <QVariant>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

namespace cppforge
{
    namespace repositories
    {
        PgModuleRepository::PgModuleRepository(QSqlDatabase &database) : database_(database) {}

        std::vector<entities::Module> PgModuleRepository::getAllModules() const
        {
            std::vector<entities::Module> modules;

            if (!database_.isOpen())
                return modules;

            QSqlQuery query(database_);
            query.prepare("SELECT id, name, description FROM modules ORDER BY id ASC");

            if (query.exec())
            {
                while (query.next())
                {
                    uint64_t id = query.value("id").toULongLong();
                    QString name = query.value("name").toString();
                    QString description = query.value("description").toString();

                    modules.emplace_back(id, name, description);
                }
            }
            else
            {
                qDebug() << "Failed to fetch all modules:" << query.lastError().text();
            }

            return modules;
        }

        std::optional<entities::Module> PgModuleRepository::getModuleById(uint64_t id) const
        {
            if (!database_.isOpen())
                return std::nullopt;

            QSqlQuery query(database_);
            query.prepare("SELECT id, name, description FROM modules WHERE id = :id");
            query.bindValue(":id", QVariant::fromValue(id));

            if (query.exec() && query.next())
            {
                uint64_t moduleId = query.value("id").toULongLong();
                QString name = query.value("name").toString();
                QString description = query.value("description").toString();

                return entities::Module(moduleId, name, description);
            }

            return std::nullopt;
        }
    } // namespace repositories
} // namespace cppforge
