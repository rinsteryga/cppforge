#include "ProfilePage.hpp"

#include "../../core/include/services/UserService.hpp"

#include <QComboBox>
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
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QSettings>
#include <QShortcut>
#include <QTimer>
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
                    background-color: palette(base);
                    border: 2px solid palette(highlight);
                    border-radius: 15px;
                }
                QLabel { color: palette(text); font-size: 16px; }
                #Title { font-weight: bold; font-size: 20px; color: palette(highlight); }
                QPushButton {
                    background-color: palette(button);
                    color: palette(button-text);
                    border-radius: 8px;
                    padding: 8px 20px;
                    font-weight: bold;
                    border: none;
                }
                QPushButton:hover { background-color: palette(highlight); }
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
}

ProfilePage::~ProfilePage() = default;

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

        if (userService_)
        {
            userService_->updateAvatar(userId, finalPath);
        }
    }

    updateAvatarDisplay(finalPath);

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
            QPixmap pix(iconPath);

            if (pix.isNull())
            {
                icon->setText("🏆");
                icon->setStyleSheet(earned ? "background-color: palette(alternate-base); border: 1px solid "
                                             "palette(mid); border-radius: 8px; font-size: 24px;"
                                           : "background-color: palette(base); border: 1px dashed palette(mid); "
                                             "border-radius: 8px; font-size: 24px; color: palette(mid);");
            }
            else
            {
                icon->setStyleSheet(
                    earned ? "background-color: palette(alternate-base); border-radius: 8px;"
                           : "background-color: palette(base); border: 1px dashed palette(mid); border-radius: 8px;");
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
            name->setStyleSheet(
                earned ? "font-size: 10px; font-weight: bold; color: palette(text); background: transparent;"
                       : "font-size: 10px; color: palette(window-text); background: transparent;");
            name->setWordWrap(true);
            name->setContentsMargins(0, 0, 0, 0);
            v->addWidget(name, 0, Qt::AlignCenter);
            v->addStretch();

            icon->setAttribute(Qt::WA_TransparentForMouseEvents);
            name->setAttribute(Qt::WA_TransparentForMouseEvents);

            achWidget->setProperty("customTooltipText", ach.getName() + (earned ? "" : " (Locked)") + ":\n" + ach.getDescription());
            achWidget->installEventFilter(this);

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
            typeTag->setStyleSheet(act.type == "Lesson"
                                       ? "background-color: palette(highlight); color: palette(highlighted-text); "
                                         "padding: 2px 8px; border-radius: 10px; font-size: 11px;"
                                       : "background-color: palette(button); color: palette(button-text); padding: 2px "
                                         "8px; border-radius: 10px; font-size: 11px;");

            auto *title = new QLabel(act.title);
            title->setStyleSheet("font-weight: 500;");

            auto *date = new QLabel(act.date);
            date->setStyleSheet("color: palette(window-text); font-size: 12px;");

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
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent;");

    contentWidget_ = new QWidget();
    contentWidget_->setObjectName("contentWidget");
    contentWidget_->setStyleSheet("background: transparent;");
    auto *mainLayout = new QHBoxLayout(contentWidget_);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(40);

    auto *leftSection = new QVBoxLayout();
    leftSection->setSpacing(30);

    auto *headerLayout = new QHBoxLayout();

    avatarLabel = new QLabel();
    avatarLabel->setFixedSize(200, 200);
    avatarLabel->setObjectName("AvatarSquare");
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setStyleSheet("background-color: transparent;");

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
        content->setObjectName("BoxContent");
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

    auto *settingsContainer = new QWidget();
    settingsContainer->setObjectName("BoxContent");
    auto *settingsLayout = new QVBoxLayout(settingsContainer);
    settingsLayout->setContentsMargins(15, 15, 15, 15);
    settingsLayout->setSpacing(10);

    auto *themeLayout = new QHBoxLayout();
    auto *themeLabel = new QLabel("Theme:");
    themeLabel->setStyleSheet("font-size: 14px; color: palette(text);");
    themeCombo_ = new QComboBox();
    themeCombo_->addItems({"Light", "Dark"});
    themeCombo_->setCursor(Qt::PointingHandCursor);
    themeLayout->addWidget(themeLabel);
    themeLayout->addWidget(themeCombo_);
    themeLayout->addStretch();

    settingsLayout->addLayout(themeLayout);
    settingsLayout->addStretch();

    auto *settingsBox = new QFrame();
    settingsBox->setObjectName("BigBlock");
    auto *boxLayout = new QVBoxLayout(settingsBox);
    boxLayout->setContentsMargins(0, 0, 0, 0);
    boxLayout->setSpacing(0);

    auto *headerLabel = new QLabel("APP SETTINGS");
    headerLabel->setProperty("class", "box-header");
    headerLabel->setAlignment(Qt::AlignCenter);
    boxLayout->addWidget(headerLabel);
    boxLayout->addWidget(settingsContainer);

    rightSection->addWidget(settingsBox);
    rightSection->addSpacing(20);

    rightSection->addWidget(createBigBox("RECENT ACTIVITY", &activityContainer));

    connect(themeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ProfilePage::onThemeChanged);

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

    scrollArea->setWidget(contentWidget_);
    rootLayout->addWidget(scrollArea);

    connect(changeAvatarBtn, &QPushButton::clicked, this, &ProfilePage::onChangeAvatarClicked);

    customTooltipLabel_ = new QLabel(this);
    customTooltipLabel_->setObjectName("CustomTooltip");
    customTooltipLabel_->hide();
}

