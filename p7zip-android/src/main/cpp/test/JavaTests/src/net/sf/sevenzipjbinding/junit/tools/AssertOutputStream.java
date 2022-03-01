package net.sf.sevenzipjbinding.junit.tools;

import net.sf.sevenzipjbinding.IInStream;
import net.sf.sevenzipjbinding.ISequentialOutStream;
import net.sf.sevenzipjbinding.SevenZipException;

import org.junit.Assert;

import java.io.IOException;
import java.nio.ByteBuffer;

public class AssertOutputStream implements ISequentialOutStream {

    private final IInStream inStream;

    public AssertOutputStream(IInStream inStream) {
        this.inStream = inStream;
    }

    public int write(byte[] data) throws SevenZipException {
        byte[] expected = new byte[data.length];
        Assert.assertEquals("Extracted data exceeds expected.", data.length, inStream.read(expected));
        Assert.assertArrayEquals("Extracted data doesn't match expected", expected, data);
        return data.length;
    }

    @Override
    public int write(ByteBuffer dst, int len) throws SevenZipException {
        return 0;
    }

    @Override
    public void close() throws IOException {

    }
}