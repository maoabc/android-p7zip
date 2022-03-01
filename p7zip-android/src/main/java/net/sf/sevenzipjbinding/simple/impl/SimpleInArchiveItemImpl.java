package net.sf.sevenzipjbinding.simple.impl;

import net.sf.sevenzipjbinding.ExtractOperationResult;
import net.sf.sevenzipjbinding.IInArchive;
import net.sf.sevenzipjbinding.ISequentialOutStream;
import net.sf.sevenzipjbinding.PropID;
import net.sf.sevenzipjbinding.SevenZipException;
import net.sf.sevenzipjbinding.simple.ISimpleInArchiveItem;

import java.util.Date;

/**
 * Standard implementation of {@link ISimpleInArchiveItem}, simplified 7-Zip-JBinding interface.
 *
 * @author Boris Brodski
 * @since 4.65-1
 */
public class SimpleInArchiveItemImpl implements ISimpleInArchiveItem {

    private final SimpleInArchiveImpl simpleInArchiveImpl;
    private final int index;

    /**
     * Create instance of {@link SimpleInArchiveItemImpl} representing archive item with index index<code>index</code>
     * of archive <code>simpleInArchiveImpl</code>.
     *
     * @param simpleInArchiveImpl opened archive
     * @param index               index of the item in archive
     */
    public SimpleInArchiveItemImpl(SimpleInArchiveImpl simpleInArchiveImpl, int index) {
        this.simpleInArchiveImpl = simpleInArchiveImpl;
        this.index = index;
    }

    /**
     * Create instance of {@link SimpleInArchiveItemImpl} representing archive item with index index<code>index</code>
     * of archive <code>sevenZipInArchive</code>.
     *
     * @param sevenZipInArchive opened archive
     * @param index             index of the item in archive
     */
    public SimpleInArchiveItemImpl(IInArchive sevenZipInArchive, int index) {
        this.simpleInArchiveImpl = new SimpleInArchiveImpl(sevenZipInArchive);
        this.index = index;
    }

    /**
     * {@inheritDoc}
     */

    public String getPath() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getStringProperty(index, PropID.PATH);
    }

    /**
     * {@inheritDoc}
     */

    public int getAttributes() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getIntProperty(index, PropID.ATTRIBUTES);
    }

    /**
     * {@inheritDoc}
     */

    public int getCRC() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getIntProperty(index, PropID.CRC);
    }

    /**
     * {@inheritDoc}
     */

    public String getComment() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getStringProperty(index, PropID.COMMENT);
    }

    /**
     * {@inheritDoc}
     */

    public long getCreationTime() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getLongProperty(index, PropID.CREATION_TIME);
    }

    /**
     * {@inheritDoc}
     */

    public String getGroup() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getStringProperty(index, PropID.GROUP);
    }

    /**
     * {@inheritDoc}
     */

    public String getHostOS() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getStringProperty(index, PropID.HOST_OS);
    }

    /**
     * {@inheritDoc}
     */

    public long getLastAccessTime() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getLongProperty(index, PropID.LAST_ACCESS_TIME);
    }

    /**
     * {@inheritDoc}
     */

    public long getLastWriteTime() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getLongProperty(index, PropID.LAST_MODIFICATION_TIME);

//        Date property = (Date) simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getProperty(index, PropID.LAST_MODIFICATION_TIME);
//        if (property == null)
//            return 0;
//        return property.getTime();
    }

    /**
     * {@inheritDoc}
     */

    public String getMethod() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getStringProperty(index, PropID.METHOD);
    }

    /**
     * {@inheritDoc}
     */

    public long getPackedSize() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getLongProperty(index, PropID.PACKED_SIZE);
    }

    /**
     * {@inheritDoc}
     */

    public int getPosition() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getIntProperty(index, PropID.POSITION);
    }

    /**
     * {@inheritDoc}
     */

    public long getSize() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getLongProperty(index, PropID.SIZE);
    }

    /**
     * {@inheritDoc}
     */

    public String getUser() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getStringProperty(index, PropID.USER);
    }

    /**
     * {@inheritDoc}
     */

    public boolean isCommented() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getIntProperty(index, PropID.COMMENTED) != 0;
    }

    /**
     * {@inheritDoc}
     */

    public boolean isEncrypted() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getIntProperty(index, PropID.ENCRYPTED) != 0;
    }

    /**
     * {@inheritDoc}
     */

    public boolean isFolder() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getIntProperty(index, PropID.IS_FOLDER) != 0;
    }

    public ExtractOperationResult extractSlow(ISequentialOutStream outStream) throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().extractSlow(index, outStream);
    }

    public ExtractOperationResult extractSlow(ISequentialOutStream outStream, String password) throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().extractSlow(index, outStream, password);
    }

    /**
     * {@inheritDoc}
     */

    public int getItemIndex() {
        return index;
    }

    @Override
    public String getLink() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getStringProperty(index, PropID.LINK);
    }

    @Override
    public int getPosixAttributes() throws SevenZipException {
        return simpleInArchiveImpl.testAndGetSafeSevenZipInArchive().getIntProperty(index, PropID.POSIX_ATTRIB);
    }
}
