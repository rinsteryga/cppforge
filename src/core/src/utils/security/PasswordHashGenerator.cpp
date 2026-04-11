#include "../../../include/utils/security/PasswordHashGenerator.hpp"

#include <QCryptographicHash>
#include <QRandomGenerator>

namespace cppforge::utils::security
{
    QString PasswordHashGenerator::generate(const QString &password, const QString &salt)
    {
        QByteArray hash = QCryptographicHash::hash(password.toUtf8() + salt.toUtf8(), QCryptographicHash::Sha256);
        return QString(hash.toHex());
    }

    bool PasswordHashGenerator::verify(const QString &password, const QString &hash, const QString &salt)
    {
        return generate(password, salt) == hash;
    }

    QString PasswordHashGenerator::generateSalt(int length)
    {
        QByteArray salt;
        salt.resize(length);

        for (int i = 0; i < length; ++i)
        {
            salt[i] = static_cast<char>(QRandomGenerator::securelySeeded().bounded(256));
        }

        return salt.toHex();
    }

} // namespace cppforge::utils::security
