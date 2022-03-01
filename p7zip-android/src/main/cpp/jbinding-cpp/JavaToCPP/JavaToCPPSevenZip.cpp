#include "StdAfx.h"
#include "Common/MyInitGuid.h"
#include "../C/7zVersion.h"

#include "SevenZipJBinding.h"

#include "JNITools.h"

#include "CPPToJava/CPPToJavaInStream.h"
#include "UniversalArchiveOpenCallback.h"
#include "CodecTools.h"

#include "JavaStatInfos/JavaPackageSevenZip.h"

#include "ScopedLocalRef.h"

using namespace NWindows;
using namespace NFile;

#ifdef MINGW
DEFINE_GUID(IID_IUnknown,
        0x00000000, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);
#endif // MINGW

#include "7zip/UI/Common/LoadCodecs.h"
#include "UnicodeHelper.h"

#ifdef _WIN32
#ifndef _UNICODE
bool g_IsNT = false;
static inline bool IsItWindowsNT()
{
    OSVERSIONINFO versionInfo;
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
    if (!::GetVersionEx(&versionInfo)) {
        return false;
    }
    return (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT);
}
#endif
#endif


#include "JavaStaticInfo.h"

int g_CodePage = -1;
/*
 * Class:     net_sf_sevenzip_SevenZip
 * Method:    nativeInitSevenZipLibrary
 * Signature: ()Ljava/lang/String;
 */
JBINDING_JNIEXPORT jstring JNICALL
Java_net_sf_sevenzipjbinding_SevenZip_nativeInitSevenZipLibrary(JNIEnv *env, jclass thiz) {

    TRACE("7-zip library initialized (TODO)")

    codecTools.init();

    jni::OutOfMemoryError::_initialize(env);

    //if (msg) {
    //	TRACE1("Error initializing 7-zip library: %s", msg)
    //	return env->NewStringUTF(msg);
    //}

    return nullptr;
}

/*
 * Class:     net_sf_sevenzipjbinding_SevenZip
 * Method:    nativeGetVersionMajor
 * Signature: ()I
 */
JBINDING_JNIEXPORT jint JNICALL
Java_net_sf_sevenzipjbinding_SevenZip_nativeGetVersionMajor(JNIEnv *, jclass) {
    TRACE("SevenZip.nativeGetVersionMajor()")

    return MY_VER_MAJOR;
}

/*
 * Class:     net_sf_sevenzipjbinding_SevenZip
 * Method:    nativeGetVersionMinor
 * Signature: ()I
 */
JBINDING_JNIEXPORT jint JNICALL
Java_net_sf_sevenzipjbinding_SevenZip_nativeGetVersionMinor(JNIEnv *, jclass) {
    TRACE("SevenZip.nativeGetVersionMinor()")

    return MY_VER_MINOR;
}

/*
 * Class:     net_sf_sevenzipjbinding_SevenZip
 * Method:    nativeGetVersionBuild
 * Signature: ()I
 */
JBINDING_JNIEXPORT jint JNICALL
Java_net_sf_sevenzipjbinding_SevenZip_nativeGetVersionBuild(JNIEnv *, jclass) {
    TRACE("SevenZip.nativeGetVersionBuild()")

    return MY_VER_BUILD;
}

/*
 * Class:     net_sf_sevenzipjbinding_SevenZip
 * Method:    nativeGetVersionVersion
 * Signature: ()Ljava/lang/String;
 */
JBINDING_JNIEXPORT jstring JNICALL
Java_net_sf_sevenzipjbinding_SevenZip_nativeGetVersionVersion(JNIEnv *env, jclass) {
    TRACE("SevenZip.nativeGetVersionVersion()")

    return env->NewStringUTF(MY_VERSION);
}

/*
 * Class:     net_sf_sevenzipjbinding_SevenZip
 * Method:    nativeGetVersionDate
 * Signature: ()Ljava/lang/String;
 */
