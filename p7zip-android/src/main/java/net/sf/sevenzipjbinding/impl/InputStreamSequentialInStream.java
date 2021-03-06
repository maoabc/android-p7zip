package net.sf.sevenzipjbinding.impl;

import net.sf.sevenzipjbinding.ISequentialInStream;
import net.sf.sevenzipjbinding.SevenZipException;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

/**
 * Input stream based implementation of {@link ISequentialInStream}.
 * 
 * @author Boris Brodski
 * @since 9.20-2.00
 */
public class InputStreamSequentialInStream implements ISequentialInStream {
    private final InputStream inputStream;

    /**
     * Create new input stream based implementation of {@link ISequentialInStream}.
     * 
     * @param inputStream
     *            base input stream.
     */
    public InputStreamSequentialInStream(InputStream inputStream) {
        this.inputStream = inputStream;
    }

    /**
     * {@inheritDoc}
     */
    public int read(byte[] data,int len) throws SevenZipException {
        if (len == 0) {
            return 0;
        }

        try {
            int result = inputStream.read(data,0,len);
            if (result < 0) {
                return 0;
            }
            return result;
        } catch (IOException e) {
            throw new SevenZipException("Error reading " + len + " bytes out of InputStream", e);
        }
    }

    @Override
    public int read(ByteBuffer src, int len) throws SevenZipException {
        return 0;
    }

    /**
     * Returns base input stream
     * 
     * @return input stream
     */
    public InputStream getInputStream() {
        return inputStream;
    }

    public void close() throws IOException {
        inputStream.close();
    }
}
