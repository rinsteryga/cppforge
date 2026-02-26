#pragma once

#include <QWidget>
#include <QPropertyAnimation>
#include <memory>
#include <vector>

class QFrame;
class QLabel;
class QPushButton;
class QProgressBar;
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
class CustomTitleBar;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

public slots:
    void fadeIn();

private:
    void setupUI();
    void setupWindowProperties();
    void setupTitleBar();
    void setupLeftPanel();          
    void setupCenterPanel();        
    void setupRightPanel();     
    void setupStyles();
    void centerWindow();

    std::unique_ptr<CustomTitleBar> customTitleBar_;
    std::unique_ptr<QPropertyAnimation> transitionAnimation_;
    std::unique_ptr<QVBoxLayout> centerPanelLayout_;
    
    std::unique_ptr<QFrame> sideBar;
    std::unique_ptr<QFrame> eventCard;
    std::unique_ptr<QFrame> dailyTaskCard;
    std::unique_ptr<QScrollArea> modulesScrollArea;
    std::unique_ptr<QWidget> modulesContainer;
    std::unique_ptr<QVBoxLayout> modulesLayout;
    
    std::vector<std::unique_ptr<QFrame>> moduleCards;
    
    QList<QLabel*> moduleProgressLabels;
    QList<QProgressBar*> moduleProgressBars;
    QList<QPushButton*> moduleButtons;

    std::unique_ptr<QHBoxLayout> footerLinksLayout;
    QPushButton* aboutBtn;
    QPushButton* contactsBtn;
    QPushButton* privacyBtn;
};