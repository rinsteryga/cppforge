#include <api/Error.hpp>
#include <api/Json.hpp>
#include <api/dto/ModuleDto.hpp>
#include <entities/Module.hpp>
#include <nlohmann/json.hpp>

#include <QFile>
#include <QString>
#include <QtTest>

#include <stdexcept>

namespace
{
    nlohmann::json readFixture(const QString &name)
    {
        const QString path = QStringLiteral(CPPFORGE_SOURCE_DIR) + QStringLiteral("/contracts/fixtures/") + name;
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly))
        {
            throw std::runtime_error("Unable to read API fixture");
        }
        return nlohmann::json::parse(file.readAll().constData());
    }
} // namespace

class TestModuleDto : public QObject
{
    Q_OBJECT

private slots:
    void serializesModuleFixture() // NOLINT(readability-convert-member-functions-to-static)
    {
        const cppforge::entities::Module module(1, QStringLiteral("Intro"), QStringLiteral("Start here"));
        const auto dto = cppforge::api::dto::toDto(module);

        QVERIFY(nlohmann::json(dto) == readFixture(QStringLiteral("module.json")));
    }

    void serializesModuleListFixture() // NOLINT(readability-convert-member-functions-to-static)
    {
        const std::vector<cppforge::entities::Module> modules = {
            {1, QStringLiteral("Intro"), QStringLiteral("Start here")}};

        QVERIFY(nlohmann::json(cppforge::api::dto::toDto(modules)) == readFixture(QStringLiteral("modules.json")));
    }

    void parsesModuleFixture() // NOLINT(readability-convert-member-functions-to-static)
    {
        const auto module = readFixture(QStringLiteral("module.json")).get<cppforge::api::dto::ModuleDto>();

        QCOMPARE(module.id, std::uint64_t(1));
        QCOMPARE(QString::fromStdString(module.name), QStringLiteral("Intro"));
    }

    void serializesErrorFixture() // NOLINT(readability-convert-member-functions-to-static)
    {
        const auto error = cppforge::api::makeError("NOT_FOUND", "Module not found", {{"id", 404}});

        QVERIFY(nlohmann::json(error) == readFixture(QStringLiteral("error_not_found.json")));
    }
};

QTEST_APPLESS_MAIN(TestModuleDto)

#include "test_ModuleDto.moc"
