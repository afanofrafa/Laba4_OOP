#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QScreen>
#include <QTimer>
#include <QCryptographicHash>
#include <QtMultimedia/QMediaPlayer>
#include <QFile>
Ui::MainWindow *u;
bool action;
QTimer *timerA;
QTimer *timerB;
QTimer *timerZ;
QTimer *timer_notAttack;
QTimer *timerStart;
QTimer *timer_time;
bool moveRight = true;
bool fl;
QString button_action;
QPointF widgetPos;
int pixmapX = 0;
int pixmapY = 0;

typedef void* (*CreateEncryption)();
typedef void (*DestroyEncryption)(void* instance);
typedef void (*CallSetKeyFunc)(void* instance, const QByteArray key);
typedef void (*CallSetFilePathFunc)(void* instance, const QString filePath);
typedef void (*CallEncryptFunc)(void* instance);
typedef void (*CallDecryptFunc)(void* instance);
typedef QByteArray (*CallEncryptTextFunc)(void* instance, QByteArray text);
typedef QByteArray (*CallDecryptTextFunc)(void* instance, QByteArray text);
CreateEncryption createEncryption;
CallSetFilePathFunc callSetFilePathFunc;
CallSetKeyFunc callSetKeyFunc;
CallEncryptFunc callEncryptFunc;
CallDecryptFunc callDecryptFunc;
DestroyEncryption destroyEncryption;
CallEncryptTextFunc callEncryptTextFunc;
CallDecryptTextFunc callDecryptTextFunc;
void SetButtonsEnabled() {
    if (u->pushButton_Meth_2->isEnabled() != fl) {
        u->pushButton_Meth_1->setEnabled(fl);
        u->pushButton_Meth_2->setEnabled(fl);
        u->pushButton_start->setEnabled(fl);
        u->listWidget_Info->setEnabled(fl);
        u->label_Info->setEnabled(fl);
    }
}
void SetButtonsVisible() {
    if (u->pushButton_Meth_2->isEnabled() != fl) {
        u->pushButton_Meth_1->setVisible(fl);
        u->pushButton_Meth_2->setVisible(fl);
        u->pushButton_start->setVisible(fl);
        u->listWidget_Info->setVisible(fl);
        u->label_Info->setVisible(fl);
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white); // Заливаем фон виджета белым цветом
    painter.drawPixmap(pixmapX, pixmapY, pixmap); // Рисуем pixmap по заданным координатам
    painter.drawRect(QRect(280, 10, 930, 700));
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), soldier(&soldier)
{
    ui->setupUi(this);
    u = ui;
    update();
    map_verify["libuGun.dll"] = "4e331de18963b8a320f4d898c1099a26bbc97c1dce18e354675447797658439a";
    map_verify["libuRifle.dll"] = "4f4a06e45dcb3041c82ec1d327f2a020044c04bb8ba999e2f551749c795a479f";
    map_verify["libFirearm.dll"] = "13cf9730526eca0c2dad00c4304c4e1e305ea253402e2115dc95472d8b73f7ca";
    map_verify["libuShotGun.dll"] = "a8e90411e3ecfcc2afd233020ef571e0847c3265a28a29236569c37ec03c676e";
    map_verify["libuMachineGun.dll"] = "cf5d4a831b96c3feeec809ea0813d8901fe2e00d72108e9c8963967a043a76e8";
    map_verify["lib_Encryption.dll"] = "ea4b57eb46adaa284221db43479290c9d9c96b9b52516a9ba1fce186f9861e1c";

    timerA = new QTimer(this);
    timerB = new QTimer(this);
    timerZ = new QTimer(this);
    timer_time = new QTimer(this);
    timerStart = new QTimer(this);
    soldier.listButtonTexts.push_back(ui->pushButton_Meth_1->text());
    soldier.listButtonTexts.push_back(ui->pushButton_Meth_2->text());
    connect(timerA, SIGNAL(timeout()), this, SLOT(doAction()));
    connect(timerStart, SIGNAL(timeout()), this, SLOT(drawTarget()));
    connect(timer_time, &QTimer::timeout, this, &MainWindow::stopShooting);
    QScreen *screen = QGuiApplication::primaryScreen();
    QSize screenSize = screen->size();
    this->setFixedSize(screenSize);
    this->move(0, 0);
    soldier.actNum["Стрелять"] = 0;
    soldier.actNum["Перезарядить"] = 1;
    soldier.actNum["Почистить"] = 2;

    soldier.fireMap["Стрелять"] = soldier.get_attack_str();
    soldier.fireMap["Перезарядить"] = soldier.get_make_weapon_usable_str();
    soldier.fireMap["Почистить"] = soldier.get_make_weapon_usable_str();

    ui->label_BuyNum->setText("0");
    ui->comboBox_ChooseWeapon->setIconSize(QSize(iconSize, iconSize));
    ui->comboBox_WeaponList->setIconSize(QSize(iconSize, iconSize));

    fl = false;
    SetButtonsVisible();
    SetButtonsEnabled();
}