void ProfilePage::applyStyles()
{
    bool isDark = false;
    if (themeService_)
    {
        isDark = (themeService_->getCurrentTheme() == cppforge::services::Theme::Dark);
    }
    else
    {
        QSettings settings("CppForge", "StudyApp");
        isDark = (settings.value("app/theme", 0).toInt() == 1);
    }

    QString hoverText;
    QString btnColor = isDark ? "#0e639c" : "#62639b";
    QString btnHover = isDark ? "#1177bb" : "#f3e8ff";
    hoverText = isDark ? "white" : "black";

    QString style = QString(R"(
        #contentWidget { background-color: palette(window); border: none; }
        #UserNameLabel { font-size: 28px; font-weight: bold; color: palette(text); }
        #StatTitle { font-size: 22px; font-weight: bold; font-style: italic; margin-top: 10px; }
        #AvatarSquare { 
            background-color: palette(alternate-base); 
            border: 2px solid palette(mid);
            border-radius: 100px;
        }
        #StatCard, #BigBlock { 
            background-color: palette(base); 
            border: 2px solid;
            border-color: palette(mid); 
            border-radius: 20px; 
        }
        .box-header { 
            background-color: palette(alternate-base); 
            padding: 5px; 
            font-weight: bold; 
            color: palette(window-text); 
            border-bottom: 1px solid;
            border-bottom-color: palette(mid);
            border-top-left-radius: 18px;
            border-top-right-radius: 18px;
        }
        #GreenValue { color: #4CAF50; font-size: 24px; font-weight: bold; }
        #GreenValue:hover { color: #4CAF50; }
        #StreakLabel { font-size: 26px; font-weight: bold; }
        #FooterLinks { color: palette(window-text); font-size: 13px; line-height: 1.5; }
        #FooterBtn {
            background: transparent;
            border: none;
            color: palette(window-text);
            font-size: 14px;
            padding: 2px 5px;
            text-align: right;
        }
        #FooterBtn:hover { color: %2; text-decoration: underline; background: transparent; }
        #ChangeAvatarBtn {
            background-color: %1; 
            color: white; 
            border: none;
            border-radius: 8px; 
            padding: 8px 16px; 
            font-weight: 600;
        }
        #ChangeAvatarBtn:hover { background-color: %2; color: %3; }
        #BoxContent { 
            background: transparent; 
            border: none; 
            border-bottom-left-radius: 18px; 
            border-bottom-right-radius: 18px; 
        }
        
        QComboBox {
            background-color: palette(base);
            border: 1px solid palette(mid);
            border-radius: 8px;
            padding: 5px 15px;
            min-width: 100px;
            color: palette(text);
        }
        QComboBox:hover { border-color: %1; }
        QComboBox::drop-down { border: none; width: 30px; }
        QComboBox::down-arrow { image: none; border-left: 5px solid transparent; border-right: 5px solid transparent; border-top: 5px solid palette(text); margin-right: 10px; }
        QComboBox QAbstractItemView {
            background-color: palette(base);
            border: 1px solid palette(mid);
            selection-background-color: %1;
            color: white;
        }

        #CustomTooltip {
            background-color: rgb(255, 255, 255);
            color: rgb(0, 0, 0);
            border: 1px solid rgb(150, 150, 150);
            padding: 6px;
            border-radius: 4px;
            font-size: 12px;
        }
    )")
                        .arg(btnColor)
                        .arg(btnHover)
                        .arg(hoverText);

    setStyleSheet(style);
    if (contentWidget_)
        contentWidget_->setStyleSheet(style);
}

