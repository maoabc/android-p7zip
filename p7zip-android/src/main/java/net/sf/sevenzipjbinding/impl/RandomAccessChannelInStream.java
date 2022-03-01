package net.sf.sevenzipjbinding.impl;

import net.sf.sevenzipjbinding.IInStream;
import net.sf.sevenzipjbinding.RandomAccessChannel;
import net.sf.sevenzipjbinding.SevenZipException;

import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

/**
 * Implementation of {@link IInStream} using {@link RandomAccessFile}.
 *
 * @author Boris Brodski
 * @since 4.65-1
 */
public class RandomAccessChannelInStream implements IInStream {
    private volatile boolean closed = false;

    private final RandomAccessChannel channel;

    /**
     * Constructs instance of the class from file channel.
     *
     * @param channel random access file to use
     */
    public RandomAccessChannelInStream(FileChannel channel) {
        this.channel = new ProxyFileChannel(channel);
    }

    public RandomAccessChannelInStream(RandomAccessChannel channel) {
        this.channel = channel;
    }

    /**
     * {@inheritDoc}
     */
    public synchronized long seek(long offset, int seekOrigin) throws SevenZipException {
        try {
            switch (seekOrigin) {
                case SEEK_SET:
                    channel.position(offset);
                    break;

                case SEEK_CUR:
                    channel.position(channel.position() + offset);
                    break;

                case SEEK_END:
                    channel.position(channel.size() + offset);
                    break;

                default:
                    throw new RuntimeException("Seek: unknown origin: " + seekOrigin);
            }

            return channel.position();
        } catch (IOException e) {
            throw new SevenZipException("Error while seek operation", e);
        }
    }

    @Override
    public synchronized int read(ByteBuffer src, int len) throws SevenZipException {
        try {
            int read = channel.read(src);
            return read < 0 ? 0 : read;
        } catch (IOException e) {
            throw new SevenZipException("Error reading random access file", e);
        }
    }

    /**
     * Closes random access file. After this call no more methods should be called.
     *
     * @throws IOException see {@link RandomAccessFile#close()}
     */
    public void close() throws IOException {
        synchronized (this) {
            if (closed) {
                return;
            }
            closed = true;
        }
        channel.close();
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            close();
        } finally {
            super.finalize();
        }
    }

}
