#include "SevenZipJBinding.h"

#include "JNITools.h"

#include "CodecTools.h"

#include "CPPToJava/CPPToJavaOutStream.h"
#include "CPPToJava/CPPToJavaArchiveUpdateCallback.h"

#include "UnicodeHelper.h"
#include "UserTrace.h"

#include "ScopedLocalRef.h"

// void updateItemsNative(int archiveFormatIndex, IOutStream outStream, int numberOfItems,
//                        IArchiveUpdateCallback archiveUpdateCallback)

static JBindingSession &GetJBindingSession(JNIEnv *env, jobject thiz) {
    jlong pointer = jni::OutArchiveImpl::jbindingSession_Get(env, thiz);
    FATALIF(!pointer, "GetJBindingSession() : pointer == NULL");

    return *(reinterpret_cast<JBindingSession *>(pointer));
}

static IOutArchive *GetArchive(JNIEnv *env, jobject thiz) {
    jlong pointer = jni::OutArchiveImpl::sevenZipArchiveInstance_Get(env, thiz);
    FATALIF(!pointer, "GetArchive() : pointer == NULL");

    return reinterpret_cast<IOutArchive *>(pointer);
}

/*
 * Class:     net_sf_sevenzipjbinding_impl_OutArchiveImpl
 * Method:    updateItemsNative
 * Signature: (ILnet/sf/sevenzipjbinding/ISequentialOutStream;ILnet/sf/sevenzipjbinding/IArchiveUpdateCallback;Z)V
 */
JBINDING_JNIEXPORT void JNICALL Java_net_sf_sevenzipjbinding_impl_OutArchiveImpl_nativeUpdateItems(
        JNIEnv *env,
        jobject thiz,
        jobject outStream,
        jint numberOfItems,
        jobject archiveUpdateCallback) {
    TRACE("OutArchiveImpl.updateItemsNative()");

    JBindingSession &jbindingSession = GetJBindingSession(env, thiz);
    JNINativeCallContext jniNativeCallContext(jbindingSession, env);
    JNIEnvInstance jniEnvInstance(jbindingSession, jniNativeCallContext, env);

    CMyComPtr<IOutArchive> outArchive(GetArchive(env, thiz));

    ScopedLocalRef<jobject> archiveFormat(jniEnvInstance,
                                          jni::OutArchiveImpl::archiveFormat_Get(env, thiz));
    int archiveFormatIndex = codecTools.getArchiveFormatIndex(jniEnvInstance, archiveFormat.get());
    jboolean isInArchiveAttached = static_cast<jboolean>(
            jni::OutArchiveImpl::inArchive_Get(env, thiz) != nullptr);

    if (isUserTraceEnabled(jniEnvInstance, thiz)) {
        if (isInArchiveAttached) {
            userTrace(jniEnvInstance, thiz,
                      UString(L"Updating ") << (UInt32) numberOfItems << L" items");
        } else {
            userTrace(jniEnvInstance, thiz,
                      UString(L"Compressing ") << (UInt32) numberOfItems << L" items");
        }
    }

    CMyComPtr<IOutStream> cppToJavaOutStream = new CPPToJavaOutStream(jbindingSession, env,
                                                                      outStream);

    CPPToJavaArchiveUpdateCallback *cppToJavaArchiveUpdateCallback = new CPPToJavaArchiveUpdateCallback(
            jbindingSession, env,
            archiveUpdateCallback,
            isInArchiveAttached,
            archiveFormatIndex,
            thiz);

    CMyComPtr<IArchiveUpdateCallback> cppToJavaArchiveUpdateCallbackPtr = cppToJavaArchiveUpdateCallback;

    HRESULT hresult = outArchive->UpdateItems(cppToJavaOutStream,
                                              static_cast<UInt32>(numberOfItems),
                                              cppToJavaArchiveUpdateCallback);
    if (hresult) {
        jniEnvInstance.reportError(hresult, "Error creating '%S' archive with %i items",
                                   (const wchar_t *) codecTools.codecs.Formats[archiveFormatIndex].Name,
                                   (int) numberOfItems);
    }

    cppToJavaArchiveUpdateCallback->freeOutItem(jniEnvInstance);
}


