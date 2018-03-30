#include "cookie.h"
#include <QDebug>
#include <QNetworkCookie>
#include <QDateTime>

#define CK_USERNAME_KEY		"hcfUser"
//#define CK_USERNAME_VAL		"64406903"
//64406903|pddthinh2@gmail.com
//#define CK_USERNAME_VAL		"64654613|c1835095@trbvm.com"
#define CK_USERNAME_VAL		"66360553|h391123@trbvm.com"

Cookie::Cookie(QObject *parent) :
	QNetworkCookieJar(parent) { }

/**
 * @brief Cookie::cookiesForUrl
 *	Set cookies for the request url
 * @param url
 * @return
 */
QList<QNetworkCookie> Cookie::cookiesForUrl(const QUrl &url) const {
	QList<QNetworkCookie> cookieList = QNetworkCookieJar::cookiesForUrl(url);

//	{
//		qDebug() << QLatin1String("Cookies size: ") << cookieList.size();

//		QNetworkCookie ck;
//		foreach (ck, cookieList) {
//			qDebug() << ck;
//		}

//		qDebug() << QLatin1String("--------------------------------------------");
//	}

	do {
		QString name, value;
		QNetworkCookie ck;
		bool blUpdated = false;
		foreach (ck, cookieList) {
			name = QString(ck.name());
			value = QString(ck.value());

			if (name == QLatin1String(CK_USERNAME_KEY)) {
				if (value != QLatin1String(CK_USERNAME_VAL)) {
					ck.setValue(QString(CK_USERNAME_VAL).toLocal8Bit());
					blUpdated = true;
				}

				break;
			}
		}

		if (blUpdated == true)
			break;

		// Add new cookie
		ck = QNetworkCookie(QString(CK_USERNAME_KEY).toLocal8Bit(),
						  QString(CK_USERNAME_VAL).toLocal8Bit());
		ck.setDomain(QLatin1String("www.homecheck.co.uk"));
		ck.setPath(QLatin1String("/"));

		QDateTime dt = QDateTime::currentDateTime();
		dt.addDays(100);
		ck.setExpirationDate(dt);
		cookieList << ck;
	} while (false);

	return cookieList;
}
