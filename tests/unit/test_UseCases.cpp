#include <app/GetModuleById.hpp>
#include <app/GetModules.hpp>
#include <entities/Module.hpp>
#include <repositories/IModuleRepository.hpp>

#include <QtTest>

#include <stdexcept>
#include <vector>

namespace
{
    class FakeModuleRepository final : public cppforge::repositories::IModuleRepository
    {
    public:
        explicit FakeModuleRepository(std::vector<cppforge::entities::Module> modules = {})
            : m_modules(std::move(modules))
        {
        }

        std::vector<cppforge::entities::Module> getAllModules() const override
        {
            if (m_shouldThrow)
            {
                throw std::runtime_error("fake repository failure");
            }
            return m_modules;
        }

        std::optional<cppforge::entities::Module> getModuleById(std::uint64_t moduleId) const override
        {
            if (m_shouldThrow)
            {
                throw std::runtime_error("fake repository failure");
            }
            for (const auto &module : m_modules)
            {
                if (module.getId() == moduleId)
                {
                    return module;
                }
            }
            return std::nullopt;
        }

        void setShouldThrow(bool shouldThrow) { m_shouldThrow = shouldThrow; }

    private:
        std::vector<cppforge::entities::Module> m_modules;
        bool m_shouldThrow{};
    };
} // namespace

class TestUseCases : public QObject
{
    Q_OBJECT

private slots:
    void getModulesReturnsRepositoryResults() // NOLINT(readability-convert-member-functions-to-static)
    {
        FakeModuleRepository repository({{1, "Intro", "Start here"}, {2, "C++", "Language basics"}});
        const cppforge::app::GetModules useCase(repository);

        const auto modules = useCase.execute();

        QCOMPARE(modules.size(), std::size_t(2));
        QCOMPARE(modules.front().getId(), std::uint64_t(1));
        QCOMPARE(modules.front().getName(), QStringLiteral("Intro"));
    }

    void getModulesHandlesEmptyRepository() // NOLINT(readability-convert-member-functions-to-static)
    {
        FakeModuleRepository repository;
        const cppforge::app::GetModules useCase(repository);

        QVERIFY(useCase.execute().empty());
    }

    void getModulesPropagatesRepositoryFailure() // NOLINT(readability-convert-member-functions-to-static)
    {
        FakeModuleRepository repository;
        repository.setShouldThrow(true);
        const cppforge::app::GetModules useCase(repository);

        QVERIFY_EXCEPTION_THROWN(useCase.execute(), std::runtime_error);
    }

    void getModuleByIdReturnsMatchingModule() // NOLINT(readability-convert-member-functions-to-static)
    {
        FakeModuleRepository repository({{17, "Pointers", "Memory and ownership"}});
        const cppforge::app::GetModuleById useCase(repository);

        const auto module = useCase.execute(17);

        if (!module.has_value())
        {
            QFAIL("Expected the requested module");
        }
        QCOMPARE(module.value().getName(), QStringLiteral("Pointers"));
    }
};

QTEST_APPLESS_MAIN(TestUseCases)

#include "test_UseCases.moc"
