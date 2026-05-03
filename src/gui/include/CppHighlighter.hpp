#pragma once

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>

/**
 * @class CppHighlighter
 * @brief Syntax highlighter for C++ source code in QTextDocument.
 *
 * Provides basic keyword, comment, and string literal highlighting
 * with support for dynamic theme switching (Light/Dark).
 */
class CppHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new CppHighlighter instance.
     * @param parent The parent QTextDocument.
     */
    explicit CppHighlighter(QTextDocument *parent = nullptr);

    /**
     * @brief Updates the highlighting colors based on the current theme.
     * @param isDark True if the dark theme is active, false otherwise.
     */
    void setTheme(bool isDark);

protected:
    /**
     * @brief Core method for syntax highlighting. Called by Qt when blocks change.
     * @param text The text block to highlight.
     */
    void highlightBlock(const QString &text) override;

private:
    /**
     * @struct HighlightingRule
     * @brief Internal structure for regex-based highlighting.
     */
    struct HighlightingRule
    {
        QRegularExpression pattern; ///< Regex pattern.
        QTextCharFormat format;     ///< Format to apply on match.
    };
    QVector<HighlightingRule> highlightingRules; ///< Active rules.

    QTextCharFormat keywordFormat;      ///< Format for C++ keywords.
    QTextCharFormat classFormat;        ///< Format for class names.
    QTextCharFormat commentFormat;      ///< Format for comments.
    QTextCharFormat quotationFormat;    ///< Format for strings.
    QTextCharFormat functionFormat;     ///< Format for function calls.
    QTextCharFormat preprocessorFormat; ///< Format for preprocessor directives.
};