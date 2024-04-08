#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    vector<pair<HINSTANCE, QString>> libraris;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            libraris.push_back(make_pair(LoadLibrary(LPCWSTR(argv[i])), argv[i]));
        }
    }
    else
    {
        libraris.push_back(make_pair(LoadLibrary(L"libFirearm.dll"), "libFirearm.dll"));
        libraris.push_back(make_pair(LoadLibrary(L"libEncryption.dll"), "libEncryption.dll"));
        libraris.push_back(make_pair(LoadLibrary(L"lib_FirearmTemplate.dll"), "lib_FirearmTemplate.dll"));
        libraris.push_back(make_pair(LoadLibrary(L"libBladedWeapon.dll"), "libBladedWeapon.dll"));
        libraris.push_back(make_pair(LoadLibrary(L"lib_BladedWeaponTemplate.dll"), "lib_BladedWeaponTemplate.dll"));
    }
    for (int i = 0; i < libraris.size(); i++) {
        if (libraris.at(i).first == NULL) {
            qDebug() << "Ошибка при загрузке библиотек";
            return 1;
        }
    }
    MainWindow w;
    w.setLibraries(libraris);
    /*if (!w.checkLibraries(libraris)) {
        qDebug() << "Ошибка при загрузке библиотек";
        return 1;
    }*/
    w.show();
    return a.exec();
}
