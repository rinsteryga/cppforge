#include "ProfilePage.hpp"

#include "../../core/include/services/UserService.hpp"

#include <QDebug>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVariant>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include <random>

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

    rightSection->addWidget(createBigBox("НАДПИСЬ"));
    rightSection->addSpacing(20);
    rightSection->addWidget(createBigBox("НАДПИСЬ"));

    auto *footer = new QLabel("О CppForge    Контакты\nКонфиденциальность");
    footer->setObjectName("FooterLinks");
    footer->setAlignment(Qt::AlignRight);
    rightSection->addWidget(footer);

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