#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPoint>

class CustomTitleBar : public QWidget {
    Q_OBJECT

public:
    explicit CustomTitleBar(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setIcon(const QIcon &icon);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    void onMinimizeClicked();
    void onMaximizeRestoreClicked();
    void onCloseClicked();

private:
    void setupUI();
    void updateMaximizeRestoreIcon();

    QLabel *iconLabel_;
    QLabel *titleLabel_;
    QPushButton *minimizeButton_;
    QPushButton *maximizeRestoreButton_;
    QPushButton *closeButton_;
    QHBoxLayout *layout_;

    QPoint dragPosition_;
    bool isMaximized_ = false;
};
