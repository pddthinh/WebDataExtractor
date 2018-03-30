#ifndef CHARTERED_MEMBER_H
#define CHARTERED_MEMBER_H

#include "architecture_base.h"

class chartered_member : public architecture_base
{
	Q_OBJECT

public:
	chartered_member(QWidget *apParent = 0);

protected:
	void doStart();
	void doWebLoadFinished();

private:
	void loadPostCode();
	void doSearch();
	void getDataLinkList();

	bool gotoNextPostCode();

private:
	int			mIdxPostCode;
	bool		mblPostCodeLoad;
	QStringList mlstPostCode;
};

#endif // CHARTERED_MEMBER_H
