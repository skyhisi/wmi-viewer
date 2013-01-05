#include "wqlhighlighter.hpp"

WqlHighlighter::WqlHighlighter(QObject* p) :
	QSyntaxHighlighter(p)
{}

WqlHighlighter::~WqlHighlighter()
{}



void WqlHighlighter::highlightBlock(const QString& text)
{
	QTextCharFormat keywordFormat;
	keywordFormat.setFontWeight(QFont::Bold);
	keywordFormat.setForeground(Qt::darkMagenta);
	QRegExp keywordExp("\\bAND|ASSOCIATORS|FROM|GROUP|HAVING|IS|ISA|KEYSONLY|LIKE|NOT|OF|OR|REFERENCES|SELECT|WHERE|WITHIN\\b");
	Q_ASSERT(keywordExp.isValid());
	
	QTextCharFormat constantFormat;
	constantFormat.setForeground(Qt::blue);
	QRegExp constantExp("\\bNULL|TRUE|FALSE|__CLASS\\b");
	Q_ASSERT(constantExp.isValid());
	
	// QTextCharFormat stringFormat;
	// stringFormat.setForeground(Qt::gray);
	// stringFormat.setFontWeight(QFont::Bold);
	// QRegExp stringExp("\\b\\0047([^\\0047]+)\\0047\\b");
	// Q_ASSERT(stringExp.isValid());
	

	highlight(keywordExp, keywordFormat, text);
	highlight(constantExp, constantFormat, text);
	// highlight(stringExp, stringFormat, text);
	
	// for (QString::const_iterator it = text.begin(); it != text.end(); ++it)
	// {
		// qDebug() << *it << it->unicode();
	// }
	
}

void WqlHighlighter::highlight(const QRegExp& exp, const QTextCharFormat& fmt, const QString& text)
{
	QRegExp e(exp);
	int index = text.indexOf(e);
	while (index >= 0)
	{
		int length = e.matchedLength();
		setFormat(index, length, fmt);
		index = text.indexOf(e, index + length);
	}
}

