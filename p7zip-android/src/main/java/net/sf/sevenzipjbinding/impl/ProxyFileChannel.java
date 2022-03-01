package net.sf.sevenzipjbinding.impl;

import net.sf.sevenzipjbinding.RandomAccessChannel;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

final class ProxyFileChannel implements RandomAccessChannel {
    private final FileChannel channel;

    ProxyFileChannel(FileChannel channel) {
        this.channel = channel;
    }

    public long position() throws IOException {
        return channel.position();
    }

    public RandomAccessChannel position(long newPosition) throws IOException {
        channel.position(newPosition);
        return this;
    }

    public long size() throws IOException {
        return channel.size();
    }

    public int read(ByteBuffer dst) throws IOException {
        return channel.read(dst);
    }

    public int write(ByteBuffer src) throws IOException {
        return channel.write(src);
    }

    public void truncate(long size) throws IOException {
        channel.truncate(size);
    }

    @Override
    public boolean isOpen() {
        return channel.isOpen();
    }

    @Override
    public void close() throws IOException {
        channel.close();
    }


}
