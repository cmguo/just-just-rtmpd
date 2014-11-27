// RtmMuxer.h

#ifndef _JUST_MUX_RTM_RTM_MUXER_H_
#define _JUST_MUX_RTM_RTM_MUXER_H_

#include "just/mux/flv/FlvMuxer.h"

namespace just
{
    namespace rtmpd
    {

        class RtmTransfer;

        class RtmMuxer
            : public just::mux::FlvMuxer
        {
        public:
            RtmMuxer(
                boost::asio::io_service & io_svc);

            ~RtmMuxer();

        private:
            virtual void add_stream(
                just::mux::StreamInfo & info, 
                just::mux::FilterPipe & pipe);

            virtual void file_header(
                just::mux::Sample & sample);

            virtual void stream_header(
                boost::uint32_t index, 
                just::mux::Sample & sample);

        private:
            std::string target_;
            boost::uint8_t header_buffer_[48];
            RtmTransfer * rtm_transfer_;
        };

        JUST_REGISTER_MUXER("rtm", RtmMuxer);

    } // namespace rtmpd
} // namespace just

#endif // _JUST_MUX_RTM_RTM_MUXER_H_
