#ifndef __JAVA_OUT_STREAM_H__INCLUDED__

#include "CPPToJavaSequentialOutStream.h"

class CPPToJavaOutStream : public virtual IOutStream, public CPPToJavaSequentialOutStream {
private:
    jni::IOutStream * _iOutStream;
    jni::ISeekableStream * _iSeekableStream;

public:
    CPPToJavaOutStream(JBindingSession & jbindingSession, JNIEnv * initEnv, jobject outStream) :
        CPPToJavaSequentialOutStream(jbindingSession, initEnv, outStream), //
                _iOutStream(jni::IOutStream::_getInstanceFromObject(initEnv, outStream)), //
                _iSeekableStream(jni::ISeekableStream::_getInstanceFromObject(initEnv, outStream)) {
        TRACE_OBJECT_CREATION("CPPToJavaOutStream")
    }

    STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize) {
        TRACE("WRITE(size=" << size << ")")
        HRESULT result = CPPToJavaSequentialOutStream::Write(data, size, processedSize);
#ifdef TRACE_ON
        if (processedSize) {
            TRACE("WRITE: size=" << size << ", was written:" << *processedSize << ", result:" << result);
        }
#endif
        return result;
    }

    STDMETHOD(QueryInterface)(REFGUID iid, void ** outObject) noexcept {
        if (iid == IID_IOutStream) {
            *outObject = (void *) (IOutStream *) this;
            AddRef();
            return S_OK;
        }

        return CPPToJavaSequentialOutStream::QueryInterface(iid, outObject);
    }

    STDMETHOD_(ULONG, AddRef)() noexcept {
        return CPPToJavaSequentialOutStream::AddRef();
    }

    STDMETHOD_(ULONG, Release)() noexcept {
        return CPPToJavaSequentialOutStream::Release();
    }

    STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition);

    STDMETHOD(SetSize)(UInt64 newSize);
};

#define __JAVA_OUT_STREAM_H__INCLUDED__
#endif // __JAVA_OUT_STREAM_H__INCLUDED__
