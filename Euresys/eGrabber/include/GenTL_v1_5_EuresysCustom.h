/* Copyright Euresys 2015 */

#ifndef GENTL_V1_4_EURESYS_CUSTOM_HEADER_FILE
#define GENTL_V1_4_EURESYS_CUSTOM_HEADER_FILE

/** @file
    Defines Coaxlink-specific constants
    @defgroup ECAPI Euresys low-level C API
    Euresys Low-level C API
 **/

#if !defined(EURESYS_GENTL_INCLUDE_ONLY_ENUM_DEFS)
#if !defined(GC_IMPORT_EXPORT) || !defined(GC_CALLTYPE)
#error Include GenTL header file before GenTL_v1_5_EuresysCustom.h
#endif

#ifdef __cplusplus
extern "C" {
namespace GenTL {
namespace EuresysCustomGenTL {
#endif
#endif

/** @defgroup CEuresysGenTL Custom GenTL (C API)
    @ingroup ECAPI
    Euresys Custom GenTL low-level C API
    @{
 **/

/** This enumeration defines Euresys custom flags of how a device is to be opened with the GenICam::TL::Client::IFOpenDevice function. */
enum DEVICE_ACCESS_FLAGS_CUSTOM_LIST
{
    DEVICE_ACCESS_CUSTOM_READONLY_DEVICE_READONLY_STREAM = DEVICE_ACCESS_CUSTOM_ID + 1, /**< Open both the device and child data stream read only */
};

/** This enumeration defines Euresys custom errors (GenTL::GC_ERROR) */
enum GC_CUSTOM_ERROR_LIST
{
    GC_ERR_CUSTOM_MULTIPLE_HANDLES                       = GC_ERR_CUSTOM_ID -  1,  /**< Different handles to the same object were found */
    GC_ERR_CUSTOM_DANGLING_HANDLES                       = GC_ERR_CUSTOM_ID -  2,  /**< Handles associated to a closed object were found */
    GC_ERR_CUSTOM_LOST_HANDLE                            = GC_ERR_CUSTOM_ID -  3,  /**< No handle associated to an opened object was found */
    GC_ERR_CUSTOM_LOCK_ERROR                             = GC_ERR_CUSTOM_ID -  4,  /**< A lock issue has been detected */
    GC_ERR_CUSTOM_SILENT_ERROR                           = GC_ERR_CUSTOM_ID -  5,  /**< Some silent errors occurred */
    GC_ERR_CUSTOM_LIBPATH_ERROR                          = GC_ERR_CUSTOM_ID -  6,  /**< Library path cannot be retrieved */
    GC_ERR_CUSTOM_DRIVER_NOT_AVAILABLE                   = GC_ERR_CUSTOM_ID -  7,  /**< Failed to access the driver */
    GC_ERR_CUSTOM_DRIVER_IO_ERROR                        = GC_ERR_CUSTOM_ID -  8,  /**< A driver error has occurred */
///@cond
    GC_ERR_CUSTOM_REVOKE_ERROR_FOLLOWING_ANNOUNCE_ERROR  = GC_ERR_CUSTOM_ID - 10,  /**< Buffer revoke error (No longer used) */
///@endcond
    GC_ERR_CUSTOM_STD_EXCEPTION                          = GC_ERR_CUSTOM_ID - 12,  /**< Caught a C++ std::exception */
    GC_ERR_CUSTOM_ALIGNMENT_ERROR                        = GC_ERR_CUSTOM_ID - 13,  /**< An alignment error occurred while accessing a module port */
    GC_ERR_CUSTOM_WAIT_FAILED                            = GC_ERR_CUSTOM_ID - 14,  /**< A wait operation failed unexpectedly */
    GC_ERR_CUSTOM_WAIT_INTERRUPTED                       = GC_ERR_CUSTOM_ID - 15,  /**< A wait operation has been interrupted */
    GC_ERR_CUSTOM_CANNOT_CREATE_NOTIFIER                 = GC_ERR_CUSTOM_ID - 16,  /**< Cannot create notification object */
    GC_ERR_CUSTOM_NOTIFIER_ERROR                         = GC_ERR_CUSTOM_ID - 17,  /**< Cannot access notification object state */
    GC_ERR_CUSTOM_LOADING_ERROR                          = GC_ERR_CUSTOM_ID - 18,  /**< An external module could not be loaded */
    GC_ERR_CUSTOM_SYMBOL_NOT_FOUND                       = GC_ERR_CUSTOM_ID - 19,  /**< A required symbol was not found */
    GC_ERR_CUSTOM_STRING_TOO_LONG                        = GC_ERR_CUSTOM_ID - 20,  /**< A string is too long */
    GC_ERR_CUSTOM_DATATYPE_MISMATCH                      = GC_ERR_CUSTOM_ID - 21,  /**< Data type mismatch */
    GC_ERR_CUSTOM_TOO_MANY_GENAPI_CONTEXTS               = GC_ERR_CUSTOM_ID - 22,  /**< Too many GenApi contexts */
    GC_ERR_CUSTOM_INCORRECT_OEM_SAFETY_KEY               = GC_ERR_CUSTOM_ID - 23,  /**< Key doesn't match programmed OEM safety key */
    GC_ERR_CUSTOM_OPAQUE_NETWORK                         = GC_ERR_CUSTOM_ID - 24,  /**< GEV Driver could not communicate with the camera */
    GC_ERR_CUSTOM_GENAPI_FEATURE_NOT_FOUND               = GC_ERR_CUSTOM_ID - 25,  /**< A GenApi feature could not be found */

    GC_ERR_CUSTOM_GENAPI_ERROR                           = GC_ERR_CUSTOM_ID - 100, /**< A GenApi error has occurred */
    GC_ERR_CUSTOM_IMAGE_ERROR                            = GC_ERR_CUSTOM_ID - 200, /**< An error occurred while processing an image */

    GC_ERR_CUSTOM_LICENSE_MANAGER_ERROR                  = GC_ERR_CUSTOM_ID - 300, /**< An error occurred in the License Manager */
    GC_ERR_CUSTOM_NO_LICENSE                             = GC_ERR_CUSTOM_ID - 301, /**< No license was found */

    /* Following codes are custom error codes returned by the driver */
///@cond
    GC_ERR_CUSTOM_IOCTL_BASE                             = -20000,                        // Reserved
///@endcond
    GC_ERR_CUSTOM_IOCTL_PCI_WRITE_CONFIG_FAILED          = GC_ERR_CUSTOM_IOCTL_BASE -  3, /**< Driver could not write to PCI configuration space of device */
    GC_ERR_CUSTOM_IOCTL_PCI_READ_CONFIG_FAILED           = GC_ERR_CUSTOM_IOCTL_BASE -  4, /**< Driver could not read from PCI configuration space of device */
    GC_ERR_CUSTOM_IOCTL_DS_CREATE_NO_DMA_ENGINE          = GC_ERR_CUSTOM_IOCTL_BASE -  5, /**< Driver could not find a DMA engine */
    GC_ERR_CUSTOM_IOCTL_DS_CREATE_NO_IRQ_HANDLER         = GC_ERR_CUSTOM_IOCTL_BASE -  6, /**< Driver has no IRQ handler */
    GC_ERR_CUSTOM_IOCTL_DS_REGISTER_EVENT_FAILED         = GC_ERR_CUSTOM_IOCTL_BASE -  7, /**< Driver could not register notification object while creating the data stream */
///@cond
    GC_ERR_CUSTOM_IOCTL_DS_GET_INFO_FAILED               = GC_ERR_CUSTOM_IOCTL_BASE -  8, // Reserved
///@endcond
    GC_ERR_CUSTOM_IOCTL_DS_OPEN_FAILED                   = GC_ERR_CUSTOM_IOCTL_BASE -  9, /**< Driver could not open DataStream module */
    GC_ERR_CUSTOM_IOCTL_DS_START_FAILED                  = GC_ERR_CUSTOM_IOCTL_BASE - 10, /**< Driver could not start the acquisition engine */
///@cond
    GC_ERR_CUSTOM_IOCTL_DS_STOP_FAILED                   = GC_ERR_CUSTOM_IOCTL_BASE - 11, // Reserved
    GC_ERR_CUSTOM_IOCTL_DEV_GET_INFO_FAILED              = GC_ERR_CUSTOM_IOCTL_BASE - 12, // Reserved
///@endcond
    GC_ERR_CUSTOM_IOCTL_DEV_GET_URL_INFO_FAILED          = GC_ERR_CUSTOM_IOCTL_BASE - 13, /**< Driver error while getting remote device URL information */
    GC_ERR_CUSTOM_IOCTL_CXP_CONNECTION_WRITE_FAILED      = GC_ERR_CUSTOM_IOCTL_BASE - 14, /**< Driver could not not write to remote device CoaXPress register */
    GC_ERR_CUSTOM_IOCTL_CXP_CONNECTION_READ_FAILED       = GC_ERR_CUSTOM_IOCTL_BASE - 15, /**< Driver could not not read remote device CoaXPress register */
///@cond
    GC_ERR_CUSTOM_IOCTL_CXP_MASTER_CONNECTION_NOT_FOUND  = GC_ERR_CUSTOM_IOCTL_BASE - 16, // Reserved
    GC_ERR_CUSTOM_IOCTL_CXP_HOST_LIBRARY_CMD_FAILED      = GC_ERR_CUSTOM_IOCTL_BASE - 17, // Reserved
///@endcond
    GC_ERR_CUSTOM_IOCTL_BUFFER_ANNOUNCE_FAILED           = GC_ERR_CUSTOM_IOCTL_BASE - 18, /**< Driver was not able to announce a buffer */
    GC_ERR_CUSTOM_IOCTL_STOP_TIMEOUT                     = GC_ERR_CUSTOM_IOCTL_BASE - 19, /**< Driver could not stop the acquisition engine in a timely fashion */
    GC_ERR_CUSTOM_IOCTL_I2C_FAILED                       = GC_ERR_CUSTOM_IOCTL_BASE - 20, /**< Driver I2C error */
    GC_ERR_CUSTOM_IOCTL_BANK_SELECT_INCONSISTENCY        = GC_ERR_CUSTOM_IOCTL_BASE - 21, /**< Driver detected a flash bank select inconsistency */
    GC_ERR_CUSTOM_IOCTL_ONBOARD_MEMORY_READ_ERROR        = GC_ERR_CUSTOM_IOCTL_BASE - 22, /**< Driver could not read from onboard memory */
    GC_ERR_CUSTOM_IOCTL_ONBOARD_MEMORY_WRITE_ERROR       = GC_ERR_CUSTOM_IOCTL_BASE - 23, /**< Driver could not write to onboard memory */
    GC_ERR_CUSTOM_IOCTL_FFC_WRITE_ERROR                  = GC_ERR_CUSTOM_IOCTL_BASE - 24, /**< Driver could not write Flat Field Correction coefficient data */

    /* Following codes are custom error codes returned by the GigE Vision module */
///@cond
    GC_ERR_CUSTOM_GEV_BASE                               = -65536,                        // Reserved [0xfffexxxx]
///@endcond
};

/** This enumeration defines Euresys custom commands to retrieve information with the GenTL::DevGetInfo function on a device handle. */
enum DEVICE_INFO_CUSTOM_CMD_LIST
{
    DEVICE_INFO_CUSTOM_DEVICE_DESCRIPTION           = DEVICE_INFO_CUSTOM_ID + 70, /**< STRING  Description of the device */
};

/** This enumeration defines Euresys custom commands to retrieve information with the GenTL::DSGetInfo function on a data stream handle. */
enum STREAM_INFO_CUSTOM_CMD_LIST
{
    STREAM_INFO_CUSTOM_STREAM_DESCRIPTION           = STREAM_INFO_CUSTOM_ID + 110, /**< STRING  Description of the stream */
};

/** This enumeration defines Euresys custom commands to retrieve information with the GenTL::DSGetBufferInfo function on a buffer handle. */
enum BUFFER_INFO_CUSTOM_CMD_LIST
{
    BUFFER_INFO_CUSTOM_START_OF_SCAN                = BUFFER_INFO_CUSTOM_ID +  1, /**< BOOL8   Flag to indicate that the buffer is the first of a scan */
    BUFFER_INFO_CUSTOM_END_OF_SCAN                  = BUFFER_INFO_CUSTOM_ID +  2, /**< BOOL8   Flag to indicate that the buffer is the last of a scan */
    BUFFER_INFO_CUSTOM_EVENT_TIMESTAMP              = BUFFER_INFO_CUSTOM_ID +  5, /**< UINT64  Timestamp of the new buffer event */
    BUFFER_INFO_CUSTOM_CXP_STREAMID                 = BUFFER_INFO_CUSTOM_ID +  6, /**< UINT16  Stream ID (word 3 in the CoaXPress rectangular image header) */
    BUFFER_INFO_CUSTOM_CXP_TAPG                     = BUFFER_INFO_CUSTOM_ID +  7, /**< UINT16  Tap geometry (words 23-24 in the CoaXPress rectangular image header) */
    BUFFER_INFO_CUSTOM_CXP_FLAGS                    = BUFFER_INFO_CUSTOM_ID +  8, /**< UINT16  Image flags (word 25 in the CoaXPress rectangular image header) */
    BUFFER_INFO_CUSTOM_LINE_PITCH                   = BUFFER_INFO_CUSTOM_ID + 10, /**< SIZET   Line pitch (or 1 for buffers containing JPEG data) */
    BUFFER_INFO_CUSTOM_STREAM_LINE_WIDTH            = BUFFER_INFO_CUSTOM_ID + 12, /**< SIZET   Value of data stream LineWidth when the buffer was announced */
    BUFFER_INFO_CUSTOM_STREAM_LINE_PITCH            = BUFFER_INFO_CUSTOM_ID + 13, /**< SIZET   Value of data stream LinePitch when the buffer was announced */
    BUFFER_INFO_CUSTOM_STREAM_STRIPE_HEIGHT         = BUFFER_INFO_CUSTOM_ID + 14, /**< SIZET   Value of data stream StripeHeight when the buffer was announced */
    BUFFER_INFO_CUSTOM_STREAM_STRIPE_PITCH          = BUFFER_INFO_CUSTOM_ID + 15, /**< SIZET   Value of data stream StripePitch when the buffer was announced */
    BUFFER_INFO_CUSTOM_STREAM_STRIPE_ARRANGEMENT    = BUFFER_INFO_CUSTOM_ID + 16, /**< UINT64  Value of data stream StripeArrangement when the buffer was announced */
    BUFFER_INFO_CUSTOM_CUSTOM_LOGIC_STATUS          = BUFFER_INFO_CUSTOM_ID + 19, /**< UINT32  CustomLogic Status associated to the buffer */
    BUFFER_INFO_CUSTOM_STREAM_STRIPE_OFFSET         = BUFFER_INFO_CUSTOM_ID + 20, /**< SIZET   Value of data stream StripeOffset when the buffer was announced */
    BUFFER_INFO_CUSTOM_STREAM_BLOCK_HEIGHT          = BUFFER_INFO_CUSTOM_ID + 21, /**< SIZET   Value of data stream BlockHeight when the buffer was announced */
    BUFFER_INFO_CUSTOM_LLE_LINES_PER_IMAGE          = BUFFER_INFO_CUSTOM_ID + 24, /**< SIZET   Number of LLE extracted lines per acquired image */
    BUFFER_INFO_CUSTOM_ALIGNMENT                    = BUFFER_INFO_CUSTOM_ID + 26, /**< SIZET   Alignment of buffer allocated by DSAllocAndAnnounceBuffer */
    BUFFER_INFO_CUSTOM_PART_SIZE                    = BUFFER_INFO_CUSTOM_ID + 27, /**< SIZET   Size of each buffer part */
    BUFFER_INFO_CUSTOM_NUM_PARTS                    = BUFFER_INFO_CUSTOM_ID + 28, /**< SIZET   Maximum number of parts in the buffer (value of data stream BufferPartCount when the buffer was announced) */
    BUFFER_INFO_CUSTOM_NUM_DELIVERED_PARTS          = BUFFER_INFO_CUSTOM_ID + 29, /**< SIZET   Number of parts available in the buffer */
    BUFFER_INFO_CUSTOM_PART_TIMESTAMPS              = BUFFER_INFO_CUSTOM_ID + 30, /**< BUFFER  64-bit array containing the timestamps of each buffer part */
};

/** @} */

/** @defgroup EuresysEvent Euresys Events
    Definition of Euresys Coaxlink-specific Events
    @ingroup CEuresysGenTL
    @ingroup Events
    @{
 **/

/** This enumeration defines Euresys custom events that can be registered with the GenTL::GCRegisterEvent function.
    For convenience, events that can be registered on the Interface module can also be registered on the child Device modules.
 **/
enum EVENT_TYPE_CUSTOM_LIST
{
    EVENT_CUSTOM_IO_TOOLBOX             = EVENT_CUSTOM_ID + 1,      /**< Notification of IO Toolbox events (on Interface module) */
    EVENT_CUSTOM_CIC                    = EVENT_CUSTOM_ID + 2,      /**< Notification of CIC events (on Device module) */
    EVENT_CUSTOM_DATASTREAM             = EVENT_CUSTOM_ID + 3,      /**< Notification of data stream events (on DataStream module) */
    EVENT_CUSTOM_CXP_INTERFACE          = EVENT_CUSTOM_ID + 4,      /**< Notification of CoaXPress events (on Interface module) */
    EVENT_CUSTOM_DEVICE_ERROR           = EVENT_CUSTOM_ID + 5,      /**< Notification of device error events (on Device module) */
    EVENT_CUSTOM_CXP_DEVICE             = EVENT_CUSTOM_ID + 6,      /**< Notification of CoaXPress device events (on Device module) */
};

/** This enumeration defines Euresys custom commands to retrieve information with the GenTL::EventGetDataInfo function.
 **/
enum EVENT_DATA_INFO_CUSTOM_CMD_LIST
{
    EVENT_DATA_CUSTOM_TIMESTAMP         = EVENT_DATA_CUSTOM_ID + 1, /**< UINT64   Timestamp of the event */
    EVENT_DATA_CUSTOM_CONTEXT_1         = EVENT_DATA_CUSTOM_ID + 2, /**< UINT32   Value of EventNotificationContext1 for this event (latched at the time the event occurred) */
    EVENT_DATA_CUSTOM_CONTEXT_2         = EVENT_DATA_CUSTOM_ID + 3, /**< UINT32   Value of EventNotificationContext2 for this event (latched at the time the event occurred) */
    EVENT_DATA_CUSTOM_CONTEXT_3         = EVENT_DATA_CUSTOM_ID + 4, /**< UINT32   Value of EventNotificationContext3 for this event (latched at the time the event occurred) */
};

#define EVENT_DATA_NUMID_CUSTOM_BASE 0x8000
/** This enumeration defines how to interpret the EVENT_DATA_NUMID for Euresys custom events.
 **/
enum EVENT_DATA_NUMID_CUSTOM_LIST
{
    // EVENT_CUSTOM_IO_TOOLBOX
    EVENT_DATA_NUMID_IO_TOOLBOX_LIN1                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x000,  /**< Line Input Tool 1 */
    EVENT_DATA_NUMID_IO_TOOLBOX_LIN2                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x001,  /**< Line Input Tool 2 */
    EVENT_DATA_NUMID_IO_TOOLBOX_LIN3                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x002,  /**< Line Input Tool 3 */
    EVENT_DATA_NUMID_IO_TOOLBOX_LIN4                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x003,  /**< Line Input Tool 4 */
    EVENT_DATA_NUMID_IO_TOOLBOX_LIN5                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x004,  /**< Line Input Tool 5 */
    EVENT_DATA_NUMID_IO_TOOLBOX_LIN6                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x005,  /**< Line Input Tool 6 */
    EVENT_DATA_NUMID_IO_TOOLBOX_LIN7                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x006,  /**< Line Input Tool 7 */
    EVENT_DATA_NUMID_IO_TOOLBOX_LIN8                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x007,  /**< Line Input Tool 8 */
    EVENT_DATA_NUMID_IO_TOOLBOX_QDC1                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x080,  /**< Quadrature Decoder Tool 1 */
    EVENT_DATA_NUMID_IO_TOOLBOX_QDC1_DIR                           = EVENT_DATA_NUMID_CUSTOM_BASE | 0x081,  /**< Quadrature Decoder Tool 1 Changed Direction */
    EVENT_DATA_NUMID_IO_TOOLBOX_QDC2                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x082,  /**< Quadrature Decoder Tool 2 */
    EVENT_DATA_NUMID_IO_TOOLBOX_QDC2_DIR                           = EVENT_DATA_NUMID_CUSTOM_BASE | 0x083,  /**< Quadrature Decoder Tool 2 Changed Direction */
    EVENT_DATA_NUMID_IO_TOOLBOX_QDC3                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x084,  /**< Quadrature Decoder Tool 3 */
    EVENT_DATA_NUMID_IO_TOOLBOX_QDC3_DIR                           = EVENT_DATA_NUMID_CUSTOM_BASE | 0x085,  /**< Quadrature Decoder Tool 3 Changed Direction */
    EVENT_DATA_NUMID_IO_TOOLBOX_QDC4                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x086,  /**< Quadrature Decoder Tool 4 */
    EVENT_DATA_NUMID_IO_TOOLBOX_QDC4_DIR                           = EVENT_DATA_NUMID_CUSTOM_BASE | 0x087,  /**< Quadrature Decoder Tool 4 Changed Direction */
    EVENT_DATA_NUMID_IO_TOOLBOX_DIV1                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x100,  /**< Divider Tool 1 */
    EVENT_DATA_NUMID_IO_TOOLBOX_DIV2                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x101,  /**< Divider Tool 2 */
    EVENT_DATA_NUMID_IO_TOOLBOX_DIV3                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x102,  /**< Divider Tool 3 */
    EVENT_DATA_NUMID_IO_TOOLBOX_DIV4                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x103,  /**< Divider Tool 4 */
    EVENT_DATA_NUMID_IO_TOOLBOX_MDV1                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x180,  /**< Multiplier/Divider Tool 1 */
    EVENT_DATA_NUMID_IO_TOOLBOX_MDV2                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x181,  /**< Multiplier/Divider Tool 2 */
    EVENT_DATA_NUMID_IO_TOOLBOX_MDV3                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x182,  /**< Multiplier/Divider Tool 3 */
    EVENT_DATA_NUMID_IO_TOOLBOX_MDV4                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x183,  /**< Multiplier/Divider Tool 4 */
    EVENT_DATA_NUMID_IO_TOOLBOX_DEL1_1                             = EVENT_DATA_NUMID_CUSTOM_BASE | 0x200,  /**< Delay Tool 1 Output 1 */
    EVENT_DATA_NUMID_IO_TOOLBOX_DEL1_2                             = EVENT_DATA_NUMID_CUSTOM_BASE | 0x201,  /**< Delay Tool 1 Output 2 */
    EVENT_DATA_NUMID_IO_TOOLBOX_DEL2_1                             = EVENT_DATA_NUMID_CUSTOM_BASE | 0x202,  /**< Delay Tool 2 Output 1 */
    EVENT_DATA_NUMID_IO_TOOLBOX_DEL2_2                             = EVENT_DATA_NUMID_CUSTOM_BASE | 0x203,  /**< Delay Tool 2 Output 2 */
    EVENT_DATA_NUMID_IO_TOOLBOX_DEL3_1                             = EVENT_DATA_NUMID_CUSTOM_BASE | 0x204,  /**< Delay Tool 3 Output 1 */
    EVENT_DATA_NUMID_IO_TOOLBOX_DEL3_2                             = EVENT_DATA_NUMID_CUSTOM_BASE | 0x205,  /**< Delay Tool 3 Output 2 */
    EVENT_DATA_NUMID_IO_TOOLBOX_DEL4_1                             = EVENT_DATA_NUMID_CUSTOM_BASE | 0x206,  /**< Delay Tool 4 Output 1 */
    EVENT_DATA_NUMID_IO_TOOLBOX_DEL4_2                             = EVENT_DATA_NUMID_CUSTOM_BASE | 0x207,  /**< Delay Tool 4 Output 2 */
    EVENT_DATA_NUMID_IO_TOOLBOX_USER_EVENT_1                       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x300,  /**< User Event 1 */
    EVENT_DATA_NUMID_IO_TOOLBOX_USER_EVENT_2                       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x301,  /**< User Event 2 */
    EVENT_DATA_NUMID_IO_TOOLBOX_USER_EVENT_3                       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x302,  /**< User Event 3 */
    EVENT_DATA_NUMID_IO_TOOLBOX_USER_EVENT_4                       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x303,  /**< User Event 4 */
    EVENT_DATA_NUMID_IO_TOOLBOX_C2C1                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x320,  /**< C2C-Link Synchronization Tool 1 */
    EVENT_DATA_NUMID_IO_TOOLBOX_C2C2                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x321,  /**< C2C-Link Synchronization Tool 2 */
    EVENT_DATA_NUMID_IO_TOOLBOX_C2C3                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x322,  /**< C2C-Link Synchronization Tool 3 */
    EVENT_DATA_NUMID_IO_TOOLBOX_EIN1                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x380,  /**< Event Input Tool 1 */
    EVENT_DATA_NUMID_IO_TOOLBOX_EIN2                               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x381,  /**< Event Input Tool 2 */
    // EVENT_CUSTOM_CXP_INTERFACE
    EVENT_DATA_NUMID_CXP_INTERFACE_CRC_ERROR_CXP_A                 = EVENT_DATA_NUMID_CUSTOM_BASE | 0x280,  /**< Detected CRC error on CXP connector A */
    EVENT_DATA_NUMID_CXP_INTERFACE_CRC_ERROR_CXP_B                 = EVENT_DATA_NUMID_CUSTOM_BASE | 0x281,  /**< Detected CRC error on CXP connector B */
    EVENT_DATA_NUMID_CXP_INTERFACE_CRC_ERROR_CXP_C                 = EVENT_DATA_NUMID_CUSTOM_BASE | 0x282,  /**< Detected CRC error on CXP connector C */
    EVENT_DATA_NUMID_CXP_INTERFACE_CRC_ERROR_CXP_D                 = EVENT_DATA_NUMID_CUSTOM_BASE | 0x283,  /**< Detected CRC error on CXP connector D */
    EVENT_DATA_NUMID_CXP_INTERFACE_CRC_ERROR_CXP_E                 = EVENT_DATA_NUMID_CUSTOM_BASE | 0x284,  /**< Detected CRC error on CXP connector E */
    EVENT_DATA_NUMID_CXP_INTERFACE_CRC_ERROR_CXP_F                 = EVENT_DATA_NUMID_CUSTOM_BASE | 0x285,  /**< Detected CRC error on CXP connector F */
    EVENT_DATA_NUMID_CXP_INTERFACE_CRC_ERROR_CXP_G                 = EVENT_DATA_NUMID_CUSTOM_BASE | 0x286,  /**< Detected CRC error on CXP connector G */
    EVENT_DATA_NUMID_CXP_INTERFACE_CRC_ERROR_CXP_H                 = EVENT_DATA_NUMID_CUSTOM_BASE | 0x287,  /**< Detected CRC error on CXP connector H */
    EVENT_DATA_NUMID_CXP_INTERFACE_CONNECTION_DETECTED_CXP_A       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4000, /**< Low level connection lock achieved on CXP connector A */
    EVENT_DATA_NUMID_CXP_INTERFACE_CONNECTION_DETECTED_CXP_B       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4001, /**< Low level connection lock achieved on CXP connector B */
    EVENT_DATA_NUMID_CXP_INTERFACE_CONNECTION_DETECTED_CXP_C       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4002, /**< Low level connection lock achieved on CXP connector C */
    EVENT_DATA_NUMID_CXP_INTERFACE_CONNECTION_DETECTED_CXP_D       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4003, /**< Low level connection lock achieved on CXP connector D */
    EVENT_DATA_NUMID_CXP_INTERFACE_CONNECTION_DETECTED_CXP_E       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4004, /**< Low level connection lock achieved on CXP connector E */
    EVENT_DATA_NUMID_CXP_INTERFACE_CONNECTION_DETECTED_CXP_F       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4005, /**< Low level connection lock achieved on CXP connector F */
    EVENT_DATA_NUMID_CXP_INTERFACE_CONNECTION_DETECTED_CXP_G       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4006, /**< Low level connection lock achieved on CXP connector G */
    EVENT_DATA_NUMID_CXP_INTERFACE_CONNECTION_DETECTED_CXP_H       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4007, /**< Low level connection lock achieved on CXP connector H */
    EVENT_DATA_NUMID_CXP_INTERFACE_CONNECTION_UNDETECTED_CXP_A     = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4010, /**< Low level connection lock lost on CXP connector A */
    EVENT_DATA_NUMID_CXP_INTERFACE_CONNECTION_UNDETECTED_CXP_B     = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4011, /**< Low level connection lock lost on CXP connector B */
    EVENT_DATA_NUMID_CXP_INTERFACE_CONNECTION_UNDETECTED_CXP_C     = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4012, /**< Low level connection lock lost on CXP connector C */
    EVENT_DATA_NUMID_CXP_INTERFACE_CONNECTION_UNDETECTED_CXP_D     = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4013, /**< Low level connection lock lost on CXP connector D */
    EVENT_DATA_NUMID_CXP_INTERFACE_CONNECTION_UNDETECTED_CXP_E     = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4014, /**< Low level connection lock lost on CXP connector E */
    EVENT_DATA_NUMID_CXP_INTERFACE_CONNECTION_UNDETECTED_CXP_F     = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4015, /**< Low level connection lock lost on CXP connector F */
    EVENT_DATA_NUMID_CXP_INTERFACE_CONNECTION_UNDETECTED_CXP_G     = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4016, /**< Low level connection lock lost on CXP connector G */
    EVENT_DATA_NUMID_CXP_INTERFACE_CONNECTION_UNDETECTED_CXP_H     = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4017, /**< Low level connection lock lost on CXP connector H */
    EVENT_DATA_NUMID_CXP_INTERFACE_DEVICE_0_READY                  = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4020, /**< CoaXPress link configuration done for Device 0 */
    EVENT_DATA_NUMID_CXP_INTERFACE_DEVICE_1_READY                  = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4021, /**< CoaXPress link configuration done for Device 1 */
    EVENT_DATA_NUMID_CXP_INTERFACE_DEVICE_2_READY                  = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4022, /**< CoaXPress link configuration done for Device 2 */
    EVENT_DATA_NUMID_CXP_INTERFACE_DEVICE_3_READY                  = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4023, /**< CoaXPress link configuration done for Device 3 */
    EVENT_DATA_NUMID_CXP_INTERFACE_DEVICE_4_READY                  = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4024, /**< CoaXPress link configuration done for Device 4 */
    EVENT_DATA_NUMID_CXP_INTERFACE_DEVICE_5_READY                  = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4025, /**< CoaXPress link configuration done for Device 5 */
    EVENT_DATA_NUMID_CXP_INTERFACE_DEVICE_6_READY                  = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4026, /**< CoaXPress link configuration done for Device 6 */
    EVENT_DATA_NUMID_CXP_INTERFACE_DEVICE_7_READY                  = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4027, /**< CoaXPress link configuration done for Device 7 */
    EVENT_DATA_NUMID_CXP_INTERFACE_DEVICE_0_LOST                   = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4030, /**< Device 0 disconnected */
    EVENT_DATA_NUMID_CXP_INTERFACE_DEVICE_1_LOST                   = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4031, /**< Device 1 disconnected */
    EVENT_DATA_NUMID_CXP_INTERFACE_DEVICE_2_LOST                   = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4032, /**< Device 2 disconnected */
    EVENT_DATA_NUMID_CXP_INTERFACE_DEVICE_3_LOST                   = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4033, /**< Device 3 disconnected */
    EVENT_DATA_NUMID_CXP_INTERFACE_DEVICE_4_LOST                   = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4034, /**< Device 4 disconnected */
    EVENT_DATA_NUMID_CXP_INTERFACE_DEVICE_5_LOST                   = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4035, /**< Device 5 disconnected */
    EVENT_DATA_NUMID_CXP_INTERFACE_DEVICE_6_LOST                   = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4036, /**< Device 6 disconnected */
    EVENT_DATA_NUMID_CXP_INTERFACE_DEVICE_7_LOST                   = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4037, /**< Device 7 disconnected */
    // EVENT_CUSTOM_CIC
    EVENT_DATA_NUMID_CIC_CAMERA_TRIGGER_RISING_EDGE                = EVENT_DATA_NUMID_CUSTOM_BASE | 0x41,   /**< Start of camera trigger */
    EVENT_DATA_NUMID_CIC_CAMERA_TRIGGER_FALLING_EDGE               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x42,   /**< End of camera trigger */
    EVENT_DATA_NUMID_CIC_STROBE_RISING_EDGE                        = EVENT_DATA_NUMID_CUSTOM_BASE | 0x43,   /**< Start of light strobe */
    EVENT_DATA_NUMID_CIC_STROBE_FALLING_EDGE                       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x44,   /**< End of light strobe */
    EVENT_DATA_NUMID_CIC_ALLOW_NEXT_CYCLE                          = EVENT_DATA_NUMID_CUSTOM_BASE | 0x45,   /**< CIC is ready for next camera cycle */
    EVENT_DATA_NUMID_CIC_DISCARDED_CIC_TRIGGER                     = EVENT_DATA_NUMID_CUSTOM_BASE | 0x46,   /**< Ignored CIC trigger because CIC is not ready for next camera cycle */
    EVENT_DATA_NUMID_CIC_PENDING_CIC_TRIGGER                       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x47,   /**< Delayed CIC trigger until CIC is ready for next camera cycle */
    EVENT_DATA_NUMID_CIC_CXP_TRIGGER_ACK                           = EVENT_DATA_NUMID_CUSTOM_BASE | 0x48,   /**< Received acknowledgement for previous CXP trigger message */
    EVENT_DATA_NUMID_CIC_CXP_TRIGGER_RESEND                        = EVENT_DATA_NUMID_CUSTOM_BASE | 0x49,   /**< Resent CXP trigger message (acknowledgement to previous CXP trigger message not received) */
    EVENT_DATA_NUMID_CIC_TRIGGER                                   = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4a,   /**< CIC trigger */
    // EVENT_CUSTOM_CXP_DEVICE
    EVENT_DATA_NUMID_CXP_DEVICE_LINK_TRIGGER                       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4d,   /**< LinkTrigger<N> received from CoaXPress device */
    // EVENT_CUSTOM_DATASTREAM
    EVENT_DATA_NUMID_DATASTREAM_START_OF_CAMERA_READOUT            = EVENT_DATA_NUMID_CUSTOM_BASE | 0x61,   /**< Starts acquiring data of a new image frame (area-scan only) */
    EVENT_DATA_NUMID_DATASTREAM_END_OF_CAMERA_READOUT              = EVENT_DATA_NUMID_CUSTOM_BASE | 0x62,   /**< Stops acquiring data of an image frame (area-scan only) */
    EVENT_DATA_NUMID_DATASTREAM_START_OF_SCAN                      = EVENT_DATA_NUMID_CUSTOM_BASE | 0x63,   /**< Starts acquiring data of a new image scan (line-scan only ) */
    EVENT_DATA_NUMID_DATASTREAM_END_OF_SCAN                        = EVENT_DATA_NUMID_CUSTOM_BASE | 0x64,   /**< Stops acquiring data of an image scan (line-scan only) */
    EVENT_DATA_NUMID_DATASTREAM_REJECTED_FRAME                     = EVENT_DATA_NUMID_CUSTOM_BASE | 0x65,   /**< Dropped image frame data (area-scan only) */
    EVENT_DATA_NUMID_DATASTREAM_REJECTED_SCAN                      = EVENT_DATA_NUMID_CUSTOM_BASE | 0x66,   /**< Dropped image scan data (line-scan only) */
    EVENT_DATA_NUMID_DATASTREAM_TRIGGER_TO_CAMERA_READOUT_TIMEOUT  = EVENT_DATA_NUMID_CUSTOM_BASE | 0x67,   /**< Trigger to camera readout timeout */
    EVENT_DATA_NUMID_DATASTREAM_CAMERA_READOUT_TIMEOUT             = EVENT_DATA_NUMID_CUSTOM_BASE | 0x68,   /**< Camera readout timeout */
    EVENT_DATA_NUMID_DATASTREAM_BROKEN_FRAME                       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x69,   /**< Broken frame due to frame store overflow (area-scan only) */
    // EVENT_CUSTOM_DEVICE_ERROR
    EVENT_DATA_NUMID_DEVICE_ERROR_STREAM_PACKET_SIZE_ERROR         = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4040, /**< Stream packet size error */
    EVENT_DATA_NUMID_DEVICE_ERROR_STREAM_PACKET_FIFO_OVERFLOW      = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4041, /**< Stream packet FIFO overflow */
    EVENT_DATA_NUMID_DEVICE_ERROR_CAMERA_TRIGGER_OVERRUN           = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4042, /**< New trigger sent to remote device even though readout of previous frame has not started yet */
    EVENT_DATA_NUMID_DEVICE_ERROR_DID_NOT_RECEIVE_TRIGGER_ACK      = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4043, /**< Trigger ignored because ACK to previous trigger has not been received yet */
    EVENT_DATA_NUMID_DEVICE_ERROR_TRIGGER_PACKET_RETRY_ERROR       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4044, /**< Trigger packet resend not successful */
    EVENT_DATA_NUMID_DEVICE_ERROR_INPUT_STREAM_FIFO_HALF_FULL      = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4045, /**< Input stream FIFO half full */
    EVENT_DATA_NUMID_DEVICE_ERROR_INPUT_STREAM_FIFO_FULL           = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4046, /**< Input stream FIFO full */
    EVENT_DATA_NUMID_DEVICE_ERROR_IMAGE_HEADER_ERROR               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4047, /**< Image header error */
    EVENT_DATA_NUMID_DEVICE_ERROR_MIG_AXI_WRITE_ERROR              = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4048, /**< MIG AXI write error */
    EVENT_DATA_NUMID_DEVICE_ERROR_MIG_AXI_READ_ERROR               = EVENT_DATA_NUMID_CUSTOM_BASE | 0x4049, /**< MIG AXI read error */
    EVENT_DATA_NUMID_DEVICE_ERROR_PACKET_WITH_UNEXPECTED_TAG       = EVENT_DATA_NUMID_CUSTOM_BASE | 0x404A, /**< Received a CXP packet with unexpected tag */
    EVENT_DATA_NUMID_DEVICE_ERROR_FILL_LEVEL_ABOVE_IL_SOS_REJECTED = EVENT_DATA_NUMID_CUSTOM_BASE | 0x404B, /**< Start of scan skipped (caused by internal exception: frame store almost full) */
    EVENT_DATA_NUMID_DEVICE_ERROR_FILL_LEVEL_ABOVE_AF_EARLY_EOS    = EVENT_DATA_NUMID_CUSTOM_BASE | 0x404C, /**< End of scan (caused by internal exception: frame store almost full) */
    EVENT_DATA_NUMID_DEVICE_ERROR_EXTERNAL_TRIGGER_REQS_TOO_CLOSE  = EVENT_DATA_NUMID_CUSTOM_BASE | 0x404D, /**< External trigger requests too close together */
};
typedef unsigned long long EVENT_DATA_NUMID_CUSTOM;

/** Value of EventSpecific for GenTL::EuresysCustomGenTL::EVENT_DATA_NUMID_CIC_DISCARDED_CIC_TRIGGER
    is a bitfield that can be interpreted according to the following definitions.
    @warning Undocumented bits must be ignored.
 **/
enum EVENT_SPECIFIC_CIC_DISCARDED_CIC_TRIGGER_LIST
{
    EVENT_SPECIFIC_CIC_DISCARDED_CIC_TRIGGER_IMAGE_BUFFER_FULL      = 0x01, /**< Cf. EVENT_SPECIFIC_CIC_DISCARDED_CIC_TRIGGER_FRAME_STORE_FULL */
    EVENT_SPECIFIC_CIC_DISCARDED_CIC_TRIGGER_FRAME_STORE_FULL       = 0x01, /**< Cause: frame store is full */
    EVENT_SPECIFIC_CIC_DISCARDED_CIC_TRIGGER_CYCLE_NOT_COMPLETED    = 0x02, /**< Cause: camera cycle not complete */
    EVENT_SPECIFIC_CIC_DISCARDED_CIC_TRIGGER_MAX_PENDING_TRIGGER    = 0x04, /**< Cause: maximum number of pending triggers already reached */
    EVENT_SPECIFIC_CIC_DISCARDED_CIC_TRIGGER_DATASTREAM_IDLE        = 0x08, /**< Cause: data stream is not active */
};

/** Value of EventSpecific for GenTL::EuresysCustomGenTL::EVENT_DATA_NUMID_CIC_PENDING_CIC_TRIGGER
    is a bitfield that can be interpreted according to the following definitions. 
    @warning Undocumented bits must be ignored.
 **/
enum EVENT_SPECIFIC_CIC_PENDING_CIC_TRIGGER_LIST
{
    EVENT_SPECIFIC_CIC_PENDING_CIC_TRIGGER_IMAGE_BUFFER_FULL        = 0x01, /**< Cf. EVENT_SPECIFIC_CIC_PENDING_CIC_TRIGGER_FRAME_STORE_FULL */
    EVENT_SPECIFIC_CIC_PENDING_CIC_TRIGGER_FRAME_STORE_FULL         = 0x01, /**< Cause: frame store is full */
    EVENT_SPECIFIC_CIC_PENDING_CIC_TRIGGER_CYCLE_NOT_COMPLETED      = 0x02, /**< Cause: camera cycle not complete */
};

/** Value of EventSpecific for GenTL::EuresysCustomGenTL::EVENT_DATA_NUMID_DATASTREAM_START_OF_SCAN
    is a bitfield that can be interpreted according to the following definitions. 
    @warning Undocumented bits must be ignored.
 **/
enum EVENT_SPECIFIC_DATASTREAM_START_OF_SCAN_LIST
{
    EVENT_SPECIFIC_DATASTREAM_START_OF_SCAN_SOFTWARE_TRIGGER        = 0x02, /**< Cause: software trigger */
    EVENT_SPECIFIC_DATASTREAM_START_OF_SCAN_HARDWARE_TRIGGER        = 0x04, /**< Cause: hardware trigger */
    EVENT_SPECIFIC_DATASTREAM_START_OF_SCAN_IMMEDIATE               = 0x08, /**< Cause: DSStartAcquisition or end of previous scan */
};

/** Value of EventSpecific for GenTL::EuresysCustomGenTL::EVENT_DATA_NUMID_DATASTREAM_END_OF_SCAN
    is a bitfield that can be interpreted according to the following definitions. 
    @warning Undocumented bits must be ignored.
 **/
enum EVENT_SPECIFIC_DATASTREAM_END_OF_SCAN_LIST
{
    EVENT_SPECIFIC_DATASTREAM_END_OF_SCAN_SOFTWARE_TRIGGER          = 0x02, /**< Cause: software trigger */
    EVENT_SPECIFIC_DATASTREAM_END_OF_SCAN_HARDWARE_TRIGGER          = 0x04, /**< Cause: hardware trigger */
    EVENT_SPECIFIC_DATASTREAM_END_OF_SCAN_AUTOMATIC                 = 0x08, /**< Cause: reached scan length */
    EVENT_SPECIFIC_DATASTREAM_END_OF_SCAN_DS_STOP_ACQUISITION       = 0x10, /**< Cause: DSStopAcquisition */
    EVENT_SPECIFIC_DATASTREAM_END_OF_SCAN_IMAGE_BUFFER_ALMOST_FULL  = 0x20, /**< Cf. EVENT_SPECIFIC_DATASTREAM_END_OF_SCAN_FRAME_STORE_ALMOST_FULL */
    EVENT_SPECIFIC_DATASTREAM_END_OF_SCAN_FRAME_STORE_ALMOST_FULL   = 0x20, /**< Cause: internal exception (frame store almost full) */
};

/** Value of EventSpecific for GenTL::EuresysCustomGenTL::EVENT_DATA_NUMID_DATASTREAM_REJECTED_FRAME
    is a bitfield that can be interpreted according to the following definitions. 
    @warning Undocumented bits must be ignored.
 **/
enum EVENT_SPECIFIC_DATASTREAM_REJECTED_FRAME_LIST
{
    EVENT_SPECIFIC_DATASTREAM_REJECTED_FRAME_IMAGE_BUFFER_FULL      = 0x01, /**< Cf. EVENT_SPECIFIC_DATASTREAM_REJECTED_FRAME_FRAME_STORE_FULL */
    EVENT_SPECIFIC_DATASTREAM_REJECTED_FRAME_FRAME_STORE_FULL       = 0x01, /**< Cause: frame store is full */
    EVENT_SPECIFIC_DATASTREAM_REJECTED_FRAME_DATASTREAM_IDLE        = 0x02, /**< Cause: data stream is not active */
    EVENT_SPECIFIC_DATASTREAM_REJECTED_FRAME_FRAME_STORE_OVERFLOW   = 0x04, /**< Cause: frame store underwent overflow */
};

/** Value of EventSpecific for GenTL::EuresysCustomGenTL::EVENT_DATA_NUMID_CXP_DEVICE_LINK_TRIGGER
    is a bitfield that can be interpreted according to the following definition.
    @warning Undocumented bits must be ignored.
 **/
typedef struct S_EVENT_SPECIFIC_CXP_DEVICE_LINK_TRIGGER
{
    unsigned char LinkTriggerN;   /**< Word 2 of the CoaXPress 2.0 high speed connection trigger packet */
    unsigned char Delay;          /**< Word 1 of the CoaXPress 2.0 high speed connection trigger packet */
    unsigned char Reserved[2];
} EVENT_SPECIFIC_CXP_DEVICE_LINK_TRIGGER;

/** @} */

/** @ingroup CEuresysGenTL
    @{
 **/

#if !defined(EURESYS_GENTL_INCLUDE_ONLY_ENUM_DEFS)
GC_API   EuresysDSAnnounceBusBuffer  (DS_HANDLE hDataStream, uint64_t busAddress, size_t iSize, void *reserved, void *pPrivate, BUFFER_HANDLE *phBuffer);
/** @cond */
GC_API_P(PEuresysDSAnnounceBusBuffer)(DS_HANDLE hDataStream, uint64_t busAddress, size_t iSize, void *reserved, void *pPrivate, BUFFER_HANDLE *phBuffer);
/** @endcond */

typedef struct S_EURESYS_EVENT_GET_DATA_ENTRY
{
    EVENT_HANDLE hEvent;    /**< event handle to wait for */
    void        *pBuffer;   /**< buffer to receive event data */
    size_t      *piSize;    /**< size of buffer pointed to by pBuffer */
    int          bGotData;  /**< flag updated by EuresysEventsGetData indicating whether data was received */
} EURESYS_EVENT_GET_DATA_ENTRY;

/** Wait for multiple events
    @param pEntries definition of events to wait for
    @param iNumEntries number of entries in pEntries
    @param piTimeout optional timeout to wait for events
    @parblock
    - if piTimeout is NULL, EuresysEventsGetData updates pEntries with currently available events data,
      it returns GC_ERR_SUCCESS even if no data was available
    - if *piTimeout == 0, EuresysEventsGetData updates pEntries with currently available events data,
      it returns GC_ERR_TIMEOUT if no data was available
    - if *piTimeout != 0, EuresysEventsGetData waits for one or more events to be notified and returns
      available data for any event at that time, it returns GC_ERR_TIMEOUT if no data was available
      before the timeout expires (returns same errors as GenTL::EventGetData)
      @endparblock
    @note pEntries[i].bGotData tells whether data was received for event pEntries[i].hEvent
 */
GC_API   EuresysEventsGetData  (EURESYS_EVENT_GET_DATA_ENTRY *pEntries, size_t iNumEntries, uint64_t *piTimeout);
/** @cond */
GC_API_P(PEuresysEventsGetData)(EURESYS_EVENT_GET_DATA_ENTRY *pEntries, size_t iNumEntries, uint64_t *piTimeout);
GC_API   EuresysEventKill      (EVENT_HANDLE hEvent);
GC_API_P(PEuresysEventKill    )(EVENT_HANDLE hEvent);
/** @endcond */
#endif

/** @} */

#if !defined(EURESYS_GENTL_INCLUDE_ONLY_ENUM_DEFS)
#ifdef __cplusplus
}
}
}
#endif
#endif

#endif
