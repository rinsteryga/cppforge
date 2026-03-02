#include "SignUpWindow.hpp"
#include "../../src/core/include/services/AuthManager.hpp"
#include "../../src/core/include/repositories/IUserRepository.hpp"

#include <QtTest>
#include <QTest>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QSignalSpy>
#include <memory>

// Mock implementation of IUserRepository for testing
class MockUserRepositorySignUp : public cppforge::repositories::IUserRepository
{
public:
    MockUserRepositorySignUp() : userAlreadyExists_(false) {}

    std::optional<cppforge::entities::User> findByEmail(const QString& email) const override
    {
        if (userAlreadyExists_ && email == "existing@example.com") {
            return cppforge::entities::User(1, "testuser", email, "hash");
        }
        return std::nullopt;
    }

    std::optional<cppforge::entities::User> findByUsername(const QString& username) const override
    {
        if (userAlreadyExists_ && username == "existinguser") {
            return cppforge::entities::User(1, username, "test@example.com", "hash");
        }
        return std::nullopt;
    }

    bool save(cppforge::entities::User user) override
    {
        return !userAlreadyExists_;
    }

    void setUserAlreadyExists(bool exists) { userAlreadyExists_ = exists; }

private:
    bool userAlreadyExists_;
};

class TestSignUpWindow : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        authManager = std::make_shared<cppforge::services::AuthManager>(
            std::make_unique<MockUserRepositorySignUp>()
        );
    }

    void cleanupTestCase()
    {
    }

    void init()
    {
        signUpWindow = std::make_unique<SignUpWindow>(authManager);
    }

    void cleanup()
    {
        if (signUpWindow) {
            signUpWindow.reset();
        }
    }

    // Базовые тесты
    void test_WindowCreation()
    {
        QVERIFY(signUpWindow != nullptr);
        QCOMPARE(signUpWindow->size(), QSize(1280, 900));
    }

    void test_WindowTitle()
    {
        QCOMPARE(signUpWindow->windowTitle(), "Sign Up - cppforge");
    }

    void test_UIElementsExist()
    {
        auto lineEdits = signUpWindow->findChildren<QLineEdit*>();
        QVERIFY(lineEdits.size() >= 3); // username, email, password
        
        auto buttons = signUpWindow->findChildren<QPushButton*>();
        QVERIFY(buttons.size() > 0);
    }

    void test_InputFieldCount()
    {
        auto lineEdits = signUpWindow->findChildren<QLineEdit*>();
        QCOMPARE(lineEdits.size(), 3); // username, email, password
    }

    void test_InputFieldPlaceholders()
    {
        auto lineEdits = signUpWindow->findChildren<QLineEdit*>();
        QVERIFY(lineEdits.size() >= 3);

        QCOMPARE(lineEdits.at(0)->placeholderText(), "Username");
        QCOMPARE(lineEdits.at(1)->placeholderText(), "Email address");
        QCOMPARE(lineEdits.at(2)->placeholderText(), "Password");
    }

    void test_PasswordFieldEchoMode()
    {
        auto lineEdits = signUpWindow->findChildren<QLineEdit*>();
        if (lineEdits.size() >= 3) {
            QCOMPARE(lineEdits.at(2)->echoMode(), QLineEdit::Password);
        }
    }

    void test_SignUpButtonExists()
    {
        auto buttons = signUpWindow->findChildren<QPushButton*>();
        
        bool foundButton = false;
        for (auto btn : buttons) {
            if (btn->text() == "Create Account") {
                foundButton = true;
                break;
            }
        }
        
        QVERIFY(foundButton);
    }

    void test_BackToLoginButtonExists()
    {
        auto buttons = signUpWindow->findChildren<QPushButton*>();
        
        bool foundButton = false;
        for (auto btn : buttons) {
            if (btn->text().contains("Already have an account")) {
                foundButton = true;
                QVERIFY(btn->isFlat());
                break;
            }
        }
        
        QVERIFY(foundButton);
    }

    void test_AllInputFieldsHaveCorrectSize()
    {
        auto lineEdits = signUpWindow->findChildren<QLineEdit*>();
        QVERIFY(lineEdits.size() >= 3);

        for (int i = 0; i < 3; i++) {
            QCOMPARE(lineEdits.at(i)->height(), 65);
            QCOMPARE(lineEdits.at(i)->width(), 500);
        }
    }

    void test_InputsAreClearableAndSettable()
    {
        auto lineEdits = signUpWindow->findChildren<QLineEdit*>();
        
        lineEdits.at(0)->setText("testuser");
        QCOMPARE(lineEdits.at(0)->text(), "testuser");
        
        lineEdits.at(0)->clear();
        QCOMPARE(lineEdits.at(0)->text(), "");
    }

private:
    std::unique_ptr<SignUpWindow> signUpWindow;
    std::shared_ptr<cppforge::services::AuthManager> authManager;
};

QTEST_MAIN(TestSignUpWindow)
#include "test_SignUpWindow.moc"
