// test_psql.cpp - сохраните в корне проекта
#include <iostream>
#include <string>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    std::cout << "=== PostgreSQL Connection Test ===" << std::endl;
    
    // Проверяем переменные окружения
    const char* env_vars[] = {"PG_HOST", "PG_PORT", "PG_DB", "PG_USER", "PG_PASSWORD"};
    
    for (auto var : env_vars) {
        char* value = std::getenv(var);
        std::cout << var << ": " << (value ? value : "(not set)") << std::endl;
    }
    
    // Проверяем, запущен ли PostgreSQL
    std::cout << "\n=== Checking PostgreSQL Service ===" << std::endl;
    
#ifdef _WIN32
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (scm) {
        SC_HANDLE service = OpenService(scm, "postgresql-x64-16", SERVICE_QUERY_STATUS);
        if (service) {
            SERVICE_STATUS status;
            if (QueryServiceStatus(service, &status)) {
                if (status.dwCurrentState == SERVICE_RUNNING) {
                    std::cout << "✅ PostgreSQL service is RUNNING" << std::endl;
                } else {
                    std::cout << "❌ PostgreSQL service is NOT running (state: " << status.dwCurrentState << ")" << std::endl;
                }
            }
            CloseServiceHandle(service);
        } else {
            std::cout << "❌ PostgreSQL service not found" << std::endl;
        }
        CloseServiceHandle(scm);
    }
#endif

    // Попробуем подключиться через psql
    std::cout << "\n=== Testing psql connection ===" << std::endl;
    char* host = std::getenv("PG_HOST");
    char* port = std::getenv("PG_PORT");
    char* db = std::getenv("PG_DB");
    char* user = std::getenv("PG_USER");
    char* password = std::getenv("PG_PASSWORD");
    
    std::string cmd = "psql ";
    if (host) cmd += "-h " + std::string(host) + " ";
    if (port) cmd += "-p " + std::string(port) + " ";
    if (db) cmd += "-d " + std::string(db) + " ";
    if (user) cmd += "-U " + std::string(user) + " ";
    cmd += "-c \"SELECT version();\"";
    
    std::cout << "Command: " << cmd << std::endl;
    int result = system(cmd.c_str());
    
    if (result == 0) {
        std::cout << "✅ psql connection SUCCESSFUL" << std::endl;
    } else {
        std::cout << "❌ psql connection FAILED (code: " << result << ")" << std::endl;
    }
    
    return 0;
}