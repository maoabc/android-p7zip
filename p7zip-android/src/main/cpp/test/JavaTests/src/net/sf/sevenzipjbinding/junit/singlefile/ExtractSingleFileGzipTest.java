package net.sf.sevenzipjbinding.junit.singlefile;

import net.sf.sevenzipjbinding.ArchiveFormat;

public class ExtractSingleFileGzipTest extends net.sf.sevenzipjbinding.junit.singlefile.ExtractSingleFileAbstractTest {

    public ExtractSingleFileGzipTest() {
        super(ArchiveFormat.GZIP, "gz", 1, 5, 9);
    }

}
