#include "../../include/utils/EnvLoader.hpp"
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QDir>

namespace cppforge::utils
{
    bool loadEnvFile(const QString &filePath)
    {
        QFile file(filePath);
        
        if (!file.exists())
        {
            qWarning() << ".env file does not exist:" << filePath;
            
            // Попробуем найти в родительских директориях
            QDir dir = QDir::current();
            for (int i = 0; i < 3; i++)
            {
                dir.cdUp();
                QString parentPath = dir.filePath(".env");
                QFile parentFile(parentPath);
                if (parentFile.exists())
                {
                    file.setFileName(parentPath);
                    qDebug() << "Found .env in parent directory:" << parentPath;
                    break;
                }
            }
        }

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
            
            // Пропускаем пустые строки и комментарии
            if (line.isEmpty() || line.startsWith('#'))
                continue;

            int eqPos = line.indexOf('=');
            if (eqPos == -1)
            {
                qWarning() << "Invalid line in .env (no '='):" << line;
                continue;
            }

            QString key = line.left(eqPos).trimmed();
            QString value = line.mid(eqPos + 1).trimmed();

            // Убираем кавычки если есть
            if ((value.startsWith('"') && value.endsWith('"')) ||
                (value.startsWith('\'') && value.endsWith('\'')))
            {
                value = value.mid(1, value.length() - 2);
            }

            qDebug() << "Setting env:" << key << "=" << (key.contains("PASSWORD") ? "***" : value);
            qputenv(key.toUtf8(), value.toUtf8());
            loadedCount++;
        }

        file.close();
        
        if (loadedCount > 0)
        {
            qDebug() << "Loaded" << loadedCount << "environment variables";
            return true;
        }
        else
        {
            qWarning() << "No environment variables loaded from:" << filePath;
            return false;
        }
    }
}