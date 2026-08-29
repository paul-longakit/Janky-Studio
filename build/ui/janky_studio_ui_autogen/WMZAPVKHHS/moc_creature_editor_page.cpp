/****************************************************************************
** Meta object code from reading C++ file 'creature_editor_page.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../ui/include/creature_studio/ui/pages/creature_editor_page.hpp"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'creature_editor_page.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_creature_studio__CreatureEditorPage_t {
    uint offsetsAndSizes[8];
    char stringdata0[36];
    char stringdata1[21];
    char stringdata2[1];
    char stringdata3[14];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_creature_studio__CreatureEditorPage_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_creature_studio__CreatureEditorPage_t qt_meta_stringdata_creature_studio__CreatureEditorPage = {
    {
        QT_MOC_LITERAL(0, 35),  // "creature_studio::CreatureEdit..."
        QT_MOC_LITERAL(36, 20),  // "stateEditorRequested"
        QT_MOC_LITERAL(57, 0),  // ""
        QT_MOC_LITERAL(58, 13)   // "backRequested"
    },
    "creature_studio::CreatureEditorPage",
    "stateEditorRequested",
    "",
    "backRequested"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_creature_studio__CreatureEditorPage[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   26,    2, 0x06,    1 /* Public */,
       3,    0,   27,    2, 0x06,    2 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject creature_studio::CreatureEditorPage::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_creature_studio__CreatureEditorPage.offsetsAndSizes,
    qt_meta_data_creature_studio__CreatureEditorPage,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_creature_studio__CreatureEditorPage_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CreatureEditorPage, std::true_type>,
        // method 'stateEditorRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'backRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void creature_studio::CreatureEditorPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CreatureEditorPage *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->stateEditorRequested(); break;
        case 1: _t->backRequested(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CreatureEditorPage::*)();
            if (_t _q_method = &CreatureEditorPage::stateEditorRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CreatureEditorPage::*)();
            if (_t _q_method = &CreatureEditorPage::backRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
    (void)_a;
}

const QMetaObject *creature_studio::CreatureEditorPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *creature_studio::CreatureEditorPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_creature_studio__CreatureEditorPage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int creature_studio::CreatureEditorPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void creature_studio::CreatureEditorPage::stateEditorRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void creature_studio::CreatureEditorPage::backRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
