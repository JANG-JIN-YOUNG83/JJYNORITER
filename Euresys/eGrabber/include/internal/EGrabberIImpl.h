/* Copyright Euresys 2015 */

#ifndef EGRABBER_IIMPL_HEADER_FILE
#define EGRABBER_IIMPL_HEADER_FILE

namespace EURESYS_NAMESPACE {

inline Buffer::Buffer(const NewBufferData &data)
: data(data)
{}
inline void Buffer::push(Internal::EGrabberBase &grabber) {
    grabber.getGenTL().dsQueueBuffer(data.dsh, data.bh);
}
#ifdef EURESYS_USE_EGRABBER_DEPRECATED_API
inline std::string Buffer::getInfoString(Internal::EGrabberBase &grabber, gc::BUFFER_INFO_CMD cmd) {
    return grabber.getGenTL().dsGetBufferInfoString(data.dsh, data.bh, cmd);
}
#endif
template <typename T> inline T Buffer::getInfo(Internal::EGrabberBase &grabber, gc::BUFFER_INFO_CMD cmd) {
    return grabber.getGenTL().dsGetBufferInfo<T>(data.dsh, data.bh, cmd);
}
inline void *Buffer::getUserPointer() {
    return data.userPointer;
}
inline void Buffer::saveToDisk(Internal::EGrabberBase &grabber, const std::string &filepath, int64_t index, const ge::ImageSaveToDiskParams *params) {
    BufferInfo bi(getInfo(grabber));
    ge::ImageConvertInput input = IMAGE_CONVERT_INPUT(static_cast<int>(bi.width),
                                                      static_cast<int>(bi.deliveredHeight),
                                                      bi.base,
                                                      bi.pixelFormat.c_str(),
                                                      &bi.size,
                                                      &bi.linePitch);
    return grabber.getGenTL().imageSaveToDisk(input, filepath, index, params);
}
inline BufferInfo Buffer::getInfo(Internal::EGrabberBase &grabber) {
    return grabber.getGenTL().dsGetBufferInfo(data.dsh, data.bh);
}

inline ScopedBuffer::ScopedBuffer(EGrabber<CallbackOnDemand> &grabberSimple, uint64_t timeout)
: grabber(grabberSimple)
, buffer(grabberSimple.pop(timeout))
{}
inline ScopedBuffer::ScopedBuffer(Internal::EGrabberBase &grabber, const NewBufferData &data)
: grabber(grabber)
, buffer(data)
{}
inline ScopedBuffer::~ScopedBuffer() {
    try {
        buffer.push(grabber);
    } catch (...) {
    }
}
#ifdef EURESYS_USE_EGRABBER_DEPRECATED_API
inline std::string ScopedBuffer::getInfoString(gc::BUFFER_INFO_CMD cmd) {
    return buffer.getInfoString(grabber, cmd);
}
#endif
template <typename T> inline T ScopedBuffer::getInfo(gc::BUFFER_INFO_CMD cmd) {
    return buffer.template getInfo<T>(grabber, cmd);
}
inline void *ScopedBuffer::getUserPointer() {
    return buffer.getUserPointer();
}
inline void ScopedBuffer::saveToDisk(const std::string &filepath, int64_t index, const ge::ImageSaveToDiskParams *params) {
    return buffer.saveToDisk(grabber, filepath, index, params);
}
inline BufferInfo ScopedBuffer::getInfo() {
    return buffer.getInfo(grabber);
}


} // namespace EURESYS_NAMESPACE

#include "generated/EGrabberIImpl_EGrabber.h"
#include "generated/numid.h"

#endif
