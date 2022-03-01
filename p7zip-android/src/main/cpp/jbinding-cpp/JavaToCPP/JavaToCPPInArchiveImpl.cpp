#include "SevenZipJBinding.h"
#include "JNITools.h"
#include "CPPToJava/CPPToJavaInStream.h"
#include "CPPToJava/CPPToJavaArchiveExtractCallback.h"
#include "CodecTools.h"

#include "JavaStatInfos/JavaPackageSevenZip.h"


static IInArchive *GetArchive(JNIEnv *env, jobject thiz) {
    jlong pointer = jni::InArchiveImpl::sevenZipArchiveInstance_Get(env, thiz);
//    FATALIF(!pointer, "GetArchive() : pointer == NULL");

    return reinterpret_cast<IInArchive *>(pointer);
}

static JBindingSession &GetJBindingSession(JNIEnv *env, jobject thiz) {
    jlong pointer = jni::InArchiveImpl::jbindingSession_Get(env, thiz);
//    FATALIF(!pointer, "GetJBindingSession() : pointer == NULL");

    return *reinterpret_cast<JBindingSession *>(pointer);
}

static CPPToJavaInStream *GetInStream(JNIEnv *env, jobject thiz) {
    jlong pointer = jni::InArchiveImpl::sevenZipInStreamInstance_Get(env, thiz);
//    FATALIF(!pointer, "GetInStream() : pointer == NULL.");

    return reinterpret_cast<CPPToJavaInStream *>(pointer);
}

int CompareIndicies(const void *pi1, const void *pi2) {
    UInt32 i1 = *(UInt32 *) pi1;
    UInt32 i2 = *(UInt32 *) pi2;
    return i1 > i2 ? 1 : (i1 < i2 ? -1 : 0);
}

/*
 * Class:     net_sf_sevenzip_impl_InArchiveImpl
 * Method:    nativeExtract
 * Signature: ([IZLnet/sf/sevenzip/IArchiveExtractCallback;)V
 */
JBINDING_JNIEXPORT void JNICALL Java_net_sf_sevenzipjbinding_impl_InArchiveImpl_nativeExtract(
        JNIEnv *env,
        jobject thiz,
        jintArray indicesArray,
        jboolean testMode,
        jobject archiveExtractCallbackObject) {
    TRACE("InArchiveImpl::nativeExtract(). ThreadID=" << PlatformGetCurrentThreadId());

    JBindingSession &jbindingSession = GetJBindingSession(env, thiz);
    JNINativeCallContext jniNativeCallContext(jbindingSession, env);

    CMyComPtr<IInArchive> archive(GetArchive(env, thiz));

    if (archive == nullptr) {
        TRACE("Archive==NULL. Do nothing...");
        return;
    }

    jint *indices = nullptr;
    UInt32 indicesCount = (UInt32) -1;
    UInt32 numberOfItems;
    HRESULT result = archive->GetNumberOfItems((UInt32 *) &numberOfItems);
    if (result != S_OK) {
        TRACE("Error getting number of items from archive. Result: 0x" << std::hex << result)
        jniNativeCallContext.reportError(result, "Error getting number of items from archive");
        return;
    }
    if (indicesArray) {
        indices = env->GetIntArrayElements(indicesArray, nullptr);

        indicesCount = static_cast<UInt32>(env->GetArrayLength(indicesArray));

        jint lastIndex = -1;
        int sortNeeded = false;
        for (UInt32 i = 0; i < indicesCount; i++) {
            if (indices[i] < 0 || indices[i] >= numberOfItems) {
                TRACE("Passed index for the extraction is incorrect: " << indices[i]
                                                                       << " (Count of items in archive: "
                                                                       << numberOfItems << ")")
                jniNativeCallContext.reportError(
                        result,
                        "Passed index for the extraction is incorrect: %i (Count of items in archive: %i)",
                        indices[i], numberOfItems);
                return;
            }
            if (lastIndex > indices[i])
                sortNeeded = true;
            lastIndex = indices[i];
        }
        if (sortNeeded)
            qsort(indices, indicesCount, 4, &CompareIndicies);
    }

    CMyComPtr<IArchiveExtractCallback> archiveExtractCallback =
            new CPPToJavaArchiveExtractCallback(jbindingSession, env, archiveExtractCallbackObject);

    TRACE("Extracting " << indicesCount << " items")
    result = archive->Extract((UInt32 *) indices, indicesCount, (Int32) testMode,
                              archiveExtractCallback);

    archiveExtractCallback.Release();

    if (indicesArray)
        env->ReleaseIntArrayElements(indicesArray, indices, JNI_ABORT);
    else
        delete[] indices;

    if (result) {
        TRACE("Extraction error. Result: 0x" << std::hex << result);
        if (indicesCount == -1) {
            jniNativeCallContext.reportError(result, "Error extracting all items");
        } else {
            jniNativeCallContext.reportError(result, "Error extracting %i item(s)", indicesCount);
        }
    } else {
        TRACE("Extraction succeeded")
    }

}

