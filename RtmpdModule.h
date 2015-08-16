// RtmpdModule.h

#ifndef _JUST_RTMPD_RTMPD_MODULE_H_
#define _JUST_RTMPD_RTMPD_MODULE_H_

#include <framework/network/ServerManager.h>
#include <framework/string/Url.h>

namespace just
{
    namespace dispatch
    {
        class DispatchModule;
    }

    namespace rtmpd
    {

        class RtmpSession;
        class RtmpDispatcher;

        class RtmpdModule 
            : public just::common::CommonModuleBase<RtmpdModule>
            , public framework::network::ServerManager<RtmpSession, RtmpdModule>
        {
        public:
            RtmpdModule(
                util::daemon::Daemon & daemon);

            virtual ~RtmpdModule();

        public:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        public:
            using just::common::CommonModuleBase<RtmpdModule>::io_svc;

            RtmpDispatcher * alloc_dispatcher(
                framework::string::Url & url, 
                boost::system::error_code & ec);

            void free_dispatcher(
                RtmpDispatcher * dispatcher);

        private:
            framework::network::NetName addr_;
            just::dispatch::DispatchModule & dispatch_module_;
        };

    } // namespace rtmpd
} // namespace just

#endif // _JUST_RTMPD_RTMPD_MODULE_H_
