// RtmTransfer.cpp

#include "just/rtmpd/Common.h"
#include "just/rtmpd/RtmTransfer.h"

#include <just/avformat/flv/FlvTagType.h>
using namespace just::avformat;

#include <just/avbase/stream/SampleBuffers.h>
#include <just/avbase/stream/FormatBuffer.h>
using namespace just::avbase;

#include <util/protocol/rtmp/RtmpMessageTraits.h>

namespace just
{
    namespace rtmpd
    {

        RtmTransfer::RtmTransfer()
            : chunk_size_(128)
        {
            msg_header_.chunk = 8;
            msg_header_.stream = 1;
            msg_header_.chunk_size = chunk_size_;
            header_.cs_id(msg_header_.chunk);
        }

        RtmTransfer::~RtmTransfer()
        {
        }

        void RtmTransfer::transfer(
            Sample & sample)
        {
            FlvTagHeader const * tag_header = (FlvTagHeader const *)sample.context;
            util::protocol::RtmpChunkHeader chunk;
            chunk.calc_timestamp = tag_header->Timestamp; // it is really 32 bits long
            chunk.message_length = tag_header->DataSize;
            chunk.message_type_id = tag_header->Type;
            chunk.message_stream_id = msg_header_.stream;
            header_.dec(chunk);
            sample.data.pop_front(); // ȥ�� FlvTagHeader
            sample.data.pop_back(); // ȥ�� previous_tag_size_
            sample.size = tag_header->DataSize;

            SampleBuffers::ConstBuffers data;

            SampleBuffers::BuffersPosition beg(sample.data.begin(), sample.data.end());
            SampleBuffers::BuffersPosition end(sample.data.end());
            boost::uint32_t left = sample.size;

            FormatBuffer abuf(header_buffer_, sizeof(header_buffer_));
            util::protocol::RtmpMessageTraits::o_archive_t oa(abuf);
            oa << header_;
            data.push_back(abuf.data());
            sample.size += abuf.size();
            abuf.consume(abuf.size());

            if (left > chunk_size_) {
                header_.fmt = 3;
                oa << header_;
            }

            while (left > chunk_size_) {
                SampleBuffers::BuffersPosition pos(beg);
                beg.increment_bytes(end, chunk_size_);
                data.insert(data.end(), SampleBuffers::range_buffers_begin(pos, beg), SampleBuffers::range_buffers_end());
                data.push_back(abuf.data());
                sample.size += abuf.size();
                left -= chunk_size_;
            }

            // the last chunk or the only one chunk
            data.insert(data.end(), SampleBuffers::range_buffers_begin(beg, end), SampleBuffers::range_buffers_end());

            data.push_front(boost::asio::buffer(&msg_header_, sizeof(msg_header_)));
            sample.size += sizeof(msg_header_);

            sample.data.swap(data);

            msg_header_.length = tag_header->DataSize;
            msg_header_.type = tag_header->Type;
            msg_header_.timestamp = tag_header->Timestamp;
            sample.context = &msg_header_;
        }

        void RtmTransfer::on_seek(
            boost::uint64_t time)
        {
            header_ = util::protocol::RtmpChunkHeader();
        }

    } // namespace rtmpd
} // namespace just
