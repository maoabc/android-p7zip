#include <ScopedLocalRef.h>
#include "SevenZipJBinding.h"

#include "JNITools.h"
#include "CPPToJavaSequentialOutStream.h"

STDMETHODIMP CPPToJavaSequentialOutStream::Write(const void *data, UInt32 size,
                                                 UInt32 *processedSize) {
    TRACE_OBJECT_CALL("Write");

    if (processedSize) {
        *processedSize = 0;
    }

    if (size == 0) {
        return S_OK;
    }

    JNIEnvInstance jniEnvInstance(_jbindingSession);

    ScopedLocalRef<jobject> buffer(jniEnvInstance, jniEnvInstance->NewDirectByteBuffer(
            const_cast<void *>(data), size));

    if (!buffer.get()) {
        jniEnvInstance.reportError("Out of local resources or out of memory");
    }

//
//     public int write(ByteBuffer data,int len);
    jint result = _iSequentialOutStream->write(jniEnvInstance, _javaImplementation, buffer.get(),
                                               size);
    if (jniEnvInstance.exceptionCheck()) {
        return S_FALSE;
    }
    if (processedSize)
        *processedSize = (UInt32) result;

    if (result <= 0) {
        jniEnvInstance.reportError("Implementation of 'int ISequentialOutStream.write(byte[])' "
                                   "should write at least one byte. Returned amount of written bytes: %i",
                                   result);
        return S_FALSE;
    }

    return S_OK;
}

