// RtmMuxer.h

#ifndef _PPBOX_MUX_RTM_RTM_MUXER_H_
#define _PPBOX_MUX_RTM_RTM_MUXER_H_

#include "ppbox/mux/flv/FlvMuxer.h"

namespace ppbox
{
    namespace rtmpd
    {

        class RtmTransfer;

        class RtmMuxer
            : public ppbox::mux::FlvMuxer
        {
        public:
            RtmMuxer();

            ~RtmMuxer();

        private:
            virtual void add_stream(
                ppbox::mux::StreamInfo & info, 
                ppbox::mux::FilterPipe & pipe);

            virtual void file_header(
                ppbox::mux::Sample & sample);

            virtual void stream_header(
                boost::uint32_t index, 
                ppbox::mux::Sample & sample);

        private:
            boost::uint8_t header_buffer_[48];
            RtmTransfer * rtm_transfer_;
        };

        PPBOX_REGISTER_MUXER("rtm", RtmMuxer);

    } // namespace rtmpd
} // namespace ppbox

#endif // _PPBOX_MUX_RTM_RTM_MUXER_H_
