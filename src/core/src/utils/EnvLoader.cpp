
#include "../../include/utils/EnvLoader.hpp"

#include <QFile>
#include <QDebug>
#include <QTextStream>

namespace cppforge::utils
{
    bool loadEnvFile(const QString &filePath)
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qCritical() << "Failed to open the .env file:" << filePath;
            return false;
        }

        QTextStream in(&file);
        int loadedCount = 0;

        while (!in.atEnd())
        {
            QString line = in.readLine().trimmed();
            if (line.isEmpty() || line.startsWith('#'))
                continue;

            int eqPos = line.indexOf('=');
            if (eqPos == -1)
                continue;

            QString key = line.left(eqPos).trimmed();
            QString value = line.mid(eqPos + 1).trimmed();

            qputenv(key.toUtf8(), value.toUtf8());
            loadedCount++;
        }

        qDebug() << "Loaded" << loadedCount << "environment variables from:" << filePath;
        return loadedCount > 0;
    }
}
