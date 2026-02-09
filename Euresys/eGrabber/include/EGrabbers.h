/* Copyright Euresys 2018 */

#ifndef EGRABBERS_HEADER_FILE
#define EGRABBERS_HEADER_FILE

#include "EGrabber.h"
#include "internal/EGrabberEnumerator.h"

/** @file

    Main EGrabbers header file (helper to manage available EGrabber instances)
 **/

namespace EURESYS_NAMESPACE {

/** @defgroup EGrabbers EGrabbers
    Euresys helper class managing available EGrabber instances
    @{
 **/

/** an invalid egrabber smart pointer (EGrabberRef) has been dereferenced */
class invalid_egrabber_ptr: public std::runtime_error {
    public:
        const size_t grabberIndex;
        const size_t interfaceIndex;
        const size_t deviceIndex;
        const size_t streamIndex;
        explicit invalid_egrabber_ptr(size_t grabberIndex, size_t interfaceIndex, size_t deviceIndex, size_t streamIndex);
};

/** A smart pointer to an EGrabber of type G
 *
    This object is automatically created by the EGrabbers instance when accessing a specific grabber.

    @sa @ref EGrabberDevice::operator[]
    @sa @ref EGrabberInterface::operator[]
    @sa @ref EGrabberSystem::operator[]
 **/
template <typename G>
class EGrabberRef {
    protected:
        Internal::EGrabberEnumerator<G> &enumerator;
    public:
        const size_t interfaceIndex; /**< the index of the interface of this grabber in @ref EGrabbers::root */
        const size_t deviceIndex;    /**< the index of the device (in the interface) of this grabber */
        const size_t streamIndex;    /**< the index of the stream (in the device) of this grabber */
        EGrabberRef(Internal::EGrabberEnumerator<G> &enumerator, size_t grabberIndex);
        EGrabberRef(Internal::EGrabberEnumerator<G> &enumerator,
                    size_t interfaceIndex, size_t deviceIndex, size_t streamIndex);
        /// the index of the grabber in the flat list of grabbers or (size_t)-1 if not found
        size_t findGrabberIndex() const;
        /// check if this grabber reference is valid (e.g. does it point to an actual EGrabber instance?)
        operator bool() const;
        /// return address of the EGrabber instance or NULL, using the raw pointer is discouraged
        G* getPtr() const;
        /// access EGrabber member variables or methods @throw invalid_egrabber_ptr
        G* operator->() const;
        /// access EGrabber member variables or methods @throw invalid_egrabber_ptr
        G& operator*() const;
        /// return the SDI of the grabber, a string identifying the grabber in the system by the Stream-Device-Interface indexes
        std::string getSdi() const;
        /// reposition this grabber in the flat list of grabbers, return its previous index in the list or (size_t)-1 if the operation failed
        size_t reposition(size_t grabberIndex);
};

/** A smart pointer to a device giving access to its streams as EGrabbers of type G
 *
    This object is automatically created by the EGrabberInterface instance obtained from @ref EGrabbers::root.

    @sa @ref EGrabberRef
    @sa @ref EGrabberInterface::operator[]
    @sa @ref EGrabberSystem::operator[]
 **/
template <typename G>
class EGrabberDevice: public EGrabberRef<G> {
    private:
        const size_t interfaceIndex; /**< the index of the interface of this device in @ref EGrabbers::root */
        const size_t deviceIndex;    /**< the index of this device (in the interface) */
    public:
        EGrabberDevice(Internal::EGrabberEnumerator<G> &enumerator, size_t interfaceIndex, size_t deviceIndex);
        /// return the number of streams managed by this device
        size_t length() const;
        /// access a stream (as an EGrabber of type G) of this device by its index
        EGrabberRef<G> operator[](size_t index);
};

/** A smart pointer to an interface giving access to its devices
 *
    This object is automatically created from @ref EGrabbers::root.

    @sa @ref EGrabberRef
    @sa @ref EGrabberDevice::operator[]
    @sa @ref EGrabberSystem::operator[]
 **/
template <typename G>
class EGrabberInterface: public EGrabberRef<G> {
    private:
        const size_t interfaceIndex; /**< the index of this interface in @ref EGrabbers::root */
    public:
        EGrabberInterface(Internal::EGrabberEnumerator<G> &enumerator, size_t interfaceIndex);
        /// return the number of devices managed by this interface
        size_t length() const;
        /// access a device of this interface by its index
        EGrabberDevice<G> operator[](size_t index);
};

/** A smart pointer to the system giving access to its interfaces
 *
    This object is the @ref EGrabbers::root.

    @sa @ref EGrabberRef
    @sa @ref EGrabberDevice::operator[]
    @sa @ref EGrabberInterface::operator[]
 **/
template <typename G>
class EGrabberSystem: public EGrabberRef<G> {
    public:
        EGrabberSystem(Internal::EGrabberEnumerator<G> &enumerator);
        /// return the number of interfaces managed by the system
        size_t length() const;
        /// access an interface of the system by its index
        EGrabberInterface<G> operator[](size_t index);
};

/** A predicate used to only include matching EGrabbers while building the list of EGrabber instances
 *
    A predicate is built by successive calls to any of EGrabberPredicate::Interface, EGrabberPredicate::Device,
    EGrabberPredicate::Stream, EGrabberPredicate::Remote. The resulting predicate is then used by
    EGrabbers::EGrabbers to only select grabbers matching all the given predicates

    @sa @ref EGrabbers::EGrabbers
    @sa @ref EGrabbers::reload
 **/
class EGrabberPredicate: public Internal::EGrabberPred {
    public:
        /// default predicate matching any EGrabber
        EGrabberPredicate();
        /// return the current predicate combined with a predicate checking whether the given string is a substring of the grabber interface ID (value of @ref EGenTL::tlGetInterfaceID)
        EGrabberPredicate Interface(const std::string &interfaceIDFilter) const;
        /// return the current predicate combined with a predicate checking whether the given string is a substring of the grabber device ID (value of @ref EGenTL::ifGetDeviceID)
        EGrabberPredicate Device(const std::string &deviceIDFilter) const;
        /// return the current predicate combined with a predicate checking whether the given string is a substring of the remote device model (value of @ref GenTL::DEVICE_INFO_MODEL)
        EGrabberPredicate Remote(const std::string &deviceModelNameFilter) const;
        /// return the current predicate combined with a predicate checking whether the given string is a substring of the grabber data stream ID (value of @ref EGenTL::devGetDataStreamID)
        EGrabberPredicate Stream(const std::string &dataStreamIDFilter) const;
};

const EGrabberPredicate defaultEGrabberPredicate = EGrabberPredicate();

/**
 * A helper class to discover and manage available EGrabber instances

   @details

   Usage:

   @code{.cpp}
   class MyGrabber: public Euresys::EGrabberCallbackOnDemand {
       public:
           // Euresys::EGrabbers requires the grabber constructor to have the following parameters
           MyGrabber(EGenTL &gentl, int interfaceIndex, int deviceIndex, int dataStreamIndex,
                     gc::DEVICE_ACCESS_FLAGS deviceOpenFlags, bool remoteRequired)
           : Euresys::EGrabberCallbackOnDemand(gentl, interfaceIndex, deviceIndex, dataStreamIndex, deviceOpenFlags, remoteRequired)
           {
               ...
           }
           void someMethod() {
               ...
           }
   };

   void example1() {
       Euresys::EGenTL genTL;

       // the grabbers constructor scans the system and creates
       // as many MyGrabber instances as the number of available EGrabbers
       Euresys::EGrabbers<MyGrabber> grabbers(genTL);

       // access all grabbers as a flat list
       for (size_t i = 0; i < grabbers.length(); ++i) {
           // grabbers[i] could be invalid (if MyGrabber constructor failed), it's
           // preferable to check the reference to avoid an invalid_egrabber_ptr
           // exception when dereferenced
           if (grabbers[i]) {
               if (grabbers[i]->isOpen<RemoteModule>()) {
                   grabbers[i]->runScript(myGrabberConfigScript);
                   grabbers[i]->reallocBuffers(3);
               }
           }
       }

       // walk the tree structure of grabbers to start acquisition and call someMethod
       for (size_t i = 0; i < grabbers.root.length(); ++i) {
           for (size_t d = 0; d < grabbers.root[i].length(); ++d) {
               // indexes are optional (and default to 0), for instance the following
               // code omits the stream index and is equivalent to grabbers.root[i][d][0]
               if (grabbers.root[i][d]) {
                   if (grabbers.root[i][d]->isOpen<RemoteModule>()) {
                       grabbers.root[i][d]->start(1);
                   }
                   grabbers.root[i][d]->someMethod();
               }
           }
       }

       // the grabbers destructor takes care of deleting the created EGrabber instances
   }

   void example2() {
       Euresys::EGenTL genTL;

       // build list of grabbers only matching the "Device1" of "2-camera" interfaces
       Euresys::EGrabbers<MyGrabber> grabbers(
           genTL, defaultDeviceOpenFlags,
           Euresys::EGrabberPredicate().Interface("2-camera").Device("Device1"));

       // access these grabbers as a flat list
       for (size_t i = 0; i < grabbers.length(); ++i) {
           if (grabbers[i]) {
               if (grabbers[i]->isOpen<RemoteModule>()) {
                   grabbers[i]->runScript(myGrabberConfigScript);
                   grabbers[i]->reallocBuffers(3);
                   // ...
               }
           }
       }
   }

   void example3() {
       Euresys::EGenTL genTL;
       Euresys::EGrabbers<MyGrabber> grabbers(genTL);

       // grabbers can be repositioned in the flat list of grabbers, this is
       // convenient if for example there is a "master" camera among the grabbers
       grabbers.root[interfaceIndexOfMaster][deviceIndexOfMaster].reposition(0);

       // now the master can be easily accessed as grabbers[0] and the slaves
       // as grabbers[1], grabbers[2]...
       for (size_t i = 1; i < grabbers.length() - 1; ++i) {
           grabbers[i].start(); // start a "slave"
       }
       grabbers[0].start(); // start the "master"
   }

   @endcode

 **/
template <typename G = EGrabber<CallbackOnDemand> >
class EGrabbers {
    private:
        EGenTL &gentl;
        Internal::EGrabberEnumerator<G> enumerator;
    public:
        /// access the interfaces of the system (hierarchical structure of GenTL modules)
        EGrabberSystem<G> root;
        /// instanciate all available grabbers of type G and organize them in both a flat list and a tree (GenTL hierarchy of modules),
        /// grabbers are created with the given open flags
        EGrabbers(EGenTL &gentl, gc::DEVICE_ACCESS_FLAGS deviceOpenFlags = defaultDeviceOpenFlags, const EGrabberPredicate &grabberPredicate = defaultEGrabberPredicate);
        /// delete the created grabbers
        virtual ~EGrabbers();
        /// return the number of grabbers in the flat list
        size_t length() const;
        /// access EGrabber instances as a flat list (built by flattening the tree of GenTL modules)
        EGrabberRef<G> operator[](size_t index);
        /// discard the current list of grabbers and rescan the system to build a new list/tree, grabbers are created with the given open flags
        void reload(gc::DEVICE_ACCESS_FLAGS deviceOpenFlags = defaultDeviceOpenFlags, const EGrabberPredicate &grabberPredicate = defaultEGrabberPredicate);
        /// discard the current list of grabbers
        void clear();
};

/** @} */

}

#include "internal/EGrabbersIImpl.h"

#endif // EGRABBERS_HEADER_FILE
