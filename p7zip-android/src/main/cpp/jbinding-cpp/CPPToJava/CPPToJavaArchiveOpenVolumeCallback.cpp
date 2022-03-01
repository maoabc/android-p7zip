#include "SevenZipJBinding.h"

#include "JNITools.h"
#include "CPPToJavaArchiveOpenVolumeCallback.h"
#include "CPPToJavaInStream.h"
#include "UnicodeHelper.h"
#include "ScopedLocalRef.h"

STDMETHODIMP CPPToJavaArchiveOpenVolumeCallback::GetProperty(PROPID propID, PROPVARIANT *value) {
    TRACE_OBJECT_CALL("GetProperty");

    TRACE("GetProperty(" << propID << ')')

    JNIEnvInstance jniEnvInstance(_jbindingSession);

    if (value) {
        value->vt = VT_NULL;
    }

    ScopedLocalRef<jobject> propIDObject(jniEnvInstance,
                                         jni::PropID::getPropIDByIndex(jniEnvInstance,
                                                                       (jint) propID));
    if (jniEnvInstance.exceptionCheck()) {
        return S_FALSE;
    }

    ScopedLocalRef<jobject> result(jniEnvInstance,
                                   _iArchiveOpenVolumeCallback->getProperty(jniEnvInstance,
                                                                            _javaImplementation,
                                                                            propIDObject.get()));
    if (jniEnvInstance.exceptionCheck()) {
        return S_FALSE;
    }

    ObjectToPropVariant(jniEnvInstance, result.get(), value);

    return S_OK;
}

STDMETHODIMP CPPToJavaArchiveOpenVolumeCallback::GetStream(const wchar_t *name,
                                                           IInStream **inStream) {
    TRACE_OBJECT_CALL("GetStream");

    JNIEnvInstance jniEnvInstance(_jbindingSession);

    if (inStream) {
        *inStream = nullptr;
    }

    ScopedLocalRef<jstring> nameString(jniEnvInstance,
                                       jniEnvInstance->NewString(UnicodeHelper(name),
                                                                 (jsize) wcslen(name)));

    ScopedLocalRef<jobject> inStreamImpl(jniEnvInstance,
                                         _iArchiveOpenVolumeCallback->getStream(jniEnvInstance,
                                                                                _javaImplementation,
                                                                                nameString.get()));
    if (jniEnvInstance.exceptionCheck()) {
        return S_FALSE;
    }

    if (inStream) {
        if (inStreamImpl.get()) {
            CPPToJavaInStream *newInStream = new CPPToJavaInStream(_jbindingSession, jniEnvInstance,
                                                                   inStreamImpl.get());

            CMyComPtr<IInStream> inStreamComPtr = newInStream;
            *inStream = inStreamComPtr.Detach();
        } else {
            //			jniInstance.ThrowSevenZipException(
            //					"IArchiveOpenVolumeCallback.GetStream() returns stream=null. "
            //						"Use non-zero return value if requested volume doesn't exists");
            return S_FALSE;
        }
    }

    return S_OK;
}

