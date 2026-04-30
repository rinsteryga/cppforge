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
     * @brief Структура данных, представляющая прогресс выполнения тестов в дуэли.
     */
    struct DuelProgress
    {
        uint32_t passedTests = 0; ///< Количество пройденных тестов
        uint32_t totalTests = 0;  ///< Общее количество тестов в задаче
    };

    /**
     * @brief Управляет сетевым взаимодействием (Peer-to-Peer) для дуэлей 1 на 1.
     *
     * DuelManager отвечает за создание TCP-сервера (хостинг) или подключение к существующему хосту.
     * Класс обрабатывает сериализацию сообщений, трансляцию прогресса и управление состоянием сессии.
     */
    class DuelManager : public QObject
    {
        Q_OBJECT

    public:
        /**
         * @brief Конструктор DuelManager.
         * @param parent Родительский QObject (опционально).
         */
        explicit DuelManager(QString localName = "Player", QObject *parent = nullptr);

        /**
         * @brief Деструктор DuelManager. Гарантирует закрытие всех соединений.
         */
        ~DuelManager() override;

        /**
         * @brief Возвращает имя локального игрока.
         */
        QString getLocalPlayerName() const { return m_localPlayerName; }

        /**
         * @brief Возвращает имя оппонента, полученное по сети.
         */
        QString getOpponentName() const { return m_opponentName; }

        /**
         * @brief Выбирает случайную задачу из базы данных и инициирует дуэль.
         *
         * Метод предназначен для вызова стороной Хоста. Выбирает случайную задачу,
         * помеченную как подходящую для дуэли, и транслирует её оппоненту.
         */
        void startRandomDuel();

        /**
         * @brief Завершает локальную сессию дуэли и отправляет финальный результат оппоненту.
         * @param score Набранные очки текущего пользователя.
         */
        void finishDuel(int score);

        /**
         * @brief Запускает локальный TCP-сервер для создания комнаты дуэли.
         * @param port Порт для прослушивания. По умолчанию 4242.
         * @return true, если сервер успешно запущен, иначе false.
         */
        bool hostRoom(quint16 port = 4242);

        /**
         * @brief Подключается к существующей комнате дуэли.
         * @param ip IP-адрес хоста.
         * @param port Порт хоста. По умолчанию 4242.
         */
        void joinRoom(const QString &ip, quint16 port = 4242);

        /**
         * @brief Отправляет данные задачи подключенному оппоненту.
         * @param task Объект задачи для передачи.
         */
        void sendTask(const cppforge::entities::CodingTask &task);

        /**
         * @brief Транслирует текущий прогресс прохождения тестов оппоненту.
         * @param progress Объект с данными о прогрессе.
         */
        void sendProgress(const cppforge::services::DuelProgress &progress);

        /**
         * @brief Отправляет идентификационные данные (никнейм) текущего пользователя.
         * @param myName Никнейм текущего пользователя.
         */
        void sendIdentity(const QString &myName);

        /**
         * @brief Уведомляет оппонента о победе текущего пользователя.
         */
        void sendWin();

        /**
         * @brief Закрывает текущее соединение и останавливает сервер, если пользователь был хостом.
         */
        void disconnectAll();

    signals:
        /**
         * @brief Сигнализирует о том, что оппонент успешно подключился к серверу.
         * @param opponentIp IP-адрес подключившегося оппонента.
         */
        void opponentConnected(const QString &opponentIp);

        /**
         * @brief Сигнализирует о получении задачи дуэли по сети.
         * @param task Десериализованный объект задачи.
         */
        void taskReceived(const cppforge::entities::CodingTask &task);

        /**
         * @brief Сигнализирует об обновлении прогресса тестов у оппонента.
         * @param progress Актуальные данные о прогрессе соперника.
         */
        void opponentProgressUpdated(const cppforge::services::DuelProgress &progress);

        /**
         * @brief Сигнализирует о том, что оппонент завершил задачу быстрее текущего пользователя.
         */
        void duelLost();

        /**
         * @brief Сигнализирует о возникновении ошибки сетевого соединения.
         * @param errorString Описание ошибки.
         */
        void connectionError(const QString &errorString);

        /**
         * @brief Сигнализирует об окончании дуэли.
         * @param winner Имя победителя (локального игрока или оппонента).
         * @param score Итоговый счет победителя.
         */
        void duelFinished(const QString &winner, int score);

        /**
         * @brief Сигнализирует о получении имени оппонента.
         * @param name Никнейм соперника.
         */
        void opponentIdentified(const QString &name);

    private slots:
        /**
         * @brief Обрабатывает новое входящее подключение на TCP-сервере.
         */
        void onNewConnection();

        /**
         * @brief Читает входящие данные из сокета и собирает их в JSON-сообщения.
         */
        void onReadyRead();

        /**
         * @brief Обрабатывает ошибки сокета.
         * @param socketError Код возникшей ошибки.
         */
        void onSocketError(QAbstractSocket::SocketError socketError);

        /**
         * @brief Выполняет очистку ресурсов при разрыве соединения.
         */
        void onSocketDisconnected();

    private:
        /**
         * @brief Анализирует полученный JSON и выполняет соответствующие действия в зависимости от типа сообщения.
         * @param json Объект сообщения.
         */
        void processMessage(const QJsonObject &json);

        /**
         * @brief Сериализует JSON-объект и записывает его в сокет с разделителем новой строки.
         * @param json Сообщение для отправки.
         */
        void sendMessage(const QJsonObject &json);

        /**
         * @brief Преобразует сущность CodingTask в JSON-объект для передачи по сети.
         * @param task Задача для сериализации.
         * @return QJsonObject, представляющий задачу.
         */
        QJsonObject serializeTask(const cppforge::entities::CodingTask &task) const;

        /**
         * @brief Восстанавливает объект CodingTask из полученного JSON-объекта.
         * @param json JSON-объект с данными задачи.
         * @return Десериализованный объект CodingTask.
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