# 7-Zip-JBinding
原版的[7-Zip-JBinding](http://sevenzipjbind.sourceforge.net/)在安卓上使用, 有严重的内存抖动问题, 主要对这个问题进行优化, 
同时也修复一些引用问题及解决zip乱码.

## 优化过的地方及原因
原7-zip-jbinding对输入输出流的处理, 每次会在循环里创建数组再复制数据传递给java层的接口:
```c++
//输入流native层实现
STDMETHODIMP CPPToJavaSequentialInStream::Read(void *data, UInt32 size, UInt32 *processedSize)
{
    ...

        //这里每次需要回调java层时,都会新建一个数组
	jbyteArray byteArray = jniEnvInstance->NewByteArray(size);
    ...

        //传递给java层接口
	jint wasRead = _iSequentialInStream->read(jniEnvInstance, _javaImplementation, byteArray);
	
    ...
	jbyte * buffer = jniEnvInstance->GetByteArrayElements(byteArray, NULL);
        //从java层复制数据到native层
	memcpy(data, buffer, size);
	jniEnvInstance->ReleaseByteArrayElements(byteArray, buffer, JNI_ABORT);

	jniEnvInstance->DeleteLocalRef(byteArray);

	return S_OK;
}

//java层接口
public interface ISequentialInStream extends Closeable {
    public int read(byte[] data) throws SevenZipException;
}

```
上面最大的问题是SequentialInStream::Read是被循环调用的, 这样会频繁创建java数组,然后不断触发gc, 从而导致内存比较剧烈的抖动. 所以只要去掉这个数组创建就可以消除内存抖动.
 刚好jni有个NewDirectByteBuffer方法用来包裹native内存直接传递到java层, 所以修改后的代码为:
```c++

STDMETHODIMP CPPToJavaSequentialInStream::Read(void *data, UInt32 size, UInt32 *processedSize) {

    ...
        //直接创建DirectByteBuffer对象
    ScopedLocalRef<jobject> buffer(jniEnvInstance, jniEnvInstance->NewDirectByteBuffer(data, size));

    if (!buffer.get()) {
        jniEnvInstance.reportError("Out of local resources or out of memory");
    }

    //调用java层
    jint wasRead = _iSequentialInStream->read(jniEnvInstance, _javaImplementation, buffer.get(),
                                              size);
    //还省去了java层数据复制到native层(不一定省去数据复制，如果在java层使用bytebuffer写入/读取数据还是会产生复制，
    //如果把buffer传给FileChannel那么数据不会经过java层，只是相当于传递data指针)
    ...

    return S_OK;
}
//java层接口则需要改为
public interface ISequentialInStream extends Closeable {
    public int read(ByteBuffer src, int len) throws SevenZipException;
}
```
经过优化, 不止去除了循环里数组创建, 还可能省去一些数据复制. 如果java层直接把ByteBuffer写入文件, 甚至只相当于把data指针传递给底层write系统调用.

输出流也有一样的问题, 改动原理和输入流一样.
修改后接口产生比较大的变化, java部分代码需要不少调整,不过相对于严重的内存抖动这个不算什么.

zip乱码问题, 主要就是得到zip的PropID.PATH时, 不对原始字节进行编码直接把它们放String里然后在java层解决进行编码. 类似下面
``` java
final String bytesStr = archive.getStringProperty(itemIndex, PropID.PATH);
final byte[] bytes = new byte[bytesStr.length()];
for (int i = 0; i < bytesStr.length(); i++) {
    //底层直接把字节数组当成字符串返回的, 所以这里直接转换为字节不会有问题
    bytes[i] = (byte) bytesStr.charAt(0);
}
//使用正确的编码, 对字节数组进行编码
Charset charset = null;
String zipItemPath = new String(bytes, charset);
```

# SAF支持
在原项目基础上增加了新的流RandomAccessChannelInStream, 这个是IInStream的子类, 可以用它得到IInArchive.
``` java
final ParcelFileDescriptor pfd = getContentResolver().openFileDescriptor("uri", "r");
//可能需要提前检测文件描述符是否可随机访问
final ParcelFileDescriptor.AutoCloseInputStream fin = new ParcelFileDescriptor.AutoCloseInputStream(pfd);
final FileChannel channel = fin.getChannel();
final RandomAccessChannelInStream inStream = new RandomAccessChannelInStream(channel);
//使用inStream对创建InArchive, 后面和原7-zip-jbinding没区别
//输出流同上
```

# 使用
下载项目, 然后把p7zip-android放到你项目目录下, 在settings.gradle里加入 include ':p7zip-android'
然后引用这个模块就行

