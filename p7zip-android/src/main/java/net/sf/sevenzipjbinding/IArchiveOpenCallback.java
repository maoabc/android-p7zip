package net.sf.sevenzipjbinding;

/**
 * Interface to receive information about open archive operation for extraction or update.
 *
 * @author Boris Brodski
 * @since 4.65-1
 */
public interface IArchiveOpenCallback {

    /**
     * Set total amount of work to be done. Both parameter are optional.
     *
     * @param files count of files to be processed (optional)
     * @param bytes count of bytes to be processed (optional)
     */
    public boolean setTotal(long files, long bytes);

    /**
     * Set amount of competed work. Both parameter are optional.
     *
     * @param files count of processed files
     * @param bytes count of processed bytes
     */
    public boolean setCompleted(long files, long bytes);
}
