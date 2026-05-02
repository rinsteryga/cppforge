#include "../include/CppHighlighter.hpp"

CppHighlighter::CppHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    setTheme(false);
}

void CppHighlighter::setTheme(bool isDark)
{
    highlightingRules.clear();

    keywordFormat.setForeground(isDark ? QColor(86, 156, 214) : QColor(0, 0, 255));
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b" << "\\bdouble\\b" << "\\benum\\b"
                    << "\\bexplicit\\b" << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b" << "\\blong\\b"
                    << "\\bnamespace\\b" << "\\boperator\\b" << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b" << "\\bunion\\b" << "\\bunsigned\\b"
                    << "\\bvirtual\\b" << "\\bvoid\\b" << "\\bvolatile\\b" << "\\bbool\\b" << "\\bif\\b" << "\\belse\\b"
                    << "\\bfor\\b" << "\\bwhile\\b" << "\\breturn\\b";

    for (const QString &pattern : keywordPatterns)
    {
        highlightingRules.append({QRegularExpression(pattern), keywordFormat});
    }

    preprocessorFormat.setForeground(isDark ? QColor(197, 134, 192) : QColor(128, 0, 128));
    highlightingRules.append({QRegularExpression("^\\s*#\\s*[a-zA-Z]+"), preprocessorFormat});

    quotationFormat.setForeground(isDark ? QColor(206, 145, 120) : QColor(163, 21, 21));
    highlightingRules.append({QRegularExpression("\".*\""), quotationFormat});

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(isDark ? QColor(220, 220, 170) : QColor(121, 94, 38));
    highlightingRules.append({QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()"), functionFormat});

    commentFormat.setForeground(isDark ? QColor(106, 153, 85) : QColor(0, 128, 0));
    highlightingRules.append({QRegularExpression("//.*"), commentFormat});
    highlightingRules.append({QRegularExpression("/\\*.*\\*/"), commentFormat});

    rehighlight();
}

void CppHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : highlightingRules)
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
