package net.sf.sevenzipjbinding.impl;

import net.sf.sevenzipjbinding.IOutStream;
import net.sf.sevenzipjbinding.RandomAccessChannel;
import net.sf.sevenzipjbinding.SevenZipException;

import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

/**
 *
 */
public class RandomAccessChannelOutStream implements IOutStream {
    private volatile boolean closed = false;

    private final RandomAccessChannel channel;

    /**
     * Constructs instance of the class from file channel.
     *
     * @param channel random access file to use
     */
    public RandomAccessChannelOutStream(FileChannel channel) {
        this.channel = new ProxyFileChannel(channel);
    }

    public RandomAccessChannelOutStream(RandomAccessChannel channel) {
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
    public synchronized void setSize(long newSize) throws SevenZipException {
        try {
            channel.truncate(newSize);
        } catch (IOException exception) {
            throw new SevenZipException("Error setting new length of the file", exception);
        }
    }

    @Override
    public synchronized int write(ByteBuffer dst, int len) throws SevenZipException {
        try {
            int count = channel.write(dst);
            return count < 0 ? 0 : count;
        } catch (IOException exception) {
            throw new SevenZipException("Error reading random access file", exception);
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
