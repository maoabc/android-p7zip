#ifndef JAVAPACKAGESEVENZIP_H_
#define JAVAPACKAGESEVENZIP_H_

#include "JavaStaticInfo.h"

#define SEVEN_ZIP_PACKAGE                                "net/sf/sevenzipjbinding"
#define SEVEN_ZIP_PACKAGE_IMPL                            "net/sf/sevenzipjbinding/impl"

#define JAVA_PROP_ID                                    SEVEN_ZIP_PACKAGE "/PropID"
#define JAVA_PROP_ID_T                                    JAVA_MAKE_SIGNATURE_TYPE(JAVA_PROP_ID)

#define JAVA_EXTRACT_ASK_MODE                            SEVEN_ZIP_PACKAGE "/ExtractAskMode"
#define JAVA_EXTRACT_ASK_MODE_T                        JAVA_MAKE_SIGNATURE_TYPE(JAVA_EXTRACT_ASK_MODE)

#define JAVA_EXTRACT_OPERATION_RESULT                    SEVEN_ZIP_PACKAGE "/ExtractOperationResult"
#define JAVA_EXTRACT_OPERATION_RESULT_T                JAVA_MAKE_SIGNATURE_TYPE(JAVA_EXTRACT_OPERATION_RESULT)

#define JAVA_IIN_ARCHIVE                                SEVEN_ZIP_PACKAGE "/IInArchive"
#define JAVA_IIN_ARCHIVE_T                                JAVA_MAKE_SIGNATURE_TYPE(JAVA_IIN_ARCHIVE)

#define JAVA_IIN_STREAM                                SEVEN_ZIP_PACKAGE "/IInStream"
#define JAVA_IIN_STREAM_T                                JAVA_MAKE_SIGNATURE_TYPE(JAVA_IIN_STREAM)

#define JAVA_ISEQUENTIAL_IN_STREAM                        SEVEN_ZIP_PACKAGE "/ISequentialInStream"
#define JAVA_ISEQUENTIAL_IN_STREAM_T                    JAVA_MAKE_SIGNATURE_TYPE(JAVA_ISEQUENTIAL_IN_STREAM)

#define JAVA_ISEQUENTIAL_OUT_STREAM                    SEVEN_ZIP_PACKAGE "/ISequentialOutStream"
#define JAVA_ISEQUENTIAL_OUT_STREAM_T                    JAVA_MAKE_SIGNATURE_TYPE(JAVA_ISEQUENTIAL_OUT_STREAM)

#define JAVA_IOUT_ARCHIVE                               SEVEN_ZIP_PACKAGE "/IOutArchive"
#define JAVA_IOUT_ARCHIVE_T                             JAVA_MAKE_SIGNATURE_TYPE(JAVA_IOUT_ARCHIVE)

#define JAVA_IOUT_ITEM_CALLBACK_BASE                    SEVEN_ZIP_PACKAGE "/IOutItemCallbackBase"
#define JAVA_IOUT_ITEM_CALLBACK_BASE_T                    JAVA_MAKE_SIGNATURE_TYPE(JAVA_IOUT_ITEM_CALLBACK_BASE)

#define JAVA_ARCHIVE_FORMAT                                SEVEN_ZIP_PACKAGE "/ArchiveFormat"
#define JAVA_ARCHIVE_FORMAT_T                            JAVA_MAKE_SIGNATURE_TYPE(JAVA_ARCHIVE_FORMAT)

#define JAVA_OUT_ITEM_FACTORY                           SEVEN_ZIP_PACKAGE_IMPL "/OutItemFactory"
#define JAVA_OUT_ITEM_FACTORY_T                         JAVA_MAKE_SIGNATURE_TYPE(JAVA_OUT_ITEM_FACTORY)

#define JAVA_OUT_ITEM                                   SEVEN_ZIP_PACKAGE_IMPL "/OutItem"
#define JAVA_OUT_ITEM_T                                 JAVA_MAKE_SIGNATURE_TYPE(JAVA_OUT_ITEM)

#define JAVA_IOUT_ITEM_BASE                             SEVEN_ZIP_PACKAGE "/IOutItemBase"
#define JAVA_IOUT_ITEM_BASE_T                           JAVA_MAKE_SIGNATURE_TYPE(JAVA_IOUT_ITEM_BASE)

#define JT_PROP_ID(name, param_spec)                    JT_PARAM(Object, JAVA_PROP_ID_T, name, param_spec)
#define JT_EXTRACT_ASK_MODE(name, param_spec)        JT_PARAM(Object, JAVA_EXTRACT_ASK_MODE_T, name, param_spec)
#define JT_EXTRACT_OPERATION_RESULT(name, param_spec)   JT_PARAM(Object, JAVA_EXTRACT_OPERATION_RESULT_T, name, param_spec)


JT_BEGIN_INTERFACE(ISequentialInStream)
        // public int read(byte[] data,int len)
