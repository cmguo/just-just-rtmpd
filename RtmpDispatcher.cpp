// RtmpSession.cpp

#include "just/rtmpd/Common.h"
#include "just/rtmpd/RtmpDispatcher.h"
#include "just/rtmpd/RtmpSink.h"

#include <util/protocol/rtmp/RtmpError.h>
#include <util/protocol/rtmp/RtmpMessage.h>
#include <util/stream/TcpSocket.h>
using namespace util::protocol;

#include <framework/system/LogicError.h>
#include <framework/string/Base16.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
using namespace framework::string;

#include <boost/bind.hpp>
#include <boost/asio/write.hpp>
using namespace boost::system;

#include <fstream>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("just.rtmpd.RtmpDispatcher", framework::logger::Debug)

namespace just
{
    namespace rtmpd
    {
        RtmpDispatcher::RtmpDispatcher(
            just::dispatch::DispatcherBase & dispatcher)
            : just::dispatch::CustomDispatcher(dispatcher)
            , sink_(NULL)
        {
        }

        RtmpDispatcher::~RtmpDispatcher()
        {
        }

        void RtmpDispatcher::async_open_play(
            RtmpSocket & socket, 
            framework::string::Url & url, 
            just::dispatch::response_t const & seek_resp, 
            just::dispatch::response_t  const & resp)
        {
            url.param(just::dispatch::param_format, "rtm");
            CustomDispatcher::async_open(url, 
                boost::bind(&RtmpDispatcher::handle_open, this, boost::ref(socket), seek_resp, resp, _1));
        }

        bool RtmpDispatcher::teardown(
            boost::system::error_code & ec)
        {
            delete sink_;
            sink_ = NULL;
            ec.clear();
            return true;
        }

        void RtmpDispatcher::handle_open(
            RtmpSocket & socket, 
            just::dispatch::response_t const & seek_resp, 
            just::dispatch::response_t const & resp, 
            boost::system::error_code ec)
        {
            if (!ec) {
                sink_ = new RtmpSink(socket);
                if (CustomDispatcher::setup(-1, *sink_, ec)) {
                    just::dispatch::SeekRange range;
                    CustomDispatcher::async_play(range, seek_resp, resp);
                    return;
                }
            }

            seek_resp(ec);
            resp(ec);
        }

    } // namespace rtmpd
} // namespace just
