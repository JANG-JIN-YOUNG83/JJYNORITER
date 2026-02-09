#ifndef EURESYS_PRODUCERS_HEADER_FILE
#define EURESYS_PRODUCERS_HEADER_FILE

namespace EURESYS_NAMESPACE {
namespace Internal {

    enum ProducerType { COAXLINK_PRODUCER, GIGELINK_PRODUCER };

    template<enum ProducerType, int> const char *getEuresysCtiEnvVariable();
    template<> inline const char *getEuresysCtiEnvVariable<COAXLINK_PRODUCER, 32>() {
        return "EURESYS_COAXLINK_GENTL32_CTI";
    }
    template<> inline const char *getEuresysCtiEnvVariable<COAXLINK_PRODUCER, 64>() {
        return "EURESYS_COAXLINK_GENTL64_CTI";
    }
    template<> inline const char *getEuresysCtiEnvVariable<GIGELINK_PRODUCER, 32>() {
        return "EURESYS_GIGELINK_GENTL32_CTI";
    }
    template<> inline const char *getEuresysCtiEnvVariable<GIGELINK_PRODUCER, 64>() {
        return "EURESYS_GIGELINK_GENTL64_CTI";
    }
} // end of Internal
} // end of EURESYS_NAMESPACE
#endif
