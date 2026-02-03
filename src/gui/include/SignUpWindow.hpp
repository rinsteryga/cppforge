#pragma once

#include <QtSql/QSqlDatabase>

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFont>
#include <QVBoxLayout>

#include <memory>

class SignUpWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SignUpWindow(QSqlDatabase& database, QWidget *parent = nullptr);
    explicit SignUpWindow(QWidget *parent = nullptr);

private:
    void onSignUpButtonClicked();
    void onCloseButtonClicked();

    void setupUI();

    void setupWindowProperties();

    void setupLabelFont(QLabel *label_);
    void setupButtonFont(QPushButton *button_);
    void setupEditFieldFont(QLineEdit *editField_);

    void setupMainLabel();
    void setupUsernameLabel();
    void setupEmailLabel();
    void setupPasswordLabel();
    void setupConfirmPasswordLabel();

    void setupSignUpButton();
    void setupCloseButton();

    void setupEditFields();

    void setupLayout();

    void setupConnections();

    QSqlDatabase* database_ = nullptr;

    std::unique_ptr<QLabel> mainLabel_;
    std::unique_ptr<QLabel> usernameLabel_;
    std::unique_ptr<QLabel> emailLabel_;
    std::unique_ptr<QLabel> passwordLabel_;
    std::unique_ptr<QLabel> confirmPasswordLabel_;

    std::unique_ptr<QFont> labelFont_;
    std::unique_ptr<QFont> editFieldFont_;
    std::unique_ptr<QFont> signUpButtonFont_;

    std::unique_ptr<QLineEdit> usernameInput_;
    std::unique_ptr<QLineEdit> emailInput_;
    std::unique_ptr<QLineEdit> passwordInput_;
    std::unique_ptr<QLineEdit> confirmPasswordInput_;
    
    std::unique_ptr<QPushButton> signUpButton_;
    std::unique_ptr<QPushButton> closeButton_;

    std::unique_ptr<QVBoxLayout> mainLayout_;
};