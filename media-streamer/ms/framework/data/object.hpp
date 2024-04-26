#pragma once

#include <ms/common/immobile.hpp>

#include <cstddef>

#include <mutex>
#include <condition_variable>
#include <span>
#include <memory>

#include <ISegment.h>
#include <IRepresentation.h>
#include <IDownloadObserver.h>
#include <IDASHMetrics.h>

namespace ms::framework::data
{

class object : public immobile, public dash::network::IDownloadObserver, public dash::metrics::IDASHMetrics
{
public:
    object(std::shared_ptr<dash::mpd::ISegment> segment, const dash::mpd::IRepresentation &representation);
    ~object();

    bool start_download();
    void abort_download();
    void wait_finished();
    int read(std::span<std::byte> data) const;
    int peek(std::span<std::byte> data) const ;
    int peek(std::span<std::byte> data, std::size_t offset) const;
    const dash::mpd::IRepresentation &get_representation() const;

    void OnDownloadStateChanged(dash::network::DownloadState state) override;
    void OnDownloadRateChanged(std::uint64_t bytes_downloaded) override;

    const std::vector<dash::metrics::ITCPConnection *> &GetTCPConnectionList() const override;
    const std::vector<dash::metrics::IHTTPTransaction *> &GetHTTPTransactionList() const override;

private:
    const std::shared_ptr<dash::mpd::ISegment> segment_;
    const dash::mpd::IRepresentation &representation_;
    dash::network::DownloadState download_state_;
    mutable std::mutex state_mutex_;
    mutable std::condition_variable state_cv_;
};

} // namespace ms::framework::data
