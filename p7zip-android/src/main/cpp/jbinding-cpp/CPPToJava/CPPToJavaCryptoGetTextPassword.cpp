#include "SevenZipJBinding.h"

#include "JNITools.h"
#include "CPPToJavaCryptoGetTextPassword.h"
#include "UnicodeHelper.h"
#include "ScopedLocalRef.h"

STDMETHODIMP CPPToJavaCryptoGetTextPassword::CryptoGetTextPassword(BSTR *password) {
    TRACE_OBJECT_CALL("CryptoGetTextPassword");

    JNIEnvInstance jniEnvInstance(_jbindingSession);

    if (password) {
        *password = nullptr;
    }

    ScopedLocalRef<jstring> passwordString(jniEnvInstance,
                                           _iCryptoGetTextPassword->cryptoGetTextPassword(
                                                   jniEnvInstance, _javaImplementation));
    if (jniEnvInstance.exceptionCheck()) {
        return S_FALSE;
    }

    if(!passwordString.get()){
        return S_FALSE;
    }

    if (password) {
        const jchar *passwordJChars = jniEnvInstance->GetStringChars(passwordString.get(), nullptr);
        //CMyComBSTR passwordBSTR((OLECHAR*)(const wchar_t*)UnicodeHelper(passwordJChars));
        //CMyComBSTR passwordBSTR(L"TestXXX");

        //printf("PASSWORD: '%S'\n", (BSTR)passwordBSTR);
        //fflush(stdout);
        StringToBstr(UString(UnicodeHelper(passwordJChars,
                                           static_cast<size_t>(jniEnvInstance->GetStringLength(
                                                   passwordString.get())))),
                     password);//passwordBSTR.MyCopy();
        jniEnvInstance->ReleaseStringChars(passwordString.get(), passwordJChars);
    }

    return S_OK;
}

STDMETHODIMP
CPPToJavaCryptoGetTextPassword2::CryptoGetTextPassword2(Int32 *passwordIsDefined, BSTR *password) {
    JNIEnvInstance jniEnvInstance(_jbindingSession);
    if (passwordIsDefined) {
        *passwordIsDefined = 0;
    }
    if (password) {
        *password = nullptr;
    }

    ScopedLocalRef<jstring> passwordString(jniEnvInstance,
                                           _iCryptoGetTextPassword2->cryptoGetTextPassword(
                                                   jniEnvInstance, _javaImplementation));

    if (jniEnvInstance.exceptionCheck()) {
        return S_FALSE;
    }
    if (!passwordString.get()) {
        return S_OK;
    }

    if (password) {
        const jchar *passwordJChars = jniEnvInstance->GetStringChars(passwordString.get(), nullptr);
        //CMyComBSTR passwordBSTR((OLECHAR*)(const wchar_t*)UnicodeHelper(passwordJChars));
        //CMyComBSTR passwordBSTR(L"TestXXX");

        //printf("PASSWORD: '%S'\n", (BSTR)passwordBSTR);
        //fflush(stdout);
        StringToBstr(UString(UnicodeHelper(passwordJChars,
                                           static_cast<size_t>(jniEnvInstance->GetStringLength(
                                                   passwordString.get())))),
                     password);//passwordBSTR.MyCopy();
        jniEnvInstance->ReleaseStringChars(passwordString.get(), passwordJChars);
        if (passwordIsDefined) {
            *passwordIsDefined = 1;
        }
    }
    return S_OK;
}