JBINDING_JNIEXPORT jstring JNICALL
Java_net_sf_sevenzipjbinding_SevenZip_nativeGetVersionDate(JNIEnv *env, jclass) {
    TRACE("SevenZip.nativeGetVersionDate()")

    return env->NewStringUTF(MY_DATE);
}

/*
 * Class:     net_sf_sevenzipjbinding_SevenZip
 * Method:    nativeGetVersionCopyright
 * Signature: ()Ljava/lang/String;
 */
JBINDING_JNIEXPORT jstring JNICALL
Java_net_sf_sevenzipjbinding_SevenZip_nativeGetVersionCopyright(JNIEnv *env, jclass) {
    TRACE("SevenZip.nativeGetVersionCopyright()")

    return env->NewStringUTF(MY_COPYRIGHT);
}


JBINDING_JNIEXPORT jobject JNICALL Java_net_sf_sevenzipjbinding_SevenZip_nativeOpenArchive(
        JNIEnv *env,
        jclass clazz,
        jobject archiveFormat,
        jobject inStream,
        jobject archiveOpenCallbackImpl) {
    TRACE("SevenZip.nativeOpenArchive()")

    JBindingSession &jbindingSession = *(new JBindingSession(env));
    DeleteInErrorCase<JBindingSession> deleteInErrorCase(jbindingSession);

    JNINativeCallContext jniNativeCallContext(jbindingSession, env);
    JNIEnvInstance jniEnvInstance(jbindingSession, jniNativeCallContext, env);

    int formatIndex = -1;
    UString formatNameString;
    if (archiveFormat) {
        formatIndex = codecTools.getArchiveFormatIndex(env, archiveFormat);
        if (formatIndex == -1) {
            jniNativeCallContext.reportError("Not registered archive format: '%S'",
                                             (const wchar_t *) formatNameString);
            deleteInErrorCase.setErrorCase();
            return nullptr;
        }
        formatNameString = codecTools.codecs.Formats[formatIndex].Name;
    }

    CMyComPtr<IInArchive> archive;
    CMyComPtr<CPPToJavaInStream> stream = new CPPToJavaInStream(jbindingSession, env, inStream);

    UniversalArchiveOpencallback *universalArchiveOpencallback = new UniversalArchiveOpencallback(
            jbindingSession, env, archiveOpenCallbackImpl);
    CMyComPtr<IArchiveOpenCallback> archiveOpenCallback = universalArchiveOpencallback;

    UInt64 maxCheckStartPosition = 4 * 1024 * 1024; // Advice from Igor Pavlov

    bool archiveFormatOpened = false;

    if (formatIndex != -1) {//如果指定格式则先根据指定的打开
        // Use one specified codec
        codecTools.codecs.CreateInArchive(formatIndex, archive);
        if (!archive) {
            fatal("Can't get InArchive class for codec %S", (const wchar_t *) formatNameString);
        }

        TRACE("Opening using codec " << codecTools.codecs.Formats[formatIndex].Name);

        universalArchiveOpencallback->setSimulateArchiveOpenVolumeCallback(
                codecTools.isCabArchive(formatIndex));

        HRESULT result = archive->Open(stream, &maxCheckStartPosition, archiveOpenCallback);

        archiveFormatOpened = (result == S_OK);
    }

    if (!archiveFormatOpened) {//格式打开错误，则循环尝试其他格式
        // Try all known codecs
        TRACE("Iterating through all available codecs...")
        bool success = false;
        for (int i = 0; i < codecTools.codecs.Formats.Size(); i++) {
            TRACE("Trying codec " << codecTools.codecs.Formats[i].Name);

            stream->Seek(0, STREAM_SEEK_SET, nullptr);

            codecTools.codecs.CreateInArchive(i, archive);
            if (!archive) {
                continue;
            }

            universalArchiveOpencallback->setSimulateArchiveOpenVolumeCallback(
                    codecTools.isCabArchive(i));

            HRESULT result = archive->Open(stream, &maxCheckStartPosition, archiveOpenCallback);
            if (result != S_OK) {
                continue;
            }

            formatNameString = codecTools.codecs.Formats[i].Name;
            success = true;
            break;
        }

        if (!success) {
            TRACE("Success=false, throwing exception...")

            jniEnvInstance.reportError(
                    "Archive file can't be opened with none of the registered codecs");
            deleteInErrorCase.setErrorCase();
            return nullptr;

        }

    }


    if (jniNativeCallContext.willExceptionBeThrown()) {
        archive->Close();
        deleteInErrorCase.setErrorCase();
        return nullptr;
    }

    TRACE("Archive opened")

    jobject inArchiveImplObject = jni::InArchiveImpl::_newInstance(env);
    if (jniEnvInstance.exceptionCheck()) {
        archive->Close();
        deleteInErrorCase.setErrorCase();
        return nullptr;
    }

    ScopedLocalRef<jstring> jstringFormatNameString(env,
                                                    env->NewString(UnicodeHelper(formatNameString),
                                                                   formatNameString.Len()));
    jni::InArchiveImpl::setArchiveFormat(env, inArchiveImplObject, jstringFormatNameString.get());
    if (jniEnvInstance.exceptionCheck()) {
        archive->Close();
        deleteInErrorCase.setErrorCase();
        return nullptr;
    }

    jni::InArchiveImpl::sevenZipArchiveInstance_Set(env, inArchiveImplObject, //
                                                    reinterpret_cast<jlong>(archive.Detach()));

    jni::InArchiveImpl::jbindingSession_Set(env, inArchiveImplObject, //
                                            reinterpret_cast<jlong>(&jbindingSession));

    jni::InArchiveImpl::sevenZipInStreamInstance_Set(env, inArchiveImplObject, //
                                                     reinterpret_cast<jlong>(stream.Detach()));

    return inArchiveImplObject;
}

