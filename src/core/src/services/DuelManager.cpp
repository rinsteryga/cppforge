#include "../../include/services/DuelManager.hpp"

#include "../../include/entities/TestCase.hpp"
#include "../../include/services/TaskManager.hpp"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtNetwork/QHostAddress>

namespace cppforge::services
{
    DuelManager::DuelManager(QString localName, QObject *parent)
        : QObject(parent), socket_(nullptr), m_localPlayerName(std::move(localName))
    {
        qRegisterMetaType<cppforge::services::DuelProgress>("cppforge::services::DuelProgress");
        qRegisterMetaType<cppforge::entities::CodingTask>("cppforge::entities::CodingTask");
    }

    DuelManager::~DuelManager()
    {
        disconnectAll();
    }

    bool DuelManager::hostRoom(quint16 port)
    {
        m_isHost = true;
        disconnectAll();
        server_ = std::make_unique<QTcpServer>(this);
        connect(server_.get(), &QTcpServer::newConnection, this, &DuelManager::onNewConnection);
        return server_->listen(QHostAddress::Any, port);
    }

    void DuelManager::joinRoom(const QString &ip, quint16 port)
    {
        m_isHost = false;

        disconnectAll();

        socket_ = new QTcpSocket(this);

        connect(socket_, &QTcpSocket::readyRead, this, &DuelManager::onReadyRead);
        connect(socket_, &QAbstractSocket::errorOccurred, this, &DuelManager::onSocketError);
        connect(socket_, &QTcpSocket::disconnected, this, &DuelManager::onSocketDisconnected);

        connect(socket_, &QTcpSocket::connected, this,
                [this]()
                {
                    sendIdentity(m_localPlayerName);

                    emit opponentConnected(socket_->peerAddress().toString());
                });

        socket_->connectToHost(ip, port);
    }

    void DuelManager::sendTask(const cppforge::entities::CodingTask &task)
    {
        QJsonObject json;
        json["type"] = "TASK";
        json["payload"] = serializeTask(task);
        sendMessage(json);
    }

    void DuelManager::sendProgress(const cppforge::services::DuelProgress &progress)
    {
        QJsonObject payload;
        payload["passed"] = static_cast<qint64>(progress.passedTests);
        payload["total"] = static_cast<qint64>(progress.totalTests);

        QJsonObject json;
        json["type"] = "PROGRESS";
        json["payload"] = payload;
        sendMessage(json);
    }

    void DuelManager::sendWin()
    {
        QJsonObject json;
        json["type"] = "WIN";
        sendMessage(json);
    }

    void DuelManager::disconnectAll()
    {
        if (socket_ != nullptr)
        {
            socket_->disconnectFromHost();
            socket_->deleteLater();
            socket_ = nullptr;
        }
        if (server_)
        {
            server_->close();
            server_.reset();
        }
        buffer_.clear();
    }

    void DuelManager::onNewConnection()
    {
        if (server_->hasPendingConnections())
        {
            socket_ = server_->nextPendingConnection();
            connect(socket_, &QTcpSocket::readyRead, this, &DuelManager::onReadyRead);
            connect(socket_, &QTcpSocket::disconnected, this, &DuelManager::onSocketDisconnected);

            sendIdentity(m_localPlayerName);

            emit opponentConnected(socket_->peerAddress().toString());
        }
    }

    void DuelManager::onReadyRead()
    {
        if (socket_ == nullptr)
        {
            return;
        }

        buffer_.append(socket_->readAll());

        int newlineIndex;
        while ((newlineIndex = buffer_.indexOf('\n')) != -1)
        {
            QByteArray chunk = buffer_.left(newlineIndex);
            buffer_.remove(0, newlineIndex + 1);

            QJsonDocument doc = QJsonDocument::fromJson(chunk);
            if (!doc.isNull() && doc.isObject())
            {
                processMessage(doc.object());
            }
        }
    }

    void DuelManager::sendIdentity(const QString &myName)
    {
        m_localPlayerName = myName;

        QJsonObject json;
        json["type"] = "IDENTIFY";
        QJsonObject payload;
        payload["nickname"] = myName;
        payload["isHost"] = m_isHost;
        json["payload"] = payload;

        sendMessage(json);
    }

    void DuelManager::onSocketError(QAbstractSocket::SocketError)
    {
        if (socket_ != nullptr)
        {
            emit connectionError(socket_->errorString());
        }
    }

    void DuelManager::onSocketDisconnected()
    {
        emit connectionError("Opponent disconnected.");
        socket_->deleteLater();
        socket_ = nullptr;
    }