//	JT_INTERFACE_METHOD(Int, read, JT_BYTE_ARRAY(data,JT_INT(len, _)))
        // public int read(ByteBuffer data,int len)
    JT_INTERFACE_METHOD(Int, read, JT_BYTE_BUFFER_OBJECT(data, JT_INT(len, _)))
JT_END_CLASS


JT_BEGIN_INTERFACE(IArchiveOpenCallback)
        // public boolean setTotal(long files, long bytes)
    JT_INTERFACE_METHOD(Boolean, setTotal, JT_LONG(files, JT_LONG(bytes, _)))

        // public boolean setCompleted(long files, long bytes)
    JT_INTERFACE_METHOD(Boolean, setCompleted, JT_LONG(files, JT_LONG(bytes, _)))
JT_END_INTERFACE


JT_BEGIN_INTERFACE(IArchiveOpenVolumeCallback)
        // public Object getProperty(PropID propId)
    JT_INTERFACE_METHOD(Object, getProperty, JT_PROP_ID(propId, _))

        // public IInStream getStream(String filename)
    JT_INTERFACE_METHOD_OBJECT(JAVA_IIN_STREAM_T, getStream, JT_STRING(filename, _))
JT_END_INTERFACE


JT_BEGIN_INTERFACE(ICryptoGetTextPassword)
        // public String cryptoGetTextPassword()
    JT_INTERFACE_METHOD(String, cryptoGetTextPassword, _)
JT_END_INTERFACE

JT_BEGIN_INTERFACE(ICryptoGetTextPassword2)
        // public String cryptoGetTextPassword()
    JT_INTERFACE_METHOD(String, cryptoGetTextPassword, _)
JT_END_INTERFACE

JT_BEGIN_INTERFACE(IProgress)
        // public void setTotal(long total)
    JT_INTERFACE_METHOD(Void, setTotal, JT_LONG(total, _))

        // public void setCompleted(long completeValue)
    JT_INTERFACE_METHOD(Void, setCompleted, JT_LONG(completeValue, _))
JT_END_INTERFACE


JT_BEGIN_INTERFACE(IArchiveExtractCallback)
        // public ISequentialOutStream getStream(int index, ExtractAskMode extractAskMode)
    JT_INTERFACE_METHOD_OBJECT(JAVA_ISEQUENTIAL_OUT_STREAM_T,
                               getStream, JT_INT(index, JT_EXTRACT_ASK_MODE(extractAskMode, _)))

        // public void prepareOperation(ExtractAskMode extractAskMode)
    JT_INTERFACE_METHOD(Void, prepareOperation, JT_EXTRACT_ASK_MODE(extractAskMode, _))

        // public void setOperationResult(ExtractOperationResult extractOperationResult)
    JT_INTERFACE_METHOD(Void, setOperationResult,
                        JT_EXTRACT_OPERATION_RESULT(extractOperationResult, _))
JT_END_INTERFACE


JT_BEGIN_INTERFACE(ISequentialOutStream)
        // public int write(byte[] data,int len)
//	JT_INTERFACE_METHOD(Int, write, JT_BYTE_ARRAY(data, JT_INT(len, _)))

        // public int write(byte[] data,int len)
    JT_INTERFACE_METHOD(Int, write, JT_BYTE_BUFFER_OBJECT(buffer, JT_INT(len, _)))
JT_END_INTERFACE


JT_BEGIN_INTERFACE(IOutStream)
        // public void setSize(long newSize)
    JT_INTERFACE_METHOD(Void, setSize, JT_LONG(newSize, _))
JT_END_INTERFACE


JT_BEGIN_INTERFACE(ISeekableStream)
        // public long seek(long offset, int seekOrigin) throws SevenZipException;
    JT_INTERFACE_METHOD(Long, seek, JT_LONG(offset, JT_INT(seekOrigin, _)))
JT_END_INTERFACE


JT_BEGIN_INTERFACE(IOutCreateCallback)
        // public void setOperationResult(boolean operationResultOk)
    JT_INTERFACE_METHOD(Void, setOperationResult, JT_BOOLEAN(operationResultOk, _))

        // public E getItemInformation(int index, OutItemFactory<E> outItemFactory) throws SevenZipException;
    JT_INTERFACE_METHOD_OBJECT(JAVA_IOUT_ITEM_BASE_T, getItemInformation, JT_INT(index,
                                                                                 JT_PARAM(Object,
                                                                                          JAVA_OUT_ITEM_FACTORY_T,
                                                                                          outItemFactory,
                                                                                          _)))

        // public ISequentialInStream getStream(int index) throws SevenZipException;
    JT_INTERFACE_METHOD_OBJECT(JAVA_ISEQUENTIAL_IN_STREAM_T, getStream, JT_INT(index, _))
JT_END_INTERFACE

