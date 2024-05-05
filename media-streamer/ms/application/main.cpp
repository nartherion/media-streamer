#include <ms/qt/player.hpp>

extern "C"
{

#include <libavutil/log.h>

} // extern "C"

#include <QApplication>

namespace
{

void libav_log(void*, int, const char*, va_list)
{}

} // namespace


int main(int argc, char *argv[])
{
    av_log_set_callback(libav_log);
    QApplication application(argc, argv);

    ms::qt::player player;
    player.show();

    return application.exec();
}
