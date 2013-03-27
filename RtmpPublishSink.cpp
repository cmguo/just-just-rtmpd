#include "ppbox/rtmpd/Common.h"
#include "ppbox/rtmpd/RtmpPublishSink.h"

#include <util/protocol/rtmp/RtmpError.h>
#include <util/protocol/rtmp/RtmpSocket.hpp>
#include <util/protocol/rtmp/RtmpMessageDataData.h>
using namespace util::protocol;

#include <boost/asio/buffer.hpp>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/logger/StringRecord.h>

namespace ppbox
{
    namespace rtmpd
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.rtmpd.RtmpPublishSink", framework::logger::Debug);

        RtmpPublishSink::RtmpPublishSink(
            boost::asio::io_service & io_svc)
            : ppbox::data::UrlSink(io_svc)
            , client_(io_svc)
            , open_step_(0)
        {
        }

        RtmpPublishSink::~RtmpPublishSink()
        {
        }

        boost::system::error_code RtmpPublishSink::open(
            framework::string::Url const & url, 
            boost::uint64_t beg, 
            boost::uint64_t end, 
            boost::system::error_code & ec)
        {
            url_ = url;
            open_step_ = 1;
            is_open(ec);
            return ec;
        }

        void RtmpPublishSink::async_open(
            framework::string::Url const & url, 
            boost::uint64_t beg, 
            boost::uint64_t end, 
            response_type const & resp)
        {
            url_ = url;
            resp_ = resp;
            handle_open(boost::system::error_code());
        }

        void RtmpPublishSink::handle_open(
            boost::system::error_code const & ec)
        {
            if (ec) {
                response(ec);
                return;
            }

            switch (open_step_) {
                case 0:
                    open_step_ = 1;
                    client_.async_connect(url_, 
                        boost::bind(&RtmpPublishSink::handle_open, this, _1));
                    break;
                case 1:
                    open_step_ = 2;
                    client_.async_publish(
                        boost::bind(&RtmpPublishSink::handle_open, this, _1));
                    break;
                case 2:
                    open_step_ = 3;
                    client_.set_write_parallel(true);
                    response(ec);
                    break;
                case 4: // cancel
                    response(boost::asio::error::operation_aborted);
                    break;
                default:
                    assert(0);
            }
        }

        void RtmpPublishSink::response(
            boost::system::error_code const & ec)
        {
            response_type resp;
            resp.swap(resp_);
            resp(ec);
        }

        bool RtmpPublishSink::is_open(
            boost::system::error_code & ec)
        {
            if (open_step_ == 3) {
                ec.clear();
                return true;
            }

            switch (open_step_) {
                case 1:
                    if (client_.connect(url_, ec))
                        break;
                    open_step_ = 2;
                case 2:
                    if (client_.publish(ec))
                        break;
                    open_step_ = 3;
                    client_.set_write_parallel(true);
                default:
                    assert(0);
            }

            return !ec;
        }

        boost::system::error_code RtmpPublishSink::close(
            boost::system::error_code & ec)
        {
            return client_.close(ec);
        }

        boost::system::error_code RtmpPublishSink::cancel(
            boost::system::error_code & ec)
        {
            return client_.cancel_forever(ec);
        }

        size_t RtmpPublishSink::private_write_some(
            buffers_t const & buffers, 
            boost::system::error_code & ec)
        {
            client_.tick(ec);
            RtmpMessageHeaderEx const & header = 
                *boost::asio::buffer_cast<RtmpMessageHeaderEx const *>(*buffers.begin());
            size_t n = client_.write_raw_msg(header, util::buffers::sub_buffers(buffers, sizeof(header)), ec);
            return n ? n + sizeof(header) : 0;
        }

        struct rtmp_source_write_handler
        {
            rtmp_source_write_handler(
                RtmpPublishSink & source, 
                util::stream::StreamConstBuffers const & buffers, 
                util::stream::StreamHandler const & handler)
                : source_(source)
                , buffers_(buffers)
                , handler_(handler)
            {
            }

            void operator()(
                boost::system::error_code const & ec, 
                size_t bytes_transferred) const
            {
                source_.handle_write_some(buffers_, handler_, ec, bytes_transferred);
            }

        private:
            RtmpPublishSink & source_;
            util::stream::StreamConstBuffers buffers_;
            util::stream::StreamHandler handler_;
        };

        void RtmpPublishSink::private_async_write_some(
            buffers_t const & buffers, 
            handler_t const & handler)
        {
            boost::system::error_code ec;
            client_.tick(ec);
            RtmpMessageHeaderEx const & header = 
                *boost::asio::buffer_cast<RtmpMessageHeaderEx const *>(*buffers.begin());
            client_.async_write_raw_msg(header, 
                util::buffers::sub_buffers(buffers, sizeof(header)), 
                rtmp_source_write_handler(*this, buffers, handler));
        }

        void RtmpPublishSink::handle_write_some(
            buffers_t const & buffers, 
            handler_t const & handler, 
            boost::system::error_code const & ec, 
            size_t bytes_transferred)
        {
            if (bytes_transferred)
                bytes_transferred += sizeof(RtmpMessageHeaderEx);
            handler(ec, bytes_transferred);
        }

        boost::system::error_code RtmpPublishSink::set_non_block(
            bool non_block, 
            boost::system::error_code & ec)
        {
            return client_.set_non_block(non_block, ec);
        }

        boost::system::error_code RtmpPublishSink::set_time_out(
            boost::uint32_t time_out, 
            boost::system::error_code & ec)
        {
            return client_.set_time_out(time_out, ec);
        }

        bool RtmpPublishSink::continuable(
            boost::system::error_code const & ec)
        {
            return ec == boost::asio::error::would_block;
        }

    } // namespace data
} // namespace ppbox
