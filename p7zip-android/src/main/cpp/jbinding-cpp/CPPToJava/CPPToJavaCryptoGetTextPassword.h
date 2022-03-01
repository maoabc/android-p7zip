#ifndef CPPTOJAVACRYPTOGETTEXTPASSWORD_H_
#define CPPTOJAVACRYPTOGETTEXTPASSWORD_H_

#include "CPPToJavaAbstract.h"
#include "JavaStatInfos/JavaPackageSevenZip.h"

class CPPToJavaCryptoGetTextPassword
        : public CPPToJavaAbstract,
          public virtual ICryptoGetTextPassword,
          public CMyUnknownImp {

private:
    jni::ICryptoGetTextPassword *_iCryptoGetTextPassword;
public:
    MY_UNKNOWN_IMP

    CPPToJavaCryptoGetTextPassword(JBindingSession &jbindingSession, JNIEnv *initEnv,
                                   jobject cryptoGetTextPassword) :
            CPPToJavaAbstract(jbindingSession, initEnv, cryptoGetTextPassword),
            _iCryptoGetTextPassword(jni::ICryptoGetTextPassword::_getInstanceFromObject(initEnv,
                                                                                        cryptoGetTextPassword)) {
        TRACE_OBJECT_CREATION("CPPToJavaCryptoGetTextPassword")
    }

    STDMETHOD(CryptoGetTextPassword)(BSTR *password);
};

class CPPToJavaCryptoGetTextPassword2
        : public CPPToJavaAbstract,
          public virtual ICryptoGetTextPassword2,
          public CMyUnknownImp {

private:
    jni::ICryptoGetTextPassword2 *_iCryptoGetTextPassword2;
public:
    MY_UNKNOWN_IMP

    CPPToJavaCryptoGetTextPassword2(JBindingSession &jbindingSession, JNIEnv *initEnv,
                                   jobject cryptoGetTextPassword) :
            CPPToJavaAbstract(jbindingSession, initEnv, cryptoGetTextPassword),
            _iCryptoGetTextPassword2(jni::ICryptoGetTextPassword2::_getInstanceFromObject(initEnv,
                                                                                        cryptoGetTextPassword)) {
        TRACE_OBJECT_CREATION("CPPToJavaCryptoGetTextPassword2")
    }

    STDMETHOD(CryptoGetTextPassword2)(Int32 *passwordIsDefined, BSTR *password);
};

#endif /*CPPTOJAVACRYPTOGETTEXTPASSWORD_H_*/