/*
 * Class:     net_sf_sevenzip_impl_InArchiveImpl
 * Method:    nativeGetNumberOfItems
 * Signature: ()I
 */
JBINDING_JNIEXPORT jint JNICALL
Java_net_sf_sevenzipjbinding_impl_InArchiveImpl_nativeGetNumberOfItems(
        JNIEnv *env,
        jobject thiz) {
    TRACE("InArchiveImpl::nativeGetNumberOfItems(). ThreadID=" << PlatformGetCurrentThreadId());

    JBindingSession &jbindingSession = GetJBindingSession(env, thiz);
    JNINativeCallContext jniNativeCallContext(jbindingSession, env);

    CMyComPtr<IInArchive> archive(GetArchive(env, thiz));


    if (archive == nullptr) {
        TRACE("Archive==NULL. Do nothing...");
        return 0;
    }

    UInt32 result;

    CHECK_HRESULT(jniNativeCallContext, archive->GetNumberOfItems(&result),
                  "Error getting number of items from archive");

    TRACE("Returning: " << result)

    return result;

}

/*
 * Class:     net_sf_sevenzip_impl_InArchiveImpl
 * Method:    nativeClose
 * Signature: ()V
 */
JBINDING_JNIEXPORT void JNICALL Java_net_sf_sevenzipjbinding_impl_InArchiveImpl_nativeClose(
        JNIEnv *env,
        jobject thiz) {
    TRACE("InArchiveImpl::nativeClose(). ThreadID=" << PlatformGetCurrentThreadId());

    JBindingSession &jbindingSession = GetJBindingSession(env, thiz);
    {
        JNINativeCallContext jniNativeCallContext(jbindingSession, env);

        CMyComPtr<IInArchive> archive(GetArchive(env, thiz));
        CMyComPtr<CPPToJavaInStream> inStream(GetInStream(env, thiz));

        if (archive == nullptr) {
            TRACE("Archive==NULL. Do nothing...");
            return;
        }

        CHECK_HRESULT(jniNativeCallContext, archive->Close(), "Error closing archive");

        archive->Release();
        inStream->Release();

        jni::InArchiveImpl::sevenZipArchiveInstance_Set(env, thiz, 0);
        jni::InArchiveImpl::jbindingSession_Set(env, thiz, 0);
        jni::InArchiveImpl::sevenZipInStreamInstance_Set(env, thiz, 0);

    }

    delete &jbindingSession;

    TRACE("InArchive closed")
}

/*
 * Class:     net_sf_sevenzip_impl_InArchiveImpl
 * Method:    nativeGetNumberOfArchiveProperties
 * Signature: ()I
 */
JBINDING_JNIEXPORT jint JNICALL
Java_net_sf_sevenzipjbinding_impl_InArchiveImpl_nativeGetNumberOfArchiveProperties(
        JNIEnv *env,
        jobject thiz) {
    TRACE("InArchiveImpl::GetNumberOfArchiveProperties()");

    JBindingSession &jbindingSession = GetJBindingSession(env, thiz);
    JNINativeCallContext jniNativeCallContext(jbindingSession, env);

    CMyComPtr<IInArchive> archive(GetArchive(env, thiz));

    if (archive == nullptr) {
        TRACE("Archive==NULL. Do nothing...");
        return 0;
    }

    UInt32 result;

    CHECK_HRESULT(jniNativeCallContext, archive->GetNumberOfArchiveProperties(&result),
                  "Error getting number of archive properties");

    return result;

}

