#ifndef TEMPO_UTILS_POSIX_RESULT_H
#define TEMPO_UTILS_POSIX_RESULT_H

#include "status.h"

namespace tempo_utils {

    constexpr const char *kTempoUtilsPosixStatusNs = "dev.zuri.ns:tempo-utils-posix-status-1";

    enum class PosixCondition {
        kE2BIG,
        kEACCES,
        kEADDRINUSE,
        kEADDRNOTAVAIL,
        kEAFNOSUPPORT,
        kEAGAIN,
        kEALREADY,
        kEBADF,
        kEBADMSG,
        kEBUSY,
        kECANCELED,
        kECHILD,
        kECONNABORTED,
        kECONNREFUSED,
        kECONNRESET,
        kEDEADLK,
        kEDESTADDRREQ,
        kEDOM,
        kEDQUOT,
        kEEXIST,
        kEFAULT,
        kEFBIG,
        kEHOSTUNREACH,
        kEIDRM,
        kEILSEQ,
        kEINPROGRESS,
        kEINTR,
        kEINVAL,
        kEIO,
        kEISCONN,
        kEISDIR,
        kELOOP,
        kEMFILE,
        kEMLINK,
        kEMSGSIZE,
        kEMULTIHOP,
        kENAMETOOLONG,
        kENETDOWN,
        kENETRESET,
        kENETUNREACH,
        kENFILE,
        kENOBUFS,
        kENODATA,
        kENODEV,
        kENOENT,
        kENOEXEC,
        kENOLCK,
        kENOLINK,
        kENOMEM,
        kENOMSG,
        kENOPROTOOPT,
        kENOSPC,
        kENOSR,
        kENOSTR,
        kENOSYS,
        kENOTCONN,
        kENOTDIR,
        kENOTEMPTY,
        kENOTRECOVERABLE,
        kENOTSOCK,
        kENOTSUP,
        kENOTTY,
        kENXIO,
        kEOPNOTSUPP,
        kEOVERFLOW,
        kEOWNERDEAD,
        kEPERM,
        kEPIPE,
        kEPROTO,
        kEPROTONOSUPPORT,
        kEPROTOTYPE,
        kERANGE,
        kEROFS,
        kESPIPE,
        kESRCH,
        kESTALE,
        kETIME,
        kETIMEDOUT,
        kETXTBSY,
        kEWOULDBLOCK,
        kEXDEV,
        NUM_CONDITIONS, // must be last
    };

    class PosixStatus : public TypedStatus<PosixCondition> {

    public:
        using TypedStatus::TypedStatus;

        static PosixStatus ok();
        static PosixStatus last(std::string_view message = {});
        static PosixStatus fromError(int errNo, std::string_view message = {});

        static bool convert(PosixStatus &dstStatus, const tempo_utils::Status &srcStatus);

    private:
        PosixStatus(tempo_utils::StatusCode statusCode, std::shared_ptr<const tempo_utils::Detail> detail);
    };

    template<>
    struct StatusTraits<PosixStatus> {
        using ConditionType = PosixCondition;
        static bool convert(PosixStatus &dstStatus, const tempo_utils::Status &srcStatus)
        {
            return PosixStatus::convert(dstStatus, srcStatus);
        }
    };

    template<>
    struct ConditionTraits<PosixCondition> {
        using StatusType = PosixStatus;
        static constexpr const char *condition_namespace() { return kTempoUtilsPosixStatusNs; }
        static constexpr StatusCode make_status_code(PosixCondition condition)
        {
            return tempo_utils::StatusCode::kInternal;
        };
        static constexpr const char *make_error_message(PosixCondition condition)
        {
            switch (condition) {
                default:
                    return "System Error";
            }
        }
    };
}

#endif // TEMPO_UTILS_POSIX_RESULT_H