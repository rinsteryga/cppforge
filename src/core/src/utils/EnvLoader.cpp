#include "utils/EnvLoader.hpp"

#include <QDebug>
#include <QFile>
#include <QTextStream>

namespace cppforge::utils
{
    bool loadEnvFile(const QString &filePath)
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Failed to open the .env file:" << filePath;
            return false;
        }

        QTextStream in(&file);
        int loadedCount = 0;

        while (!in.atEnd())
        {
            QString line = in.readLine().trimmed();
            if (line.isEmpty() || line.startsWith('#'))
            {
                continue;
            }

            int eqPos = line.indexOf('=');
            if (eqPos == -1)
            {
                continue;
            }

            QString key = line.left(eqPos).trimmed();
            QString value = line.mid(eqPos + 1).trimmed();

            const QByteArray keyBytes = key.toUtf8();
            if (qEnvironmentVariableIsSet(keyBytes.constData()))
            {
                // Process environment always wins over the file, which keeps deployed
                // overrides authoritative and makes the precedence explicit.
                continue;
            }

            qputenv(keyBytes, value.toUtf8());
            loadedCount++;
        }

        qDebug() << "Loaded" << loadedCount << "environment variables from:" << filePath;
        return true;
    }
} // namespace cppforge::utils
