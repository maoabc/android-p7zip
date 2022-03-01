#include "SevenZipJBinding.h"

#include "JNITools.h"
#include "CPPToJavaArchiveOpenCallback.h"

STDMETHODIMP CPPToJavaArchiveOpenCallback::SetCompleted(const UInt64 *files, const UInt64 *bytes) {
    TRACE_OBJECT_CALL("SetCompleted");

    JNIEnvInstance jniEnvInstance(_jbindingSession);

    jlong filesLong = 0;
    jlong bytesLong = 0;

    if (files) {
        filesLong = static_cast<jlong>(*files);
    }

    if (bytes) {
        bytesLong = static_cast<jlong>(*bytes);
    }

    jboolean completed = _iArchiveOpenCallback->setCompleted(jniEnvInstance, _javaImplementation,
                                                             filesLong,
                                                             bytesLong);
    return !completed ? S_FALSE : S_OK;
}

STDMETHODIMP CPPToJavaArchiveOpenCallback::SetTotal(const UInt64 *files, const UInt64 *bytes) {
    TRACE_OBJECT_CALL("SetTotal");

    JNIEnvInstance jniEnvInstance(_jbindingSession);

    jlong filesLong = 0;
    jlong bytesLong = 0;

    if (files) {
        filesLong = static_cast<jlong>(*files);
    }

    if (bytes) {
        bytesLong = static_cast<jlong>(*bytes);
    }

    jboolean total = _iArchiveOpenCallback->setTotal(jniEnvInstance, _javaImplementation, filesLong,
                                                     bytesLong);
    return !total ? S_FALSE : S_OK;
}