void ProfilePage::setThemeService(cppforge::services::ThemeService *service)
{
    themeService_ = service;
    if (themeService_)
    {
        if (themeCombo_)
        {
            themeCombo_->blockSignals(true);
            themeCombo_->setCurrentIndex(themeService_->getCurrentTheme() == cppforge::services::Theme::Dark ? 1 : 0);
            themeCombo_->blockSignals(false);
        }
        connect(themeService_, &cppforge::services::ThemeService::themeChanged, this, &ProfilePage::applyStyles);
    }
}

void ProfilePage::onThemeChanged(int index)
{
    if (themeService_)
    {
        themeService_->setTheme(index == 1 ? cppforge::services::Theme::Dark : cppforge::services::Theme::Light);
    }
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
        updateAvatarDisplay(fileName);

        if (userService_)
        {
            userService_->updateAvatar(currentUserId, fileName);
        }
        emit avatarChanged(fileName);
    }
}

void ProfilePage::updateAvatarDisplay(const QString &path)
{
    if (!avatarLabel)
        return;

    QPixmap pix(path);
    if (pix.isNull())
    {
        avatarLabel->setText("👤");
        avatarLabel->setAlignment(Qt::AlignCenter);
        avatarLabel->setStyleSheet("background-color: palette(button); border-radius: 100px;");
    }
    else
    {
        QPixmap rounded(200, 200);
        rounded.fill(Qt::transparent);
        QPainter painter(&rounded);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        QPainterPath clipPath;
        clipPath.addEllipse(0, 0, 200, 200);
        painter.setClipPath(clipPath);
        painter.drawPixmap(0, 0, pix.scaled(200, 200, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        avatarLabel->setPixmap(rounded);
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

bool ProfilePage::eventFilter(QObject *watched, QEvent *event)
{
    if (!customTooltipLabel_)
        return QWidget::eventFilter(watched, event);

    if (event->type() == QEvent::Enter)
    {
        QString text = watched->property("customTooltipText").toString();
        if (!text.isEmpty())
        {
            customTooltipLabel_->setText(text);
            customTooltipLabel_->adjustSize();
            customTooltipLabel_->raise();
            
            QWidget *w = qobject_cast<QWidget*>(watched);
            if (w) {
                QPoint pos = w->mapTo(this, QPoint(0, w->height() + 5));
                if (pos.x() + customTooltipLabel_->width() > this->width()) {
                    pos.setX(this->width() - customTooltipLabel_->width() - 5);
                }
                if (pos.y() + customTooltipLabel_->height() > this->height()) {
                    pos.setY(w->mapTo(this, QPoint(0, -customTooltipLabel_->height() - 5)).y());
                }
                customTooltipLabel_->move(pos);
            }
            customTooltipLabel_->show();
        }
    }
    else if (event->type() == QEvent::Leave)
    {
        customTooltipLabel_->hide();
    }
    
    return QWidget::eventFilter(watched, event);
}
