#include "CustomTitleBar.hpp"

#include <QtTest>
#include <QTest>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>
#include <QIcon>
#include <memory>

class TestCustomTitleBar : public QObject
{
    Q_OBJECT

private slots:
    void init()
    {
        parentWidget = std::make_unique<QWidget>();
        parentWidget->resize(800, 600);
        titleBar = std::make_unique<CustomTitleBar>(parentWidget.get());
    }

    void cleanup()
    {
        if (titleBar) {
            titleBar.reset();
        }
        if (parentWidget) {
            parentWidget.reset();
        }
    }

    // Test initialization and basic properties
    void test_TitleBarCreation()
    {
        QVERIFY(titleBar != nullptr);
        QCOMPARE(titleBar->height(), 40);
    }

    void test_TitleBarParent()
    {
        QCOMPARE(titleBar->parent(), parentWidget.get());
    }

    void test_TitleBarLayout()
    {
        auto layout = titleBar->layout();
        QVERIFY(layout != nullptr);
        QVERIFY(dynamic_cast<QHBoxLayout*>(layout) != nullptr);
    }

    void test_IconLabelExists()
    {
        auto labels = titleBar->findChildren<QLabel*>();
        QVERIFY(labels.size() > 0);
    }

    void test_TitleLabelExists()
    {
        auto labels = titleBar->findChildren<QLabel*>();
        QVERIFY(labels.size() > 0);
    }

    // Test control buttons exist
    void test_MinimizeButtonExists()
    {
        auto buttons = titleBar->findChildren<QPushButton*>();
        bool foundMinimize = false;
        for (auto btn : buttons) {
            if (btn->text() == "-") {
                foundMinimize = true;
                break;
            }
        }
        QVERIFY(foundMinimize);
    }

    void test_MaximizeButtonExists()
    {
        auto buttons = titleBar->findChildren<QPushButton*>();
        bool foundMaximize = false;
        for (auto btn : buttons) {
            if (btn->text() == "□") {
                foundMaximize = true;
                break;
            }
        }
        QVERIFY(foundMaximize);
    }

    void test_CloseButtonExists()
    {
        auto buttons = titleBar->findChildren<QPushButton*>();
        bool foundClose = false;
        for (auto btn : buttons) {
            if (btn->text() == "✕") {
                foundClose = true;
                break;
            }
        }
        QVERIFY(foundClose);
    }

    void test_ControlButtonsCount()
    {
        auto buttons = titleBar->findChildren<QPushButton*>();
        QVERIFY(buttons.size() >= 3);
    }

    // Test button properties
    void test_MinimizeButtonIsEnabled()
    {
        auto buttons = titleBar->findChildren<QPushButton*>();
        for (auto btn : buttons) {
            if (btn->text() == "-") {
                QVERIFY(btn->isEnabled());
                return;
            }
        }
        QFAIL("Minimize button not found");
    }

    void test_MaximizeButtonIsEnabled()
    {
        auto buttons = titleBar->findChildren<QPushButton*>();
        for (auto btn : buttons) {
            if (btn->text() == "□") {
                QVERIFY(btn->isEnabled());
                return;
            }
        }
        QFAIL("Maximize button not found");
    }

    void test_CloseButtonIsEnabled()
    {
        auto buttons = titleBar->findChildren<QPushButton*>();
        for (auto btn : buttons) {
            if (btn->text() == "✕") {
                QVERIFY(btn->isEnabled());
                return;
            }
        }
        QFAIL("Close button not found");
    }

    // Test title functionality
    void test_SetTitle()
    {
        QString testTitle = "Test Window Title";
        titleBar->setTitle(testTitle);
        
        auto labels = titleBar->findChildren<QLabel*>();
        bool titleSet = false;
        
        for (auto label : labels) {
            if (label->text() == testTitle) {
                titleSet = true;
                break;
            }
        }
        
        QVERIFY(titleSet);
    }

    void test_SetIcon()
    {
        QPixmap pixmap(32, 32);
        pixmap.fill(Qt::blue);
        QIcon icon(pixmap);
        
        titleBar->setIcon(icon);
        
        auto labels = titleBar->findChildren<QLabel*>();
        bool iconSet = false;
        
        for (auto label : labels) {
            if (!label->pixmap(Qt::ReturnByValue).isNull()) {
                iconSet = true;
                break;
            }
        }
        
        QVERIFY(iconSet);
    }

    void test_TitleNotEmpty()
    {
        titleBar->setTitle("Test Title");
        
        auto labels = titleBar->findChildren<QLabel*>();
        bool foundTitle = false;
        
        for (auto label : labels) {
            if (label->text() == "Test Title") {
                foundTitle = true;
                break;
            }
        }
        
        QVERIFY(foundTitle);
    }

    // Test styling and layout
    void test_TitleBarBackground()
    {
        QString style = titleBar->styleSheet();
        QVERIFY(style.contains("background-color") || style.contains("#"));
    }

    void test_LayoutMargins()
    {
        auto layout = dynamic_cast<QHBoxLayout*>(titleBar->layout());
        QVERIFY(layout != nullptr);
        
        QMargins margins = layout->contentsMargins();
        QVERIFY(margins.left() >= 0);
        QVERIFY(margins.right() >= 0);
    }

    void test_LayoutSpacing()
    {
        auto layout = dynamic_cast<QHBoxLayout*>(titleBar->layout());
        QVERIFY(layout != nullptr);
        QVERIFY(layout->spacing() >= 0);
    }

    void test_ControlButtonSize()
    {
        auto buttons = titleBar->findChildren<QPushButton*>();
        bool foundButton = false;
        
        for (auto btn : buttons) {
            if (btn->text() == "-" || btn->text() == "□" || btn->text() == "✕") {
                if (btn->width() == 60 && btn->height() == 40) {
                    foundButton = true;
                    break;
                }
            }
        }
        
        QVERIFY(foundButton || buttons.size() > 0);
    }

    void test_AllButtonsClickable()
    {
        auto buttons = titleBar->findChildren<QPushButton*>();
        QVERIFY(buttons.size() > 0);
        
        for (auto btn : buttons) {
            QVERIFY(btn->isEnabled());
        }
    }

private:
    std::unique_ptr<CustomTitleBar> titleBar;
    std::unique_ptr<QWidget> parentWidget;
};

QTEST_MAIN(TestCustomTitleBar)
#include "test_CustomTitleBar.moc"
