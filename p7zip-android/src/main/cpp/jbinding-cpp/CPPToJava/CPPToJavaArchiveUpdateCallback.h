#ifndef CPPTOJAVAARCHIVEUPDATECALLBACK_H_
#define CPPTOJAVAARCHIVEUPDATECALLBACK_H_

#include "CPPToJavaAbstract.h"
#include "CPPToJavaProgress.h"
#include "CodecTools.h"
#include "CPPToJavaCryptoGetTextPassword.h"

class CPPToJavaArchiveUpdateCallback : public virtual IArchiveUpdateCallback,
                                       public CPPToJavaProgress {

private:
    ICryptoGetTextPassword2 *_cryptoGetTextPassword2Impl;
    jni::IOutCreateCallback *_iOutCreateCallback;
    jobject _outItem;
    int _outItemLastIndex;
    int _archiveFormatIndex;
    jobject _outArchive;
    jclass _inStreamInterface;
    bool _isInArchiveAttached;

public:
    CPPToJavaArchiveUpdateCallback(JBindingSession &jbindingSession, JNIEnv *initEnv,
                                   jobject archiveUpdateCallback, bool isInArchiveAttached,
                                   int archiveFormatIndex, jobject outArchive) :
            CPPToJavaProgress(jbindingSession, initEnv, archiveUpdateCallback),
            _iOutCreateCallback(jni::IOutCreateCallback::_getInstanceFromObject(
                    initEnv, archiveUpdateCallback)),
            _outItemLastIndex(-1),
            _archiveFormatIndex(archiveFormatIndex),
            _outItem(NULL),
            _isInArchiveAttached(isInArchiveAttached) {
        TRACE_OBJECT_CREATION("CPPToJavaArchiveOpenCallback")
        jclass cryptoGetTextPasswordClass = initEnv->FindClass(CRYPTOGETTEXTPASSWORD2_CLASS);
        FATALIF(cryptoGetTextPasswordClass == NULL,
                "Can't find class "
                        CRYPTOGETTEXTPASSWORD_CLASS);

        if (initEnv->IsInstanceOf(_javaImplementation, cryptoGetTextPasswordClass)) {
            CMyComPtr<ICryptoGetTextPassword2> cryptoGetTextPasswordComPtr =
                    new CPPToJavaCryptoGetTextPassword2(_jbindingSession, initEnv,
                                                        _javaImplementation);
            _cryptoGetTextPassword2Impl = cryptoGetTextPasswordComPtr.Detach();
        } else {
            _cryptoGetTextPassword2Impl = NULL;
        }
        _outArchive = initEnv->NewGlobalRef(outArchive);

        _inStreamInterface = static_cast<jclass>(initEnv->NewGlobalRef(
                initEnv->FindClass(INSTREAM_CLASS)));
        FATALIF(!_inStreamInterface, "Class "
                INSTREAM_CLASS
                " not found");
    }

    ~CPPToJavaArchiveUpdateCallback() {
        JNIEnvInstance jniEnvInstance(_jbindingSession);
        if (_cryptoGetTextPassword2Impl) {
            _cryptoGetTextPassword2Impl->Release();
        }
        if (_inStreamInterface) {
            jniEnvInstance->DeleteGlobalRef(_inStreamInterface);
        }
        if (_outArchive) {
            jniEnvInstance->DeleteGlobalRef(_outArchive);
        }
    }

    STDMETHOD(GetUpdateItemInfo)(UInt32 index, Int32 *newData, /*1 - new data, 0 - old data */
                                 Int32 *newProperties, /* 1 - new properties, 0 - old properties */
                                 UInt32 *indexInArchive /* -1 if there is no in archive, or if doesn't matter */
    );

    STDMETHOD(GetProperty)(UInt32 index, PROPID propID, PROPVARIANT *value);

    STDMETHOD(GetStream)(UInt32 index, ISequentialInStream **inStream);

    STDMETHOD(SetOperationResult)(Int32 operationResult);

    STDMETHOD(SetTotal)(UInt64 total) {
        TRACE_OBJECT_CALL("SetTotal");
        return CPPToJavaProgress::SetTotal(total);
    }

    STDMETHOD(SetCompleted)(const UInt64 *completeValue) {
        TRACE_OBJECT_CALL("SetCompleted");
        return CPPToJavaProgress::SetCompleted(completeValue);
    }

    STDMETHOD(QueryInterface)(REFGUID refguid, void **p) noexcept {
        TRACE_OBJECT_CALL("QueryInterface");
        if (refguid == IID_ICryptoGetTextPassword2 && _cryptoGetTextPassword2Impl) {
            *p = (void *) (ICryptoGetTextPassword2 *) _cryptoGetTextPassword2Impl;
            _cryptoGetTextPassword2Impl->AddRef();
            return S_OK;
        }

        return CPPToJavaProgress::QueryInterface(refguid, p);
    }

    STDMETHOD_(ULONG, AddRef)() noexcept {
        TRACE_OBJECT_CALL("AddRef");
        return CPPToJavaProgress::AddRef();
    }

    STDMETHOD_(ULONG, Release)() noexcept {
        TRACE_OBJECT_CALL("Release");
        return CPPToJavaProgress::Release();
    }

    void freeOutItem(JNIEnvInstance &jniEnvInstance);


private:
    LONG getOrUpdateOutItem(JNIEnvInstance &jniEnvInstance, int index);
};

#endif /*CPPTOJAVAARCHIVEUPDATECALLBACK_H_*/
