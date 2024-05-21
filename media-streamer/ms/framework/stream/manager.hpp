#pragma once

#include <ms/utils/frame_acceptor.hpp>
#include <ms/framework/stream/receiver.hpp>
#include <ms/framework/data/decoder.hpp>
#include <ms/av/frame.hpp>

#include <cstddef>

#include <IMPD.h>

namespace ms::framework::stream
{

class manager
{
public:
    manager(const std::size_t buffer_size, const receiver::mpd_pointer mpd, utils::frame_acceptor &acceptor);

    bool start();
    void stop();
    void set_representation(receiver::mpd_pointer mpd, const dash::mpd::IPeriod &period,
                            const dash::mpd::IAdaptationSet &adaptation_set,
                            const dash::mpd::IRepresentation &representation);

private:
    stream::receiver receiver_;
    data::decoder decoder_;
    bool is_running_ = false;
};

} // ms::framework::stream
