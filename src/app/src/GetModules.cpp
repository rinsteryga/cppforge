#include "app/GetModules.hpp"

#include <entities/Module.hpp>

namespace cppforge::app
{
    GetModules::GetModules(const repositories::IModuleRepository &repository) : m_repository(std::cref(repository)) {}

    std::vector<entities::Module> GetModules::execute() const
    {
        return m_repository.get().getAllModules();
    }
} // namespace cppforge::app
