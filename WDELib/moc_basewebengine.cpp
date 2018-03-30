/****************************************************************************
** Meta object code from reading C++ file 'basewebengine.h'
**
** Created: Sat Oct 23 22:33:08 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "basewebengine.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'basewebengine.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BaseWebEngine[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       4,   54, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x08,
      34,   14,   14,   14, 0x08,
      63,   52,   14,   14, 0x08,
      97,   92,   14,   14, 0x08,
     131,  120,   14,   14, 0x08,
     165,  155,   14,   14, 0x08,
     225,  190,   14,   14, 0x08,
     266,   14,   14,   14, 0x08,

 // properties: name, type, flags
     301,  290, 0x00095009,
     321,  311, 0x00095009,
     337,  329, 0x0a095003,
     345,  329, 0x0a095003,

       0        // eod
};

static const char qt_meta_stringdata_BaseWebEngine[] = {
    "BaseWebEngine\0\0sltBtnStartClick()\0"
    "sltBtnStopClick()\0astrStatus\0"
    "sltWebStatusChanged(QString)\0aURL\0"
    "sltWebURLChanged(QUrl)\0aiProgress\0"
    "sltWebLoadProgress(int)\0ablStatus\0"
    "sltWebLoadFinished(bool)\0"
    "astrLink,astrTitle,astrTextContent\0"
    "sltWebLinkHover(QString,QString,QString)\0"
    "sltTimerForceStopLoad()\0WebEngine*\0"
    "mpWebView\0ENU_STATE\0meState\0QString\0"
    "mstrURL\0mstrStatus\0"
};

const QMetaObject BaseWebEngine::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_BaseWebEngine,
      qt_meta_data_BaseWebEngine, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BaseWebEngine::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BaseWebEngine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BaseWebEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BaseWebEngine))
        return static_cast<void*>(const_cast< BaseWebEngine*>(this));
    return QWidget::qt_metacast(_clname);
}

int BaseWebEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sltBtnStartClick(); break;
        case 1: sltBtnStopClick(); break;
        case 2: sltWebStatusChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: sltWebURLChanged((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 4: sltWebLoadProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: sltWebLoadFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: sltWebLinkHover((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 7: sltTimerForceStopLoad(); break;
        default: ;
        }
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< WebEngine**>(_v) = getWebView(); break;
        case 1: *reinterpret_cast< ENU_STATE*>(_v) = getState(); break;
        case 2: *reinterpret_cast< QString*>(_v) = getURL(); break;
        case 3: *reinterpret_cast< QString*>(_v) = getStatus(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 2: setURL(*reinterpret_cast< QString*>(_v)); break;
        case 3: setStatus(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