JT_BEGIN_CLASS(SEVEN_ZIP_PACKAGE_IMPL, OutItemFactory)
        // public OutItemFactory(ArchiveFormat archiveFormat, int index) {
    JT_CLASS_CONSTRUCTOR(JT_PARAM(Object, JAVA_IOUT_ARCHIVE_T, outArchive, JT_INT(index, _)))
JT_END_CLASS

JT_BEGIN_CLASS(SEVEN_ZIP_PACKAGE_IMPL, OutItem)

    JT_CLASS_CONSTRUCTOR(_)

    JT_FIELD_OBJECT(index, JAVA_INTEGER_T)

    JT_FIELD_OBJECT(dataSize, JAVA_LONG_T)

    JT_FIELD_OBJECT(propertyAttributes, JAVA_INTEGER_T)

    JT_FIELD_OBJECT(propertyPosixAttributes, JAVA_INTEGER_T)

    JT_FIELD_OBJECT(propertyPath, JAVA_STRING_T)

    JT_FIELD_OBJECT(propertyIsDir, JAVA_BOOLEAN_T)

    JT_FIELD_OBJECT(propertyLastModificationTime, JAVA_DATE_T)

    JT_FIELD_OBJECT(propertyLastAccessTime, JAVA_DATE_T)

    JT_FIELD_OBJECT(propertyCreationTime, JAVA_DATE_T)

    JT_FIELD_OBJECT(propertyUser, JAVA_STRING_T)

    JT_FIELD_OBJECT(propertyGroup, JAVA_STRING_T)

    JT_FIELD_OBJECT(propertySymlink, JAVA_STRING_T)

    JT_FIELD_OBJECT(propertyIsAnti, JAVA_BOOLEAN_T)

    JT_FIELD_OBJECT(updateIsNewData, JAVA_BOOLEAN_T)

    JT_FIELD_OBJECT(updateIsNewProperties, JAVA_BOOLEAN_T)

    JT_FIELD_OBJECT(updateOldArchiveItemIndex, JAVA_INTEGER_T)

        // final void verify(boolean update)
    JT_CLASS_FINAL_METHOD(Void, verify, JT_BOOLEAN(update, _))
JT_END_CLASS


JT_BEGIN_CLASS(SEVEN_ZIP_PACKAGE_IMPL, InArchiveImpl)

    JT_FIELD(Long, jbindingSession)

    JT_FIELD(Long, sevenZipArchiveInstance)

    JT_FIELD(Long, sevenZipInStreamInstance)

    JT_CLASS_FINAL_METHOD(Void, setArchiveFormat, JT_STRING(archiveFormat, _))
JT_END_CLASS


JT_BEGIN_CLASS(SEVEN_ZIP_PACKAGE_IMPL, OutArchiveImpl)

    JT_FIELD(Long, jbindingSession)

    JT_FIELD(Long, sevenZipArchiveInstance)

    JT_FIELD_OBJECT(archiveFormat, JAVA_ARCHIVE_FORMAT_T)

    JT_FIELD_OBJECT(inArchive, JAVA_IIN_ARCHIVE_T)

    JT_FIELD(Boolean, trace)

    JT_CLASS_FINAL_METHOD(Void, traceMessage, JT_STRING(message, _))
JT_END_CLASS


JT_BEGIN_CLASS(SEVEN_ZIP_PACKAGE, PropertyInfo)

    JT_FIELD(String, name)

    JT_FIELD_OBJECT(propID, JAVA_PROP_ID_T)

    JT_FIELD(Class, varType)
JT_END_CLASS


JT_BEGIN_CLASS(SEVEN_ZIP_PACKAGE, PropID)
        // public static PropID getPropIDByIndex(int propIDIndex)
    JT_CLASS_STATIC_METHOD_OBJECT(JAVA_PROP_ID_T, getPropIDByIndex, JT_INT(propIDIndex, _))
JT_END_CLASS


JT_BEGIN_CLASS(SEVEN_ZIP_PACKAGE, ExtractAskMode)
        // public static ExtractAskMode getExtractAskModeByIndex(int index)
    JT_CLASS_STATIC_METHOD_OBJECT(JAVA_EXTRACT_ASK_MODE_T, getExtractAskModeByIndex,
                                  JT_INT(index, _))
JT_END_CLASS


JT_BEGIN_CLASS(SEVEN_ZIP_PACKAGE, ExtractOperationResult)
        // public static ExtractOperationResult getOperationResult(int index)
    JT_CLASS_STATIC_METHOD_OBJECT(JAVA_EXTRACT_OPERATION_RESULT_T, getOperationResult,
                                  JT_INT(index, _))
JT_END_CLASS


JT_BEGIN_CLASS(SEVEN_ZIP_PACKAGE, ArchiveFormat)

    JT_FIELD(Int, codecIndex)

    JT_FIELD(String, methodName)
JT_END_CLASS

#endif /* JAVAPACKAGESEVENZIP_H_ */
