#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <map>
#include <QMainWindow>
#include <windows.h>
#include <QScreen>
#include <QSize>
#include <QPainter>
#include <QPixmap>
#include <QPainterPath>
#include <QPushButton>
#include <QEvent>
#include <QByteArray>
#include <QMouseEvent>
#include <QToolButton>
#include <QComboBox>
#include "Static/visitor_drawvisitor.h"
#define WeaponTypesCount 4
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
struct wepData {
    HINSTANCE hDll;
    Weapon *weap;
    int number;
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Soldier soldier;
    QPixmap pixmap;
    void setPixmapPosition(int x, int y);
    void setPixmap(const QPixmap &newPixmap);
    void setLibraries(const vector<pair<HINSTANCE, QString>> &libs);
    vector<pair<HINSTANCE, QString>> getLibraries();
    bool checkLibraries(vector<pair<HINSTANCE, QString>> &libs);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

    void doAfterAction();

    void doAction();

    void on_pushButton_Buy_clicked();

    void on_pushButton_Throw_clicked();

    void on_comboBox_WeaponList_currentIndexChanged(int index);

    void on_pushButton_start_clicked();

    void drawTarget();

    void stopShooting();

    bool checkActionAttack();

    void accepting(QString action, int index);

    void onButtonClicked();

    void buttonsClear();

    void SetButtonsEnabled();

    void SetButtonsVisible();

    void allow_attack();

private:
    Ui::MainWindow *ui;
    int const iconSize = 150;
    QPushButton button;
    map<QToolButton*, QString> buttonsImgs;
    vector<QToolButton*> buttons;
    vector<pair<HINSTANCE, QString>> libraries;
    vector<HINSTANCE> librs;
    typedef Weapon* (*CreateInstance)(int);
    map<int, pair<HINSTANCE, CreateInstance>> wepMap;
    vector<wepData> weapon_vect;
    QComboBox *comboBox;
    map<QString, QString> map_verify;
    QString generateSignature(const QString filePath);
    bool verifySignature(const QString filePath, QString expectedSignature);
};
#endif // MAINWINDOW_H
