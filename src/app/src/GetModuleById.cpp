#include "app/GetModuleById.hpp"

#include <entities/Module.hpp>

namespace cppforge::app
{
    GetModuleById::GetModuleById(const repositories::IModuleRepository &repository)
        : m_repository(std::cref(repository))
    {
    }

    std::optional<entities::Module> GetModuleById::execute(std::uint64_t moduleId) const
    {
        return m_repository.get().getModuleById(moduleId);
    }
} // namespace cppforge::app
