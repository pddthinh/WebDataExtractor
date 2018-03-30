#ifndef WHITEPAGE_COM_H
#define WHITEPAGE_COM_H

#include "WDEInclude.h"

using namespace nsBaseWebEngine;
using namespace nsDatabase;

class whitepage_com : public BaseWebEngine
{
public:
	whitepage_com(QWidget *apParent = 0);

protected:
	void doStart();
	void doStop();
	void doWebLoadFinished();
	void initDatabase();
};

#endif // WHITEPAGE_COM_H
