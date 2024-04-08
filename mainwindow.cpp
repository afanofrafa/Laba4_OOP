#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QScreen>
#include <QTimer>
#include <QCryptographicHash>
#include <QtMultimedia/QMediaPlayer>
#include <QFile>
Ui::MainWindow *u;
bool action;
bool isMouseRelease;
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
void MainWindow::SetButtonsEnabled() {
    if (ui->listWidget_Info->isEnabled() != fl) {
        ui->listWidget_Info->setEnabled(fl);
        for (int i = 0; i < buttons.size(); i++)
            buttons[i]->setEnabled(fl);
    }
    if (timerZ->isActive())
        timerZ->stop();
}
void MainWindow::SetButtonsVisible() {
    if (ui->listWidget_Info->isEnabled() != fl) {
        ui->listWidget_Info->setVisible(fl);
        ui->label_Info->setVisible(fl);
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
    map_verify["libEncryption.dll"] = "39c16cf10d59cffe31acdb0f04a6fa8653ec569aa1e7db39cc06c0d12de83e87";
    map_verify["libFirearm.dll"] = "a74f0b7dd7681ad36a8adeca63c195516afe6ed7860a82026bc45cea224acb46";
    map_verify["lib_FirearmTemplate.dll"] = "b28dba362f136afe604ffbfdd03ebecbc76c96f5d976190e7aebe6bd6984a18d";
    timerA = new QTimer(this);
    timerB = new QTimer(this);
    timerZ = new QTimer(this);
    timer_time = new QTimer(this);
    timerStart = new QTimer(this);
    timer_notAttack = new QTimer(this);

    comboBox = nullptr;

    connect(timerA, SIGNAL(timeout()), this, SLOT(doAction()));
    connect(timerStart, SIGNAL(timeout()), this, SLOT(drawTarget()));
    connect(timer_time, &QTimer::timeout, this, &MainWindow::stopShooting);
    connect(timerZ, &QTimer::timeout, this, &MainWindow::SetButtonsEnabled);
    connect(timer_notAttack, &QTimer::timeout, this, &MainWindow::allow_attack);
    QScreen *screen = QGuiApplication::primaryScreen();
    QSize screenSize = screen->size();
    this->setFixedSize(screenSize);
    this->move(0, 0);

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
    if (action)
        isMouseRelease = false;
    else
        isMouseRelease = true;
    if (event->button() == Qt::LeftButton && timerStart->isActive() && !action && !timerA->isActive() && !weapon_vect.empty()) {
        QPointF globalPos = event->globalPosition();
        widgetPos = mapFromGlobal(globalPos);
        if (widgetPos.x() >= 280 && widgetPos.y() >= 10 && widgetPos.x() <= 1210 && widgetPos.y() <= 710) {
            button_action = comboBox->itemData(comboBox->currentIndex(), Qt::UserRole).toString();
            action = true;
            doAction();
        }
        //else
            //doAfterAction();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (comboBox != nullptr && isMouseRelease) {
        QPointF globalPos = event->globalPosition();
        widgetPos = mapFromGlobal(globalPos);
        if (widgetPos.x() >= 280 && widgetPos.y() >= 10 && widgetPos.x() <= 1210 && widgetPos.y() <= 710) {
            if (button_action == comboBox->itemData(comboBox->currentIndex(), Qt::UserRole).toString())
                doAfterAction();
        }
    }
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
        if (libraries[i].second[3] == '_') {
            librs.push_back(libraries[i].first);
            typedef void (*SetTimer)(QTimer*);
            SetTimer setTimerA = (SetTimer)GetProcAddress(libraries[i].first, "SetTimerA");
            SetTimer setTimerB = (SetTimer)GetProcAddress(libraries[i].first, "SetTimerB");
            if (setTimerA != NULL && setTimerB != NULL) {
                setTimerA(timerA);
                setTimerB(timerB);
            }
            else
                qDebug() << "setTimerA setTimerB NULL";
            SetImg setImgs = (SetImg)GetProcAddress(libraries[i].first, "SetImgs");
            if (setImgs != NULL)
                setImgs(ui->comboBox_ChooseWeapon);
            else
                qDebug() << "setImgs NULL";
            CreateInstance createInstance = (CreateInstance)GetProcAddress(libraries[i].first, "createInstance");
            if (createInstance != NULL) {
                typedef int (*WeaponsCount)();
                WeaponsCount weaponsCount = (WeaponsCount)GetProcAddress(libraries[i].first, "WeaponsCount");
                if (weaponsCount != NULL) {
                    int wN = weaponsCount();
                    for (int ii = 0; ii < wN; ii++)
                        wepMap[j++] = make_pair(libraries[i].first, createInstance);
                }
            }
            else
                qDebug() << "mainwindow 111 method NULL j == " + QString::number(j);
        }
    }
    soldier.set_maps(librs);
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
    int number = ui->comboBox_ChooseWeapon->currentIndex();
    auto iterator = wepMap.find(number);
    if (iterator != wepMap.end()) {
        for (int i = 0; i <= number && i < wepMap.size() - 1; i++) {
            if (wepMap.at(i).second == wepMap.at(number).second) {
                number -= i;
                break;
            }

        }
        auto weapon = iterator->second.second(number);
        wepData wepD;
        wepD.hDll = iterator->second.first;
        wepD.weap = weapon;
        wepD.number = number;
        weapon_vect.push_back(wepD);
        typedef void (*SetImgByIndex)(QComboBox*, int);
        SetImgByIndex setImgByIndex = (SetImgByIndex)GetProcAddress(iterator->second.first, "SetImgByIndex");
        if (setImgByIndex != NULL)
            setImgByIndex(ui->comboBox_WeaponList, number);
        else
            qDebug() << "mainwindow 235 method NULL";
        ui->label_BuyNum->setText(QString::number(weapon_vect.size()));
    }
}


