package net.sf.sevenzipjbinding.impl;

import net.sf.sevenzipjbinding.IOutCreateArchiveZstd;
import net.sf.sevenzipjbinding.IOutItemZstd;
import net.sf.sevenzipjbinding.IOutUpdateArchiveZstd;

/**
 * GZip specific archive create and update class.
 * 
 * @author Boris Brodski
 * @since 9.20-2.00
 */
public class OutArchiveZstdImpl extends OutArchiveImpl<IOutItemZstd> implements IOutCreateArchiveZstd,
        IOutUpdateArchiveZstd {

    /**
     * {@inheritDoc}
     */
    public void setLevel(int compressionLevel) {
        featureSetLevel(compressionLevel);
    }

}
