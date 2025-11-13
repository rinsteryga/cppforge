#include "../../../include/utils/security/PasswordHashGenerator.hpp"

#include <QCryptographicHash>

namespace cppforge::utils::security
{
    QString PasswordHashGenerator::generate(const QString &password)
    {
        QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
        return QString(hash.toHex());
    }

    bool PasswordHashGenerator::verify(const QString &password, const QString &hash)
    {
        return generate(password) == hash;
    }
}