/*
 * Class:     net_sf_sevenzip_impl_InArchiveImpl
 * Method:    nativeGetArchivePropertyInfo
 * Signature: (I)Lnet/sf/sevenzip/PropertyInfo;
 */
JBINDING_JNIEXPORT jobject JNICALL
Java_net_sf_sevenzipjbinding_impl_InArchiveImpl_nativeGetArchivePropertyInfo(
        JNIEnv *env,
        jobject thiz,
        jint index) {
    TRACE("InArchiveImpl::nativeGetArchivePropertyInfo()");

    JBindingSession &jbindingSession = GetJBindingSession(env, thiz);
    JNINativeCallContext jniNativeCallContext(jbindingSession, env);
    JNIEnvInstance jniEnvInstance(jbindingSession, jniNativeCallContext, env);

    CMyComPtr<IInArchive> archive(GetArchive(env, thiz));

    if (archive == nullptr) {
        TRACE("Archive==NULL. Do nothing...")
        return nullptr;
    }

    VARTYPE type;
    BSTR name;
    PROPID propID;

    CHECK_HRESULT1(jniNativeCallContext,
                   archive->GetArchivePropertyInfo(index, &name, &propID, &type),
                   "Error getting archive property info with index %i", index);

    jobject propertInfo = jni::PropertyInfo::_newInstance(env);
    if (jniEnvInstance.exceptionCheck()) {
        return nullptr;
    }

    ScopedLocalRef<jstring> javaName(env, env->NewString(reinterpret_cast<const jchar *>(name),
                                                         SysStringLen(name)));
    jclass javaType = VarTypeToJavaType(jniEnvInstance, type);

    ScopedLocalRef<jobject> propIDObject(env, jni::PropID::getPropIDByIndex(env, propID));
    if (jniEnvInstance.exceptionCheck()) {
        return nullptr;
    }

    jni::PropertyInfo::propID_Set(env, propertInfo, propIDObject.get());
    jni::PropertyInfo::name_Set(env, propertInfo, javaName.get());
    jni::PropertyInfo::varType_Set(env, propertInfo, javaType);

    return propertInfo;

}

/*
 * Class:     net_sf_sevenzip_impl_InArchiveImpl
 * Method:    nativeGetArchiveProperty
 * Signature: (J)Lnet/sf/sevenzip/PropVariant;
 */
JBINDING_JNIEXPORT jobject JNICALL
Java_net_sf_sevenzipjbinding_impl_InArchiveImpl_nativeGetArchiveProperty(
        JNIEnv *env,
        jobject thiz,
        jint propID) {
    TRACE("InArchiveImpl::nativeGetArchiveProperty");

    JBindingSession &jbindingSession = GetJBindingSession(env, thiz);
    JNINativeCallContext jniNativeCallContext(jbindingSession, env);
    JNIEnvInstance jniEnvInstance(jbindingSession, jniNativeCallContext, env);

    CMyComPtr<IInArchive> archive(GetArchive(env, thiz));

    if (archive == nullptr) {
        TRACE("Archive==NULL. Do nothing...")
        return nullptr;
    }

    NWindows::NCOM::CPropVariant PropVariant;

    CHECK_HRESULT1(jniNativeCallContext, archive->GetArchiveProperty(propID, &PropVariant),
                   "Error getting property mit Id: %lu", propID);

    return PropVariantToObject(jniEnvInstance, &PropVariant);

}

/*
 * Class:     net_sf_sevenzip_impl_InArchiveImpl
 * Method:    nativeGetStringArchiveProperty
 * Signature: (I)Ljava/lang/String;
 */
JBINDING_JNIEXPORT jstring JNICALL
Java_net_sf_sevenzipjbinding_impl_InArchiveImpl_nativeGetStringArchiveProperty(
        JNIEnv *env,
        jobject thiz,
        jint propID) {
    TRACE("InArchiveImpl::nativeGetStringArchiveProperty");

    JBindingSession &jbindingSession = GetJBindingSession(env, thiz);
    JNINativeCallContext jniNativeCallContext(jbindingSession, env);
    JNIEnvInstance jniEnvInstance(jbindingSession, jniNativeCallContext, env);

    CMyComPtr<IInArchive> archive(GetArchive(env, thiz));

    if (archive == nullptr) {
        TRACE("Archive==NULL. Do nothing...");
        return nullptr;
    }

    NWindows::NCOM::CPropVariant PropVariant;

    CHECK_HRESULT1(jniNativeCallContext, archive->GetArchiveProperty(propID, &PropVariant),
                   "Error getting property mit Id: %lu", propID);

    return PropVariantToString(env, propID, PropVariant);

}