    void DuelManager::processMessage(const QJsonObject &json)
    {
        QString type = json["type"].toString();
        QJsonObject payload = json["payload"].toObject();

        if (type == "IDENTIFY")
        {
            m_opponentName = payload["nickname"].toString();
            emit opponentIdentified(m_opponentName);
        }

        else if (type == "TASK")
        {
            cppforge::entities::CodingTask task = deserializeTask(payload);
            emit taskReceived(task);
        }

        else if (type == "PROGRESS")
        {
            cppforge::services::DuelProgress progress;
            progress.passedTests = static_cast<uint32_t>(payload["passed"].toInt());
            progress.totalTests = static_cast<uint32_t>(payload["total"].toInt());
            emit opponentProgressUpdated(progress);
        }

        else if (type == "SURRENDER")
        {
            emit duelFinished(m_localPlayerName, 1000);
        }
        else if (type == "WIN" || type == "FINISH")
        {
            int finalScore = payload["score"].toInt();
            QString winner = payload["winner"].toString();

            if (winner.isEmpty() && type == "FINISH")
            {
                winner = m_localPlayerName;
            }

            emit duelFinished(winner, finalScore);
        }
    }

    void DuelManager::sendMessage(const QJsonObject &json)
    {
        if (socket_ == nullptr || socket_->state() != QAbstractSocket::ConnectedState)
        {
            return;
        }

        QJsonDocument doc(json);
        QByteArray data = doc.toJson(QJsonDocument::Compact) + '\n';
        socket_->write(data);
        socket_->flush();
    }

    void DuelManager::startRandomDuel()
    {
        qDebug() << "Requesting random duel task from Database...";

        auto &tm = TaskManager::instance();

        cppforge::entities::CodingTask task = tm.getRandomDuelTaskFromDb();

        if (task.getId() == 0)
        {
            qDebug() << "CRITICAL ERROR: No duel task returned from DB! Check 'is_duel' flag or SQL connection.";
            return;
        }

        qDebug() << "Task found! Sending to server and UI. Title:" << task.getTitle();

        sendTask(task);

        emit taskReceived(task);
    }

    QJsonObject DuelManager::serializeTask(const cppforge::entities::CodingTask &task) const
    {
        QJsonObject json;
        json["id"] = static_cast<qint64>(task.getId());
        if (task.getLessonId().has_value())
        {
            json["lesson_id"] = static_cast<qint64>(task.getLessonId().value());
        }
        if (task.getDuelTopic().has_value())
        {
            json["duel_topic"] = task.getDuelTopic().value();
        }
        json["title"] = task.getTitle();
        json["description"] = task.getDescription();
        json["initial_code"] = task.getInitialCode();
        json["time_limit"] = task.getTimeLimit();
        json["memory_limit"] = task.getMemoryLimit();

        QJsonArray testsArray;
        for (const auto &testCase : task.getTestCases())
        {
            QJsonObject testJson;
            testJson["id"] = static_cast<qint64>(testCase.getId());
            testJson["input"] = testCase.getInput();
            testJson["expected_output"] = testCase.getExpectedOutput();
            testJson["is_public"] = testCase.isPublic();
            testsArray.append(testJson);
        }
        json["test_cases"] = testsArray;

        return json;
    }

    void DuelManager::finishDuel(int score)
    {
        QJsonObject payload;
        payload["score"] = score;
        payload["winner"] = m_localPlayerName;

        QJsonObject json;
        json["type"] = "FINISH";
        json["payload"] = payload;

        sendMessage(json);

        emit duelFinished(m_localPlayerName, score);
    }

    void DuelManager::surrender()
    {
        QJsonObject json;
        json["type"] = "SURRENDER";
        QJsonObject payload;
        payload["surrenderer"] = m_localPlayerName;
        json["payload"] = payload;

        sendMessage(json);

        emit duelFinished(m_opponentName, 0);
    }

    cppforge::entities::CodingTask DuelManager::deserializeTask(const QJsonObject &json) const
    {
        uint64_t id = json["id"].toVariant().toULongLong();
        std::optional<uint64_t> lessonId = std::nullopt;
        if (json.contains("lesson_id"))
        {
            lessonId = json["lesson_id"].toVariant().toULongLong();
        }
        std::optional<QString> duelTopic = std::nullopt;
        if (json.contains("duel_topic"))
        {
            duelTopic = json["duel_topic"].toString();
        }
        QString title = json["title"].toString();
        QString description = json["description"].toString();
        QString initialCode = json["initial_code"].toString();
        int32_t timeLimit = json["time_limit"].toInt();
        int32_t memoryLimit = json["memory_limit"].toInt();

        std::set<cppforge::entities::TestCase> testCases;
        QJsonArray testsArray = json["test_cases"].toArray();
        for (const auto &val : testsArray)
        {
            QJsonObject testJson = val.toObject();
            uint64_t testId = testJson["id"].toVariant().toULongLong();
            QString input = testJson["input"].toString();
            QString output = testJson["expected_output"].toString();
            bool isPublic = testJson["is_public"].toBool();
            testCases.emplace(testId, input, output, isPublic);
        }

        return entities::CodingTask(id, lessonId, title, description, initialCode, testCases, timeLimit, memoryLimit,
                                    std::nullopt, std::nullopt, duelTopic);
    }
} // namespace cppforge::services
