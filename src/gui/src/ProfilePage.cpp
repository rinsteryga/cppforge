#include "ProfilePage.hpp"

#include "../../core/include/services/UserService.hpp"

#include <QDebug>
#include <QDialog>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVariant>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include <random>

namespace
{
    class InfoDialog : public QDialog
    {
    public:
        InfoDialog(const QString &title, const QString &text, QWidget *parent = nullptr) : QDialog(parent)
        {
            setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
            setAttribute(Qt::WA_TranslucentBackground);

            auto *layout = new QVBoxLayout(this);
            auto *card = new QFrame();
            card->setObjectName("DialogCard");
            card->setStyleSheet(R"(
                #DialogCard {
                    background-color: white;
                    border: 2px solid #62639b;
                    border-radius: 15px;
                }
                QLabel { color: #333; font-size: 14px; }
                #Title { font-weight: bold; font-size: 18px; color: #62639b; }
                QPushButton {
                    background-color: #62639b;
                    color: white;
                    border-radius: 8px;
                    padding: 8px 20px;
                    font-weight: bold;
                    border: none;
                }
                QPushButton:hover { background-color: #51528a; }
            )");

            auto *cardLayout = new QVBoxLayout(card);
            cardLayout->setContentsMargins(25, 25, 25, 25);
            cardLayout->setSpacing(15);

            auto *titleLabel = new QLabel(title);
            titleLabel->setObjectName("Title");
            cardLayout->addWidget(titleLabel);

            auto *contentLabel = new QLabel(text);
            contentLabel->setWordWrap(true);
            contentLabel->setMinimumWidth(300);
            cardLayout->addWidget(contentLabel);

            auto *closeBtn = new QPushButton("Понятно");
            closeBtn->setCursor(Qt::PointingHandCursor);
            connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
            cardLayout->addWidget(closeBtn, 0, Qt::AlignRight);

            layout->addWidget(card);
        }
    };
} // namespace

ProfilePage::ProfilePage(QWidget *parent) : QWidget(parent)
{
    setupUI();
    applyStyles();
}

void ProfilePage::setUserData(int userId, const QString &name, const QString &avatarPath)
{
    currentUserId = userId;

    if (userNameLabel)
    {
        userNameLabel->setText(name);
    }

    QString finalPath = avatarPath;

    if (finalPath.isEmpty() || finalPath == "NULL")
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 4);
        finalPath = QString(":/images/default_avatar%1.png").arg(dis(gen));

        qDebug() << "First time login. Assigning random avatar:" << finalPath;

        QSqlQuery query;
        query.prepare("UPDATE users SET avatar_path = :path WHERE id = :id");
        query.bindValue(":path", finalPath);
        query.bindValue(":id", userId);

        if (!query.exec())
        {
            qDebug() << "Error saving initial avatar to DB:" << query.lastError().text();
        }
    }

    QPixmap pix(finalPath);
    if (!pix.isNull())
    {
        avatarLabel->setPixmap(pix.scaled(200, 200, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }

    if (userService_)
    {
        int solvedCount = userService_->getSolvedTasksCount(userId);
        if (solvedTasksLabel)
        {
            solvedTasksLabel->setText(QString::number(solvedCount));
        }
    }
}

void ProfilePage::setUserService(cppforge::services::UserService *service)
{
    userService_ = service;
}

void ProfilePage::setupUI()
{
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(50, 50, 50, 50);
    mainLayout->setSpacing(60);

    auto *leftSection = new QVBoxLayout();
    leftSection->setSpacing(30);

    auto *headerLayout = new QHBoxLayout();

    avatarLabel = new QLabel();
    avatarLabel->setFixedSize(200, 200);
    avatarLabel->setObjectName("AvatarSquare");
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setStyleSheet("background-color: #f0f0f0;");

    auto *infoLayout = new QVBoxLayout();

    userNameLabel = new QLabel("Загрузка...");
    userNameLabel->setObjectName("UserNameLabel");

    auto *changeAvatarBtn = new QPushButton("Изменить фото");
    changeAvatarBtn->setMinimumWidth(150);
    changeAvatarBtn->setCursor(Qt::PointingHandCursor);
    changeAvatarBtn->setObjectName("ChangeAvatarBtn");

    infoLayout->addStretch();
    infoLayout->addWidget(userNameLabel);
    infoLayout->addWidget(changeAvatarBtn);
    infoLayout->addStretch();

    headerLayout->addWidget(avatarLabel);
    headerLayout->addSpacing(30);
    headerLayout->addLayout(infoLayout);
    headerLayout->addStretch();

    leftSection->addLayout(headerLayout);

    auto *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #E0E0E0;");
    leftSection->addWidget(line);

    auto *statTitle = new QLabel("Статистика");
    statTitle->setObjectName("StatTitle");
    leftSection->addWidget(statTitle);

    auto *statsGrid = new QGridLayout();
    statsGrid->setSpacing(20);
    for (int i = 0; i < 4; ++i)
    {
        auto *card = new QFrame();
        card->setObjectName("StatCard");
        card->setMinimumSize(220, 110);

        if (i == 0)
        {
            auto *cLayout = new QVBoxLayout(card);
            auto *t = new QLabel("Заданий решено:");
            solvedTasksLabel = new QLabel("0");
            solvedTasksLabel->setObjectName("GreenValue");
            cLayout->addWidget(t);
            cLayout->addWidget(solvedTasksLabel);
        }
        statsGrid->addWidget(card, i / 2, i % 2);
    }
    leftSection->addLayout(statsGrid);
    leftSection->addStretch();

    auto *rightSection = new QVBoxLayout();

    auto *streakLayout = new QHBoxLayout();
    streakLayout->addStretch();
    auto *streakIcon = new QLabel("🔥 14");
    streakIcon->setObjectName("StreakLabel");
    streakLayout->addWidget(streakIcon);
    rightSection->addLayout(streakLayout);

    auto createBigBox = [this](const QString &titleText)
    {
        auto *box = new QFrame();
        box->setObjectName("BigBlock");
        box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        auto *l = new QVBoxLayout(box);
        auto *title = new QLabel(titleText);
        title->setProperty("class", QString("box-header"));
        l->addWidget(title, 0, Qt::AlignTop);
        return box;
    };

    rightSection->addWidget(createBigBox("ДОСТИЖЕНИЯ"));
    rightSection->addSpacing(20);
    rightSection->addWidget(createBigBox("НЕДАВНЯЯ АКТИВНОСТЬ"));

    auto *footerLayout = new QHBoxLayout();
    footerLayout->addStretch();

    auto createFooterBtn = [this](const QString &text, auto slot)
    {
        auto *btn = new QPushButton(text);
        btn->setObjectName("FooterBtn");
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, slot);
        return btn;
    };

    footerLayout->addWidget(createFooterBtn("О CppForge", &ProfilePage::onAboutClicked));
    footerLayout->addWidget(new QLabel("   "));
    footerLayout->addWidget(createFooterBtn("Контакты", &ProfilePage::onContactsClicked));

    auto *footerContainer = new QVBoxLayout();
    footerContainer->addLayout(footerLayout);

    auto *privacyLayout = new QHBoxLayout();
    privacyLayout->addStretch();
    privacyLayout->addWidget(createFooterBtn("Конфиденциальность", &ProfilePage::onPrivacyClicked));
    footerContainer->addLayout(privacyLayout);

    rightSection->addStretch();
    rightSection->addLayout(footerContainer);

    mainLayout->addLayout(leftSection, 3);
    mainLayout->addLayout(rightSection, 2);

    connect(changeAvatarBtn, &QPushButton::clicked, this, &ProfilePage::onChangeAvatarClicked);
}