/*
 * Class:     net_sf_sevenzipjbinding_SevenZip
 * Method:    nativeCreateArchive
 * Signature: (Lnet/sf/sevenzipjbinding/impl/OutArchiveImpl;Lnet/sf/sevenzipjbinding/ArchiveFormat;)V
 */
JBINDING_JNIEXPORT void JNICALL Java_net_sf_sevenzipjbinding_SevenZip_nativeCreateArchive(
        JNIEnv *env,
        jclass clazz,
        jobject outArchiveImpl,
        jobject archiveFormat) {
    TRACE("SevenZip.nativeCreateArchive()")

    JBindingSession &jbindingSession = *(new JBindingSession(env));
    DeleteInErrorCase<JBindingSession> deleteInErrorCase(jbindingSession);

    JNINativeCallContext jniNativeCallContext(jbindingSession, env);
    JNIEnvInstance jniEnvInstance(jbindingSession, jniNativeCallContext, env);

    int archiveFormatIndex = codecTools.getArchiveFormatIndex(jniEnvInstance, archiveFormat);

    if (archiveFormatIndex < 0 ||
        codecTools.codecs.Formats[archiveFormatIndex].CreateOutArchive == nullptr) {
        jniEnvInstance.reportError(
                "Internal error during creating OutArchive. Archive format index: %i",
                archiveFormatIndex);
        deleteInErrorCase.setErrorCase();
        return;
    }

    CMyComPtr<IOutArchive> outArchive;
    HRESULT hresult = codecTools.codecs.CreateOutArchive(archiveFormatIndex, outArchive);
    if (hresult) {
        jniEnvInstance.reportError(hresult, "Error creating OutArchive for archive format %S",
                                   (const wchar_t *) codecTools.codecs.Formats[archiveFormatIndex].Name);
        deleteInErrorCase.setErrorCase();
        return;
    }

    jni::OutArchiveImpl::sevenZipArchiveInstance_Set(env, outArchiveImpl, //
                                                     reinterpret_cast<jlong>(outArchive.Detach()));

    jni::OutArchiveImpl::jbindingSession_Set(env, outArchiveImpl, //
                                             reinterpret_cast<jlong>(&jbindingSession));

    jni::OutArchiveImpl::archiveFormat_Set(env, outArchiveImpl, archiveFormat);
}
