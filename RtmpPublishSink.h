// RtmpPublishSink.h

#ifndef _PPBOX_RTMPD_RTMP_PUBLISH_SINK_H_
#define _PPBOX_RTMPD_RTMP_PUBLISH_SINK_H_

#include <ppbox/data/base/UrlSink.h>

#include <util/protocol/rtmp/RtmpClient.h>

namespace ppbox
{
    namespace rtmpd
    {

        struct rtmp_source_write_handler;

        class RtmpPublishSink
            : public ppbox::data::UrlSink
        {
        public:
            RtmpPublishSink(
                boost::asio::io_service & io_svc);

            virtual ~RtmpPublishSink();

        public:
            virtual boost::system::error_code open(
                framework::string::Url const & url, 
                boost::uint64_t beg, 
                boost::uint64_t end, 
                boost::system::error_code & ec);

            using ppbox::data::UrlSink::open;

            virtual void async_open(
                framework::string::Url const & url, 
                boost::uint64_t beg, 
                boost::uint64_t end, 
                response_type const & resp);

            using ppbox::data::UrlSink::async_open;

            virtual bool is_open(
                boost::system::error_code & ec);

            virtual boost::system::error_code close(
                boost::system::error_code & ec);

        public:
            virtual boost::system::error_code cancel(
                boost::system::error_code & ec);

        public:
            virtual boost::system::error_code set_non_block(
                bool non_block, 
                boost::system::error_code & ec);

            virtual boost::system::error_code set_time_out(
                boost::uint32_t time_out, 
                boost::system::error_code & ec);

            virtual bool continuable(
                boost::system::error_code const & ec);

        private:
            // implement util::stream::Sink
            virtual std::size_t private_write_some(
                buffers_t const & buffers,
                boost::system::error_code & ec);

            virtual void private_async_write_some(
                buffers_t const & buffers, 
                handler_t const & handler);

        private:
            void handle_open(
                boost::system::error_code const & ec);

            void response(
                boost::system::error_code const & ec);

            friend struct rtmp_source_write_handler;

            void handle_write_some(
                buffers_t const & buffers,
                handler_t const & handler, 
                boost::system::error_code const & ec, 
                size_t bytes_transferred);

        private:
            util::protocol::RtmpClient client_;
            response_type resp_;
            size_t open_step_;
            framework::string::Url url_;
        };

        PPBOX_REGISTER_URL_SINK("rtmp", RtmpPublishSink);

    } // namespace rtmpd
} // namespace ppbox

#endif // _PPBOX_RTMPD_RTMP_SINK_H_