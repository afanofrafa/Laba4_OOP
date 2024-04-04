#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    vector<pair<HINSTANCE, QString>> libraries;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            libraries.push_back(make_pair(LoadLibrary(LPCWSTR(argv[i])), argv[i]));
        }
    }
    else
    {
        libraries.push_back(make_pair(LoadLibrary(L"lib_Encryption.dll"), "lib_Encryption.dll"));
        //libraries.push_back(make_pair(LoadLibrary(L"libuGun.dll"), "libuGun.dll"));
    }
    for (int i = 0; i < libraries.size(); i++) {
        if (libraries.at(i).first == NULL) {
            qDebug() << "Ошибка при загрузке библиотек";
            return 1;
        }
    }
    MainWindow w;
    w.setLibraries(libraries);
    /*if (!w.checkLibraries(libraries)) {
        qDebug() << "Ошибка при загрузке библиотек";
        return 1;
    }*/
    w.show();
    return a.exec();
}
