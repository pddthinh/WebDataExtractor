#include "peterblackshaw.h"

peterblackshaw::peterblackshaw(QString url, QWidget *apParent)
	:real_estate(url, apParent)
{
}

void peterblackshaw::doWebLoadFinished()
{
	switch (meState)
	{
	case E_STATE_INIT:
		break;

	case E_STATE_GET_DETAIL_INFO:
		break;

	default:
		break;
	}
}

void peterblackshaw::parseData()
{

}

void peterblackshaw::parseOfficeContact()
{

}