MainWindow::~MainWindow()
{
    delete timerA;
    delete timerB;
    delete timerZ;
    delete timerStart;
    delete timer_time;
    delete ui;
}
void MainWindow::drawTarget() {
    int sp = ui->spinBox_speed->value();
    if (moveRight) {
        setPixmapPosition(pixmapX, pixmapY);
        pixmapX += sp;
        if (pixmapX >= 1110)
            moveRight = false;
    }
    else {
        setPixmapPosition(pixmapX, pixmapY);
        pixmapX -= sp;
        if (pixmapX <= 280)
            moveRight = true;
    }
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && timerStart->isActive() && !action && !timerA->isActive() && !weapon_vect.empty()) {
        QPointF globalPos = event->globalPosition();
        widgetPos = mapFromGlobal(globalPos);
        if (widgetPos.x() >= 280 && widgetPos.y() >= 10 && widgetPos.x() <= 1210 && widgetPos.y() <= 710) {
            button_action = "Стрелять";
            action = true;
            doAction();
        }
        else
            doAfterAction();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (button_action == "Стрелять")
        doAfterAction();
}

void MainWindow::setPixmapPosition(int x, int y)
{
    pixmapX = x;
    pixmapY = y;
    update(); // Перерисовываем виджет
}

void MainWindow::setPixmap(const QPixmap &newPixmap)
{
    pixmap = newPixmap;
    update(); // Перерисовываем виджет
}

void MainWindow::setLibraries(const vector<pair<HINSTANCE, QString>> &libs)
{
    libraries = libs;
    int j = 0;
    typedef void (*SetImg)(QComboBox *comboBox);
    for (int i = 0; i < libraries.size(); i++) {
        if (libraries[i].second[3] == 'u') {
            SetImg setImg = (SetImg)GetProcAddress(libraries[i].first, "SetImg");
            setImg(ui->comboBox_ChooseWeapon);
            CreateInstance createInstance = (CreateInstance)GetProcAddress(libraries[i].first, "createInstance");
            if (createInstance != NULL)
                wepMap[j++] = make_pair(libraries[i].first, createInstance);
            else
                qDebug() << "mainwindow 111 method NULL j == " + QString::number(j);
        }
    }
}

vector<pair<HINSTANCE, QString>> MainWindow::getLibraries()
{
    return libraries;
}

bool MainWindow::checkLibraries(vector<pair<HINSTANCE, QString> > &libs)
{
    bool b = true;
    for (int i = 0; i < libs.size(); i++) {
        auto it = map_verify.find(libs[i].second);
        if (it != map_verify.end() && !verifySignature(it->first, it->second)) {
            b = false;
            break;
        }
    }
    return b;
}


void MainWindow::on_pushButton_Buy_clicked()
{
    auto iterator = wepMap.find(ui->comboBox_ChooseWeapon->currentIndex());
    if (iterator != wepMap.end()) {
        auto weapon = iterator->second.second();
        weapon_vect.push_back(make_pair(iterator->second.first, weapon));
        typedef void (*SetTimer)(QTimer*);
        SetTimer setTimerA = (SetTimer)GetProcAddress(iterator->second.first, "SetTimerA");
        SetTimer setTimerB = (SetTimer)GetProcAddress(iterator->second.first, "SetTimerB");
        setTimerA(timerA);
        setTimerB(timerB);
        typedef void (*SetImg)(QComboBox *comboBox);
        SetImg setImg = (SetImg)GetProcAddress(iterator->second.first, "SetImg");
        if (setImg != NULL)
            setImg(ui->comboBox_WeaponList);
        else
            qDebug() << "mainwindow 214 method NULL";
        typedef int (*GetCount)(void*);
        GetCount getCount = (GetCount)GetProcAddress(iterator->second.first, "GetCount");
        ui->label_BuyNum->setText(QString::number(getCount(weapon)));
    }
}


void MainWindow::on_pushButton_Throw_clicked()
{
    if (!weapon_vect.empty()) {
        int index = ui->comboBox_WeaponList->currentIndex();
        auto weapon = weapon_vect.at(index);
        typedef void (*DestroyInstance)(Weapon*);
        DestroyInstance destroyInstance = (DestroyInstance)GetProcAddress(weapon.first, "destroyInstance");
        if (destroyInstance != NULL)
            destroyInstance(weapon.second);
        else
            qDebug() << "mainwindow 227 method NULL";
        typedef int (*GetCount)(void*);
        GetCount getCount = (GetCount)GetProcAddress(weapon.first, "GetCount");
        ui->label_BuyNum->setText(QString::number(getCount(weapon.second)));
        weapon_vect.erase(weapon_vect.begin() + index);
        ui->comboBox_WeaponList->removeItem(index);
    }
}

