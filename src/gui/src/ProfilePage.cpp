#include "ProfilePage.hpp"

#include "../../core/include/services/UserService.hpp"

#include <QDebug>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QImageReader>
#include <QKeySequence>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QShortcut>
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
                QLabel { color: #333; font-size: 16px; }
                #Title { font-weight: bold; font-size: 20px; color: #62639b; }
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
            contentLabel->setFixedWidth(450);
            cardLayout->addWidget(contentLabel);

            auto *closeBtn = new QPushButton("Got it");
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

    auto *secretShortcut = new QShortcut(QKeySequence("Alt+L, B"), this);
    connect(secretShortcut, &QShortcut::activated, this, &ProfilePage::secretTaskTriggered);
}

void ProfilePage::setUserData(uint64_t userId, const QString &name, const QString &avatarPath)
{
    currentUserId = userId;

    if (userNameLabel)
    {
        userNameLabel->setText(name);
    }

    QString finalPath = avatarPath;

    if (finalPath.isEmpty() || finalPath == "NULL")
    {
        QDir imagesDir(":/images");
        qDebug() << "Available resource images:" << imagesDir.entryList(QDir::Files);
        QStringList filters;
        filters << "default_avatar*.png";
        QStringList availableAvatars = imagesDir.entryList(filters, QDir::Files);

        if (!availableAvatars.isEmpty())
        {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, availableAvatars.size() - 1);
            finalPath = ":/images/" + availableAvatars[dis(gen)];
        }
        else
        {
            finalPath = ":/images/default_avatar1.png";
        }

        qDebug() << "Assigned random avatar from resources:" << finalPath;

        if (userService_)
        {
            userService_->updateAvatar(userId, finalPath);
        }
    }

    QPixmap pix(finalPath);
    if (pix.isNull())
    {
        qDebug() << "[Avatar] Failed to load from:" << finalPath;
        avatarLabel->setText("👤");
        avatarLabel->setAlignment(Qt::AlignCenter);
        avatarLabel->setStyleSheet("background-color: #EEE; font-size: 80px; border-radius: 20px;");
    }
    else
    {
        avatarLabel->setPixmap(pix.scaled(200, 200, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        avatarLabel->setStyleSheet("border-radius: 20px;");
    }

    if (userService_)
    {
        solvedTasksLabel->setText(QString::number(userService_->getSolvedTasksCount(userId)));
        completedLessonsLabel->setText(QString::number(userService_->getCompletedLessonsCount(userId)));
        achievementsLabel->setText(QString::number(userService_->getAchievementsCount(userId)));
        totalSubmissionsLabel->setText(QString::number(userService_->getTotalSubmissionsCount(userId)));
        streakLabel->setText(QString("🔥 %1").arg(userService_->getStreak(userId)));

        auto userOpt = userService_->findById(userId);
        if (userOpt)
        {
            duelPointsLabel->setText(QString::number(userOpt->getDuelPoints()));
            int total = userOpt->getDuelWins() + userOpt->getDuelLosses();
            if (total > 0)
            {
                double wr = (static_cast<double>(userOpt->getDuelWins()) / total) * 100.0;
                winRateLabel->setText(QString("%1%").arg(wr, 0, 'f', 1));
            }
            else
            {
                winRateLabel->setText("0%");
            }
        }

        auto clearLayout = [](QLayout *layout)
        {
            if (!layout)
            {
                return;
            }
            while (auto item = layout->takeAt(0))
            {
                if (item->widget())
                {
                    delete item->widget();
                }
                delete item;
            }
        };

        clearLayout(achievementsContainer->layout());
        clearLayout(activityContainer->layout());

        auto achievements = userService_->getAllAchievementsStatus(userId);
        auto *achGrid = qobject_cast<QGridLayout *>(achievementsContainer->layout());

        int row = 0;
        int col = 0;
        for (const auto &ach : achievements)
        {
            auto *achWidget = new QWidget();
            auto *v = new QVBoxLayout(achWidget);
            v->setContentsMargins(5, 8, 5, 8);
            v->setSpacing(8);

            auto *icon = new QLabel();
            icon->setFixedSize(65, 65);
            icon->setAlignment(Qt::AlignCenter);

            bool earned = ach.getDateEarned().time_since_epoch().count() > 0;
            QString iconPath = ach.getIconPath().trimmed();
            qDebug() << "[Achievement] Attempting to load icon for" << ach.getName() << "path: [" << iconPath << "]";
            QPixmap pix(iconPath);

            if (pix.isNull())
            {
                qDebug() << "[Achievement] Icon NOT FOUND for" << ach.getName() << "at path:" << iconPath;
                icon->setText("🏆");
                icon->setStyleSheet(earned ? "background: #eee; border-radius: 8px; font-size: 24px;"
                                           : "background: #f9f9f9; border-radius: 8px; font-size: 24px; color: "
                                             "#ccc; border: 1px dashed #ddd;");
            }
            else
            {
                icon->setStyleSheet(earned ? "background: #f0f0f0; border-radius: 8px;"
                                           : "background: #f9f9f9; border-radius: 8px; border: 1px dashed #ddd;");
                if (!earned)
                {
                    QImage img = pix.toImage().convertToFormat(QImage::Format_Grayscale8);
                    icon->setPixmap(
                        QPixmap::fromImage(img).scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                }
                else
                {
                    icon->setPixmap(pix.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                }
            }

            v->addWidget(icon, 0, Qt::AlignCenter);
            auto *name = new QLabel(ach.getName());
            name->setAlignment(Qt::AlignCenter);
            name->setStyleSheet(earned ? "font-size: 10px; font-weight: bold; color: #333; background: transparent;"
                                       : "font-size: 10px; color: #777; background: transparent;");
            name->setWordWrap(true);
            name->setContentsMargins(0, 0, 0, 0);
            v->addWidget(name, 0, Qt::AlignCenter);
            v->addStretch();

            achWidget->setToolTip(ach.getName() + (earned ? "" : " (Locked)") + ": " + ach.getDescription());

            if (achGrid)
            {
                achGrid->addWidget(achWidget, row, col);
                col++;
                if (col >= 4)
                {
                    col = 0;
                    row++;
                }
            }
        }

        auto activities = userService_->getRecentActivity(userId, 3);
        auto *actLayout = activityContainer->layout();
        for (const auto &act : activities)
        {
            auto *rowWidget = new QWidget();
            auto *h = new QHBoxLayout(rowWidget);
            h->setContentsMargins(0, 5, 0, 5);

            auto *typeTag = new QLabel(act.type);
            typeTag->setStyleSheet(
                act.type == "Lesson"
                    ? "background: #e3f2fd; color: #1976d2; padding: 2px 8px; border-radius: 10px; font-size: 11px;"
                    : "background: #e8f5e9; color: #388e3c; padding: 2px 8px; border-radius: 10px; font-size: 11px;");

            auto *title = new QLabel(act.title);
            title->setStyleSheet("font-weight: 500;");

            auto *date = new QLabel(act.date);
            date->setStyleSheet("color: #888; font-size: 12px;");

            h->addWidget(typeTag);
            h->addWidget(title, 1);
            h->addWidget(date);
            actLayout->addWidget(rowWidget);
        }
        if (activities.empty())
        {
            actLayout->addWidget(new QLabel("No activity yet..."));
        }
    }
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

    userNameLabel = new QLabel("Loading...");
    userNameLabel->setObjectName("UserNameLabel");

    auto *changeAvatarBtn = new QPushButton("Change Avatar");
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

    auto *statTitle = new QLabel("Statistics");
    statTitle->setObjectName("StatTitle");
    leftSection->addWidget(statTitle);

    auto *statsGrid = new QGridLayout();
    statsGrid->setSpacing(20);

    auto createStatCard = [this, statsGrid](int index, const QString &title, QLabel **valueLabel)
    {
        auto *card = new QFrame();
        card->setObjectName("StatCard");
        card->setMinimumSize(220, 110);
        auto *cLayout = new QVBoxLayout(card);
        auto *t = new QLabel(title);
        *valueLabel = new QLabel("0");
        (*valueLabel)->setObjectName("GreenValue");
        cLayout->addWidget(t);
        cLayout->addWidget(*valueLabel);
        statsGrid->addWidget(card, index / 2, index % 2);
    };

    createStatCard(0, "Tasks Solved:", &solvedTasksLabel);
    createStatCard(1, "Lessons Completed:", &completedLessonsLabel);
    createStatCard(2, "Achievements:", &achievementsLabel);
    createStatCard(3, "Total Attempts:", &totalSubmissionsLabel);
    createStatCard(4, "Duel Points:", &duelPointsLabel);
    createStatCard(5, "Win Rate:", &winRateLabel);

    leftSection->addLayout(statsGrid);
    leftSection->addStretch();

    auto *rightSection = new QVBoxLayout();

    auto *streakLayout = new QHBoxLayout();
    streakLayout->addStretch();
    streakLabel = new QLabel("🔥 0");
    streakLabel->setObjectName("StreakLabel");
    streakLayout->addWidget(streakLabel);
    rightSection->addLayout(streakLayout);

    auto createBigBox = [this](const QString &titleText, QWidget **container)
    {
        auto *box = new QFrame();
        box->setObjectName("BigBlock");
        auto *v = new QVBoxLayout(box);
        v->setContentsMargins(0, 0, 0, 0);
        auto *h = new QLabel(" " + titleText);
        h->setProperty("class", "box-header");
        v->addWidget(h);

        auto *content = new QWidget();
        QLayout *contentLayout = nullptr;
        if (titleText == "ACHIEVEMENTS")
        {
            contentLayout = new QGridLayout(content);
        }
        else
        {
            contentLayout = new QVBoxLayout(content);
        }
        contentLayout->setContentsMargins(15, 10, 15, 10);
        v->addWidget(content);
        *container = content;

        return box;
    };

    rightSection->addWidget(createBigBox("ACHIEVEMENTS", &achievementsContainer));
    rightSection->addSpacing(20);
    rightSection->addWidget(createBigBox("RECENT ACTIVITY", &activityContainer));

    auto *footerLayout = new QHBoxLayout();
    footerLayout->setSpacing(10);
    footerLayout->addStretch();

    auto createFooterBtn = [this](const QString &text, auto slot)
    {
        auto *btn = new QPushButton(text);
        btn->setObjectName("FooterBtn");
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, slot);
        return btn;
    };

    auto createSeparator = []()
    {
        auto *sep = new QLabel("•");
        sep->setStyleSheet("color: #BBB; font-size: 14px;");
        return sep;
    };

    footerLayout->addWidget(createFooterBtn("About cppforge", &ProfilePage::onAboutClicked));
    footerLayout->addWidget(createSeparator());
    footerLayout->addWidget(createFooterBtn("Contacts", &ProfilePage::onContactsClicked));
    footerLayout->addWidget(createSeparator());
    footerLayout->addWidget(createFooterBtn("Privacy Policy", &ProfilePage::onPrivacyClicked));

    rightSection->addSpacing(30);
    rightSection->addLayout(footerLayout);
    rightSection->addStretch();

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
            font-size: 14px;
            padding: 2px 5px;
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
    QStringList formats;
    for (const QByteArray &format : QImageReader::supportedImageFormats())
    {
        formats << "*." + QString(format).toLower();
    }
    QString filter = QString("Images (%1)").arg(formats.join(" "));

    QString fileName = QFileDialog::getOpenFileName(this, "Select Avatar", "", filter);
    if (!fileName.isEmpty())
    {
        QPixmap pix(fileName);
        if (!pix.isNull())
        {
            avatarLabel->setPixmap(pix.scaled(200, 200, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

            if (userService_)
            {
                userService_->updateAvatar(currentUserId, fileName);
            }
        }
    }
}

void ProfilePage::onAboutClicked()
{
    QString aboutText =
        "cppforge is an educational platform for learning C and C++ programming languages. "
        "Our course takes you from operating system basics to system programming and modern C++ standards, "
        "including PvP mode and practical tasks.\n\n"
        "The development team consists of RANEPA college graduates who brought the idea of a truly "
        "cool learning platform to life.";
    InfoDialog dlg("About cppforge", aboutText, this);
    dlg.exec();
}

void ProfilePage::onContactsClicked()
{
    InfoDialog dlg("Contacts", "rinsterr@yandex.ru — for all questions and suggestions", this);
    dlg.exec();
}

void ProfilePage::onPrivacyClicked()
{
    QString privacyText =
        "We value your privacy. The application stores your data exclusively locally on your device "
        "and does not transfer it to third parties.\n\n"
        "By using cppforge, you agree to the local storage of learning progress and profile settings.";
    InfoDialog dlg("Privacy Policy", privacyText, this);
    dlg.exec();
}
