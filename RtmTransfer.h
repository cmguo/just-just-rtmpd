// RtmTransfer.h

#ifndef _JUST_MUX_RTM_RTM_TRANSFER_H_
#define _JUST_MUX_RTM_RTM_TRANSFER_H_

#include "just/mux/Transfer.h"

#include <util/protocol/rtmp/RtmpChunkHeader.h>
#include <util/protocol/rtmp/RtmpMessage.h>

namespace just
{
    namespace rtmpd
    {

        class RtmTransfer
            : public just::mux::Transfer
        {
        public:
            RtmTransfer();

            ~RtmTransfer();

        public:
            virtual void transfer(
                just::mux::Sample & sample);

            virtual void on_seek(
                boost::uint64_t time);

        private:
            boost::uint8_t header_buffer_[20];
            util::protocol::RtmpChunkHeader header_;
            util::protocol::RtmpMessageHeaderEx msg_header_;
            boost::uint32_t chunk_size_;
        };

    } // namespace rtmpd
} // namespace just

#endif // _JUST_MUX_RTM_RTM_TRANSFER_H_
