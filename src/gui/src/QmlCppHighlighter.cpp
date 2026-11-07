#include "QmlCppHighlighter.hpp"

namespace cppforge::gui
{
    QmlCppHighlighter::QmlCppHighlighter(QObject *parent) : QObject(parent), m_highlighter(new CppHighlighter(this)) {}

    QmlCppHighlighter::~QmlCppHighlighter()
    {
        if (m_highlighter)
        {
            m_highlighter->setDocument(nullptr);
        }
    }

    void QmlCppHighlighter::setTextDocument(QQuickTextDocument *doc)
    {
        if (m_quickDocument == doc)
            return;

        if (m_quickDocument)
        {
            disconnect(m_quickDocument.data(), &QObject::destroyed, this, nullptr);
            if (m_highlighter)
            {
                m_highlighter->setDocument(nullptr);
            }
        }

        m_quickDocument = doc;
        if (m_quickDocument)
        {
            connect(m_quickDocument.data(), &QObject::destroyed, this,
                    [this]()
                    {
                        if (m_highlighter)
                        {
                            m_highlighter->setDocument(nullptr);
                        }
                        m_quickDocument = nullptr;
                    });

            if (m_quickDocument->textDocument())
            {
                if (!m_highlighter)
                {
                    m_highlighter = new CppHighlighter(this);
                }
                m_highlighter->setDocument(m_quickDocument->textDocument());
                m_highlighter->setTheme(m_isDark);
            }
        }

        emit textDocumentChanged();
    }

    void QmlCppHighlighter::setIsDark(bool dark)
    {
        if (m_isDark == dark)
            return;

        m_isDark = dark;
        if (m_highlighter)
        {
            m_highlighter->setTheme(m_isDark);
        }
        emit isDarkChanged();
    }
} // namespace cppforge::gui
