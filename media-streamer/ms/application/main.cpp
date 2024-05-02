#include <ms/qt/player.hpp>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    ms::qt::player player;
    player.show();

    return application.exec();
}