/*
 * Class:     net_sf_sevenzip_impl_InArchiveImpl
 * Method:    nativeGetNumberOfProperties
 * Signature: ()I
 */
JBINDING_JNIEXPORT jint JNICALL
Java_net_sf_sevenzipjbinding_impl_InArchiveImpl_nativeGetNumberOfProperties(
        JNIEnv *env,
        jobject thiz) {
    TRACE("InArchiveImpl::nativeGetNumberOfProperties");

    JBindingSession &jbindingSession = GetJBindingSession(env, thiz);
    JNINativeCallContext jniNativeCallContext(jbindingSession, env);
    JNIEnvInstance jniEnvInstance(jbindingSession, jniNativeCallContext, env);

    CMyComPtr<IInArchive> archive(GetArchive(env, thiz));

    if (archive == nullptr) {
        TRACE("Archive==NULL. Do nothing...");
        return 0;
    }

    UInt32 result;

    CHECK_HRESULT(jniNativeCallContext, archive->GetNumberOfProperties(&result),
                  "Error getting number of properties");

    return result;

}

JBINDING_JNIEXPORT jint JNICALL
Java_net_sf_sevenzipjbinding_impl_InArchiveImpl_nativeGetIntProperty(JNIEnv *env,
                                                                     jobject thiz,
                                                                     jint index,
                                                                     jint propID) {
    TRACE("InArchiveImpl::nativeGetIntProperty");

    JBindingSession &jbindingSession = GetJBindingSession(env, thiz);
    JNINativeCallContext jniNativeCallContext(jbindingSession, env);
    JNIEnvInstance jniEnvInstance(jbindingSession, jniNativeCallContext, env);

    CMyComPtr<IInArchive> archive(GetArchive(env, thiz));

    if (archive == nullptr) {
        TRACE("Archive==NULL. Do nothing...");
        return 0;
    }

    NWindows::NCOM::CPropVariant propVariant;

    //    TRACE3("Index: %i, PropID: %i, archive: 0x%08X", index, propID, (unsigned int)(Object *)(CPPToJavaInStream *)(void*)(*(&archive)))
    CHECK_HRESULT2(jniNativeCallContext, archive->GetProperty(index, propID, &propVariant),
                   "Error getting property with propID=%lu for item %i", propID, index);

    switch (propVariant.vt) {
        case VT_EMPTY:
        case VT_NULL:
        case VT_VOID:
            return 0;
        case VT_I1:
            return (jint) propVariant.cVal;

        case VT_I2:
            return (jint) propVariant.iVal;

        case VT_INT: // TODO Check this: Variant 'VT_INT'
        case VT_I4:
            return (jint) propVariant.lVal;

        case VT_UI1:
            return (jint) propVariant.bVal;

        case VT_UI2:
            return (jint) propVariant.uiVal;

        case VT_UINT: // TODO Check this: Variant 'VT_UINT'
        case VT_UI4:
            return (jint) propVariant.ulVal;
        case VT_BOOL:
            return propVariant.boolVal ? 1 : 0;
        default:
            jniEnvInstance.reportError("Unsupported Int PropVariant type. VarType: %i",
                                       propVariant.vt);
    }
    return 0;
}

