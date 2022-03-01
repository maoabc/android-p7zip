#ifndef CPPTOJAVASEQUENTIALOUTSTREAM_H_
#define CPPTOJAVASEQUENTIALOUTSTREAM_H_

#include "7zip/Archive/IArchive.h"
#include "Common/MyCom.h"
#include "CPPToJavaAbstract.h"
#include "JavaStatInfos/JavaPackageSevenZip.h"

class CPPToJavaSequentialOutStream : public CPPToJavaAbstract,
	public virtual ISequentialOutStream,
	public CMyUnknownImp
{
private:
    jni::ISequentialOutStream * _iSequentialOutStream;
public:
	MY_UNKNOWN_IMP1(ISequentialOutStream)


	CPPToJavaSequentialOutStream(JBindingSession & jbindingSession, JNIEnv * initEnv, jobject javaSequentialOutStreamImpl) :
		CPPToJavaAbstract(jbindingSession, initEnv, javaSequentialOutStreamImpl),
		        _iSequentialOutStream(jni::ISequentialOutStream::_getInstanceFromObject(initEnv, javaSequentialOutStreamImpl))
	{
	    TRACE_OBJECT_CREATION("CPPToJavaSequentialOutStream")
	}

	STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize);
};


#endif /*CPPTOJAVASEQUENTIALOUTSTREAM_H_*/
