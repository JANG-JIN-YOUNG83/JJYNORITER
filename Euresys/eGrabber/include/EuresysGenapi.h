/* Copyright Euresys 2014 */

#ifndef EURESYS_GENAPI_HEADER_FILE
#define EURESYS_GENAPI_HEADER_FILE

/** @file
    Header file for Euresys GenApi implementation
 **/

#if !defined(GC_IMPORT_EXPORT) || !defined(GC_CALLTYPE)
#error Include GenTL header file before EuresysGenapi.h
#endif

#define GENAPI_API GC_IMPORT_EXPORT GC_ERROR GC_CALLTYPE

#ifndef GC_USER_DEFINED_TYPES
#  if defined(_WIN32)
#    if !defined _STDINT_H && !defined _STDINT
        typedef __int64 int64_t;
#    endif
#  endif
#endif /* GC_DEFINE_TYPES */

#ifdef __cplusplus
extern "C" {
namespace GenTL {
namespace EuresysCustomGenTL {
#endif

enum GENAPI_ERROR_CODE_LIST {
    GENAPI_ERR_SUCCESS                                  = 0,    /**< "No error"                                             []                    */
    GENAPI_ERR_UNKNOWN_ERROR                            = 1,    /**< "An unknown error occurred"                            []                    */
    GENAPI_ERR_NODE_NOT_FOUND                           = 2,    /**< "%1 not found"                                         [node]                */
    GENAPI_ERR_NO_INTERFACE                             = 3,    /**< "%1 has no %2 interface"                               [node, interface]     */
    GENAPI_ERR_ACCESS_READ_ONLY                         = 4,    /**< "%1 is read-only"                                      [node]                */
    GENAPI_ERR_ACCESS_WRITE_ONLY                        = 5,    /**< "%1 is write-only"                                     [node]                */
    GENAPI_ERR_ACCESS_NOT_AVAILABLE                     = 6,    /**< "%1 is not available"                                  [node]                */
    GENAPI_ERR_ACCESS_NOT_IMPLEMENTED                   = 7,    /**< "%1 is not implemented"                                [node]                */
    GENAPI_ERR_ACCESS_LOCKED                            = 8,    /**< "%1 is locked"                                         [node]                */
    GENAPI_ERR_XML_ELEMENT_NOT_FOUND                    = 9,    /**< "XML element %2 not found (%1)"                        [node, element]       */
    GENAPI_ERR_XML_ATTRIBUTE_NOT_FOUND                  = 10,   /**< "XML attribute %2 not found (%1)"                      [node, attribute]     */
    GENAPI_ERR_PORT_NOT_FOUND                           = 11,   /**< "Port %2 not found (%1)"                               [node, port]          */
    GENAPI_ERR_PORT_IO_FAILURE                          = 12,   /**< "Port %2 error (%1: %3)"                               [node, port, message] */
    GENAPI_ERR_REGISTER_TOO_SMALL                       = 13,   /**< "Register too small (%1)"                              [node]                */
    GENAPI_ERR_REGISTER_LENGTH_MISMATCH                 = 14,   /**< "Register length mismatch (%1)"                        [node]                */
    GENAPI_ERR_XML_PARSING_FAILURE                      = 15,   /**< "Invalid XML"                                          []                    */
    GENAPI_ERR_REGISTER_DESCRIPTION_NOT_FOUND           = 16,   /**< "Register description not found"                       []                    */
    GENAPI_ERR_NODE_TYPE_NOT_IMPLEMENTED                = 17,   /**< "Node type %2 not implemented (%1)"                    [node, nodeType]      */
    GENAPI_ERR_XML_ELEMENT_NOT_SUPPORTED                = 18,   /**< "XML element %2 not supported (%1)"                    [node, element]       */
    GENAPI_ERR_ENUM_ENTRY_NOT_FOUND                     = 19,   /**< "Entry %2 not found (%1)"                              [node, enumEntry]     */
    GENAPI_ERR_INVALID_BOOLEAN_VALUE                    = 20,   /**< "Invalid boolean value %2 (%1)"                        [node, value]         */
    GENAPI_ERR_INVALID_ENUMERATION_INT_VALUE            = 21,   /**< "Invalid enumeration integer value %2 (%1)"            [node, value]         */
    GENAPI_ERR_INVALID_ENUMERATION_STRING_VALUE         = 22,   /**< "Invalid enumeration value %2 (%1)"                    [node, value]         */
    GENAPI_ERR_UNAVAILABLE_ENUMERATION_STRING_VALUE     = 23,   /**< "Entry %2 is not available (%1)"                       [node, value]         */
    GENAPI_ERR_INVALID_NODE_NAME                        = 24,   /**< "Invalid node name (%1)"                               [name]                */
    GENAPI_ERR_INVALID_SELECTORS                        = 25,   /**< "Invalid selector (%1)"                                [node]                */
    GENAPI_ERR_UNKNOWN_PROPERTY_NAME                    = 26,   /**< "Unknown property %2 (%1)"                             [node, property]      */
    GENAPI_ERR_PROPERTY_NOT_DEFINED                     = 27,   /**< "Property %2 not defined (%1)"                         [node, property]      */
    GENAPI_ERR_EVALUATION_FAILURE                       = 28,   /**< "Error while evaluating %1 (%2)"                       [node, message]       */
    GENAPI_ERR_XML_VALUE_PARSING_FAILURE                = 29,   /**< "Could not parse %2 (%1)"                              [node, value]         */
    GENAPI_ERR_XML_UNKNOWN_VALUE                        = 30,   /**< "Unknown value %3 while parsing %2 (%1)"               [node, name, value]   */
    GENAPI_ERR_CANNOT_COERCE_TO_INTEGER                 = 31,   /**< "Cannot coerce %1 to integer"                          [node]                */
    GENAPI_ERR_CANNOT_COERCE_TO_FLOAT                   = 32,   /**< "Cannot coerce %1 to float"                            [node]                */
    GENAPI_ERR_CANNOT_COERCE_TO_STRING                  = 33,   /**< "Cannot coerce %1 to string"                           [node]                */
    GENAPI_ERR_CANNOT_COERCE_TO_BOOLEAN                 = 34,   /**< "Cannot coerce %1 to boolean"                          [node]                */
    GENAPI_ERR_CANNOT_COERCE_TO_COMMAND                 = 35,   /**< "Cannot coerce %1 to command"                          [node]                */
    GENAPI_ERR_CANNOT_COERCE_TO_ENUMERATION             = 36,   /**< "Cannot coerce %1 to enumeration"                      [node]                */
    GENAPI_ERR_CANNOT_COERCE_FROM_INTEGER               = 37,   /**< "Cannot coerce %1 from integer"                        [node]                */
    GENAPI_ERR_CANNOT_COERCE_FROM_FLOAT                 = 38,   /**< "Cannot coerce %1 from float"                          [node]                */
    GENAPI_ERR_CANNOT_COERCE_FROM_STRING                = 39,   /**< "Cannot coerce %1 from string"                         [node]                */
    GENAPI_ERR_CANNOT_COERCE_FROM_BOOLEAN               = 40,   /**< "Cannot coerce %1 from boolean"                        [node]                */
    GENAPI_ERR_CANNOT_COERCE_FROM_COMMAND               = 41,   /**< "Cannot coerce %1 from command"                        [node]                */
    GENAPI_ERR_CANNOT_COERCE_FROM_ENUMERATION           = 42,   /**< "Cannot coerce %1 from enumeration"                    [node]                */
    GENAPI_ERR_UNKNOWN_ERROR_ON_NODE                    = 43,   /**< "An unknown error occurred (%1)"                       [node]                */
    GENAPI_ERR_INVALID_AT_COMMAND_MATCH_SYNTAX          = 44,   /**< "Invalid match syntax (%1)"                            [node]                */
    GENAPI_ERR_INVALID_AT_COMMAND_ARGUMENT              = 45,   /**< "Invalid argument (%1)"                                [node]                */
    GENAPI_ERR_UNKNOWN_CONFIG_NAME                      = 46,   /**< "Unknown configuration (%1)"                           [name]                */
    GENAPI_ERR_BAD_CONFIG_TYPE                          = 47,   /**< "Bad configuration type (%1)"                          [name]                */
    GENAPI_ERR_FLOAT_VALUE_TOO_SMALL                    = 48,   /**< "%1 cannot be smaller than %2"                         [node, value]         */
    GENAPI_ERR_FLOAT_VALUE_TOO_LARGE                    = 49,   /**< "%1 cannot be greater than %2"                         [node, value]         */
    GENAPI_ERR_INVALID_INT_INCREMENT                    = 50,   /**< "%1 has invalid increment value %2"                    [node, value]         */
    GENAPI_ERR_INT_VALUE_TOO_SMALL                      = 51,   /**< "%1 cannot be smaller than %2"                         [node, value]         */
    GENAPI_ERR_INT_VALUE_TOO_LARGE                      = 52,   /**< "%1 cannot be greater than %2"                         [node, value]         */
    GENAPI_ERR_INT_VALUE_NOT_IN_LINE_WITH_INCREMENT     = 53,   /**< "%1 not in line with increment value %2"               [node, value]         */
    GENAPI_ERR_VAR_ALREADY_EXISTS                       = 54,   /**< "Variable already exists (%1)"                         [variable]            */
    GENAPI_ERR_INVALID_VAR_NAME                         = 55,   /**< "Invalid variable name (%1)"                           [variable]            */
    GENAPI_ERR_FLOAT_TO_INTEGER_OVERFLOW                = 56,   /**< "Overflow in float-to-integer conversion of %2 (%1)"   [node, value]         */
    GENAPI_ERR_INTERNAL_EXCEPTION                       = 57,   /**< "An internal exception occurred (%1)"                  [message]             */
    GENAPI_ERR_INTERNAL_EXCEPTION_ON_NODE               = 58,   /**< "An internal exception occurred (%1: %2)"              [node, message]       */
    GENAPI_ERR_NODE_ERROR                               = 59,   /**< "%2 (%1)"                                              [node, message]       */
    GENAPI_ERR_MAX_RECURSION_DEPTH_EXCEEDED             = 60,   /**< "Maximum recursion depth exceeded"                     []                    */
};
typedef int32_t GENAPI_ERROR_CODE;

enum GENAPI_ERROR_PARAMETER_TYPE_LIST {
    GENAPI_ERROR_PARAMETER_TYPE_STRING  = 0,
    GENAPI_ERROR_PARAMETER_TYPE_INTEGER = 1,
    GENAPI_ERROR_PARAMETER_TYPE_FLOAT   = 2,
};
typedef uint32_t GENAPI_ERROR_PARAMETER_TYPE;
typedef uint32_t GENAPI_ERROR_PARAMETER_RESERVED;

typedef union U_GENAPI_ERROR_PARAMETER_VALUE {
    const char *string;
    int64_t     integer;
    double      floatingPoint;
} GENAPI_ERROR_PARAMETER_VALUE;

#pragma pack (push, 1)
typedef struct S_GENAPI_ERROR_PARAMETER {
    GENAPI_ERROR_PARAMETER_TYPE type;
    GENAPI_ERROR_PARAMETER_RESERVED reserved;
    GENAPI_ERROR_PARAMETER_VALUE value;
} GENAPI_ERROR_PARAMETER;
#pragma pack (pop)

#pragma pack (push, 1)
typedef struct S_GENAPI_ERROR_INFO {
    GENAPI_ERROR_CODE errorCode;
    uint32_t parameterCount;
    GENAPI_ERROR_PARAMETER parameters[1];
} GENAPI_ERROR_INFO;
#pragma pack (pop)

/** @defgroup CGenApi GenApi (C API)
    @ingroup ECAPI
    Euresys GenApi low-level C API
    @{
 **/

GENAPI_API GenapiSetString(PORT_HANDLE h, const char *sFeature, const char *sValue);
GENAPI_API GenapiGetString(PORT_HANDLE h, const char *sFeature, char *sValue, size_t *piSize);
GENAPI_API GenapiSetInteger(PORT_HANDLE h, const char *sFeature, int64_t iValue);
GENAPI_API GenapiGetInteger(PORT_HANDLE h, const char *sFeature, int64_t *piValue);
GENAPI_API GenapiSetFloat(PORT_HANDLE h, const char *sFeature, double dValue);
GENAPI_API GenapiGetFloat(PORT_HANDLE h, const char *sFeature, double *pdValue);
GENAPI_API GenapiExecuteCommand(PORT_HANDLE h, const char *sFeature);
GENAPI_API GenapiRunScript(const PORT_HANDLE *hPorts, size_t iNumPorts, const char *sScript, size_t reserved1, void *reserved2);
GENAPI_API GenapiGetLastError(GENAPI_ERROR_INFO *pInfo, size_t *piSize);

/** @cond */

/* typedefs for dynamic loading */
#define GENAPI_API_P(function) typedef GC_ERROR( GC_CALLTYPE *function )

GENAPI_API_P(PGenapiSetString)(PORT_HANDLE h, const char *sFeature, const char *sValue);
GENAPI_API_P(PGenapiGetString)(PORT_HANDLE h, const char *sFeature, char *sValue, size_t *piSize);
GENAPI_API_P(PGenapiSetInteger)(PORT_HANDLE h, const char *sFeature, int64_t iValue);
GENAPI_API_P(PGenapiGetInteger)(PORT_HANDLE h, const char *sFeature, int64_t *piValue);
GENAPI_API_P(PGenapiSetFloat)(PORT_HANDLE h, const char *sFeature, double dValue);
GENAPI_API_P(PGenapiGetFloat)(PORT_HANDLE h, const char *sFeature, double *pdValue);
GENAPI_API_P(PGenapiExecuteCommand)(PORT_HANDLE h, const char *sFeature);
GENAPI_API_P(PGenapiRunScript)(const PORT_HANDLE *hPorts, size_t iNumPorts, const char *sScript, size_t reserved1, void *reserved2);
GENAPI_API_P(PGenapiGetLastError)(GENAPI_ERROR_INFO *pInfo, size_t *piSize);

/** @endcond */


/** @} */

#ifdef __cplusplus
}
}
}
#endif

#endif