void MainWindow::on_pushButton_Throw_clicked()
{
    if (!weapon_vect.empty()) {
        buttonsClear();
        int index = ui->comboBox_WeaponList->currentIndex();
        auto weapon = weapon_vect.at(index);
        typedef void (*DestroyInstance)(Weapon*);
        DestroyInstance destroyInstance = (DestroyInstance)GetProcAddress(weapon.hDll, "destroyInstance");
        if (destroyInstance != NULL)
            destroyInstance(weapon.weap);
        else
            qDebug() << "mainwindow 227 method NULL";
        weapon_vect.erase(weapon_vect.begin() + index);
        ui->label_BuyNum->setText(QString::number(weapon_vect.size()));
        ui->comboBox_WeaponList->removeItem(index);
    }
}

void MainWindow::on_comboBox_WeaponList_currentIndexChanged(int index)
{
    if (!weapon_vect.empty()) {
        soldier.listAttackTexts.clear();
        soldier.listButtonTexts.clear();
        accepting("", index);
        buttonsClear();
        if (!soldier.listButtonTexts.empty()) {
            int x = 1295;
            int y = 280;
            for (list<QString>::iterator it = soldier.listButtonTexts.begin(); it != soldier.listButtonTexts.end(); it++) {
                int length = (*it).length();
                char* path = new char[length + 1]; // +1 для завершающего нулевого символа
                for (int i = 0; i < length; ++i) {
                    QChar ch = (*it).at(i);
                    path[i] = ch.unicode(); // или ch.unicode()
                }
                path[length] = '\0';

                QToolButton *button = new QToolButton(this);
                button->setIcon(QIcon(path));
                button->setFixedSize(100, 100);
                button->setAutoFillBackground(true);
                button->setIconSize(button->size());
                button->setStyleSheet("border-radius: 50px;");
                connect(button, &QToolButton::clicked, this, &MainWindow::onButtonClicked);
                button->setProperty(path, (*it));
                delete[] path;
                button->move(x, y);
                y += 110;
                button->show();
                buttonsImgs[button] = *it;
                buttons.push_back(button);
            }
        }
        if (!soldier.listAttackTexts.empty()) {
            int i = 0;
            comboBox = new QComboBox(this);
            comboBox->setFixedSize(80, 50);
            comboBox->setIconSize(QSize(50, 50));
            comboBox->move(1305, 220);
            for (list<QString>::iterator it = soldier.listAttackTexts.begin(); it != soldier.listAttackTexts.end(); it++) {
                comboBox->addItem(QIcon(*it), "");
                comboBox->setItemData(i, *it, Qt::UserRole);
                i++;
            }
            comboBox->show();
        }
        fl = true;
    }
    else
        fl = false;
    SetButtonsVisible();
    SetButtonsEnabled();
}
bool MainWindow::checkActionAttack() {
    unordered_map<QString, pair<unordered_map<QString, QString>, unordered_map<QString, int>>> weaponMap;
    weaponMap = soldier.get_weaponMap();
    typedef QString (*GetWeaponType)();
    bool fl = true;
    bool b = false;
    for (auto lib : librs) {
        GetWeaponType getWeaponType = (GetWeaponType)GetProcAddress(lib, "GetWeaponType");
        if (getWeaponType != NULL) {
            auto it = weaponMap.find(getWeaponType());
            if (it == weaponMap.end()) {
                auto it1 = it->second.first.find(button_action);
                if (it1 != it->second.first.end())
                    fl = false;
                else {
                    if (it1->second != Attack_str)
                        b = true;
                    break;
                }
            }
        }
        else
            qDebug() << "mainwindow  method NULL";

    }
    if (!fl)
        return false;
    if (b)
        return true;
    if (soldier.isAttack && (widgetPos.x() < 280 || widgetPos.x() > 1210 || widgetPos.y() < 10 || widgetPos.y() > 710))
        return false;
    else {
        QPoint globalPos = QCursor::pos();
        widgetPos = mapFromGlobal(globalPos);
        typedef void (*SetCoordinates)(void*, QPointF, int, int);
        SetCoordinates setCoordinates =
            (SetCoordinates)GetProcAddress(weapon_vect[ui->comboBox_WeaponList->currentIndex()].hDll,"SetCoordinates");
        setCoordinates(weapon_vect[ui->comboBox_WeaponList->currentIndex()].weap, widgetPos, pixmapX, pixmapY);
        return true;
    }
}
void MainWindow::doAction() {
    if (checkActionAttack()) {
        soldier.listAttackTexts.clear();
        soldier.listButtonTexts.clear();
        accepting(button_action, ui->comboBox_WeaponList->currentIndex());
    }
    //else
        //mouseReleaseEvent(nullptr);
}
void MainWindow::allow_attack() {
    action = false;
    if (timer_notAttack->isActive())
        timer_notAttack->stop();
}
void MainWindow::doAfterAction() {
    fl = false;
    QTimer *timer = nullptr;
    if (!soldier.isAttack)
        timer = timerB;
    else
        timer = timerA;
    if (timer->isActive()) {
        SetButtonsEnabled();
        fl = true;
        timerZ->start(timer->remainingTime());
        timer_notAttack->start(timer->remainingTime());
        timer->stop();
    }
    //button_action = "";
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

void MainWindow::accepting(QString action, int index)
{
    auto weapon = weapon_vect.at(index);
    typedef QPixmap& (*Accept)(Soldier*, QString, HINSTANCE, Weapon*, QPixmap*);
    Accept accept = (Accept)GetProcAddress(weapon.hDll, "Accept");
    setPixmap(accept(&soldier, action, weapon.hDll, weapon.weap, &pixmap));
    //weapon.second->accept(&soldier, "", weapon.first, weapon.second, &pixmap);

    typedef int (*GetInfoSize)(void*);
    GetInfoSize getInfoSize = (GetInfoSize)GetProcAddress(weapon.hDll, "GetInfoSize");
    int len = 0;
    if (getInfoSize != NULL)
        len = getInfoSize(weapon.weap);
    else
        qDebug() << "mainwindow 258 method NULL";
    ui->listWidget_Info->clear();
    typedef QString (*Indexator)(void*, int);
    for (int i = 0; i < len; i++) {
        QListWidgetItem *listItem = new QListWidgetItem;
        Indexator indexator = (Indexator)GetProcAddress(weapon.hDll, "indexator");
        if (indexator != NULL) {
            listItem->setText(indexator(weapon.weap, i));
            ui->listWidget_Info->addItem(listItem);
        }
        else
            qDebug() << "mainwindow 270 method NULL";
        //delete listItem;
    }
}

void MainWindow::onButtonClicked()
{
    QToolButton *clickedButton = qobject_cast<QToolButton*>(sender());
    button_action = buttonsImgs.at(clickedButton);
    action = true;
    doAction();
    doAfterAction();
}

void MainWindow::buttonsClear()
{
    for (int i = 0; i < buttons.size(); i++) {
        delete buttons[i];
        buttons[i] = nullptr;
    }
    buttons.clear();
    buttonsImgs.clear();
    if (comboBox != nullptr) {
        delete comboBox;
        comboBox = nullptr;
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
