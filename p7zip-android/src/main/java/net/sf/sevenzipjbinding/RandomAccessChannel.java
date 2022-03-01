package net.sf.sevenzipjbinding;

import java.io.IOException;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;

public interface RandomAccessChannel extends WritableByteChannel, ReadableByteChannel {

    long position() throws IOException;

    RandomAccessChannel position(long newPosition) throws IOException;

    long size() throws IOException;


    void truncate(long size) throws IOException;
}