JBINDING_JNIEXPORT jlong JNICALL
Java_net_sf_sevenzipjbinding_impl_InArchiveImpl_nativeGetLongProperty(JNIEnv *env,
                                                                      jobject thiz,
                                                                      jint index,
                                                                      jint propID) {
    TRACE("InArchiveImpl::nativeGetLongProperty");

    JBindingSession &jbindingSession = GetJBindingSession(env, thiz);
    JNINativeCallContext jniNativeCallContext(jbindingSession, env);
    JNIEnvInstance jniEnvInstance(jbindingSession, jniNativeCallContext, env);

    CMyComPtr<IInArchive> archive(GetArchive(env, thiz));

    if (archive == nullptr) {
        TRACE("Archive==NULL. Do nothing...");
        return 0;
    }

    NWindows::NCOM::CPropVariant propVariant;

    //    TRACE3("Index: %i, PropID: %i, archive: 0x%08X", index, propID, (unsigned int)(Object *)(CPPToJavaInStream *)(void*)(*(&archive)))
    CHECK_HRESULT2(jniNativeCallContext, archive->GetProperty(index, propID, &propVariant),
                   "Error getting property with propID=%lu for item %i", propID, index);

    switch (propVariant.vt) {
        case VT_EMPTY:
        case VT_NULL:
        case VT_VOID:
            return 0;

        case VT_INT: // TODO Check this: Variant 'VT_INT'
        case VT_I4:
            return (jint) propVariant.lVal;

        case VT_UINT: // TODO Check this: Variant 'VT_UINT'
        case VT_UI4:
            return (jint) propVariant.ulVal;

            //包含之前的32位
        case VT_I8:
            return (jlong) propVariant.hVal.QuadPart;
        case VT_UI8:
            return (jlong) propVariant.uhVal.QuadPart;
        case VT_DATE:
        case VT_FILETIME: {
            LONGLONG time = (((LONGLONG) propVariant.filetime.dwHighDateTime) << 32) |
                            propVariant.filetime.dwLowDateTime;
            LONGLONG javaTime = (time - FILETIME_TO_JAVATIME_SHIFT) / FILETIME_TO_JAVATIME_FACTOR;
            return (jlong) javaTime;
        }
        default:
            jniEnvInstance.reportError("Unsupported PropVariant type. VarType: %i",
                                       propVariant.vt);
    }
    return 0;
}

/*
 * Class:     net_sf_sevenzip_impl_InArchiveImpl
 * Method:    nativeGetProperty
 * Signature: (IJ)Ljava/lang/Object;
 */
JBINDING_JNIEXPORT jobject JNICALL
Java_net_sf_sevenzipjbinding_impl_InArchiveImpl_nativeGetProperty(
        JNIEnv *env,
        jobject thiz,
        jint index,
        jint propID) {
    TRACE("InArchiveImpl::nativeGetProperty");

    JBindingSession &jbindingSession = GetJBindingSession(env, thiz);
    JNINativeCallContext jniNativeCallContext(jbindingSession, env);
    JNIEnvInstance jniEnvInstance(jbindingSession, jniNativeCallContext, env);

    CMyComPtr<IInArchive> archive(GetArchive(env, thiz));

    if (archive == nullptr) {
        TRACE("Archive==NULL. Do nothing...");
        return nullptr;
    }

    NWindows::NCOM::CPropVariant propVariant;

    //    TRACE3("Index: %i, PropID: %i, archive: 0x%08X", index, propID, (unsigned int)(Object *)(CPPToJavaInStream *)(void*)(*(&archive)))
    CHECK_HRESULT2(jniNativeCallContext, archive->GetProperty(index, propID, &propVariant),
                   "Error getting property with propID=%lu for item %i", propID, index);

    return PropVariantToObject(jniEnvInstance, &propVariant);

}

/*
 * Class:     net_sf_sevenzip_impl_InArchiveImpl
 * Method:    nativeGetStringProperty
 * Signature: (II)Ljava/lang/String;
 */
JBINDING_JNIEXPORT jstring JNICALL
Java_net_sf_sevenzipjbinding_impl_InArchiveImpl_nativeGetStringProperty(
        JNIEnv *env,
        jobject thiz,
        jint index,
        jint propID) {
    TRACE("InArchiveImpl::nativeGetStringProperty");

    JBindingSession &jbindingSession = GetJBindingSession(env, thiz);
    JNINativeCallContext jniNativeCallContext(jbindingSession, env);

    CMyComPtr<IInArchive> archive(GetArchive(env, thiz));

    if (archive == nullptr) {
        TRACE("Archive==NULL. Do nothing...");
        return nullptr;
    }

    NWindows::NCOM::CPropVariant propVariant;

    CHECK_HRESULT2(jniNativeCallContext, archive->GetProperty(index, propID, &propVariant),
                   "Error getting property with propID=%lu for item %i", propID, index);

    return PropVariantToString(env, static_cast<PROPID>(propID), propVariant);

}

