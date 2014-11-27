// RtmMuxer.cpp

#include "just/rtmpd/Common.h"
#include "just/rtmpd/RtmMuxer.h"
#include "just/rtmpd/RtmTransfer.h"

#include <just/mux/filter/MergeFilter.h>
using namespace just::mux;

#include <just/avformat/flv/FlvDataType.h>
using namespace just::avformat;

namespace just
{
    namespace rtmpd
    {

        RtmMuxer::RtmMuxer(
            boost::asio::io_service & io_svc)
            : FlvMuxer(io_svc)
            , rtm_transfer_(NULL)
        {
            format("flv");
            config().register_module("RtmMuxer")
                << CONFIG_PARAM_NAME_RDWR("target", target_);
        }

        RtmMuxer::~RtmMuxer()
        {
        }

        void RtmMuxer::add_stream(
            StreamInfo & info, 
            FilterPipe & pipe)
        {
            FlvMuxer::add_stream(info, pipe);
            if (rtm_transfer_ == NULL) {
                rtm_transfer_ = new RtmTransfer;
            }
            pipe.insert(new MergeFilter(rtm_transfer_));
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
            // delete FlvHeader
            sample.data.pop_front();
            sample.size -= 9 + 4;
            // add "@setDataFrame"
            if (target_ == "fms") {
                FlvDataValue setDataFrame = "@setDataFrame";
                FormatBuffer buf(header_buffer_, sizeof(header_buffer_));
                FlvOArchive oa(buf);
                oa << setDataFrame;
                sample.data.insert(sample.data.begin() + 1, buf.data());
                sample.size += buf.size();
                FlvTagHeader * tag_header = (FlvTagHeader *)sample.context;
                tag_header->DataSize = tag_header->DataSize + buf.size();
            }
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
} // namespace just
