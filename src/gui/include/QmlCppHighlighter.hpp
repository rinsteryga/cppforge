#pragma once

#include "CppHighlighter.hpp"

#include <QObject>
#include <QPointer>
#include <QQuickTextDocument>
#include <QtQml/qqmlregistration.h>

namespace cppforge::gui
{
    /**
     * @class QmlCppHighlighter
     * @brief Bridges CppHighlighter to Qt Quick TextEdit/TextArea elements.
     */
    class QmlCppHighlighter : public QObject
    {
        Q_OBJECT
        QML_ELEMENT

        Q_PROPERTY(QQuickTextDocument *textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)
        Q_PROPERTY(bool isDark READ isDark WRITE setIsDark NOTIFY isDarkChanged)

    public:
        explicit QmlCppHighlighter(QObject *parent = nullptr);
        ~QmlCppHighlighter() override;

        QQuickTextDocument *textDocument() const { return m_quickDocument; }
        void setTextDocument(QQuickTextDocument *doc);

        bool isDark() const { return m_isDark; }
        void setIsDark(bool dark);

    signals:
        void textDocumentChanged();
        void isDarkChanged();

    private:
        QPointer<QQuickTextDocument> m_quickDocument;
        QPointer<CppHighlighter> m_highlighter;
        bool m_isDark = true;
    };
} // namespace cppforge::gui
