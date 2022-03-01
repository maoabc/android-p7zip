package net.sf.sevenzipjbinding;

/**
 * Feature interface for the setting 'maximal number of threads' to use during compression or update operation. Use
 * {@link SevenZip#openOutArchive(ArchiveFormat)} to get implementation of this interface.
 * 
 * @author Boris Brodski
 * @since 9.20-2.00
 */
public interface IOutFeatureSetEncryptionMethod {

    public void setEncryptionMethod(String em) throws SevenZipException;
}
