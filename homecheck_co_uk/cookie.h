#ifndef COOKIE_H
#define COOKIE_H

#include <QNetworkCookieJar>
class QNetworkCookie;

class Cookie : public QNetworkCookieJar
{
	Q_OBJECT
public:
	explicit Cookie(QObject *parent = 0);

	QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const;
};

#endif // COOKIE_H
