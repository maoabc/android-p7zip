package net.sf.sevenzipjbinding;

/**
 * Archive format independent (generic) archive item data class. It contains all information about a single archive
 * item, that is needed for a create or an update archive operation. Use this interface in the code, that should be able
 * to handle multiple archive formats is a generic way. See {@link IOutItemBase} for details.
 *
 * @author Boris Brodski
 * @see IOutItemBase
 * @since 9.20-2.00
 */
public interface IOutItemAllFormats extends IOutItem7z, IOutItemBZip2, IOutItemGZip, IOutItemTar, IOutItemZip, IOutItemXz, IOutItemZstd {

}
