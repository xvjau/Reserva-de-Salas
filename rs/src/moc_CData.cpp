/****************************************************************************
** Meta object code from reading C++ file 'CData.h'
**
** Created: Sat Apr 29 18:34:21 2006
**      by: The Qt Meta Object Compiler version 59 (Qt 4.1.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "CData.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CData.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.1.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_CData[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_CData[] = {
    "CData\0"
};

const QMetaObject CData::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CData,
      qt_meta_data_CData, 0 }
};

const QMetaObject *CData::metaObject() const
{
    return &staticMetaObject;
}

void *CData::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CData))
	return static_cast<void*>(const_cast<CData*>(this));
    return QObject::qt_metacast(_clname);
}

int CData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_CReservaList[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      19,   14,   13,   13, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_CReservaList[] = {
    "CReservaList\0\0_pos\0showReservaMenu(QPoint)\0"
};

const QMetaObject CReservaList::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_CReservaList,
      qt_meta_data_CReservaList, 0 }
};

const QMetaObject *CReservaList::metaObject() const
{
    return &staticMetaObject;
}

void *CReservaList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CReservaList))
	return static_cast<void*>(const_cast<CReservaList*>(this));
    return QFrame::qt_metacast(_clname);
}

int CReservaList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: showReservaMenu(*reinterpret_cast< const QPoint(*)>(_a[1])); break;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void CReservaList::showReservaMenu(const QPoint _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_CSemana[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      21,    9,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CSemana[] = {
    "CSemana\0\0event,count\0onFBEvent(int,int)\0"
};

const QMetaObject CSemana::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CSemana,
      qt_meta_data_CSemana, 0 }
};

const QMetaObject *CSemana::metaObject() const
{
    return &staticMetaObject;
}

void *CSemana::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CSemana))
	return static_cast<void*>(const_cast<CSemana*>(this));
    return QObject::qt_metacast(_clname);
}

int CSemana::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: onFBEvent(*reinterpret_cast< int(*)>(_a[1]),*reinterpret_cast< int(*)>(_a[2])); break;
        }
        _id -= 1;
    }
    return _id;
}
