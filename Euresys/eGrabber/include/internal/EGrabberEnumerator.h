/* Copyright Euresys 2018 */

#ifndef EGRABBER_ENUMERATOR_HEADER_FILE
#define EGRABBER_ENUMERATOR_HEADER_FILE

namespace EURESYS_NAMESPACE {
namespace Internal {

class EGrabberPred {
    public:
        EGrabberPred()
        {}
        bool testInterfaceID(const std::string &ifID) const {
            return testFiltersOn(ifIDFilters, ifID);
        }
        bool testDeviceID(const std::string &devID) const {
            return testFiltersOn(devIDFilters, devID);
        }
        bool testDeviceModel(const std::string &deviceModel) const {
            return testFiltersOn(devModelFilters, deviceModel);
        }
        bool testDataStreamID(const std::string &dsID) const {
            return testFiltersOn(dsIDFilters, dsID);
        }
    protected:
        void addInterfaceFilter(const std::string &ifIDFilter) {
            ifIDFilters.push_back(ifIDFilter);
        }
        void addDeviceFilter(const std::string &devIDFilter) {
            devIDFilters.push_back(devIDFilter);
        }
        void addDeviceModelNameFilter(const std::string &devModelFilter) {
            devModelFilters.push_back(devModelFilter);
        }
        void addDataStreamFilter(const std::string &dsIDFilter) {
            dsIDFilters.push_back(dsIDFilter);
        }
    private:
        std::vector<std::string> ifIDFilters;
        std::vector<std::string> devIDFilters;
        std::vector<std::string> devModelFilters;
        std::vector<std::string> dsIDFilters;
        bool isSubString(const std::string &s1, const std::string &s2) const {
            return s2.find(s1) != std::string::npos;
        }
        bool testFiltersOn(const std::vector<std::string> &filters, const std::string &candidate) const {
            for (std::vector<std::string>::const_iterator it = filters.begin(); it != filters.end(); ++it) {
                if (!isSubString(*it, candidate)) {
                    return false;
                }
            }
            return true;
        }
};

template <typename G>
class EGrabberEnumerator {
    private:
        EGenTL &gentl;
        struct grabber_t {
            size_t interfaceIndex;
            size_t deviceIndex;
            size_t streamIndex;
            G *ptr;
        };
        std::vector<grabber_t> grabbers;
        struct stream_t {
            size_t grabberIndex;
        };
        struct device_t {
            std::vector<stream_t> streams;
        };
        struct interface_t {
            std::vector<device_t> devices;
        };
        struct system_t {
            std::vector<interface_t> interfaces;
        };
        system_t system;
        void cleanup() {
            for (size_t i = 0; i < grabbers.size(); ++i) {
                if (grabbers[i].ptr) {
                    delete grabbers[i].ptr;
                    gentl.traceCtx.hTrace<'I',0x2883d12b01533a92ULL,'i','i','i'>("S%_-D%_-I%_: deleted",
                        (int)grabbers[i].streamIndex, (int)grabbers[i].deviceIndex, (int)grabbers[i].interfaceIndex);
                    grabbers[i].ptr = 0;
                }
            }
            grabbers.clear();
            system.interfaces.clear();
        }
    public:
        EGrabberEnumerator(EGenTL &gentl, gc::DEVICE_ACCESS_FLAGS deviceOpenFlags, const EGrabberPred &pred)
        : gentl(gentl)
        {
            if (!gentl.isShared()) {
                throw client_error("EGenTL shared mode is required");
            }
            reload(deviceOpenFlags, pred);
        }
        virtual ~EGrabberEnumerator() {
            cleanup();
        }
        void reload(gc::DEVICE_ACCESS_FLAGS deviceOpenFlags, const EGrabberPred &pred) {
            cleanup();
            gc::TL_HANDLE tlh = GENTL_INVALID_HANDLE;
            gc::IF_HANDLE ifh = GENTL_INVALID_HANDLE;
            gc::DEV_HANDLE devh = GENTL_INVALID_HANDLE;
            try {
                tlh = gentl.tlOpen();
                gentl.tlUpdateInterfaceList(tlh);
                uint32_t ifCount = gentl.tlGetNumInterfaces(tlh);
                for (uint32_t ifIx = 0; ifIx < ifCount; ++ifIx) {
                    std::string ifID(gentl.tlGetInterfaceID(tlh, ifIx));
                    ifh = gentl.tlOpenInterface(tlh, ifID);
                    gentl.ifUpdateDeviceList(ifh);
                    uint32_t devCount = gentl.ifGetNumDevices(ifh);
                    interface_t iface;
                    for (uint32_t devIx = 0; devIx < devCount; ++devIx) {
                        std::string devID(gentl.ifGetDeviceID(ifh, devIx));
                        devh = gentl.ifOpenDevice(ifh, devID, deviceOpenFlags);
                        std::string deviceModel;
                        try {
                            deviceModel = gentl.devGetInfo<std::string>(devh, gc::DEVICE_INFO_MODEL);
                        } catch (...) {
                        }
                        uint32_t dsCount = gentl.devGetNumDataStreams(devh);
                        device_t device;
                        for (uint32_t dsIx = 0; dsIx < dsCount; ++dsIx) {
                            std::string dsID(gentl.devGetDataStreamID(devh, dsIx));
                            bool selected =
                                pred.testInterfaceID(ifID) &&
                                pred.testDeviceID(devID) &&
                                pred.testDataStreamID(dsID) &&
                                pred.testDeviceModel(deviceModel);
                            stream_t stream = { (selected) ? grabbers.size() : (size_t)-1 };
                            if (selected) {
                                grabber_t grabber = { ifIx, devIx, dsIx, 0 };
                                grabbers.push_back(grabber);
                            }
                            device.streams.push_back(stream);
                        }
                        iface.devices.push_back(device);
                        gentl.devClose(devh);
                        devh = GENTL_INVALID_HANDLE;
                    }
                    system.interfaces.push_back(iface);
                    gentl.ifClose(ifh);
                    ifh = GENTL_INVALID_HANDLE;
                }
                gentl.tlClose(tlh);
                tlh = GENTL_INVALID_HANDLE;
            } catch (...) {
                if (devh != GENTL_INVALID_HANDLE) {
                    try { gentl.devClose(devh); } catch (...) {}
                }
                if (ifh != GENTL_INVALID_HANDLE) {
                    try { gentl.ifClose(ifh); } catch (...) {}
                }
                if (tlh != GENTL_INVALID_HANDLE) {
                    try { gentl.tlClose(tlh); } catch (...) {}
                }
                throw;
            }
            for (size_t i = 0; i < grabbers.size(); ++i) {
                try {
                    bool remoteRequired = false;
                    // the remote device is not required to create the grabber instance
                    grabbers[i].ptr = new G(gentl,
                        (int)grabbers[i].interfaceIndex,
                        (int)grabbers[i].deviceIndex,
                        (int)grabbers[i].streamIndex,
                        deviceOpenFlags,
                        remoteRequired);
                    gentl.traceCtx.hTrace<'I',0xd80ef3a6bda2c6acULL,'i','i','i'>("S%_-D%_-I%_: created",
                        (int)grabbers[i].streamIndex, (int)grabbers[i].deviceIndex, (int)grabbers[i].interfaceIndex);
                } catch (const gentl_error& err) {
                    gentl.traceCtx.hTrace<'E',0x44e906a41719ddeeULL,'i','i','i','s'>("S%_-D%_-I%_: not created: %_",
                        (int)grabbers[i].streamIndex, (int)grabbers[i].deviceIndex, (int)grabbers[i].interfaceIndex,
                        err.what());
                }
            }
        }
        void clear() {
            cleanup();
        }
        size_t getInterfaceCount() const {
            return system.interfaces.size();
        }
        size_t getDeviceCount(size_t interfaceIndex) const {
            if (interfaceIndex < getInterfaceCount()) {
                return system.interfaces[interfaceIndex].devices.size();
            }
            return 0;
        }
        size_t getStreamCount(size_t interfaceIndex, size_t deviceIndex) const {
            if (deviceIndex < getDeviceCount(interfaceIndex)) {
                return system.interfaces[interfaceIndex].devices[deviceIndex].streams.size();
            }
            return 0;
        }
        size_t getGrabberCount() const {
            return grabbers.size();
        }
        size_t getGrabberInterfaceIndex(size_t grabberIndex) const {
            if (grabberIndex < getGrabberCount()) {
                return grabbers[grabberIndex].interfaceIndex;
            }
            return (size_t)-1;
        }
        size_t getGrabberDeviceIndex(size_t grabberIndex) const {
            if (grabberIndex < getGrabberCount()) {
                return grabbers[grabberIndex].deviceIndex;
            }
            return (size_t)-1;
        }
        size_t getGrabberStreamIndex(size_t grabberIndex) const {
            if (grabberIndex < getGrabberCount()) {
                return grabbers[grabberIndex].streamIndex;
            }
            return (size_t)-1;
        }
        G *getGrabber(size_t grabberIndex) const {
            if (grabberIndex < getGrabberCount()) {
                return grabbers[grabberIndex].ptr;
            }
            return 0;
        }
        size_t getGrabberIndex(size_t interfaceIndex, size_t deviceIndex, size_t streamIndex) const {
            if (streamIndex < getStreamCount(interfaceIndex, deviceIndex)) {
                return system.interfaces[interfaceIndex].devices[deviceIndex].streams[streamIndex].grabberIndex;
            }
            return (size_t)-1;
        }
        G *getGrabber(size_t interfaceIndex, size_t deviceIndex, size_t streamIndex) const {
            return getGrabber(getGrabberIndex(interfaceIndex, deviceIndex, streamIndex));
        }
        size_t reposition(size_t interfaceIndex, size_t deviceIndex, size_t streamIndex, size_t grabberIndex) {
            if (grabberIndex < getGrabberCount()) {
                size_t fromIndex = getGrabberIndex(interfaceIndex, deviceIndex, streamIndex);
                if (fromIndex < getGrabberCount()) {
                    std::vector<grabber_t> gs(grabbers);
                    grabber_t g = gs[fromIndex];
                    gs.erase(gs.begin() + fromIndex);
                    gs.insert(gs.begin() + grabberIndex, g);
                    size_t ix = 0;
                    for (typename std::vector<grabber_t>::const_iterator it = gs.begin(); it != gs.end(); ++it) {
                        const grabber_t &g(*it);
                        system.interfaces[g.interfaceIndex].devices[g.deviceIndex].streams[g.streamIndex].grabberIndex = ix++;
                    }
                    grabbers.swap(gs);
                    return fromIndex;
                }
            }
            return (size_t)-1;
        }
};

} // namespace Internal
} // namespace EURESYS_NAMESPACE

#endif

