/* Copyright Euresys 2019 */

#ifndef EURESYS_GENAPI_ERRORS_HEADER_FILE
#define EURESYS_GENAPI_ERRORS_HEADER_FILE

/** @file
    Header file for Euresys GenApi Error Descriptions (Optional)
 **/

#ifdef __cplusplus
extern "C" {
namespace GenTL {
namespace EuresysCustomGenTL {
#endif

/** @defgroup CGenApi GenApi (C API)
    @ingroup ECAPI
    Euresys GenApi helper functions to create error description formats
    @{
 **/

typedef struct S_GENAPI_ERROR_FORMAT {
    const char *format;
    uint32_t parameterCount;
} GENAPI_ERROR_FORMAT;

static GENAPI_ERROR_FORMAT MkGenapiErrorFormat(const char *format, uint32_t parameterCount) {
    GENAPI_ERROR_FORMAT f = { format, parameterCount };
    return f;
}

static GENAPI_ERROR_FORMAT GenapiErrorFormat(GENAPI_ERROR_CODE code) {
    switch (code) {
        case GENAPI_ERR_CANNOT_COERCE_TO_COMMAND:
            return MkGenapiErrorFormat("Cannot coerce %1 to command", 1); /* [node] */
        case GENAPI_ERR_INT_VALUE_NOT_IN_LINE_WITH_INCREMENT:
            return MkGenapiErrorFormat("%1 not in line with increment value %2", 2); /* [node, value] */
        case GENAPI_ERR_UNKNOWN_ERROR_ON_NODE:
            return MkGenapiErrorFormat("An unknown error occurred (%1)", 1); /* [node] */
        case GENAPI_ERR_ACCESS_READ_ONLY:
            return MkGenapiErrorFormat("%1 is read-only", 1); /* [node] */
        case GENAPI_ERR_MAX_RECURSION_DEPTH_EXCEEDED:
            return MkGenapiErrorFormat("Maximum recursion depth exceeded", 0); /* [] */
        case GENAPI_ERR_CANNOT_COERCE_FROM_FLOAT:
            return MkGenapiErrorFormat("Cannot coerce %1 from float", 1); /* [node] */
        case GENAPI_ERR_CANNOT_COERCE_TO_BOOLEAN:
            return MkGenapiErrorFormat("Cannot coerce %1 to boolean", 1); /* [node] */
        case GENAPI_ERR_INVALID_AT_COMMAND_MATCH_SYNTAX:
            return MkGenapiErrorFormat("Invalid match syntax (%1)", 1); /* [node] */
        case GENAPI_ERR_INVALID_ENUMERATION_STRING_VALUE:
            return MkGenapiErrorFormat("Invalid enumeration value %2 (%1)", 2); /* [node, value] */
        case GENAPI_ERR_CANNOT_COERCE_FROM_COMMAND:
            return MkGenapiErrorFormat("Cannot coerce %1 from command", 1); /* [node] */
        case GENAPI_ERR_CANNOT_COERCE_TO_ENUMERATION:
            return MkGenapiErrorFormat("Cannot coerce %1 to enumeration", 1); /* [node] */
        case GENAPI_ERR_CANNOT_COERCE_FROM_BOOLEAN:
            return MkGenapiErrorFormat("Cannot coerce %1 from boolean", 1); /* [node] */
        case GENAPI_ERR_SUCCESS:
            return MkGenapiErrorFormat("No error", 0); /* [] */
        case GENAPI_ERR_XML_VALUE_PARSING_FAILURE:
            return MkGenapiErrorFormat("Could not parse %2 (%1)", 2); /* [node, value] */
        case GENAPI_ERR_REGISTER_DESCRIPTION_NOT_FOUND:
            return MkGenapiErrorFormat("Register description not found", 0); /* [] */
        case GENAPI_ERR_REGISTER_TOO_SMALL:
            return MkGenapiErrorFormat("Register too small (%1)", 1); /* [node] */
        case GENAPI_ERR_PORT_IO_FAILURE:
            return MkGenapiErrorFormat("Port %2 error (%1: %3)", 3); /* [node, port, message] */
        case GENAPI_ERR_INVALID_AT_COMMAND_ARGUMENT:
            return MkGenapiErrorFormat("Invalid argument (%1)", 1); /* [node] */
        case GENAPI_ERR_ACCESS_NOT_IMPLEMENTED:
            return MkGenapiErrorFormat("%1 is not implemented", 1); /* [node] */
        case GENAPI_ERR_CANNOT_COERCE_FROM_ENUMERATION:
            return MkGenapiErrorFormat("Cannot coerce %1 from enumeration", 1); /* [node] */
        case GENAPI_ERR_NODE_TYPE_NOT_IMPLEMENTED:
            return MkGenapiErrorFormat("Node type %2 not implemented (%1)", 2); /* [node, nodeType] */
        case GENAPI_ERR_INVALID_NODE_NAME:
            return MkGenapiErrorFormat("Invalid node name (%1)", 1); /* [name] */
        case GENAPI_ERR_CANNOT_COERCE_TO_STRING:
            return MkGenapiErrorFormat("Cannot coerce %1 to string", 1); /* [node] */
        case GENAPI_ERR_CANNOT_COERCE_TO_INTEGER:
            return MkGenapiErrorFormat("Cannot coerce %1 to integer", 1); /* [node] */
        case GENAPI_ERR_ACCESS_WRITE_ONLY:
            return MkGenapiErrorFormat("%1 is write-only", 1); /* [node] */
        case GENAPI_ERR_UNAVAILABLE_ENUMERATION_STRING_VALUE:
            return MkGenapiErrorFormat("Entry %2 is not available (%1)", 2); /* [node, value] */
        case GENAPI_ERR_INT_VALUE_TOO_SMALL:
            return MkGenapiErrorFormat("%1 cannot be smaller than %2", 2); /* [node, value] */
        case GENAPI_ERR_FLOAT_VALUE_TOO_LARGE:
            return MkGenapiErrorFormat("%1 cannot be greater than %2", 2); /* [node, value] */
        case GENAPI_ERR_CANNOT_COERCE_TO_FLOAT:
            return MkGenapiErrorFormat("Cannot coerce %1 to float", 1); /* [node] */
        case GENAPI_ERR_ENUM_ENTRY_NOT_FOUND:
            return MkGenapiErrorFormat("Entry %2 not found (%1)", 2); /* [node, enumEntry] */
        case GENAPI_ERR_INVALID_BOOLEAN_VALUE:
            return MkGenapiErrorFormat("Invalid boolean value %2 (%1)", 2); /* [node, value] */
        case GENAPI_ERR_CANNOT_COERCE_FROM_INTEGER:
            return MkGenapiErrorFormat("Cannot coerce %1 from integer", 1); /* [node] */
        case GENAPI_ERR_XML_ELEMENT_NOT_FOUND:
            return MkGenapiErrorFormat("XML element %2 not found (%1)", 2); /* [node, element] */
        case GENAPI_ERR_UNKNOWN_ERROR:
            return MkGenapiErrorFormat("An unknown error occurred", 0); /* [] */
        case GENAPI_ERR_FLOAT_VALUE_TOO_SMALL:
            return MkGenapiErrorFormat("%1 cannot be smaller than %2", 2); /* [node, value] */
        case GENAPI_ERR_NODE_ERROR:
            return MkGenapiErrorFormat("%2 (%1)", 2); /* [node, message] */
        case GENAPI_ERR_ACCESS_LOCKED:
            return MkGenapiErrorFormat("%1 is locked", 1); /* [node] */
        case GENAPI_ERR_INVALID_ENUMERATION_INT_VALUE:
            return MkGenapiErrorFormat("Invalid enumeration integer value %2 (%1)", 2); /* [node, value] */
        case GENAPI_ERR_NODE_NOT_FOUND:
            return MkGenapiErrorFormat("%1 not found", 1); /* [node] */
        case GENAPI_ERR_INTERNAL_EXCEPTION_ON_NODE:
            return MkGenapiErrorFormat("An internal exception occurred (%1: %2)", 2); /* [node, message] */
        case GENAPI_ERR_REGISTER_LENGTH_MISMATCH:
            return MkGenapiErrorFormat("Register length mismatch (%1)", 1); /* [node] */
        case GENAPI_ERR_XML_ELEMENT_NOT_SUPPORTED:
            return MkGenapiErrorFormat("XML element %2 not supported (%1)", 2); /* [node, element] */
        case GENAPI_ERR_INTERNAL_EXCEPTION:
            return MkGenapiErrorFormat("An internal exception occurred (%1)", 1); /* [message] */
        case GENAPI_ERR_XML_ATTRIBUTE_NOT_FOUND:
            return MkGenapiErrorFormat("XML attribute %2 not found (%1)", 2); /* [node, attribute] */
        case GENAPI_ERR_CANNOT_COERCE_FROM_STRING:
            return MkGenapiErrorFormat("Cannot coerce %1 from string", 1); /* [node] */
        case GENAPI_ERR_INT_VALUE_TOO_LARGE:
            return MkGenapiErrorFormat("%1 cannot be greater than %2", 2); /* [node, value] */
        case GENAPI_ERR_XML_UNKNOWN_VALUE:
            return MkGenapiErrorFormat("Unknown value %3 while parsing %2 (%1)", 3); /* [node, name, value] */
        case GENAPI_ERR_FLOAT_TO_INTEGER_OVERFLOW:
            return MkGenapiErrorFormat("Overflow in float-to-integer conversion of %2 (%1)", 2); /* [node, value] */
        case GENAPI_ERR_UNKNOWN_PROPERTY_NAME:
            return MkGenapiErrorFormat("Unknown property %2 (%1)", 2); /* [node, property] */
        case GENAPI_ERR_INVALID_SELECTORS:
            return MkGenapiErrorFormat("Invalid selector (%1)", 1); /* [node] */
        case GENAPI_ERR_NO_INTERFACE:
            return MkGenapiErrorFormat("%1 has no %2 interface", 2); /* [node, interface] */
        case GENAPI_ERR_XML_PARSING_FAILURE:
            return MkGenapiErrorFormat("Invalid XML", 0); /* [] */
        case GENAPI_ERR_INVALID_INT_INCREMENT:
            return MkGenapiErrorFormat("%1 has invalid increment value %2", 2); /* [node, value] */
        case GENAPI_ERR_UNKNOWN_CONFIG_NAME:
            return MkGenapiErrorFormat("Unknown configuration (%1)", 1); /* [name] */
        case GENAPI_ERR_BAD_CONFIG_TYPE:
            return MkGenapiErrorFormat("Bad configuration type (%1)", 1); /* [name] */
        case GENAPI_ERR_EVALUATION_FAILURE:
            return MkGenapiErrorFormat("Error while evaluating %1 (%2)", 2); /* [node, message] */
        case GENAPI_ERR_PORT_NOT_FOUND:
            return MkGenapiErrorFormat("Port %2 not found (%1)", 2); /* [node, port] */
        case GENAPI_ERR_PROPERTY_NOT_DEFINED:
            return MkGenapiErrorFormat("Property %2 not defined (%1)", 2); /* [node, property] */
        case GENAPI_ERR_VAR_ALREADY_EXISTS:
            return MkGenapiErrorFormat("Variable already exists (%1)", 1); /* [variable] */
        case GENAPI_ERR_ACCESS_NOT_AVAILABLE:
            return MkGenapiErrorFormat("%1 is not available", 1); /* [node] */
        case GENAPI_ERR_INVALID_VAR_NAME:
            return MkGenapiErrorFormat("Invalid variable name (%1)", 1); /* [variable] */
        default:
            return MkGenapiErrorFormat("An undefined error occurred", 0); /* [] */
    }
}

/** @} */

#ifdef __cplusplus
}
}
}
#endif

