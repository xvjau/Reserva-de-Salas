/****************************************************************************
** Meta object code from reading C++ file 'CNotification.h'
**
** Created: Sat Apr 29 18:34:26 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "CNotification.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CNotification.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_CNotification[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      27,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      44,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CNotification[] = {
    "CNotification\0\0event,count\0FBEvent(int,int)\0onTimer()\0"
};

const QMetaObject CNotification::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CNotification,
      qt_meta_data_CNotification, 0 }
};

const QMetaObject *CNotification::metaObject() const
{
    return &staticMetaObject;
}

void *CNotification::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CNotification))
	return static_cast<void*>(const_cast<CNotification*>(this));
    if (!strcmp(_clname, "IBPP::EventInterface"))
	return static_cast<IBPP::EventInterface*>(const_cast<CNotification*>(this));
    return QObject::qt_metacast(_clname);
}

int CNotification::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: FBEvent(*reinterpret_cast< int(*)>(_a[1]),*reinterpret_cast< int(*)>(_a[2])); break;
        case 1: onTimer(); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void CNotification::FBEvent(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
