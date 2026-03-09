#include "PgQuizRepository.hpp"
#include "../../core/include/entities/Quiz.hpp"
#include "../../core/include/entities/QuizOption.hpp"

#include <QDebug>
#include <QVariant>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

namespace cppforge
{
    namespace repositories
    {
        PgQuizRepository::PgQuizRepository(QSqlDatabase &database) : database_(database) {}

        std::vector<entities::Quiz> PgQuizRepository::getQuizzesByLessonId(uint64_t lessonId) const
        {
            std::vector<entities::Quiz> quizzes;

            if (!database_.isOpen())
                return quizzes;

            QSqlQuery query(database_);
            query.prepare("SELECT id, title, question FROM quizzes WHERE lesson_id = :lesson_id ORDER BY id ASC");
            query.bindValue(":lesson_id", QVariant::fromValue(lessonId));

            if (query.exec())
            {
                while (query.next())
                {
                    uint64_t quizId = query.value("id").toULongLong();
                    QString title = query.value("title").toString();
                    QString question = query.value("question").toString();

                    std::set<entities::QuizOption> options;
                    QSqlQuery optionQuery(database_);
                    optionQuery.prepare("SELECT id, option_text, is_correct FROM quiz_options WHERE quiz_id = :quiz_id ORDER BY id ASC");
                    optionQuery.bindValue(":quiz_id", QVariant::fromValue(quizId));

                    if (optionQuery.exec())
                    {
                        while (optionQuery.next())
                        {
                            uint64_t optId = optionQuery.value("id").toULongLong();
                            QString optText = optionQuery.value("option_text").toString();
                            bool isCorrect = optionQuery.value("is_correct").toBool();
                            options.emplace(optId, optText, isCorrect);
                        }
                    }

                    quizzes.emplace_back(quizId, lessonId, title, question, options);
                }
            }
            else
            {
                qDebug() << "Failed to fetch quizzes:" << query.lastError().text();
            }

            return quizzes;
        }

        std::optional<entities::Quiz> PgQuizRepository::getQuizById(uint64_t id) const
        {
            if (!database_.isOpen())
                return std::nullopt;

            QSqlQuery query(database_);
            query.prepare("SELECT lesson_id, title, question FROM quizzes WHERE id = :id");
            query.bindValue(":id", QVariant::fromValue(id));

            if (query.exec() && query.next())
            {
                uint64_t lessonId = query.value("lesson_id").toULongLong();
                QString title = query.value("title").toString();
                QString question = query.value("question").toString();

                std::set<entities::QuizOption> options;
                QSqlQuery optionQuery(database_);
                optionQuery.prepare("SELECT id, option_text, is_correct FROM quiz_options WHERE quiz_id = :quiz_id ORDER BY id ASC");
                optionQuery.bindValue(":quiz_id", QVariant::fromValue(id));

                if (optionQuery.exec())
                {
                    while (optionQuery.next())
                    {
                        uint64_t optId = optionQuery.value("id").toULongLong();
                        QString optText = optionQuery.value("option_text").toString();
                        bool isCorrect = optionQuery.value("is_correct").toBool();
                        options.emplace(optId, optText, isCorrect);
                    }
                }

                return entities::Quiz(id, lessonId, title, question, options);
            }

            return std::nullopt;
        }
    } // namespace repositories
} // namespace cppforge
