#ifndef MCSINSTALLER_H
#define MCSINSTALLER_H

#include "mcs.h"

class MCSInstaller : public MCS
{
public:
	MCSInstaller(QWidget *apParent = 0);

protected:
	void initDatabase();

protected:
	void extractSearchResult();
	void getDetailData();
};

#endif // MCSINSTALLER_H
