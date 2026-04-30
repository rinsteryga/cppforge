#pragma once

#include "../entities/CodingTask.hpp"
#include "TaskManager.hpp"

#include <QJsonObject>
#include <QObject>
#include <QRandomGenerator>
#include <QString>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include <cstdint>
#include <memory>

namespace cppforge::services
{
    /**
     * @brief Data structure representing test execution progress in a duel.
     */
    struct DuelProgress
    {
        uint32_t passedTests = 0; ///< Number of passed tests
        uint32_t totalTests = 0;  ///< Total number of tests in the task
    };

    /**
     * @brief Manages Peer-to-Peer network interaction for 1-on-1 duels.
     *
     * DuelManager is responsible for creating a TCP server (hosting) or connecting to an existing host.
     * The class handles message serialization, progress broadcasting, and session state management.
     */
    class DuelManager : public QObject
    {
        Q_OBJECT

    public:
        /**
         * @brief DuelManager constructor.
         * @param localName The name of the local player.
         * @param parent Optional QObject parent.
         */
        explicit DuelManager(QString localName = "Player", QObject *parent = nullptr);

        /**
         * @brief DuelManager destructor. Ensures all connections are closed.
         */
        ~DuelManager() override;

        /**
         * @brief Returns the local player's name.
         */
        QString getLocalPlayerName() const { return m_localPlayerName; }

        /**
         * @brief Returns the opponent's name received over the network.
         */
        QString getOpponentName() const { return m_opponentName; }

        /**
         * @brief Selects a random task from the database and initiates a duel.
         *
         * This method is intended to be called by the Host. It selects a random task
         * marked as suitable for duels and broadcasts it to the opponent.
         */
        void startRandomDuel();

        /**
         * @brief Completes the local duel session and sends the final result to the opponent.
         * @param score The current user's earned score.
         */
        void finishDuel(int score);

        /**
         * @brief Starts a local TCP server to create a duel room.
         * @param port Port to listen on. Defaults to 4242.
         * @return true if the server started successfully, false otherwise.
         */
        bool hostRoom(quint16 port = 4242);

        /**
         * @brief Connects to an existing duel room.
         * @param ip Host's IP address.
         * @param port Host's port. Defaults to 4242.
         */
        void joinRoom(const QString &ip, quint16 port = 4242);

        /**
         * @brief Sends task data to the connected opponent.
         * @param task Task object to transmit.
         */
        void sendTask(const cppforge::entities::CodingTask &task);

        /**
         * @brief Broadcasts current test execution progress to the opponent.
         * @param progress Progress data object.
         */
        void sendProgress(const cppforge::services::DuelProgress &progress);

        /**
         * @brief Sends the current user's identification (nickname).
         * @param myName Current user's nickname.
         */
        void sendIdentity(const QString &myName);

        /**
         * @brief Notifies the opponent of the current user's victory.
         */
        void sendWin();

        /**
         * @brief Closes the current connection and stops the server if the user was the host.
         */
        void disconnectAll();

    signals:
        /**
         * @brief Signals that an opponent has successfully connected to the server.
         * @param opponentIp IP address of the connected opponent.
         */
        void opponentConnected(const QString &opponentIp);

        /**
         * @brief Signals that a duel task has been received over the network.
         * @param task Deserialized task object.
         */
        void taskReceived(const cppforge::entities::CodingTask &task);

        /**
         * @brief Signals an update in the opponent's test progress.
         * @param progress Latest opponent progress data.
         */
        void opponentProgressUpdated(const cppforge::services::DuelProgress &progress);

        /**
         * @brief Signals that the opponent completed the task faster than the current user.
         */
        void duelLost();

        /**
         * @brief Signals a network connection error.
         * @param errorString Error description.
         */
        void connectionError(const QString &errorString);

        /**
         * @brief Signals the end of the duel.
         * @param winner Winner's name (local player or opponent).
         * @param score Winner's final score.
         */
        void duelFinished(const QString &winner, int score);

        /**
         * @brief Signals that the opponent's name has been received.
         * @param name Opponent's nickname.
         */
        void opponentIdentified(const QString &name);

    private slots:
        /**
         * @brief Handles a new incoming connection on the TCP server.
         */
        void onNewConnection();

        /**
         * @brief Reads incoming data from the socket and assembles it into JSON messages.
         */
        void onReadyRead();

        /**
         * @brief Handles socket errors.
         * @param socketError The occurred error code.
         */
        void onSocketError(QAbstractSocket::SocketError socketError);

        /**
         * @brief Performs cleanup when the connection is lost.
         */
        void onSocketDisconnected();

    private:
        /**
         * @brief Analyzes the received JSON and performs actions based on message type.
         * @param json Message object.
         */
        void processMessage(const QJsonObject &json);

        /**
         * @brief Serializes a JSON object and writes it to the socket with a newline delimiter.
         * @param json Message to send.
         */
        void sendMessage(const QJsonObject &json);

        /**
         * @brief Converts a CodingTask entity to a JSON object for network transmission.
         * @param task Task to serialize.
         * @return QJsonObject representing the task.
         */
        QJsonObject serializeTask(const cppforge::entities::CodingTask &task) const;

        /**
         * @brief Reconstructs a CodingTask object from a received JSON object.
         * @param json JSON object with task data.
         * @return Deserialized CodingTask object.
         */
        cppforge::entities::CodingTask deserializeTask(const QJsonObject &json) const;

        std::unique_ptr<QTcpServer> server_;
        QTcpSocket *socket_ = nullptr;
        QByteArray buffer_;
        QString m_localPlayerName;
        QString m_opponentName;
        bool m_isHost = false;
    };
} // namespace cppforge::services

#include <QMetaType>
Q_DECLARE_METATYPE(cppforge::services::DuelProgress)