#ifndef MEDIAFIRE_H
#define MEDIAFIRE_H

#include "../WDELib/WDEInclude.h"

using namespace nsBaseWebEngine;

#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>

#define DEF_STR_DOWNLOAD_LINK_TEXT		"Click here to start download from MediaFire.."

class MediaFire : public BaseWebEngine
{
public:
	enum DOWN_MODE
	{
		E_MODE_FROM_MEDIA_FOLDER = 0,
		E_MODE_FROM_SAVED_FILE,
		E_MODE_MAX
	};

public:
	explicit MediaFire(QWidget *parent = 0);
	void setSavedLinkPath(QString astrPath);
	void startDownload(DOWN_MODE aeMode);

// virtual
protected:
	void doStart();
	void doWebLoadFinished();
	void doStop(){}
	void initDatabase(){}

protected:
	bool isCategory(QString astrURL);		// check is this Category of links

	bool loadLinkFromFile();
	void startDownloadFromFile();

	void initControlExtra();
	bool updateItemPerPage();
	void getSubPage();

	void getDownloadLink();

protected:
	QString			mstrSavedLinkPath;
	QStringList		mlstDownloadLink;
	DOWN_MODE		meDownMode;
};

#endif // MEDIAFIRE_H
