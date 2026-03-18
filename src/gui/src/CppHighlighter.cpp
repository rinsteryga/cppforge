#include "../include/CppHighlighter.hpp"

CppHighlighter::CppHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    // Ключевые слова (синий или фиолетовый)
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bslots\\b"
                    << "\\bstatic\\b" << "\\bstruct\\b" << "\\btemplate\\b"
                    << "\\btypedef\\b" << "\\btypename\\b" << "\\bunion\\b"
                    << "\\bunsigned\\b" << "\\bvirtual\\b" << "\\bvoid\\b"
                    << "\\bvolatile\\b" << "\\bbool\\b" << "\\bif\\b"
                    << "\\belse\\b" << "\\bfor\\b" << "\\bwhile\\b" << "\\breturn\\b";

    for (const QString &pattern : keywordPatterns)
    {
        highlightingRules.append({QRegularExpression(pattern), keywordFormat});
    }

    // Препроцессор (#include и т.д. - темно-оранжевый)
    preprocessorFormat.setForeground(QColor(200, 100, 0));
    highlightingRules.append({QRegularExpression("^\\s*#\\s*[a-zA-Z]+"), preprocessorFormat});

    // Кавычки/Строки (красный)
    quotationFormat.setForeground(Qt::darkRed);
    highlightingRules.append({QRegularExpression("\".*\""), quotationFormat});

    // Функции (темно-синий курсив)
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    highlightingRules.append({QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()"), functionFormat});

    // Комментарии (серый)
    commentFormat.setForeground(Qt::darkGray);
    highlightingRules.append({QRegularExpression("//.*"), commentFormat});
    highlightingRules.append({QRegularExpression("/\\*.*\\*/"), commentFormat});
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