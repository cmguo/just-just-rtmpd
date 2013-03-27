// RtmpSink.h

#ifndef _PPBOX_RTMPD_RTMP_SINK_H_
#define _PPBOX_RTMPD_RTMP_SINK_H_

#include <util/stream/Sink.h>

#include <util/protocol/rtmp/RtmpSocket.hpp>
#include <util/protocol/rtmp/RtmpMessageDataData.h>

namespace ppbox
{
    namespace rtmpd
    {

        class RtmpSink
            : public util::stream::Sink
        {
        public:
            RtmpSink(
                util::protocol::RtmpSocket & socket)
                : util::stream::Sink(socket.get_io_service())
                , socket_(socket)
            {
            }

        public:
            virtual std::size_t private_write_some(
                buffers_t const & buffers,
                boost::system::error_code & ec)
            {
                util::protocol::RtmpMessageHeaderEx const & header = 
                    *boost::asio::buffer_cast<util::protocol::RtmpMessageHeaderEx const *>(*buffers.begin());
                size_t n = socket_.write_raw_msg(header, util::buffers::sub_buffers(buffers, sizeof(header)), ec);
                return n ? n + sizeof(header) : 0;
            }

        private:
            util::protocol::RtmpSocket & socket_;
        };

    } // namespace rtmpd
} // namespace ppbox

#endif // _PPBOX_RTMPD_RTMP_SINK_H_
