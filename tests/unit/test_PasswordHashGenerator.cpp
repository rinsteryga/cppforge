#include "../../src/core/include/utils/security/PasswordHashGenerator.hpp"

#include <QtTest>

using namespace cppforge::utils::security;

class TestPasswordHashGenerator : public QObject
{
    Q_OBJECT

private:
    void test_GenerateAndVerify()
    {
        QString password = "cppforge_secret";
        QString salt = PasswordHashGenerator::generateSalt();
        QString hash = PasswordHashGenerator::generate(password, salt);

        QVERIFY(!hash.isEmpty());
        QVERIFY(PasswordHashGenerator::verify(password, hash, salt));
        QVERIFY(!PasswordHashGenerator::verify("wrong_password", hash, salt));
    }
};

QTEST_MAIN(TestPasswordHashGenerator)

#include "test_PasswordHashGenerator.moc"