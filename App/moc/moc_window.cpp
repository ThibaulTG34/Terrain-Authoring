/****************************************************************************
** Meta object code from reading C++ file 'window.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../window.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Window_t {
    QByteArrayData data[19];
    char stringdata0[195];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Window_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Window_t qt_meta_stringdata_Window = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Window"
QT_MOC_LITERAL(1, 7, 12), // "AmplitudeMIN"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 1), // "v"
QT_MOC_LITERAL(4, 23, 12), // "AmplitudeMAX"
QT_MOC_LITERAL(5, 36, 11), // "initTexture"
QT_MOC_LITERAL(6, 48, 1), // "p"
QT_MOC_LITERAL(7, 50, 9), // "initRidge"
QT_MOC_LITERAL(8, 60, 9), // "initRiver"
QT_MOC_LITERAL(9, 70, 9), // "ChangeFPS"
QT_MOC_LITERAL(10, 80, 3), // "fps"
QT_MOC_LITERAL(11, 84, 10), // "ChangeReso"
QT_MOC_LITERAL(12, 95, 4), // "reso"
QT_MOC_LITERAL(13, 100, 18), // "UpdateAmplitudeMax"
QT_MOC_LITERAL(14, 119, 18), // "UpdateAmplitudeMin"
QT_MOC_LITERAL(15, 138, 16), // "UpdateDegrePente"
QT_MOC_LITERAL(16, 155, 13), // "UpdateDensite"
QT_MOC_LITERAL(17, 169, 12), // "UpdateAltMax"
QT_MOC_LITERAL(18, 182, 12) // "UpdateAltMin"

    },
    "Window\0AmplitudeMIN\0\0v\0AmplitudeMAX\0"
    "initTexture\0p\0initRidge\0initRiver\0"
    "ChangeFPS\0fps\0ChangeReso\0reso\0"
    "UpdateAmplitudeMax\0UpdateAmplitudeMin\0"
    "UpdateDegrePente\0UpdateDensite\0"
    "UpdateAltMax\0UpdateAltMin"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Window[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,
       4,    1,   82,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   85,    2, 0x0a /* Public */,
       7,    1,   88,    2, 0x0a /* Public */,
       8,    1,   91,    2, 0x0a /* Public */,
       9,    1,   94,    2, 0x0a /* Public */,
      11,    1,   97,    2, 0x0a /* Public */,
      13,    1,  100,    2, 0x0a /* Public */,
      14,    1,  103,    2, 0x0a /* Public */,
      15,    1,  106,    2, 0x0a /* Public */,
      16,    1,  109,    2, 0x0a /* Public */,
      17,    1,  112,    2, 0x0a /* Public */,
      18,    1,  115,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Float,    3,
    QMetaType::Void, QMetaType::Float,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QPixmap,    6,
    QMetaType::Void, QMetaType::QPixmap,    6,
    QMetaType::Void, QMetaType::QPixmap,    6,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Float,    3,
    QMetaType::Void, QMetaType::Float,    3,

       0        // eod
};

void Window::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Window *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->AmplitudeMIN((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 1: _t->AmplitudeMAX((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 2: _t->initTexture((*reinterpret_cast< QPixmap(*)>(_a[1]))); break;
        case 3: _t->initRidge((*reinterpret_cast< QPixmap(*)>(_a[1]))); break;
        case 4: _t->initRiver((*reinterpret_cast< QPixmap(*)>(_a[1]))); break;
        case 5: _t->ChangeFPS((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->ChangeReso((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->UpdateAmplitudeMax((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->UpdateAmplitudeMin((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->UpdateDegrePente((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->UpdateDensite((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->UpdateAltMax((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 12: _t->UpdateAltMin((*reinterpret_cast< float(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Window::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Window::AmplitudeMIN)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Window::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Window::AmplitudeMAX)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Window::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_Window.data,
    qt_meta_data_Window,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Window::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Window::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Window.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Window::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void Window::AmplitudeMIN(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Window::AmplitudeMAX(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
