#include "PgUserRepository.hpp"
#include "../../core/include/entities/User.hpp"
#include "../../core/include/utils/security/PasswordHashGenerator.hpp"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QVariant>
#include <QDebug>

namespace cppforge::repositories
{
    PgUserRepository::PgUserRepository(QSqlDatabase &database) : database_(database) {}

    std::optional<entities::User> PgUserRepository::findByEmail(const QString &email) const
    {
        if (!database_.isOpen())
        {
            qCritical() << "database is closed";
            return std::nullopt;
        }

        QSqlQuery query(database_);
        query.prepare("SELECT id, username, email, password_hash FROM users WHERE email = :email");
        query.bindValue(":email", email);

        if (!query.exec())
        {
            qCritical() << "query execution failed";
            return std::nullopt;
        }

        if (query.next())
        {
            entities::User user(
                query.value("id").toInt(),
                query.value("username").toString(),
                query.value("email").toString(),
                query.value("password_hash").toString());
            return user;
        }

        return std::nullopt;
    }

    void PgUserRepository::save(cppforge::entities::User user)
    {
        if (!database_.isOpen())
        {
            qCritical() << "database is closed";
            return;
        }

        QSqlQuery query(database_);
        QString hashedPassword = cppforge::utils::security::PasswordHashGenerator::generate(user.getPasswordHash());

        query.prepare("INSERT INTO users (username, email, password_hash) "
                      "VALUES (:username, :email, :password_hash)");
        query.bindValue(":username", user.getUsername());
        query.bindValue(":email", user.getEmail());
        query.bindValue(":password_hash", hashedPassword);

        if (!query.exec())
            qCritical() << "failed to insert user:" << query.lastError().text();
    }
}
