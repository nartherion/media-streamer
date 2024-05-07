/*
 * LibavDecoder.h
 *****************************************************************************
 * Copyright (C) 2012, bitmovin Softwareentwicklung OG, All Rights Reserved
 *
 * Email: libdash-dev@vicky.bitmovin.net
 *
 * This source code and its use and distribution, is subject to the terms
 * and conditions of the applicable license agreement.
 *****************************************************************************/

#pragma once

#include <ms/av/format_context.hpp>
#include <ms/av/codec_context.hpp>
#include <ms/av/frame.hpp>
#include <ms/av/packet.hpp>
#include <ms/framework/data/packet_provider.hpp>
#include <ms/utils/frame_acceptor.hpp>

#include <vector>
#include <optional>
#include <functional>

namespace ms::utils
{

class decoder
{
public:
    static std::optional<decoder> create(framework::data::packet_provider &provider,
                                         frame_acceptor &acceptor);

    void decode();
    void abort();

private:
    decoder(av::format_context format_context, frame_acceptor &acceptor);

    std::optional<av::codec_context> find_codec(int index);
    std::optional<av::packet> get_next_packet();
    std::optional<av::frame> decode_frame(av::packet packet);
    bool decode_media(av::packet packet, av::frame &frame);
    void notify(av::frame frame);

    const std::vector<av::codec_context> codecs_;
    frame_acceptor &frame_acceptor_;
    av::format_context format_context_;
    bool is_decoding_ = false;
};

} // namespace ms::utils
