#ifndef architecture_com_H
#define architecture_com_H

#include "architecture_base.h"

class chartered_practice : public architecture_base
{
	Q_OBJECT

public:
	chartered_practice(QWidget *apParent = 0);

protected:
	void doStart();
	void doWebLoadFinished();

protected:
	void searchFirm();
	void getFirmDetailLink();
};

#endif // architecture_com_H
