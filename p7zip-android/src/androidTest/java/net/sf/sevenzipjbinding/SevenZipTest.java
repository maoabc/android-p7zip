package net.sf.sevenzipjbinding;

import android.util.Log;

import net.sf.sevenzipjbinding.impl.RandomAccessFileInStream;
import net.sf.sevenzipjbinding.simple.ISimpleInArchive;
import net.sf.sevenzipjbinding.simple.ISimpleInArchiveItem;

import org.junit.Test;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

public class SevenZipTest {
    public static final String TAG = "SevenZipTest";

        /*
    @Test
    public void openInArchive() throws SevenZipException, FileNotFoundException {
        //测试依赖环境,懒得改注释掉
        String name = "/sdcard/0.7z";
        RandomAccessFile randomAccessFile = new RandomAccessFile(name, "rw");

        IInArchive inArchive = SevenZip.openInArchive(ArchiveFormat.SEVEN_ZIP, new RandomAccessFileInStream(randomAccessFile));

        ISimpleInArchive simpleInterface = inArchive.getSimpleInterface();
        long s = System.currentTimeMillis();

        for (ISimpleInArchiveItem item : simpleInterface.archiveItems()) {
            String path = item.getPath();
            if ("test/myzip.zip".equalsIgnoreCase(path)) {
                inArchive.extract(new int[]{item.getItemIndex()}, false, new IArchiveExtractCallback() {
                    @Override
                    public ISequentialOutStream getStream(int index, ExtractAskMode extractAskMode) throws SevenZipException {
                        return new ISequentialOutStream() {
                            @Override
                            public void close() throws IOException {

                            }

                            @Override
                            public int write(ByteBuffer dst, int len) throws SevenZipException {
                                return len;
                            }
                        };
                    }

                    @Override
                    public void prepareOperation(ExtractAskMode extractAskMode) throws SevenZipException {
                        Log.d(TAG, "prepareOperation: ");

                    }

                    @Override
                    public void setOperationResult(ExtractOperationResult extractOperationResult) throws SevenZipException {
                        Log.d(TAG, "setOperationResult: " + extractOperationResult);

                    }

                    @Override
                    public void setTotal(long total) throws SevenZipException {
                        Log.d(TAG, "setTotal: " + total);
                    }

                    @Override
                    public void setCompleted(long complete) throws SevenZipException {
                        Log.d(TAG, "setCompleted: " + complete);
                    }
                });
            }
        }
        System.out.println("endTime " + (System.currentTimeMillis() - s));


//        int numberOfItems = inArchive.getNumberOfItems();
//        for (int num = 3; num < numberOfItems; num++) {
//
//            for (int i = 0; i < inArchive.getNumberOfProperties(); i++) {
//                PropertyInfo propertyInfo = inArchive.getPropertyInfo(i);
//                Object property = inArchive.getProperty(num, propertyInfo.propID);
//                System.out.println(property);
//            }
//            inArchive.extractSlow(num, new ISequentialOutStream() {
//                @Override
//                public int write(byte[] data) throws SevenZipException {
//                    System.out.println(new String(data));
//                    return data.length;
//                }
//            });

//        }

        inArchive.close();
    }

    @Test
    public void testRandomFile() throws IOException {
        RandomAccessFile file = new RandomAccessFile("/sdcard/cal.java", "rw");

        file.seek(1024);

        FileChannel channel = file.getChannel();
        long position = channel.position();
        long size = channel.size();
        long length = file.length();
        file.setLength(1024*4);
        channel.truncate(10000);
        long position2 = channel.position();
        long size2 = channel.size();
        long length2 = file.length();
        channel.position(1024 * 5);
        long position3 = channel.position();
        System.out.println();

    }
         */
}