#ifdef EURESYS_GENAPI_ERROR_DESCRIPTION
#ifdef __cplusplus
#include <sstream>
/** @defgroup CppGenApi GenApi (C API)
    @ingroup EGrabber
    Euresys GenApi helper functions to create error descriptions
    @{
 **/
/** Create an error description from a genapi_error description, the function
    is available when EURESYS_GENAPI_ERROR_DESCRIPTION is defined
 **/
static std::string GenapiErrorDescription(const Euresys::genapi_error &err) {
    std::string res;
    GenTL::EuresysCustomGenTL::GENAPI_ERROR_FORMAT format(
        GenTL::EuresysCustomGenTL::GenapiErrorFormat(err.genapi_error_code));
    res.assign(format.format);
    for (uint32_t i = 0; i < format.parameterCount; ++i) {
        std::ostringstream key;
        key << "%" << static_cast<int>(i + 1);
        std::ostringstream value;
        if (i < err.parameter_count()) {
            switch (err.parameter_type(i)) {
                case GenTL::EuresysCustomGenTL::GENAPI_ERROR_PARAMETER_TYPE_STRING:
                    value << err.string_parameter(i);
                    break;
                case GenTL::EuresysCustomGenTL::GENAPI_ERROR_PARAMETER_TYPE_INTEGER:
                    value << err.integer_parameter(i);
                    break;
                case GenTL::EuresysCustomGenTL::GENAPI_ERROR_PARAMETER_TYPE_FLOAT:
                    value << err.float_parameter(i);
                    break;
                default:
                    value << "?";
                    break;
            }
        } else {
            value << "?";
        }
        std::string k(key.str());
        std::string v(value.str());
        for (size_t pos = res.find(k); pos != std::string::npos; pos = res.find(k)) {
            res.replace(pos, k.size(), v);
        }
    }
    return res;
}
/** @} */
#endif /* __cplusplus */
#endif /* EURESYS_GENAPI_ERROR_DESCRIPTION */

#endif
