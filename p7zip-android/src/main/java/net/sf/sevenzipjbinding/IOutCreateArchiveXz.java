package net.sf.sevenzipjbinding;

/**
 * The interface provides functionality to create new Xz.<br>
 * Standard way to get implementation is to use {@link SevenZip#openOutArchiveXz()}. See {@link IOutCreateArchive}
 * -JavaDoc for more information.
 *
 * <i>NOTE:</i> Each instance should be closed using {@link IOutArchive#close()} method.
 *
 * @author Boris Brodski
 * @see IOutCreateArchive
 * @see ArchiveFormat#TAR
 * @since 9.20-2.00
 */
public interface IOutCreateArchiveXz extends IOutCreateArchive<IOutItemXz> {
}
