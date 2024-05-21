#include <ms/framework/stream/manager.hpp>
#include <ms/av/frame.hpp>

namespace ms::framework::stream
{

namespace
{

} // namespace

manager::manager(const std::size_t buffer_size, const receiver::mpd_pointer mpd, utils::frame_acceptor &acceptor)
    : receiver_(mpd, buffer_size),
      decoder_(receiver_, acceptor)
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

void manager::set_representation(const std::shared_ptr<const dash::mpd::IMPD> mpd, const dash::mpd::IPeriod &period,
                                 const dash::mpd::IAdaptationSet &adaptation_set,
                                 const dash::mpd::IRepresentation &representation)
{
    receiver_.set_representation(mpd, period, adaptation_set, representation);
}

} // namespace ms::framework::stream
