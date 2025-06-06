
#include <tempo_utils/posix_result.h>

tempo_utils::PosixStatus::PosixStatus(
    tempo_utils::StatusCode statusCode,
    std::shared_ptr<const tempo_utils::Detail> detail)
    : tempo_utils::TypedStatus<PosixCondition>(statusCode, detail)
{
}

inline tempo_utils::PosixCondition
errno_to_condition(int errNo)
{
    switch (errNo) {
        case E2BIG:
            return tempo_utils::PosixCondition::kE2BIG;
        case EACCES:
            return tempo_utils::PosixCondition::kEACCES;
        case EADDRINUSE:
            return tempo_utils::PosixCondition::kEADDRINUSE;
        case EADDRNOTAVAIL:
            return tempo_utils::PosixCondition::kEADDRNOTAVAIL;
        case EAFNOSUPPORT:
            return tempo_utils::PosixCondition::kEAFNOSUPPORT;
        case EAGAIN:
            return tempo_utils::PosixCondition::kEAGAIN;
        case EALREADY:
            return tempo_utils::PosixCondition::kEALREADY;
        case EBADF:
            return tempo_utils::PosixCondition::kEBADF;
        case EBADMSG:
            return tempo_utils::PosixCondition::kEBADMSG;
        case EBUSY:
            return tempo_utils::PosixCondition::kEBUSY;
        case ECANCELED:
            return tempo_utils::PosixCondition::kECANCELED;
        case ECHILD:
            return tempo_utils::PosixCondition::kECHILD;
        case ECONNABORTED:
            return tempo_utils::PosixCondition::kECONNABORTED;
        case ECONNREFUSED:
            return tempo_utils::PosixCondition::kECONNREFUSED;
        case ECONNRESET:
            return tempo_utils::PosixCondition::kECONNRESET;
        case EDEADLK:
            return tempo_utils::PosixCondition::kEDEADLK;
        case EDESTADDRREQ:
            return tempo_utils::PosixCondition::kEDESTADDRREQ;
        case EDOM:
            return tempo_utils::PosixCondition::kEDOM;
        case EDQUOT:
            return tempo_utils::PosixCondition::kEDQUOT;
        case EEXIST:
            return tempo_utils::PosixCondition::kEEXIST;
        case EFAULT:
            return tempo_utils::PosixCondition::kEFAULT;
        case EFBIG:
            return tempo_utils::PosixCondition::kEFBIG;
        case EHOSTUNREACH:
            return tempo_utils::PosixCondition::kEHOSTUNREACH;
        case EIDRM:
            return tempo_utils::PosixCondition::kEIDRM;
        case EILSEQ:
            return tempo_utils::PosixCondition::kEILSEQ;
        case EINPROGRESS:
            return tempo_utils::PosixCondition::kEINPROGRESS;
        case EINTR:
            return tempo_utils::PosixCondition::kEINTR;
        case EINVAL:
            return tempo_utils::PosixCondition::kEINVAL;
        case EIO:
            return tempo_utils::PosixCondition::kEIO;
        case EISCONN:
            return tempo_utils::PosixCondition::kEISCONN;
        case EISDIR:
            return tempo_utils::PosixCondition::kEISDIR;
        case ELOOP:
            return tempo_utils::PosixCondition::kELOOP;
        case EMFILE:
            return tempo_utils::PosixCondition::kEMFILE;
        case EMLINK:
            return tempo_utils::PosixCondition::kEMLINK;
        case EMSGSIZE:
            return tempo_utils::PosixCondition::kEMSGSIZE;
        case EMULTIHOP:
            return tempo_utils::PosixCondition::kEMULTIHOP;
        case ENAMETOOLONG:
            return tempo_utils::PosixCondition::kENAMETOOLONG;
        case ENETDOWN:
            return tempo_utils::PosixCondition::kENETDOWN;
        case ENETRESET:
            return tempo_utils::PosixCondition::kENETRESET;
        case ENETUNREACH:
            return tempo_utils::PosixCondition::kENETUNREACH;
        case ENFILE:
            return tempo_utils::PosixCondition::kENFILE;
        case ENOBUFS:
            return tempo_utils::PosixCondition::kENOBUFS;
        case ENODATA:
            return tempo_utils::PosixCondition::kENODATA;
        case ENODEV:
            return tempo_utils::PosixCondition::kENODEV;
        case ENOENT:
            return tempo_utils::PosixCondition::kENOENT;
        case ENOEXEC:
            return tempo_utils::PosixCondition::kENOEXEC;
        case ENOLCK:
            return tempo_utils::PosixCondition::kENOLCK;
        case ENOLINK:
            return tempo_utils::PosixCondition::kENOLINK;
        case ENOMEM:
            return tempo_utils::PosixCondition::kENOMEM;
        case ENOMSG:
            return tempo_utils::PosixCondition::kENOMSG;
        case ENOPROTOOPT:
            return tempo_utils::PosixCondition::kENOPROTOOPT;
        case ENOSPC:
            return tempo_utils::PosixCondition::kENOSPC;
        case ENOSR:
            return tempo_utils::PosixCondition::kENOSR;
        case ENOSTR:
            return tempo_utils::PosixCondition::kENOSTR;
        case ENOSYS:
            return tempo_utils::PosixCondition::kENOSYS;
        case ENOTCONN:
            return tempo_utils::PosixCondition::kENOTCONN;
        case ENOTDIR:
            return tempo_utils::PosixCondition::kENOTDIR;
        case ENOTEMPTY:
            return tempo_utils::PosixCondition::kENOTEMPTY;
        case ENOTRECOVERABLE:
            return tempo_utils::PosixCondition::kENOTRECOVERABLE;
        case ENOTSOCK:
            return tempo_utils::PosixCondition::kENOTSOCK;
        case ENOTSUP:
            return tempo_utils::PosixCondition::kENOTSUP;
        case ENOTTY:
            return tempo_utils::PosixCondition::kENOTTY;
        case ENXIO:
            return tempo_utils::PosixCondition::kENXIO;
#if ENOTSUP != EOPNOTSUPP
        case EOPNOTSUPP:
            return tempo_utils::PosixCondition::kEOPNOTSUPP;
#endif
        case EOVERFLOW:
            return tempo_utils::PosixCondition::kEOVERFLOW;
        case EOWNERDEAD:
            return tempo_utils::PosixCondition::kEOWNERDEAD;
        case EPERM:
            return tempo_utils::PosixCondition::kEPERM;
        case EPIPE:
            return tempo_utils::PosixCondition::kEPIPE;
        case EPROTO:
            return tempo_utils::PosixCondition::kEPROTO;
        case EPROTONOSUPPORT:
            return tempo_utils::PosixCondition::kEPROTONOSUPPORT;
        case EPROTOTYPE:
            return tempo_utils::PosixCondition::kEPROTOTYPE;
        case ERANGE:
            return tempo_utils::PosixCondition::kERANGE;
        case EROFS:
            return tempo_utils::PosixCondition::kEROFS;
        case ESPIPE:
            return tempo_utils::PosixCondition::kESPIPE;
        case ESRCH:
            return tempo_utils::PosixCondition::kESRCH;
        case ESTALE:
            return tempo_utils::PosixCondition::kESTALE;
        case ETIME:
            return tempo_utils::PosixCondition::kETIME;
        case ETIMEDOUT:
            return tempo_utils::PosixCondition::kETIMEDOUT;
        case ETXTBSY:
            return tempo_utils::PosixCondition::kETXTBSY;
        // MacOS maps EWOULDBLOCK to EAGAIN
        //case EWOULDBLOCK:
        //        return tempo_utils::PosixCondition::kEWOULDBLOCK;
        case EXDEV:
            return tempo_utils::PosixCondition::kEXDEV;
        default:
            TU_UNREACHABLE();
    }
}

tempo_utils::PosixStatus
tempo_utils::PosixStatus::last(std::string_view message)
{
    return PosixStatus(errno_to_condition(errno), message);
}

tempo_utils::PosixStatus
tempo_utils::PosixStatus::fromError(int errNo, std::string_view message)
{
    return PosixStatus(errno_to_condition(errNo), message);
}

bool
tempo_utils::PosixStatus::convert(PosixStatus &dstStatus, const tempo_utils::Status &srcStatus)
{
    std::string_view srcNs = srcStatus.getErrorCategory();
    std::string_view dstNs = kTempoUtilsPosixStatusNs;
    if (srcNs != dstNs)
        return false;
    dstStatus = PosixStatus(srcStatus.getStatusCode(), srcStatus.getDetail());
    return true;
}
