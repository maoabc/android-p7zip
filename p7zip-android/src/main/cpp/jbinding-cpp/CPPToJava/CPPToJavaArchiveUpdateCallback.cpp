#include "SevenZipJBinding.h"

#include "JNITools.h"
#include "CPPToJavaArchiveUpdateCallback.h"
#include "CPPToJavaSequentialInStream.h"
#include "CPPToJavaInStream.h"
#include "UnicodeHelper.h"
#include "CodecTools.h"
#include "ScopedLocalRef.h"

#include "UserTrace.h"

void CPPToJavaArchiveUpdateCallback::freeOutItem(JNIEnvInstance &jniEnvInstance) {
    if (_outItem) {
        jniEnvInstance->DeleteGlobalRef(_outItem);
        _outItem = nullptr;
    }
}

LONG CPPToJavaArchiveUpdateCallback::getOrUpdateOutItem(JNIEnvInstance &jniEnvInstance, int index) {
    if (_outItemLastIndex == index && _outItem) {
        return S_OK;
    }

    freeOutItem(jniEnvInstance);

    ScopedLocalRef<jobject> outItemFactory(jniEnvInstance,
                                           jni::OutItemFactory::newInstance(jniEnvInstance,
                                                                            _outArchive, index));
    if (jniEnvInstance.exceptionCheck()) {
        return S_FALSE;
    }

    ScopedLocalRef<jobject> outItem(jniEnvInstance,
                                    _iOutCreateCallback->getItemInformation(jniEnvInstance,
                                                                            _javaImplementation,
                                                                            index,
                                                                            outItemFactory.get()));
    if (jniEnvInstance.exceptionCheck()) {
        return S_FALSE;
    }

    if (outItem.get() == nullptr) {
        jniEnvInstance.reportError("IOutCreateCallback.getItemInformation() should return "
                                   "a non-null reference to an item information object. Use outItemFactory to create an instance. "
                                   "Fill the new object with all necessary information about the archive item being processed.");
        return S_FALSE;
    }


    jni::OutItem::verify(jniEnvInstance, outItem.get(),
                         static_cast<jboolean>(_isInArchiveAttached));
    if (jniEnvInstance.exceptionCheck()) {
        return S_FALSE;
    }

    _outItem = jniEnvInstance->NewGlobalRef(outItem.get());
    _outItemLastIndex = index;

    return S_OK;
}


STDMETHODIMP CPPToJavaArchiveUpdateCallback::GetUpdateItemInfo(UInt32 index,
                                                               Int32 *newData, /*1 - new data, 0 - old data */
                                                               Int32 *newProperties, /* 1 - new properties, 0 - old properties */
                                                               UInt32 *indexInArchive /* -1 if there is no in archive, or if doesn't matter */
) {
    TRACE_OBJECT_CALL("GetUpdateItemInfo");

    JNIEnvInstance jniEnvInstance(_jbindingSession);

    LONG result = getOrUpdateOutItem(jniEnvInstance, index);
    if (result) {
        return result;
    }

    UString traceMsg;
    bool isUserTrace = isUserTraceEnabled(jniEnvInstance, _outArchive);

    if (isUserTrace) {
        traceMsg += L"Get update info";
    }

    if (newData) {
        if (_isInArchiveAttached) {
            ScopedLocalRef<jobject> newDataObject(jniEnvInstance,
                                                  jni::OutItem::updateIsNewData_Get(jniEnvInstance,
                                                                                    _outItem));
            if (newDataObject.get()) {
                *newData = jni::Boolean::booleanValue(jniEnvInstance, newDataObject.get());
                if (jniEnvInstance.exceptionCheck()) {
                    return S_FALSE;
                }
            } else {
                jniEnvInstance.reportError(
                        "The attribute 'updateNewData' of the corresponding IOutItem* class shouldn't be null (index=%i)",
                        index);
                return S_FALSE;
            }
        } else {
            *newData = 1;
        }
        if (isUserTrace) {
            traceMsg += L" (new data: ";
            if (*newData) {
                traceMsg += L"true)";
            } else {
                traceMsg += L"false)";
            }
        }
    }

    if (newProperties) {
        if (_isInArchiveAttached) {
            ScopedLocalRef<jobject> newPropertiesObject(jniEnvInstance,
                                                        jni::OutItem::updateIsNewProperties_Get(
                                                                jniEnvInstance,
                                                                _outItem));
            if (newPropertiesObject.get()) {
                *newProperties = jni::Boolean::booleanValue(jniEnvInstance,
                                                            newPropertiesObject.get());
                if (jniEnvInstance.exceptionCheck()) {
                    return S_FALSE;
                }
            } else {
                jniEnvInstance.reportError(
                        "The attribute 'updateNewProperties' of the corresponding IOutItem* class shouldn't be null (index=%i)",
                        index);
                return S_FALSE;
            }
        } else {
            *newProperties = 1;
        }
        if (isUserTrace) {
            traceMsg += L" (new props: ";
            if (*newProperties) {
                traceMsg += L"true)";
            } else {
                traceMsg += L"false)";
            }
        }
    }

    if (indexInArchive) {
        if (_isInArchiveAttached) {
            ScopedLocalRef<jobject> oldArchiveItemIndexObject(jniEnvInstance,
                                                              jni::OutItem::updateOldArchiveItemIndex_Get(
                                                                      jniEnvInstance, _outItem));
            if (oldArchiveItemIndexObject.get()) {
                *indexInArchive = (UInt32) jni::Integer::intValue(jniEnvInstance,
                                                                  oldArchiveItemIndexObject.get());
                if (jniEnvInstance.exceptionCheck()) {
                    return S_FALSE;
                }
            } else {
                *indexInArchive = (UInt32) -1;
            }
        } else {
            *indexInArchive = (UInt32) -1;
        }
        if (isUserTrace) {
            traceMsg += UString(L" (old index: ") << (Int32) *indexInArchive << L")";
        }
    }

    if (isUserTrace) {
        traceMsg += UString(L" (index: ") << index << L")";
        userTrace(jniEnvInstance, _outArchive, traceMsg);
    }

    return S_OK;
}

