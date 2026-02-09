/* Copyright Euresys 2018 */

#ifndef EGRABBERS_IIMPL_HEADER_FILE
#define EGRABBERS_IIMPL_HEADER_FILE

namespace EURESYS_NAMESPACE {
namespace Internal {

static std::string buildSdi(size_t interfaceIndex, size_t deviceIndex, size_t streamIndex) {
    std::stringstream ss;
    ss << "S";
    if (streamIndex == (size_t)-1) {
       ss << "?";
    } else {
       ss << (int)streamIndex;
    }
    ss << "-D";
    if (deviceIndex == (size_t)-1) {
       ss << "?";
    } else {
       ss << (int)deviceIndex;
    }
    ss << "-I";
    if (interfaceIndex == (size_t)-1) {
       ss << "?";
    } else {
       ss << (int)interfaceIndex;
    }
    return ss.str();
}

static std::string buildInvalidEGrabberPtrErrorDesc(size_t grabberIndex, size_t interfaceIndex, size_t deviceIndex, size_t streamIndex) {
    if (grabberIndex == (size_t)-1 &&
        interfaceIndex == (size_t)-1 &&
        deviceIndex == (size_t)-1 &&
        streamIndex == (size_t)-1) {
        return std::string();
    } else {
        std::stringstream ss;
        ss << ": ";
        if (grabberIndex != (size_t)-1) {
            ss << "grabbers[" << (int)grabberIndex << "] ";
        }
        ss << Internal::buildSdi(interfaceIndex, deviceIndex, streamIndex);
        return ss.str();
    }
}

} // namespace Internal

inline invalid_egrabber_ptr::invalid_egrabber_ptr(size_t grabberIndex, size_t interfaceIndex, size_t deviceIndex, size_t streamIndex)
: std::runtime_error("invalid reference" + Internal::buildInvalidEGrabberPtrErrorDesc(grabberIndex, interfaceIndex, deviceIndex, streamIndex))
, grabberIndex(grabberIndex)
, interfaceIndex(interfaceIndex)
, deviceIndex(deviceIndex)
, streamIndex(streamIndex)
{}

template <typename G>
inline EGrabberRef<G>::EGrabberRef(Internal::EGrabberEnumerator<G> &enumerator, size_t grabberIndex)
: enumerator(enumerator)
, interfaceIndex(enumerator.getGrabberInterfaceIndex(grabberIndex))
, deviceIndex(enumerator.getGrabberDeviceIndex(grabberIndex))
, streamIndex(enumerator.getGrabberStreamIndex(grabberIndex))
{}

template <typename G>
inline EGrabberRef<G>::EGrabberRef(Internal::EGrabberEnumerator<G> &enumerator,
    size_t interfaceIndex, size_t deviceIndex, size_t streamIndex)
: enumerator(enumerator)
, interfaceIndex(interfaceIndex)
, deviceIndex(deviceIndex)
, streamIndex(streamIndex)
{}

template <typename G>
inline size_t EGrabberRef<G>::findGrabberIndex() const {
    return enumerator.getGrabberIndex(interfaceIndex, deviceIndex, streamIndex);
}

template <typename G>
inline EGrabberRef<G>::operator bool() const {
    return getPtr() != 0;
}

template <typename G>
inline G* EGrabberRef<G>::getPtr() const {
    return enumerator.getGrabber(interfaceIndex, deviceIndex, streamIndex);
}

template <typename G>
inline G* EGrabberRef<G>::operator->() const {
    G *g = getPtr();
    if (g) {
        return g;
    } else {
        throw invalid_egrabber_ptr(findGrabberIndex(), interfaceIndex, deviceIndex, streamIndex);
    }
}

template <typename G>
inline G& EGrabberRef<G>::operator*() const {
    G *g = getPtr();
    if (g) {
        return *g;
    } else {
        throw invalid_egrabber_ptr(findGrabberIndex(), interfaceIndex, deviceIndex, streamIndex);
    }
}

template <typename G>
inline std::string EGrabberRef<G>::getSdi() const {
    return Internal::buildSdi(interfaceIndex, deviceIndex, streamIndex);
}

template <typename G>
inline size_t EGrabberRef<G>::reposition(size_t grabberIndex) {
    return enumerator.reposition(interfaceIndex, deviceIndex, streamIndex, grabberIndex);
}

template <typename G>
inline EGrabberDevice<G>::EGrabberDevice(Internal::EGrabberEnumerator<G> &enumerator,
    size_t interfaceIndex, size_t deviceIndex)
: EGrabberRef<G>(enumerator, interfaceIndex, deviceIndex, 0)
, interfaceIndex(interfaceIndex)
, deviceIndex(deviceIndex)
{}

template <typename G>
inline size_t EGrabberDevice<G>::length() const {
    return EGrabberRef<G>::enumerator.getStreamCount(interfaceIndex, deviceIndex);
}

template <typename G>
inline EGrabberRef<G> EGrabberDevice<G>::operator[](size_t index) {
    return EGrabberRef<G>(EGrabberRef<G>::enumerator, interfaceIndex, deviceIndex, index);
}

template <typename G>
inline EGrabberInterface<G>::EGrabberInterface(Internal::EGrabberEnumerator<G> &enumerator,
    size_t interfaceIndex)
: EGrabberRef<G>(enumerator, interfaceIndex, 0, 0)
, interfaceIndex(interfaceIndex)
{}

template <typename G>
inline size_t EGrabberInterface<G>::length() const {
    return EGrabberRef<G>::enumerator.getDeviceCount(interfaceIndex);
}

template <typename G>
inline EGrabberDevice<G> EGrabberInterface<G>::operator[](size_t index) {
    return EGrabberDevice<G>(EGrabberRef<G>::enumerator, interfaceIndex, index);
}

template <typename G>
inline EGrabberSystem<G>::EGrabberSystem(Internal::EGrabberEnumerator<G> &enumerator)
: EGrabberRef<G>(enumerator, 0, 0, 0)
{}

template <typename G>
inline size_t EGrabberSystem<G>::length() const {
    return EGrabberRef<G>::enumerator.getInterfaceCount();
}

template <typename G>
inline EGrabberInterface<G> EGrabberSystem<G>::operator[](size_t index) {
    return EGrabberInterface<G>(EGrabberRef<G>::enumerator, index);
}

template <typename G>
inline EGrabbers<G>::EGrabbers(EGenTL &gentl, gc::DEVICE_ACCESS_FLAGS deviceOpenFlags, const EGrabberPredicate &grabberPredicate)
: gentl(gentl)
, enumerator(gentl, deviceOpenFlags, grabberPredicate)
, root(enumerator)
{}

template <typename G>
inline EGrabbers<G>::~EGrabbers()
{}

template <typename G>
inline size_t EGrabbers<G>::length() const {
    return enumerator.getGrabberCount();
}

template <typename G>
inline EGrabberRef<G> EGrabbers<G>::operator[](size_t index) {
    return EGrabberRef<G>(enumerator, index);
}

template <typename G>
inline void EGrabbers<G>::reload(gc::DEVICE_ACCESS_FLAGS deviceOpenFlags, const EGrabberPredicate &grabberPredicate) {
    enumerator.reload(deviceOpenFlags, grabberPredicate);
}

template <typename G>
inline void EGrabbers<G>::clear() {
    enumerator.clear();
}

inline EGrabberPredicate::EGrabberPredicate()
: Internal::EGrabberPred()
{}

inline EGrabberPredicate EGrabberPredicate::Interface(const std::string &interfaceIDFilter) const {
    EGrabberPredicate p(*this);
    p.addInterfaceFilter(interfaceIDFilter);
    return p;
}

inline EGrabberPredicate EGrabberPredicate::Device(const std::string &deviceIDFilter) const {
    EGrabberPredicate p(*this);
    p.addDeviceFilter(deviceIDFilter);
    return p;
}

inline EGrabberPredicate EGrabberPredicate::Remote(const std::string &deviceModelNameFilter) const {
    EGrabberPredicate p(*this);
    p.addDeviceModelNameFilter(deviceModelNameFilter);
    return p;
}

inline EGrabberPredicate EGrabberPredicate::Stream(const std::string &dataStreamIDFilter) const {
    EGrabberPredicate p(*this);
    p.addDataStreamFilter(dataStreamIDFilter);
    return p;
}

} // namespace EURESYS_NAMESPACE

#endif // EGRABBERS_IIMPL_HEADER_FILE