JBINDING_JNIEXPORT void JNICALL
Java_net_sf_sevenzipjbinding_impl_OutArchiveImpl_nativeSetProperties
        (JNIEnv *env, jobject thiz, jobjectArray jnames, jobjectArray jvalues) {
    TRACE("OutArchiveImpl::setLevelNative(). ThreadID=" << PlatformGetCurrentThreadId());

    JBindingSession &jbindingSession = GetJBindingSession(env, thiz);
    JNINativeCallContext jniNativeCallContext(jbindingSession, env);
    JNIEnvInstance jniEnvInstance(jbindingSession, jniNativeCallContext, env);

    CMyComPtr<IOutArchive> outArchive(GetArchive(env, thiz));

    // TODO Move query interface to the central location in J2C+SevenZip.cpp
    CMyComPtr<ISetProperties> setProperties;
    HRESULT result = outArchive->QueryInterface(IID_ISetProperties, (void **) &setProperties);
    if (result != S_OK) {
        TRACE("Error getting IID_ISetProperties interface. Result: 0x" << std::hex << result)
        jniNativeCallContext.reportError(result, "Error getting IID_ISetProperties interface.");
        return;
    }


    //keys values

    CRecordVector<const wchar_t *> names;

    CRecordVector<const UString *> needDels;

    jsize values_len = jniEnvInstance->GetArrayLength(jvalues);

    jclass stringClass = VarTypeToJavaType(jniEnvInstance, VT_BSTR);
    jclass integerClass = VarTypeToJavaType(jniEnvInstance, VT_UINT);

    NWindows::NCOM::CPropVariant *propValues = new NWindows::NCOM::CPropVariant[values_len];

    for (int i = 0; i < values_len; ++i) {

        ScopedLocalRef<jstring> jname(jniEnvInstance,
                                      static_cast<jstring>(jniEnvInstance->GetObjectArrayElement(
                                              jnames, i)));

        const jchar *nameChars = jniEnvInstance->GetStringChars(jname.get(), nullptr);
        const UString *item = new UString(UnicodeHelper(nameChars,
                                                        static_cast<size_t>(jniEnvInstance->GetStringLength(
                                                                jname.get()))));
        needDels.Add(item);

        names.Add(*item);

        jniEnvInstance->ReleaseStringChars(jname.get(), nameChars);

        //value

        ScopedLocalRef<jobject> jvalue(jniEnvInstance,
                                       jniEnvInstance->GetObjectArrayElement(jvalues, i));

        if (jniEnvInstance->IsInstanceOf(jvalue.get(), integerClass)) {
            jint value = jni::Integer::intValue(jniEnvInstance, jvalue.get());
            propValues[i] = (UINT32) value;
        } else if (jniEnvInstance->IsInstanceOf(jvalue.get(), stringClass)) {
            if (jvalue.get()) {
                const jchar *jchars = env->GetStringChars(static_cast<jstring>(jvalue.get()),
                                                          nullptr);

                propValues[i] = UString(UnicodeHelper(jchars, static_cast<size_t>(
                        env->GetStringLength(static_cast<jstring>(jvalue.get())))));

                env->ReleaseStringChars(static_cast<jstring>(jvalue.get()), jchars);
            } else {
                propValues[i] = false;
            }
        }
    }

    result = setProperties->SetProperties(&names.Front(), propValues, names.Size());

    for (int i = 0; i < needDels.Size(); ++i) {
        delete needDels[i];
    }

    delete[] propValues;

    if (result) {
        TRACE("Error setting  property. Result: 0x" << std::hex << result)
        jniNativeCallContext.reportError(result, "Error setting 'Level' property.");
        return;
    }
}

/*
 * Class:     net_sf_sevenzipjbinding_impl_OutArchiveImpl
 * Method:    nativeClose
 * Signature: ()V
 */
JBINDING_JNIEXPORT void JNICALL Java_net_sf_sevenzipjbinding_impl_OutArchiveImpl_nativeClose
        (JNIEnv *env, jobject thiz) {

    TRACE("InArchiveImpl::nativeClose(). ThreadID=" << PlatformGetCurrentThreadId());

    JBindingSession &jbindingSession = GetJBindingSession(env, thiz);
    {
        JNINativeCallContext jniNativeCallContext(jbindingSession, env);
        JNIEnvInstance jniEnvInstance(jbindingSession, jniNativeCallContext, env);

        CMyComPtr<IOutArchive> outArchive(GetArchive(env, thiz));

        outArchive->Release();

        jni::OutArchiveImpl::sevenZipArchiveInstance_Set(env, thiz, 0);
        jni::OutArchiveImpl::jbindingSession_Set(env, thiz, 0);

        TRACE("sevenZipArchiveInstance and jbindingSession references cleared, outArchive released")
    }
    delete &jbindingSession;

    TRACE("OutArchive closed")
}
