#include "../../include/services/StaticAnalyzer.hpp"

namespace cppforge::services
{
    std::optional<QString> StaticAnalyzer::analyze(const entities::CodingTask &task, const QString &code) const
    {
        if (auto blacklistOpt = task.getBlacklist(); blacklistOpt.has_value())
        {
            for (const auto &word : blacklistOpt.value())
            {
                if (code.contains(word))
                {
                    return std::optional<QString>("Use of forbidden word: " + word);
                }
            }
        }

        if (auto whitelistOpt = task.getWhitelist(); whitelistOpt.has_value())
        {
            for (const auto &word : whitelistOpt.value())
            {
                if (!code.contains(word))
                {
                    return std::optional<QString>("Missing required word: " + word);
                }
            }
        }

        return std::nullopt;
    }
} // namespace cppforge::services
