#include "PgUserRepository.hpp"
#include "../../core/include/entities/User.hpp"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QVariant>
#include <QDateTime>
#include <QDebug>

namespace cppforge
{
    namespace repositories
    {
        PgUserRepository::PgUserRepository(QSqlDatabase &database)
            : database_(database)
        {
        }

        std::optional<entities::User> PgUserRepository::findByEmail(const QString& email) const
        {
            if (!database_.isOpen()) return std::nullopt;

            QSqlQuery query(database_);
            query.prepare("SELECT id, username, email, password_hash, avatar_path, bio, created_at FROM users WHERE email = :email");
            query.bindValue(":email", email);

            if (query.exec() && query.next())
            {
                uint64_t id = query.value("id").toULongLong();
                QString username = query.value("username").toString();
                QString userEmail = query.value("email").toString();
                QString passwordHash = query.value("password_hash").toString();
                QString avatarPath = query.value("avatar_path").toString();
                QString bio = query.value("bio").toString();
                
                auto createdAt = std::chrono::system_clock::from_time_t(query.value("created_at").toDateTime().toSecsSinceEpoch());

                entities::User user(id, username, userEmail, passwordHash, createdAt);
                user.setAvatarPath(avatarPath);
                user.setBio(bio);

                return user;
            }

            return std::nullopt;
        }

        std::optional<entities::User> PgUserRepository::findByUsername(const QString& username) const
        {
            if (!database_.isOpen()) return std::nullopt;

            QSqlQuery query(database_);
            query.prepare("SELECT id, username, email, password_hash, avatar_path, bio, created_at FROM users WHERE username = :username");
            query.bindValue(":username", username);

            if (query.exec() && query.next())
            {
                uint64_t id = query.value("id").toULongLong();
                QString fetchedUsername = query.value("username").toString();
                QString userEmail = query.value("email").toString();
                QString passwordHash = query.value("password_hash").toString();
                QString avatarPath = query.value("avatar_path").toString();
                QString bio = query.value("bio").toString();
                
                auto createdAt = std::chrono::system_clock::from_time_t(query.value("created_at").toDateTime().toSecsSinceEpoch());

                entities::User user(id, fetchedUsername, userEmail, passwordHash, createdAt);
                user.setAvatarPath(avatarPath);
                user.setBio(bio);

                return user;
            }

            return std::nullopt;
        }

        bool PgUserRepository::save(entities::User user)
        {
            if (!database_.isOpen()) return false;

            QSqlQuery query(database_);
            if (user.getId() == 0)
            {
                query.prepare("INSERT INTO users (username, email, password_hash, avatar_path, bio) VALUES (:username, :email, :password_hash, :avatar_path, :bio)");
                query.bindValue(":username", user.getUsername());
                query.bindValue(":email", user.getEmail());
                query.bindValue(":password_hash", user.getPasswordHash());
                query.bindValue(":avatar_path", user.getAvatarPath());
                query.bindValue(":bio", user.getBio());
                
                if (!query.exec()) {
                    qDebug() << "INSERT failed inside PgUserRepository:" << query.lastError().text();
                    return false;
                }
                return true;
            }
            else
            {
                query.prepare("UPDATE users SET username = :username, email = :email, password_hash = :password_hash, avatar_path = :avatar_path, bio = :bio WHERE id = :id");
                query.bindValue(":username", user.getUsername());
                query.bindValue(":email", user.getEmail());
                query.bindValue(":password_hash", user.getPasswordHash());
                query.bindValue(":avatar_path", user.getAvatarPath());
                query.bindValue(":bio", user.getBio());
                query.bindValue(":id", QVariant::fromValue(user.getId()));
                
                if (!query.exec()) {
                    qDebug() << "UPDATE failed inside PgUserRepository:" << query.lastError().text();
                    return false;
                }
                return true;
            }
        }
    }
}
