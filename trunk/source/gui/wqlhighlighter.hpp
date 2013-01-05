#ifndef _WMIVIEWER_WQLHIGHLIGHTER_HPP_
#define _WMIVIEWER_WQLHIGHLIGHTER_HPP_

#include <QtGui>

class WqlHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
	public:
		WqlHighlighter(QObject* parent = 0);
		virtual ~WqlHighlighter();
		
		virtual void highlightBlock(const QString& text);
		
	private:
		void highlight(const QRegExp& exp, const QTextCharFormat& fmt, const QString& text);
		Q_DISABLE_COPY(WqlHighlighter);
};

#endif
