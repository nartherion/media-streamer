#include <ms/framework/data/object.hpp>

namespace ms::framework::data
{

object::object(const std::shared_ptr<dash::mpd::ISegment> segment, const dash::mpd::IRepresentation &representation)
    : segment_(segment),
      representation_(representation)
{}

object::~object()
{
    if (download_state_ == dash::network::IN_PROGRESS)
    {
        abort_download();
    }

    segment_->DetachDownloadObserver(this);
    wait_finished();
}

bool object::start_download()
{
    segment_->AttachDownloadObserver(this);
    return segment_->StartDownload();
}

void object::abort_download()
{
    segment_->AbortDownload();
    OnDownloadStateChanged(dash::network::ABORTED);
}

void object::wait_finished()
{
    std::unique_lock lock(state_mutex_);
    state_cv_.wait(lock,
        [this] { return download_state_ == dash::network::COMPLETED || download_state_ == dash::network::ABORTED; });
}

int object::read(const std::span<std::byte> data) const
{
    return segment_->Read(reinterpret_cast<std::uint8_t *>(data.data()), data.size());
}

int object::peek(const std::span<std::byte> data) const
{
    return segment_->Peek(reinterpret_cast<std::uint8_t *>(data.data()), data.size());
}

int object::peek(const std::span<std::byte> data, const std::size_t offset) const
{
    return segment_->Peek(reinterpret_cast<std::uint8_t *>(data.data()), data.size(), offset);
}

const dash::mpd::IRepresentation &object::get_representation() const
{
    return representation_;
}

void object::OnDownloadStateChanged(const dash::network::DownloadState download_state)
{
    std::unique_lock lock(state_mutex_);
    download_state_ = download_state;
    state_cv_.notify_all();
}

void object::OnDownloadRateChanged(const std::uint64_t) {}

const std::vector<dash::metrics::ITCPConnection *> &object::GetTCPConnectionList() const
{
    return segment_->GetTCPConnectionList();
}

const std::vector<dash::metrics::IHTTPTransaction *> &object::GetHTTPTransactionList() const
{
    return segment_->GetHTTPTransactionList();
}

} // namespace ms::framework::data
