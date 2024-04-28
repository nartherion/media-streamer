#include <ms/framework/stream/manager.hpp>
#include <ms/av/frame.hpp>

#include <spdlog/spdlog.h>

namespace ms::framework::stream
{

namespace
{

} // namespace

manager::manager(const std::size_t buffer_size, media::frame_acceptor &acceptor, const dash::mpd::IMPD &mpd)
    : receiver_(mpd, buffer_size),
      decoder_(acceptor, receiver_)
{}

bool manager::start()
{
    if (is_running_)
    {
        return false;
    }

    if (!receiver_.start())
    {
        return false;
    }

    if (!decoder_.start())
    {
        return false;
    }

    is_running_ = true;
    return true;
}

void manager::stop()
{
    if (!is_running_)
    {
        return;
    }

    is_running_ = false;
    decoder_.stop();
    receiver_.stop();
}

void manager::set_representation(const dash::mpd::IPeriod &period, const dash::mpd::IAdaptationSet &adaptation_set,
                                 const dash::mpd::IRepresentation &representation)
{
    receiver_.set_representation(period, adaptation_set, representation);
}

} // namespace ms::framework::stream
