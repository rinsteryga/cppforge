#pragma once

#include "../entities/CodingTask.hpp"

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include <cstdint>
#include <memory>

namespace cppforge::services
{
    /**
     * @brief Data structure representing progress in a duel.
     */
    struct DuelProgress
    {
        uint32_t passedTests = 0;
        uint32_t totalTests = 0;
    };

    /**
     * @brief Manages Peer-to-Peer network communication for 1v1 coding duels.
     *
     * DuelManager is responsible for hosting a local TCP server or connecting to
     * an existing host. It handles message serialization, progress broadcasting,
     * and duel state management.
     */
    class DuelManager : public QObject
    {
        Q_OBJECT

    public:
        /**
         * @brief Constructs a new DuelManager instance.
         * @param parent The optional parent QObject.
         */
        explicit DuelManager(QObject *parent = nullptr);

        /**
         * @brief Destructor for DuelManager.
         */
        ~DuelManager() override;

        /**
         * @brief Starts a local TCP server to host a duel room.
         * @param port The port to listen on. Default is 4242.
         * @return True if the server started successfully, false otherwise.
         */
        bool hostRoom(quint16 port = 4242);

        /**
         * @brief Connects to an existing host room.
         * @param ip The IP address of the host.
         * @param port The port the host is listening on.
         */
        void joinRoom(const QString &ip, quint16 port = 4242);

        /**
         * @brief Sends a coding task to the connected opponent.
         * @param task The coding task to be sent.
         */
        void sendTask(const cppforge::entities::CodingTask &task);

        /**
         * @brief Broadcasts the current test progress to the opponent.
         * @param progress The progress instance.
         */
        void sendProgress(const cppforge::services::DuelProgress &progress);

        /**
         * @brief Notifies the opponent that this user has won the duel.
         */
        void sendWin();

        /**
         * @brief Closes the current connection and stops the server if hosting.
         */
        void disconnectAll();

    signals:
        /**
         * @brief Emitted when an opponent successfully connects.
         * @param opponentIp The IP address of the connected opponent.
         */
        void opponentConnected(const QString &opponentIp);

        /**
         * @brief Emitted when the host sends the duel task.
         * @param task The received coding task.
         */
        void taskReceived(const cppforge::entities::CodingTask &task);

        /**
         * @brief Emitted when the opponent's test progress is updated.
         * @param progress The opponent's progress instance.
         */
        void opponentProgressUpdated(const cppforge::services::DuelProgress &progress);

        /**
         * @brief Emitted when the opponent finishes all tests before the current user.
         */
        void duelLost();

        /**
         * @brief Emitted when there is a connection error.
         * @param errorString Description of the error.
         */
        void connectionError(const QString &errorString);

    private slots:
        void onNewConnection();
        void onReadyRead();
        void onSocketError(QAbstractSocket::SocketError socketError);
        void onSocketDisconnected();

    private:
        void processMessage(const QJsonObject &json);
        void sendMessage(const QJsonObject &json);
        QJsonObject serializeTask(const cppforge::entities::CodingTask &task) const;
        cppforge::entities::CodingTask deserializeTask(const QJsonObject &json) const;

        std::unique_ptr<QTcpServer> server_;
        QTcpSocket *socket_;
        QByteArray buffer_;
    };
} // namespace cppforge::services

#include <QMetaType>
Q_DECLARE_METATYPE(cppforge::services::DuelProgress)
