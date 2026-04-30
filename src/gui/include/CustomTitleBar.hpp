#pragma once

#include <QIcon>
#include <QPoint>
#include <QString>
#include <QWidget>

class QHBoxLayout;
class QLabel;
class QPushButton;
class QMouseEvent;

class CustomTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit CustomTitleBar(QWidget *parent = nullptr);
    ~CustomTitleBar() override = default;

    void setTitle(const QString &title);
    void setIcon(const QIcon &icon);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onMinimizeClicked();
    void onMaximizeRestoreClicked();
    void onCloseClicked();

private:
    void setupUI();

    QLabel *iconLabel_ = nullptr;
    QLabel *titleLabel_ = nullptr;
    QPushButton *minimizeButton_ = nullptr;
    QPushButton *maximizeRestoreButton_ = nullptr;
    QPushButton *closeButton_ = nullptr;
    QHBoxLayout *layout_ = nullptr;

    QPoint dragPosition_;
    bool isResizing_ = false;
};