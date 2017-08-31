/****************************************************************************
** Meta object code from reading C++ file 'config_manager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "config_manager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'config_manager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_tremble__ConfigManager_t {
    QByteArrayData data[6];
    char stringdata0[68];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_tremble__ConfigManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_tremble__ConfigManager_t qt_meta_stringdata_tremble__ConfigManager = {
    {
QT_MOC_LITERAL(0, 0, 22), // "tremble::ConfigManager"
QT_MOC_LITERAL(1, 23, 6), // "Launch"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 8), // "Shutdown"
QT_MOC_LITERAL(4, 40, 21), // "FullScreenStateChange"
QT_MOC_LITERAL(5, 62, 5) // "state"

    },
    "tremble::ConfigManager\0Launch\0\0Shutdown\0"
    "FullScreenStateChange\0state"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_tremble__ConfigManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x0a /* Public */,
       3,    0,   30,    2, 0x0a /* Public */,
       4,    1,   31,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,

       0        // eod
};

void tremble::ConfigManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ConfigManager *_t = static_cast<ConfigManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Launch(); break;
        case 1: _t->Shutdown(); break;
        case 2: _t->FullScreenStateChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject tremble::ConfigManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_tremble__ConfigManager.data,
      qt_meta_data_tremble__ConfigManager,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *tremble::ConfigManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *tremble::ConfigManager::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_tremble__ConfigManager.stringdata0))
        return static_cast<void*>(const_cast< ConfigManager*>(this));
    return QObject::qt_metacast(_clname);
}

int tremble::ConfigManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
