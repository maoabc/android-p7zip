#include <ScopedLocalRef.h>
#include "SevenZipJBinding.h"

#include "JBindingTools.h"
#include "CPPToJavaSequentialInStream.h"


STDMETHODIMP CPPToJavaSequentialInStream::Read(void *data, UInt32 size, UInt32 *processedSize) {
    TRACE_OBJECT_CALL("Read");

    JNIEnvInstance jniEnvInstance(_jbindingSession);

    if (processedSize) {
        *processedSize = 0;
    }


//    ScopedLocalRef<jbyteArray> barr(jniEnvInstance, jniEnvInstance->NewByteArray(size));

//    if (!barr.get()) {
//        jniEnvInstance.reportError("Out of local resources or out of memory");
//    }
    ScopedLocalRef<jobject> buffer(jniEnvInstance, jniEnvInstance->NewDirectByteBuffer(data, size));

    if (!buffer.get()) {
        jniEnvInstance.reportError("Out of local resources or out of memory");
    }

    jint wasRead = _iSequentialInStream->read(jniEnvInstance, _javaImplementation, buffer.get(),
                                              size);
    if (jniEnvInstance.exceptionCheck()) {
        return S_FALSE;
    }

    if (processedSize) {
        *processedSize = (UInt32) wasRead;
    }

//    jniEnvInstance->GetByteArrayRegion(barr.get(), 0, wasRead, static_cast<jbyte *>(data));
//	jbyte * buffer = jniEnvInstance->GetByteArrayElements(byteArray, NULL);
//	memcpy(data, buffer, size);
//	jniEnvInstance->ReleaseByteArrayElements(byteArray, buffer, JNI_ABORT);

    return S_OK;
}

