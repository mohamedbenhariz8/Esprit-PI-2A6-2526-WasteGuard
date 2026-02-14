/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtCharts/qlineseries.h>
#include <QtCharts/qabstractbarseries.h>
#include <QtCharts/qvbarmodelmapper.h>
#include <QtCharts/qboxplotseries.h>
#include <QtCharts/qcandlestickseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qpieseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qboxplotseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qpieseries.h>
#include <QtCharts/qpieseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qxyseries.h>
#include <QtCharts/qxyseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qboxplotseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qpieseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCharts/qxyseries.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSMainWindowENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMainWindowENDCLASS = QtMocHelpers::stringData(
    "MainWindow",
    "on_btnNouveau_clicked",
    "",
    "on_btnAnnuler_Ajout_clicked",
    "on_btnModifier_clicked",
    "on_btnAnalyser_clicked",
    "on_btnSimulerBadge_clicked",
    "on_btnSupprimer_clicked",
    "on_cbProjetStats_currentIndexChanged",
    "arg1",
    "on_btnFichePaie_clicked",
    "on_btnClient_clicked",
    "on_btn_ajouter_client_clicked",
    "on_btn_modifier_client_clicked",
    "on_btn_annuler_client_clicked",
    "on_btnNouveau_client_clicked",
    "onClientAdded",
    "matricule",
    "nom",
    "email",
    "bacs",
    "score",
    "paiement",
    "onClientModified",
    "row",
    "on_btnSave_Add_clicked",
    "on_btnSave_Mod_clicked",
    "handleMaintEditClicked",
    "handleMaintDeleteClicked",
    "goAffichage",
    "goAjout",
    "goModification",
    "goStatistiques",
    "handleEditClicked",
    "handleDeleteClicked"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMainWindowENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  164,    2, 0x08,    1 /* Private */,
       3,    0,  165,    2, 0x08,    2 /* Private */,
       4,    0,  166,    2, 0x08,    3 /* Private */,
       5,    0,  167,    2, 0x08,    4 /* Private */,
       6,    0,  168,    2, 0x08,    5 /* Private */,
       7,    0,  169,    2, 0x08,    6 /* Private */,
       8,    1,  170,    2, 0x08,    7 /* Private */,
      10,    0,  173,    2, 0x08,    9 /* Private */,
      11,    0,  174,    2, 0x08,   10 /* Private */,
      12,    0,  175,    2, 0x08,   11 /* Private */,
      13,    0,  176,    2, 0x08,   12 /* Private */,
      14,    0,  177,    2, 0x08,   13 /* Private */,
      15,    0,  178,    2, 0x08,   14 /* Private */,
      16,    6,  179,    2, 0x08,   15 /* Private */,
      23,    7,  192,    2, 0x08,   22 /* Private */,
      25,    0,  207,    2, 0x08,   30 /* Private */,
      26,    0,  208,    2, 0x08,   31 /* Private */,
      27,    0,  209,    2, 0x08,   32 /* Private */,
      28,    0,  210,    2, 0x08,   33 /* Private */,
      29,    0,  211,    2, 0x08,   34 /* Private */,
      30,    0,  212,    2, 0x08,   35 /* Private */,
      31,    0,  213,    2, 0x08,   36 /* Private */,
      32,    0,  214,    2, 0x08,   37 /* Private */,
      33,    0,  215,    2, 0x08,   38 /* Private */,
      34,    0,  216,    2, 0x08,   39 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString,   17,   18,   19,   20,   21,   22,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString,   24,   17,   18,   19,   20,   21,   22,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSMainWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMainWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMainWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'on_btnNouveau_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnAnnuler_Ajout_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnModifier_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnAnalyser_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnSimulerBadge_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnSupprimer_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_cbProjetStats_currentIndexChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'on_btnFichePaie_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnClient_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btn_ajouter_client_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btn_modifier_client_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btn_annuler_client_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnNouveau_client_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onClientAdded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'onClientModified'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'on_btnSave_Add_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnSave_Mod_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleMaintEditClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleMaintDeleteClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'goAffichage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'goAjout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'goModification'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'goStatistiques'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleEditClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleDeleteClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_btnNouveau_clicked(); break;
        case 1: _t->on_btnAnnuler_Ajout_clicked(); break;
        case 2: _t->on_btnModifier_clicked(); break;
        case 3: _t->on_btnAnalyser_clicked(); break;
        case 4: _t->on_btnSimulerBadge_clicked(); break;
        case 5: _t->on_btnSupprimer_clicked(); break;
        case 6: _t->on_cbProjetStats_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->on_btnFichePaie_clicked(); break;
        case 8: _t->on_btnClient_clicked(); break;
        case 9: _t->on_btn_ajouter_client_clicked(); break;
        case 10: _t->on_btn_modifier_client_clicked(); break;
        case 11: _t->on_btn_annuler_client_clicked(); break;
        case 12: _t->on_btnNouveau_client_clicked(); break;
        case 13: _t->onClientAdded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[6]))); break;
        case 14: _t->onClientModified((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[6])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[7]))); break;
        case 15: _t->on_btnSave_Add_clicked(); break;
        case 16: _t->on_btnSave_Mod_clicked(); break;
        case 17: _t->handleMaintEditClicked(); break;
        case 18: _t->handleMaintDeleteClicked(); break;
        case 19: _t->goAffichage(); break;
        case 20: _t->goAjout(); break;
        case 21: _t->goModification(); break;
        case 22: _t->goStatistiques(); break;
        case 23: _t->handleEditClicked(); break;
        case 24: _t->handleDeleteClicked(); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMainWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 25)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 25;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 25)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 25;
    }
    return _id;
}
QT_WARNING_POP