void MainWindow::on_comboBox_WeaponList_currentIndexChanged(int index)
{
    if (!weapon_vect.empty()) {
        accepting("", index);
        auto it = soldier.listButtonTexts.begin();
        ui->pushButton_Meth_1->setText(*it);
        advance(it, 1);
        ui->pushButton_Meth_2->setText(*it);
        fl = true;
    }
    else
        fl = false;
    SetButtonsVisible();
    SetButtonsEnabled();
}
bool MainWindow::checkActionAttack() {
    auto it = soldier.fireMap.find(button_action);
    if (it == soldier.fireMap.end())
        return false;
    if (it->second != "Attack")
        return true;
    if (widgetPos.x() < 280 || widgetPos.x() > 1210 || widgetPos.y() < 10 || widgetPos.y() > 710)
        return false;
    else {
        QPoint globalPos = QCursor::pos();
        widgetPos = mapFromGlobal(globalPos);
        typedef void (*SetCoordinates)(void*, QPointF, int, int);
        SetCoordinates setCoordinates =
            (SetCoordinates)GetProcAddress(weapon_vect[ui->comboBox_WeaponList->currentIndex()].first,"SetCoordinates");
        setCoordinates(weapon_vect[ui->comboBox_WeaponList->currentIndex()].second, widgetPos, pixmapX, pixmapY);
        return true;
    }
}
void MainWindow::doAction() {
    if (checkActionAttack()) {
        accepting(button_action, ui->comboBox_WeaponList->currentIndex());
    }
    else
        mouseReleaseEvent(nullptr);
}
void allow_attack() {
    action = false;
}
void MainWindow::doAfterAction() {
    fl = false;
    QTimer *timer = nullptr;
    if (!soldier.isAttack)
        timer = timerB;
    else
        timer = timerA;
    SetButtonsEnabled();
    fl = true;
    timerZ->singleShot(timer->remainingTime(), this, SetButtonsEnabled);
    timer_notAttack->singleShot(timer->remainingTime(), this, allow_attack);
    timer->stop();
}


void MainWindow::stopShooting() {
    if (timerStart->isActive()) {
        timerStart->stop();
        timer_time->stop();
        setPixmap(QPixmap());
        ui->pushButton_start->setText("Старт");
    }
}
void MainWindow::on_pushButton_start_clicked()
{
    if (timerStart->isActive()) {
        stopShooting();
    }
    else {
        setPixmap(QPixmap(":/img/target4.jpg"));
        setPixmapPosition(280, 50);
        timerStart->start(1);
        timer_time->start(ui->spinBox_time->value() * 1000);
        moveRight = true;
        ui->pushButton_start->setText("Завершить");
    }
}


void MainWindow::on_pushButton_Meth_1_clicked()
{
    button_action = ui->pushButton_Meth_1->text();
    action = true;
    doAction();
    doAfterAction();
}


void MainWindow::on_pushButton_Meth_2_clicked()
{
    button_action = ui->pushButton_Meth_2->text();
    action = true;
    doAction();
    doAfterAction();
}

void MainWindow::accepting(QString action, int index)
{
    auto weapon = weapon_vect.at(index);
    typedef QPixmap& (*Accept)(Soldier*, QString, HINSTANCE, Weapon*, QPixmap*);
    Accept accept = (Accept)GetProcAddress(weapon.first, "Accept");
    setPixmap(accept(&soldier, action, weapon.first, weapon.second, &pixmap));
    //weapon.second->accept(&soldier, "", weapon.first, weapon.second, &pixmap);

    typedef int (*GetInfoSize)(void*);
    GetInfoSize getInfoSize = (GetInfoSize)GetProcAddress(weapon.first, "GetInfoSize");
    int len = 0;
    if (getInfoSize != NULL)
        len = getInfoSize(weapon.second);
    else
        qDebug() << "mainwindow 258 method NULL";
    ui->listWidget_Info->clear();
    typedef QString (*Indexator)(void*, int);
    for (int i = 0; i < len; i++) {
        QListWidgetItem *listItem = new QListWidgetItem;
        Indexator indexator = (Indexator)GetProcAddress(weapon.first, "indexator");
        if (indexator != NULL) {
            listItem->setText(indexator(weapon.second, i));
            ui->listWidget_Info->addItem(listItem);
        }
        else
            qDebug() << "mainwindow 270 method NULL";
        //delete listItem;
    }
}

QString MainWindow::generateSignature(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Ошибка: не удалось открыть файл.";
        return "";
    }
    QByteArray fileData = file.readAll();
    QByteArray hashData = QCryptographicHash::hash(fileData, QCryptographicHash::Sha256);
    QString signature = hashData.toHex();

    file.close();
    return signature;
}

bool MainWindow::verifySignature(const QString filePath, QString expectedSignature)
{
    QString actualSignature = generateSignature(filePath);
    return (actualSignature == expectedSignature);
}