STDMETHODIMP CPPToJavaArchiveUpdateCallback::GetProperty(UInt32 index, PROPID propID,
                                                         PROPVARIANT *value) {

//	#define JNI_TYPE_STRING                              jstring
//	#define JNI_TYPE_INTEGER                             jobject
//	#define JNI_TYPE_UINTEGER                            jobject
//	#define JNI_TYPE_DATE                                jobject
//	#define JNI_TYPE_BOOLEAN                             jboolean
//	#define JNI_TYPE_LONG                                jlong

#define ASSIGN_VALUE_TO_C_PROP_VARIANT_STRING(_value)                                                               \
        const jchar * jChars = jniEnvInstance->GetStringChars((jstring)_value, NULL);                               \
        if (!jChars) {                                                                                              \
            return S_FALSE;                                                                                         \
        }                                                                                                           \
        cPropVariant = UString(UnicodeHelper(jChars,jniEnvInstance->GetStringLength((jstring)_value)));             \
        jniEnvInstance->ReleaseStringChars((jstring)_value, jChars);                                                \

#define ASSIGN_VALUE_TO_C_PROP_VARIANT_INTEGER(_value)                                                              \
        cPropVariant = (Int32)jni::Integer::intValue(jniEnvInstance, _value);                                       \
        if (jniEnvInstance.exceptionCheck()) {                                                                      \
            return S_FALSE;                                                                                         \
        }                                                                                                           \

#define ASSIGN_VALUE_TO_C_PROP_VARIANT_BOOLEAN(_value)                                                              \
        cPropVariant = (bool)jni::Boolean::booleanValue(jniEnvInstance, _value);                                    \
        if (jniEnvInstance.exceptionCheck()) {                                                                      \
            return S_FALSE;                                                                                         \
        }                                                                                                           \

#define ASSIGN_VALUE_TO_C_PROP_VARIANT_LONG(_value)                                                                 \
        cPropVariant = (UInt64)jni::Long::longValue(jniEnvInstance, _value);                                        \
        if (jniEnvInstance.exceptionCheck()) {                                                                      \
            return S_FALSE;                                                                                         \
        }                                                                                                           \

#define ASSIGN_VALUE_TO_C_PROP_VARIANT_UINTEGER(_value)                                                             \
        cPropVariant = (UInt32)jni::Integer::intValue(jniEnvInstance, _value);                                      \
        if (jniEnvInstance.exceptionCheck()) {                                                                      \
            return S_FALSE;                                                                                         \
        }                                                                                                           \

#define ASSIGN_VALUE_TO_C_PROP_VARIANT_DATE(_value)                                                                 \
        FILETIME filetime;                                                                                          \
        if (!ObjectToFILETIME(jniEnvInstance, _value, filetime)) {                                                  \
            return S_FALSE;                                                                                         \
        }                                                                                                           \
        cPropVariant = filetime;                                                                                    \

#define GET_ATTRIBUTE(TYPE, fieldName)                                                                              \
    {                                                                                                               \
        if (isUserTraceEnabled(jniEnvInstance, _outArchive)) {                                                      \
            userTrace(jniEnvInstance, _outArchive,                                                                  \
                UString(L"Get property '" #fieldName "' (index: ") << index << L")");                               \
        }                                                                                                           \
        jobject jvalue = jni::OutItem::fieldName##_Get(jniEnvInstance, _outItem);                                   \
        if (jvalue) {                                                                                               \
            ASSIGN_VALUE_TO_C_PROP_VARIANT_##TYPE(jvalue);                                                          \
            jniEnvInstance->DeleteLocalRef(jvalue);                                                                 \
        }                                                                                                           \
        break;                                                                                                      \
    }

    TRACE_OBJECT_CALL("GetProperty");
    JNIEnvInstance jniEnvInstance(_jbindingSession);

    if (!value) {
        return S_OK;
    }

    value->vt = VT_NULL;
    NWindows::NCOM::CPropVariant cPropVariant;

    if (propID == kpidIsDir
        && (codecTools.isGZipArchive(_archiveFormatIndex)
            || codecTools.isXZArchive(_archiveFormatIndex)
            || codecTools.isZstdArchive(_archiveFormatIndex)
            || codecTools.isBZip2Archive(_archiveFormatIndex))) {
        cPropVariant = false;
        cPropVariant.Detach(value);
        return S_OK;
    }

    if (propID == kpidTimeType) {
        cPropVariant = NFileTimeType::kWindows;
        cPropVariant.Detach(value);
        return S_OK;
    }

    LONG result = getOrUpdateOutItem(jniEnvInstance, index);
    if (result) {
        return result;
    }

    switch (propID) {
        case kpidSize: GET_ATTRIBUTE(LONG, dataSize)

        case kpidAttrib: GET_ATTRIBUTE(UINTEGER, propertyAttributes)
        case kpidPosixAttrib: GET_ATTRIBUTE(UINTEGER, propertyPosixAttributes)
        case kpidPath: GET_ATTRIBUTE(STRING, propertyPath)
        case kpidIsDir: GET_ATTRIBUTE(BOOLEAN, propertyIsDir)
        case kpidIsAnti: GET_ATTRIBUTE(BOOLEAN, propertyIsAnti)
        case kpidMTime: GET_ATTRIBUTE(DATE, propertyLastModificationTime)
        case kpidATime: GET_ATTRIBUTE(DATE, propertyLastAccessTime)
        case kpidCTime: GET_ATTRIBUTE(DATE, propertyCreationTime)
        case kpidUser: GET_ATTRIBUTE(STRING, propertyUser)
        case kpidGroup: GET_ATTRIBUTE(STRING, propertyGroup)
        case kpidSymLink: GET_ATTRIBUTE(STRING, propertySymlink)
        case kpidHardLink:
            break;

        case kpidTimeType: // Should be processed by now
        default:
#ifdef _DEBUG
            printf("kpidNoProperty: %i\n", (int) kpidNoProperty);
            printf("kpidMainSubfile: %i\n", (int) kpidMainSubfile);
            printf("kpidHandlerItemIndex: %i\n", (int) kpidHandlerItemIndex);
            printf("kpidPath: %i\n", (int) kpidPath);
            printf("kpidName: %i\n", (int) kpidName);
            printf("kpidExtension: %i\n", (int) kpidExtension);
            printf("kpidIsDir: %i\n", (int) kpidIsDir);
            printf("kpidSize: %i\n", (int) kpidSize);
            printf("kpidPackSize: %i\n", (int) kpidPackSize);
            printf("kpidAttrib: %i\n", (int) kpidAttrib);
            printf("kpidCTime: %i\n", (int) kpidCTime);
            printf("kpidATime: %i\n", (int) kpidATime);
            printf("kpidMTime: %i\n", (int) kpidMTime);
            printf("kpidSolid: %i\n", (int) kpidSolid);
            printf("kpidCommented: %i\n", (int) kpidCommented);
            printf("kpidEncrypted: %i\n", (int) kpidEncrypted);
            printf("kpidSplitBefore: %i\n", (int) kpidSplitBefore);
            printf("kpidSplitAfter: %i\n", (int) kpidSplitAfter);
            printf("kpidDictionarySize: %i\n", (int) kpidDictionarySize);
            printf("kpidCRC: %i\n", (int) kpidCRC);
            printf("kpidType: %i\n", (int) kpidType);
            printf("kpidIsAnti: %i\n", (int) kpidIsAnti);
            printf("kpidMethod: %i\n", (int) kpidMethod);
            printf("kpidHostOS: %i\n", (int) kpidHostOS);
            printf("kpidFileSystem: %i\n", (int) kpidFileSystem);
            printf("kpidUser: %i\n", (int) kpidUser);
            printf("kpidGroup: %i\n", (int) kpidGroup);
            printf("kpidBlock: %i\n", (int) kpidBlock);
            printf("kpidComment: %i\n", (int) kpidComment);
            printf("kpidPosition: %i\n", (int) kpidPosition);
            printf("kpidPrefix: %i\n", (int) kpidPrefix);
            printf("kpidNumSubDirs: %i\n", (int) kpidNumSubDirs);
            printf("kpidNumSubFiles: %i\n", (int) kpidNumSubFiles);
            printf("kpidUnpackVer: %i\n", (int) kpidUnpackVer);
            printf("kpidVolume: %i\n", (int) kpidVolume);
            printf("kpidIsVolume: %i\n", (int) kpidIsVolume);
            printf("kpidOffset: %i\n", (int) kpidOffset);
            printf("kpidLinks: %i\n", (int) kpidLinks);
            printf("kpidNumBlocks: %i\n", (int) kpidNumBlocks);
            printf("kpidNumVolumes: %i\n", (int) kpidNumVolumes);
            printf("kpidTimeType: %i\n", (int) kpidTimeType);
            printf("kpidBit64: %i\n", (int) kpidBit64);
            printf("kpidBigEndian: %i\n", (int) kpidBigEndian);
            printf("kpidCpu: %i\n", (int) kpidCpu);
            printf("kpidPhySize: %i\n", (int) kpidPhySize);
            printf("kpidHeadersSize: %i\n", (int) kpidHeadersSize);
            printf("kpidChecksum: %i\n", (int) kpidChecksum);
            printf("kpidCharacts: %i\n", (int) kpidCharacts);
            printf("kpidVa: %i\n", (int) kpidVa);
            printf("kpidId: %i\n", (int) kpidId);
            printf("kpidShortName: %i\n", (int) kpidShortName);
            printf("kpidCreatorApp: %i\n", (int) kpidCreatorApp);
            printf("kpidSectorSize: %i\n", (int) kpidSectorSize);
            printf("kpidPosixAttrib: %i\n", (int) kpidPosixAttrib);
            printf("kpidLink: %i\n", (int) kpidLink);
            printf("kpidTotalSize: %i\n", (int) kpidTotalSize);
            printf("kpidFreeSpace: %i\n", (int) kpidFreeSpace);
            printf("kpidClusterSize: %i\n", (int) kpidClusterSize);
            printf("kpidVolumeName: %i\n", (int) kpidVolumeName);
            printf("kpidLocalName: %i\n", (int) kpidLocalName);
            printf("kpidProvider: %i\n", (int) kpidProvider);
            printf("kpidUserDefined: %i\n", (int) kpidUserDefined);
#endif // _DEBUG

            jniEnvInstance.reportError(
                    "CPPToJavaArchiveUpdateCallback::GetProperty() : unexpected propID=%u", propID);
            return S_FALSE;
    }

    cPropVariant.Detach(value);

    return S_OK;
}

STDMETHODIMP
CPPToJavaArchiveUpdateCallback::GetStream(UInt32 index, ISequentialInStream **inStream) {
    TRACE_OBJECT_CALL("GetStream");
    JNIEnvInstance jniEnvInstance(_jbindingSession);

    if (!inStream) {
        return S_OK;
    }

    if (isUserTraceEnabled(jniEnvInstance, _outArchive)) {
        userTrace(jniEnvInstance, _outArchive,
                  UString(L"Get stream (index: ") << index << L")");
    }

    ScopedLocalRef<jobject> inStreamImpl(jniEnvInstance,
                                         _iOutCreateCallback->getStream(jniEnvInstance,
                                                                        _javaImplementation,
                                                                        index));
    if (jniEnvInstance.exceptionCheck()) {
        return S_FALSE;
    }

    if (inStreamImpl.get()) {

        if (jniEnvInstance->IsInstanceOf(inStreamImpl.get(), _inStreamInterface)) {
            CPPToJavaInStream *newInStream = new CPPToJavaInStream(_jbindingSession, jniEnvInstance,
                                                                   inStreamImpl.get());
            CMyComPtr<IInStream> inStreamComPtr = newInStream;
            *inStream = inStreamComPtr.Detach();
        } else {
            CPPToJavaSequentialInStream *newInStream = new CPPToJavaSequentialInStream(
                    _jbindingSession, jniEnvInstance, inStreamImpl.get());
            CMyComPtr<ISequentialInStream> inStreamComPtr = newInStream;
            *inStream = inStreamComPtr.Detach();
        }
    } else {
        return S_FALSE;
    }

    return S_OK;
}

STDMETHODIMP CPPToJavaArchiveUpdateCallback::SetOperationResult(Int32 operationResult) {
    TRACE_OBJECT_CALL("SetOperationResult");
    JNIEnvInstance jniEnvInstance(_jbindingSession);

    jboolean operationResultBoolean = static_cast<jboolean>(operationResult ==
                                                            NArchive::NUpdate::NOperationResult::kOK);

    _iOutCreateCallback->setOperationResult(jniEnvInstance, _javaImplementation,
                                            operationResultBoolean);
    if (jniEnvInstance.exceptionCheck()) {
        return S_FALSE;
    }

    return S_OK;
}