void ProfilePage::applyStyles()
{
    setStyleSheet(R"(
        #UserNameLabel { font-size: 28px; font-weight: bold; color: #222; }
        #StatTitle { font-size: 22px; font-weight: bold; font-style: italic; margin-top: 10px; }
        #AvatarSquare { background-color: #f0f0f0; border: 1px solid #DDD; }
        #StatCard, #BigBlock { background-color: white; border: 2px solid #EAEAEA; border-radius: 20px; }
        .box-header { background-color: #F8F9FB; padding: 5px; font-weight: bold; color: #555; border-bottom: 1px solid #EEE; }
        #GreenValue { color: #4CAF50; font-size: 24px; font-weight: bold; }
        #StreakLabel { font-size: 26px; font-weight: bold; }
        #FooterLinks { color: #888; font-size: 13px; line-height: 1.5; }
        #FooterBtn {
            background: transparent;
            border: none;
            color: #888;
            font-size: 13px;
            padding: 0;
            text-align: right;
        }
        #FooterBtn:hover { color: #62639b; text-decoration: underline; }
        #ChangeAvatarBtn {
            background-color: #f8f9ff; color: #62639b; border: 1px solid #62639b;
            border-radius: 5px; padding: 8px; font-weight: 500;
        }
        #ChangeAvatarBtn:hover { background-color: #eef0ff; }
    )");
}

void ProfilePage::onChangeAvatarClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите аватар", "", "Images (*.png *.jpg *.jpeg)");
    if (!fileName.isEmpty())
    {
        QPixmap pix(fileName);
        if (!pix.isNull())
        {
            avatarLabel->setPixmap(pix.scaled(200, 200, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

            QSqlQuery query;
            query.prepare("UPDATE users SET avatar_path = :path WHERE id = :id");
            query.bindValue(":path", fileName);
            query.bindValue(":id", currentUserId);
            query.exec();
        }
    }
}

void ProfilePage::onAboutClicked()
{
    QString aboutText =
        "cppforge — это образовательная платформа для изучения языков программирования C и C++. "
        "Наш курс ведет от основ операционных систем к системному программированию и современным стандартам C++, "
        "включая PvP-режим и практические задачи.\n\n"
        "Команда разработчиков проекта состоит из выпускников КМПО РАНХиГС, которые воплотили идею по-настоящему "
        "крутой платформы для обучения.";
    InfoDialog dlg("О CppForge", aboutText, this);
    dlg.exec();
}

void ProfilePage::onContactsClicked()
{
    InfoDialog dlg("Контакты", "rinsterr@yandex.ru — по всем вопросам и предложениям", this);
    dlg.exec();
}

void ProfilePage::onPrivacyClicked()
{
    QString privacyText =
        "Мы ценим вашу конфиденциальность. Приложение хранит ваши данные исключительно локально на вашем устройстве "
        "и не передает их третьим лицам.\n\n"
        "Используя CppForge, вы соглашаетесь с локальным хранением прогресса обучения и настроек профиля.";
    InfoDialog dlg("Конфиденциальность", privacyText, this);
    dlg.exec();
}