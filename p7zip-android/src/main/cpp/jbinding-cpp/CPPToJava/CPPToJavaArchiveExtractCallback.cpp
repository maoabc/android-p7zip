#include "SevenZipJBinding.h"

#include "JNITools.h"
#include "CPPToJavaArchiveExtractCallback.h"
#include "CPPToJavaSequentialOutStream.h"
#include "ScopedLocalRef.h"



STDMETHODIMP CPPToJavaArchiveExtractCallback::GetStream(UInt32 index,
                                                        ISequentialOutStream **outStream,
                                                        Int32 askExtractMode) {
    TRACE_OBJECT_CALL("GetStream");

    JNIEnvInstance jniEnvInstance(_jbindingSession);

    if (outStream) {
        *outStream = nullptr;
    }

    ScopedLocalRef<jobject> askExtractModeObject(jniEnvInstance,
                                                 jni::ExtractAskMode::getExtractAskModeByIndex(
                                                         jniEnvInstance,
                                                         (jint) askExtractMode));
    if (jniEnvInstance.exceptionCheck()) {
        return S_FALSE;
    }

    // public SequentialOutStream getStream(int index, ExtractAskMode extractAskMode);
    ScopedLocalRef<jobject> result(jniEnvInstance,
                                   _iArchiveExtractCallback->getStream(jniEnvInstance,
                                                                       _javaImplementation,
                                                                       (jint) index,
                                                                       askExtractModeObject.get()));
    if (jniEnvInstance.exceptionCheck()) {
        return S_FALSE;
    }

    if (result.get() == nullptr) {
        *outStream = nullptr;
        return S_OK;
    }

    CMyComPtr<ISequentialOutStream> outStreamComPtr = new CPPToJavaSequentialOutStream(
            _jbindingSession, jniEnvInstance, result.get());
    *outStream = outStreamComPtr.Detach();

    return S_OK;
}

STDMETHODIMP CPPToJavaArchiveExtractCallback::PrepareOperation(Int32 askExtractMode) {
    TRACE_OBJECT_CALL("PrepareOperation");

    JNIEnvInstance jniEnvInstance(_jbindingSession);

    ScopedLocalRef<jobject> askExtractModeObject(jniEnvInstance,
                                                 jni::ExtractAskMode::getExtractAskModeByIndex(
                                                         jniEnvInstance,
                                                         (jint) askExtractMode));
    if (jniEnvInstance.exceptionCheck()) {
        return S_FALSE;
    }

    // public void prepareOperation(ExtractAskMode extractAskMode);
    _iArchiveExtractCallback->prepareOperation(jniEnvInstance, _javaImplementation,
                                               askExtractModeObject.get());

    return jniEnvInstance.exceptionCheck() ? S_FALSE : S_OK;
}

STDMETHODIMP CPPToJavaArchiveExtractCallback::SetOperationResult(Int32 resultEOperationResult) {
    TRACE_OBJECT_CALL("SetOperationResult");

    JNIEnvInstance jniEnvInstance(_jbindingSession);

    ScopedLocalRef<jobject> resultEOperationResultObject(jniEnvInstance,
                                                         jni::ExtractOperationResult::getOperationResult(
                                                                 jniEnvInstance,
                                                                 (jint) resultEOperationResult));

    if (jniEnvInstance.exceptionCheck()) {
        return S_FALSE;
    }

    // public void setOperationResult(ExtractOperationResult extractOperationResult);
    _iArchiveExtractCallback->setOperationResult(jniEnvInstance, _javaImplementation,
                                                 resultEOperationResultObject.get());

    return jniEnvInstance.exceptionCheck() ? S_FALSE : S_OK;
}
