// RtmMuxer.cpp

#include "ppbox/rtmpd/Common.h"
#include "ppbox/rtmpd/RtmMuxer.h"
#include "ppbox/rtmpd/RtmTransfer.h"

#include <ppbox/mux/filter/MergeFilter.h>
using namespace ppbox::mux;

#include <ppbox/avformat/flv/FlvDataType.h>
using namespace ppbox::avformat;

namespace ppbox
{
    namespace rtmpd
    {

        RtmMuxer::RtmMuxer()
            : rtm_transfer_(NULL)
        {
            format("flv");
        }

        RtmMuxer::~RtmMuxer()
        {
            if (rtm_transfer_) {
                delete rtm_transfer_;
                rtm_transfer_ = NULL;
            }
        }

        void RtmMuxer::add_stream(
            StreamInfo & info, 
            FilterPipe & pipe)
        {
            FlvMuxer::add_stream(info, pipe);
            if (rtm_transfer_ == NULL) {
                rtm_transfer_ = new RtmTransfer;
            }
            pipe.push_back(new MergeFilter(rtm_transfer_));
        }

        void RtmMuxer::file_header(
            Sample & sample)
        {
            //FlvDataValue v1("|RtmpSampleAccess");
            //FlvDataValue v2(FlvDataType::BOOL);
            //v2.Bool = 0;
            //FlvDataValue v3(FlvDataType::BOOL);
            //v3.Bool = 0;

            //FormatBuffer buf(header_buffer_, sizeof(header_buffer_));
            //FlvOArchive oa(buf);
            //oa << v1 << v2 << v3;

            //sample.data.push_back(buf.data());
            //sample.size = buf.size();
            //buf.consume(buf.size());
            //rtm_transfer_->file_header(sample);
            //boost::uint32_t size = boost::asio::buffer_size(sample.data.front());
            //util::buffers::buffers_copy(buf.prepare(size);

            FlvMuxer::file_header(sample);
            sample.data.pop_front(); // delete FlvHeader
            sample.size -= 9 + 4;
            rtm_transfer_->transfer(sample);
        }

        void RtmMuxer::stream_header(
            boost::uint32_t index, 
            Sample & sample)
        {
            FlvMuxer::stream_header(index, sample);
            rtm_transfer_->transfer(sample);
        }

    } // namespace rtmpd
} // namespace ppbox
