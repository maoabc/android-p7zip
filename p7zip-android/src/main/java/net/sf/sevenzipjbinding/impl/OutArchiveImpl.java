package net.sf.sevenzipjbinding.impl;

import androidx.annotation.Keep;

import net.sf.sevenzipjbinding.ArchiveFormat;
import net.sf.sevenzipjbinding.IInArchive;
import net.sf.sevenzipjbinding.IOutArchive;
import net.sf.sevenzipjbinding.IOutCreateCallback;
import net.sf.sevenzipjbinding.IOutItemBase;
import net.sf.sevenzipjbinding.ISequentialOutStream;
import net.sf.sevenzipjbinding.SevenZipException;

import java.io.IOException;
import java.io.PrintStream;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

/**
 * Common archive create and update class.
 *
 * @param <T> the type of the item callback implementation
 * @author Boris Brodski
 * @see OutArchive7zImpl
 * @see OutArchiveZipImpl
 * @see OutArchiveGZipImpl
 * @see OutArchiveBZip2Impl
 * @see OutArchiveTarImpl
 * @since 9.20-2.00
 */
// TODO null check all parameters: If null slips through into native code there will be no NPE :( 
public class OutArchiveImpl<T extends IOutItemBase> implements IOutArchive<T> {

    @Keep
    private long jbindingSession;
    @Keep
    private long sevenZipArchiveInstance;

    /**
     * Instance of {@link IInArchive} for connected OutArchives.
     */
    @Keep
    private volatile IInArchive inArchive;

    /**
     * Archive format enum value.
     */
    @Keep
    private ArchiveFormat archiveFormat;

    private PrintStream tracePrintStream;

    private final Map<String, Object> properties = new HashMap<>(8);

    private boolean trace; // Read by native code

    protected void setInArchive(IInArchive inArchive) {
        this.inArchive = inArchive;
    }

    protected void featureSetLevel(int compressionLevel) {
        properties.put("X", compressionLevel);
    }

    protected void featureSetThreadCount(int threadCount) {
        properties.put("MT", threadCount);
    }

    protected void featureSetEncryptionMethod(String encryptionMethod) {
        if (encryptionMethod != null) {
            properties.put("EM", encryptionMethod);
        }

    }


    protected void applyFeatures() throws SevenZipException {
        if (properties.isEmpty()) {
            return;
        }
        Set<Map.Entry<String, Object>> entries = properties.entrySet();

        int size = entries.size();
        String[] names = new String[size];
        Object[] values = new Object[size];

        int i = 0;
        for (Map.Entry<String, Object> entry : entries) {
            names[i] = entry.getKey();
            values[i] = entry.getValue();
            i++;
        }
        nativeSetProperties(names, values);

    }

    /**
     * {@inheritDoc}
     */
    public ArchiveFormat getArchiveFormat() {
        return archiveFormat;
    }

    protected void setArchiveFormat(ArchiveFormat archiveFormat) {
        this.archiveFormat = archiveFormat;
    }

    /**
     * Set solid features.
     *
     * @param solidBlockSpec <code>null</code> - turn solid off
     * @throws SevenZipException 7-Zip or 7-Zip-JBinding error occur. Use {@link SevenZipException#printStackTraceExtended()} to get
     *                           stack traces of this SevenZipException and of the all thrown 'cause by' exceptions.
     */
    protected void setSolidSpecInternal(String solidBlockSpec) throws SevenZipException {
        if (solidBlockSpec != null) {
            properties.put("S", solidBlockSpec);
        }
    }

    private native void nativeUpdateItems(ISequentialOutStream outStream, int numberOfItems,
                                          Object archiveUpdateCallback) throws SevenZipException;


    protected native void nativeSetProperties(String[] names, Object[] values) throws SevenZipException;

    /**
     * {@inheritDoc}
     */
    public void close() throws IOException {
        if (inArchive != null) {
            return; // In case of connected OutArchive no explicit closing necessary. 
        }
        inArchive = null;
        nativeClose();
    }

    private native void nativeClose() throws SevenZipException;

    /**
     * {@inheritDoc}
     */
    public void updateItems(ISequentialOutStream outStream, int numberOfItems, IOutCreateCallback<T> outUpdateCallback)
            throws SevenZipException {
        doUpdateItems(outStream, numberOfItems, outUpdateCallback);
    }

    private void doUpdateItems(ISequentialOutStream outStream, int numberOfItems,
                               IOutCreateCallback<?> archiveCreateCallback) throws SevenZipException {
        applyFeatures();
        nativeUpdateItems(outStream, numberOfItems, archiveCreateCallback);
    }

    /**
     * {@inheritDoc}
     */
    public void createArchive(ISequentialOutStream outStream, int numberOfItems,
                              IOutCreateCallback<? extends T> outCreateCallback) throws SevenZipException {
        doUpdateItems(outStream, numberOfItems, outCreateCallback);
    }

    /**
     * {@inheritDoc}
     */
    public IInArchive getConnectedInArchive() {
        return inArchive;
    }

    // Called by native code
    private void traceMessage(String message) {
        if (trace) {
            if (tracePrintStream == null) {
                System.out.println(message);
            } else {
                tracePrintStream.println(message);
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    public void setTracePrintStream(PrintStream tracePrintStream) {
        this.tracePrintStream = tracePrintStream;
    }

    /**
     * {@inheritDoc}
     */
    public PrintStream getTracePrintStream() {
        return tracePrintStream;
    }

    /**
     * {@inheritDoc}
     */
    public void setTrace(boolean trace) {
        this.trace = trace;
    }

    /**
     * {@inheritDoc}
     */
    public boolean isTrace() {
        return trace;
    }
}
