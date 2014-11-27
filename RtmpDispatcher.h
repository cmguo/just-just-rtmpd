// RtpDispatcher.h

#ifndef _JUST_RTMPD_RTMP_DISPATCHER_H_
#define _JUST_RTMPD_RTMP_DISPATCHER_H_

#include <just/dispatch/CustomDispatcher.h>

namespace util
{
    namespace protocol
    {
        class RtmpSocket;
    }
}

namespace just
{
    namespace rtmpd
    {

        class RtmpSink;

        class RtmpDispatcher 
            : public just::dispatch::CustomDispatcher
        {
        public:

            RtmpDispatcher(
                just::dispatch::DispatcherBase & dispatcher);

            ~RtmpDispatcher();

        public:
            void async_open_play(
                util::protocol::RtmpSocket & socket, 
                framework::string::Url & url, 
                just::dispatch::response_t const & seek_resp, 
                just::dispatch::response_t  const & resp);

            bool teardown(
                boost::system::error_code & ec);

        private:
            void handle_open(
                util::protocol::RtmpSocket & socket, 
                just::dispatch::response_t const & seek_resp, 
                just::dispatch::response_t const & resp, 
                boost::system::error_code ec);

        private:
            just::avbase::MediaInfo media_info_;
            util::stream::Sink * sink_;
        };

    } // namespace rtmpd
} // namespace just

#endif // _JUST_RTMPD_RTP_DISPATCHER_H_
