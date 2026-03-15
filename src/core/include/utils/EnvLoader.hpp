#pragma once

#include <QString>

namespace cppforge::utils
{
    /**
     * @brief Parses and loads environment variables from a .env file.
     * 
     * This utility reads a key-value file and exports the found entries 
     * into the system environment, making them accessible via standard
     * environment variable retrieval methods.
     * 
     * @param filePath The relative or absolute path to the .env file.
     * @return True if the file was found and loaded; otherwise false.
     */
    bool loadEnvFile(const QString &filePath = "../../../../.env");
} // namespace cppforge::utils