/*
 * Class:     net_sf_sevenzip_impl_InArchiveImpl
 * Method:    nativeGetPropertyInfo
 * Signature: (I)Lnet/sf/sevenzip/PropertyInfo;
 */
JBINDING_JNIEXPORT jobject JNICALL
Java_net_sf_sevenzipjbinding_impl_InArchiveImpl_nativeGetPropertyInfo(
        JNIEnv *env,
        jobject thiz,
        jint index) {
    TRACE("InArchiveImpl::nativeGetPropertyInfo");

    JBindingSession &jbindingSession = GetJBindingSession(env, thiz);
    JNINativeCallContext jniNativeCallContext(jbindingSession, env);
    JNIEnvInstance jniEnvInstance(jbindingSession, jniNativeCallContext, env);

    CMyComPtr<IInArchive> archive(GetArchive(env, thiz));

    if (archive == nullptr) {
        TRACE("Archive==NULL. Do nothing...");
        return nullptr;
    }

    VARTYPE type;
    BSTR name;
    PROPID propID;

    CHECK_HRESULT1(jniNativeCallContext, archive->GetPropertyInfo(index, &name, &propID, &type),
                   "Error getting property info with index %i", index);

    jobject propertInfo = jni::PropertyInfo::_newInstance(env);
    if (jniEnvInstance.exceptionCheck()) {
        return nullptr;
    }

    ScopedLocalRef<jstring> javaName(env, env->NewString(reinterpret_cast<const jchar *>(name),
                                                         SysStringLen(name)));
    jclass javaType = VarTypeToJavaType(jniEnvInstance, type);

    ScopedLocalRef<jobject> propIDObject(env, jni::PropID::getPropIDByIndex(env, propID));
    if (jniEnvInstance.exceptionCheck()) {
        return nullptr;
    }

    jni::PropertyInfo::propID_Set(env, propertInfo, propIDObject.get());
    jni::PropertyInfo::name_Set(env, propertInfo, javaName.get());
    jni::PropertyInfo::varType_Set(env, propertInfo, javaType);

    return propertInfo;
}

/*
 * Class:     net_sf_sevenzipjbinding_impl_InArchiveImpl
 * Method:    nativeConnectOutArchive
 * Signature: (Lnet/sf/sevenzipjbinding/impl/OutArchiveImpl;Lnet/sf/sevenzipjbinding/ArchiveFormat;)V
 */
JBINDING_JNIEXPORT void JNICALL
Java_net_sf_sevenzipjbinding_impl_InArchiveImpl_nativeConnectOutArchive
        (JNIEnv *env, jobject thiz, jobject outArchiveImpl, jobject archiveFormat) {

    TRACE("InArchiveImpl::nativeConnectOutArchive");

    JBindingSession &jbindingSession = GetJBindingSession(env, thiz);
    JNINativeCallContext jniNativeCallContext(jbindingSession, env);
    JNIEnvInstance jniEnvInstance(jbindingSession, jniNativeCallContext, env);

    CMyComPtr<IInArchive> archive(GetArchive(env, thiz));

    int archiveFormatIndex = codecTools.getArchiveFormatIndex(jniEnvInstance, archiveFormat);

    if (archiveFormatIndex < 0 || codecTools.codecs.Formats[archiveFormatIndex].CreateOutArchive ==
                                  nullptr) {
        jniEnvInstance.reportError(
                "Internal error during creating OutArchive. Archive format index: %i",
                archiveFormatIndex);
        return;
    }

    CMyComPtr<IOutArchive> outArchive;

    HRESULT hresult = archive->QueryInterface(IID_IOutArchive, (void **) &outArchive);
    if (hresult) {
        jniEnvInstance.reportError(hresult,
                                   "Error connecting OutArchive to the InArchive for archive format %S",
                                   (const wchar_t *) codecTools.codecs.Formats[archiveFormatIndex].Name);
        return;
    }

    jni::OutArchiveImpl::sevenZipArchiveInstance_Set(env, outArchiveImpl, //
                                                     reinterpret_cast<jlong>(outArchive.Detach()));

    jni::OutArchiveImpl::jbindingSession_Set(env, outArchiveImpl, //
                                             reinterpret_cast<jlong>(&jbindingSession));
}
