/* Copyright Euresys 2014, 2017 */

#ifndef EURESYS_EGENTL_ERRORS_HEADER_FILE
#define EURESYS_EGENTL_ERRORS_HEADER_FILE

#include <stdexcept>
#include <vector>

/** @file
    Defines exception used by Euresys::EGenTL and Euresys::EGrabber
 **/

namespace EURESYS_NAMESPACE {

namespace gc = GenTL;
namespace ge = GenTL::EuresysCustomGenTL;

template<typename T>
std::string intToString(T n) {
    std::stringstream ss;
    ss << n;
    return ss.str();
}

/** cti loading error */
class cti_loading_error: public std::runtime_error {
    public:
        explicit cti_loading_error(const std::string &path)
        : std::runtime_error(std::string("Cannot load ") + path)
        {}
};

/** a cti symbol is missing */
class missing_gentl_symbol: public std::runtime_error {
    public:
        explicit missing_gentl_symbol(const std::string &path, const std::string &symbol)
        : std::runtime_error(std::string("Missing symbol ") + symbol + " in " + path)
        {}
};

/** an info command did not return the expected data type */
class unexpected_data_type: public std::runtime_error {
    public:
        gc::INFO_DATATYPE type;
        explicit unexpected_data_type(gc::INFO_DATATYPE type)
        : std::runtime_error(std::string("Unexpected data type ") +
                             intToString(type))
        , type(type)
        {}
};

/** a command did not return the expected data size */
class unexpected_data_size: public std::runtime_error {
    public:
        size_t expected;
        size_t returned;
        explicit unexpected_data_size(size_t expected, size_t returned)
        : std::runtime_error(std::string("Unexpected data size, expected ") +
                             intToString(expected) + " but got " +
                             intToString(returned))
        , expected(expected)
        , returned(returned)
        {}
};

/** something wrong with the parameters provided by the caller */
class client_error: public std::runtime_error {
    public:
        explicit client_error(const std::string &msg)
        : std::runtime_error(msg)
        {}
};

/** some error reported by the GenTL library */
class gentl_error: public std::runtime_error {
    public:
        gc::GC_ERROR gc_err;
        explicit gentl_error(gc::GC_ERROR err)
        : std::runtime_error(std::string("GenTL error ") +
                             intToString(err))
        , gc_err(err)
        {}
        gentl_error(gc::GC_ERROR err, const std::string &description)
        : std::runtime_error(std::string("GenTL error ") +
                             intToString(err) +
                             ", " + description)
        , gc_err(err)
        {}
};

/** genapi_error is a detailed gentl_error(GC_ERR_CUSTOM_GENAPI_ERROR) reported by the Genapi module */
class genapi_error: public gentl_error {
    public:
        ge::GENAPI_ERROR_CODE genapi_error_code; /**< the GenApi module error code */
        explicit genapi_error(ge::GENAPI_ERROR_CODE code)
        : gentl_error(ge::GC_ERR_CUSTOM_GENAPI_ERROR,
                      std::string("GenApi error code ") + intToString(code))
        , genapi_error_code(code)
        {}
        ~genapi_error() throw() {}
        /// Get the number of parameters stored in this error
        size_t parameter_count() const {
            return types.size();
        }
        /// Get the type of the parameter at a given index (0 <= index < parameter_count())
        ge::GENAPI_ERROR_PARAMETER_TYPE parameter_type(size_t index) const {
            return types.at(index);
        }
        /// Get the string parameter at a given index (0 <= index < parameter_count())
        const std::string &string_parameter(size_t index) const {
            if (types.at(index) == ge::GENAPI_ERROR_PARAMETER_TYPE_STRING) {
                return strings.at(indexes.at(index));
            }
            throw client_error("invalid parameter type");
        }
        /// Get the integer parameter at a given index (0 <= index < parameter_count())
        int64_t integer_parameter(size_t index) const {
            if (types.at(index) == ge::GENAPI_ERROR_PARAMETER_TYPE_INTEGER) {
                return integers.at(indexes.at(index));
            }
            throw client_error("invalid parameter type");
        }
        /// Get the float parameter at a given index (0 <= index < parameter_count())
        double float_parameter(size_t index) const {
            if (types.at(index) == ge::GENAPI_ERROR_PARAMETER_TYPE_FLOAT) {
                return floats.at(indexes.at(index));
            }
            throw client_error("invalid parameter type");
        }
        /// Add a string parameter to the error (called by internal EGenTL classes)
        void add_string_parameter(const char *p) {
            types.push_back(ge::GENAPI_ERROR_PARAMETER_TYPE_STRING);
            indexes.push_back(strings.size());
            strings.push_back(p);
        }
        /// Add an integer parameter to the error (called by internal EGenTL classes)
        void add_integer_parameter(int64_t p) {
            types.push_back(ge::GENAPI_ERROR_PARAMETER_TYPE_INTEGER);
            indexes.push_back(integers.size());
            integers.push_back(p);
        }
        /// Add a float parameter to the error (called by internal EGenTL classes)
        void add_float_parameter(double p) {
            types.push_back(ge::GENAPI_ERROR_PARAMETER_TYPE_FLOAT);
            indexes.push_back(floats.size());
            floats.push_back(p);
        }
    private:
        std::vector<ge::GENAPI_ERROR_PARAMETER_TYPE> types;
        std::vector<size_t> indexes;
        std::vector<std::string> strings;
        std::vector<int64_t> integers;
        std::vector<double> floats;
};

/** thread error */
class thread_error: public std::runtime_error {
    public:
        thread_error(const std::string &msg)
        : std::runtime_error(msg)
        {}
};

} // namespace EURESYS_NAMESPACE

#endif
