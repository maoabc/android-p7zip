package net.sf.sevenzipjbinding;

/**
 * The interface provides functionality to update existing GZip archives.<br>
 * The standard way of getting the implementation of this interface is to use
 * {@link IInArchive#getConnectedOutArchive()} method like this:<br>
 * <br>
 *
 * <pre>
 *  {@link IInArchive} inArchive = {@link SevenZip}.openInArchive(null, inStream);
 *  {@link IOutUpdateArchive}{@code <}{@link IOutItemBase}> outArchive = inArchive.openOutArchive();
 *
 *  if (outArchive instanceof {@link IOutUpdateArchiveXz}) {
 *    {@link IOutUpdateArchiveXz} outUpdateArchiveGZip = ({@link IOutUpdateArchiveXz})outArchive;
 *    ...
 *  }
 *
 *  outArchive.updateItems(...);
 *
 *  ...
 *
 *  inArchive.close();
 * </pre>
 *
 * @author Boris Brodski
 * @see IOutUpdateArchive
 * @see IInArchive
 * @see IOutItemBase
 * @see IOutItemAllFormats
 * @since 9.20-2.00
 */
public interface IOutUpdateArchiveXz extends IOutUpdateArchive<IOutItemXz>, //
        IOutFeatureSetLevel {
}
