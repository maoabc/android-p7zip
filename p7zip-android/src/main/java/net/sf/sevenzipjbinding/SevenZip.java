package net.sf.sevenzipjbinding;

import net.sf.sevenzipjbinding.impl.OutArchive7zImpl;
import net.sf.sevenzipjbinding.impl.OutArchiveBZip2Impl;
import net.sf.sevenzipjbinding.impl.OutArchiveGZipImpl;
import net.sf.sevenzipjbinding.impl.OutArchiveImpl;
import net.sf.sevenzipjbinding.impl.OutArchiveTarImpl;
import net.sf.sevenzipjbinding.impl.OutArchiveXzImpl;
import net.sf.sevenzipjbinding.impl.OutArchiveZipImpl;
import net.sf.sevenzipjbinding.impl.OutArchiveZstdImpl;
import net.sf.sevenzipjbinding.impl.RandomAccessFileInStream;

import java.io.File;
import java.security.AccessController;
import java.security.PrivilegedAction;

/**
 * 7-Zip-JBinding main class.
 *
 * <br>
 * The platform jar is a additional jar file <code>sevenzipjbinding-<i>Platform</i>.jar</code> with one or more native
 * libraries for respective one or more platforms. Here is some examples of 7-Zip-JBinding platform jar files.
 * <ul>
 * <li><code>sevenzipjbinding-Linux-i386.jar</code> with native library for exact one platform: Linux, 32 bit</li>
 * <li><code>sevenzipjbinding-AllWindows.jar</code> with native libraries for two platforms: Windows 32 and 64 bit</li>
 * <li><code>sevenzipjbinding-AllPlatforms.jar</code> with native libraries for all available platforms</li>
 * </ul>
 * The single and multiple platform jar files can be determined by counting dashes in the filename. Single platform jar
 * files always contain two dashes in their names.<br>
 * <br>
 * Here is a schema of the different initialization processes:
 *
 *
 *
 * <br>
 * By default the initialization occurred within the
 * {@link AccessController#doPrivileged(PrivilegedAction)} block. This can be overruled by setting
 * <code>sevenzip.no_doprivileged_initialization</code> system property. For example: <blockquote>
 * <code>java -Dsevenzip.no_doprivileged_initialization=1 ...</code> </blockquote>
 *
 *
 * <h3>Creating new archives</h3>
 * <p>
 * There are two ways to create a new archive:
 * <ul>
 * <li>Use {@link SevenZip#openOutArchive(ArchiveFormat)} method. It will return an instance of the
 * {@link IOutCreateArchive}{@code <}{@link IOutItemAllFormats}{@code >} interface allowing creation of an archive of
 * any supported archive format. To get all currently supported formats see the 'compression' column of the
 * {@link ArchiveFormat} -JavaDoc.
 * <li>Use one of the <code>SevenZip.openOutArchiveXxx</code> methods, that provide implementations of corresponding
 * archive format specific interfaces. Those interfaces contain all supported configuration methods for selected archive
 * format and are more convenient in cases, where only one archive format should be supported.
 * </ul>
 * <p>
 * For more information see {@link IOutCreateArchive}.
 *
 * <h3>Updating existing archives</h3>
 * <p>
 * In order to update an existing archive three simple steps are necessary:
 * <ul>
 * <li>Open the existing archive need to be modified (getting an instance of the {@link IInArchive} interface)
 * <li>Call {@link IInArchive#getConnectedOutArchive()} to get connected instance of the {@link IOutUpdateArchive}
 * interface
 * <li>Call {@link IOutUpdateArchive#updateItems(ISequentialOutStream, int, IOutCreateCallback)} to start the archive
 * update operation
 * </ul>
 * <p>
 * During update operation user may copy item properties or item properties and content from the existing archive
 * significantly improving performance comparing to extract and re-compress alternative.<br>
 * <br>
 * For more information see {@link IOutUpdateArchive}.
 *
 * @author Boris Brodski
 * @since 4.65-1
 */
public class SevenZip {
    static {
        System.loadLibrary("p7zip-jni");
        try {
            nativeInitSevenZipLibrary();
        } catch (SevenZipNativeInitializationException e) {
            e.printStackTrace();
        }
    }

    /**
     * Version information about 7-Zip.
     *
     * @author Boris Brodski
     * @since 9.20-2.00
     */
    public static class Version {
        /**
         * Major version of the 7-Zip engine
         */
        public int major;

        /**
         * Minor version of the 7-Zip engine
         */
        public int minor;

        /**
         * Build id of the 7-Zip engine
         */

        public int build;

        /**
         * Formatted version of the 7-Zip engine
         */
        public String version;

        /**
         * Version date
         */
        public String date;

        /**
         * copyright
         */
        public String copyright;
    }

    private static final String SEVENZIPJBINDING_VERSION = "9.20-2.00beta";

    private static final String SYSTEM_PROPERTY_TMP = "java.io.tmpdir";

    private static File[] temporaryArtifacts = null;

    /**
     * Hide default constructor
     */
    private SevenZip() {

    }


    /**
     * Returns list of the temporary created artifacts (one directory and one or more files within this directory). The
     * directory is always the last element in the array.
     *
     * @return array of {@link File}s.
     */
    public static synchronized File[] getTemporaryArtifacts() {
        return temporaryArtifacts;
    }


    private static File createOrVerifyTmpDir(File tmpDirectory) throws SevenZipNativeInitializationException {
        File tmpDirFile;
        if (tmpDirectory != null) {
            tmpDirFile = tmpDirectory;
        } else {
            String systemPropertyTmp = System.getProperty(SYSTEM_PROPERTY_TMP);
            if (systemPropertyTmp == null) {
                throwInitException("can't determinte tmp directory. Use may use -D" + SYSTEM_PROPERTY_TMP
                        + "=<path to tmp dir> parameter for jvm to fix this.");
            }
            tmpDirFile = new File(systemPropertyTmp);
        }

        if (!tmpDirFile.exists() || !tmpDirFile.isDirectory()) {
            throwInitException("invalid tmp directory '" + tmpDirectory + "'");
        }

        if (!tmpDirFile.canWrite()) {
            throwInitException("can't create files in '" + tmpDirFile.getAbsolutePath() + "'");
        }
        return tmpDirFile;
    }


    /**
     * Open archive of type <code>archiveFormat</code> from the input stream <code>inStream</code> using 'archive open
     * call back' listener <code>archiveOpenCallback</code>. To open archive from the file, use
     * {@link RandomAccessFileInStream}.
     *
     * @param archiveFormat       format of archive
     * @param inFile              input file path
     * @param archiveOpenCallback archive open call back listener to use. You can optionally implement {@link ICryptoGetTextPassword} to
     *                            specify password to use.
     * @return implementation of {@link IInArchive} which represents opened archive.
     * @throws SevenZipException    7-Zip or 7-Zip-JBinding error occur. Use {@link SevenZipException#printStackTraceExtended()} to get
     *                              stack traces of this SevenZipException and of the all thrown 'cause by' exceptions.
     * @throws NullPointerException is thrown, if inStream is null
     * @see #openInArchive(ArchiveFormat, String)
     */

//    public static IInArchive openInArchive(ArchiveFormat archiveFormat, String inFile,
//                                           IArchiveOpenCallback archiveOpenCallback) throws SevenZipException {
//        IArchiveOpenCallback openArchiveCallbackToUse = archiveOpenCallback;
//        if (openArchiveCallbackToUse == null) {
//             TODO Test this!
//            openArchiveCallbackToUse = new DummyOpenArchiveCallback();
//        }
//        if (archiveFormat != null) {
//            return nativeOpenArchive2(archiveFormat, inFile, openArchiveCallbackToUse);
//        }
//        return null;
//    }

    /**
     * Open archive of type <code>archiveFormat</code> from the input stream <code>inStream</code>. To open archive from
     * the file, use {@link RandomAccessFileInStream}.
     *
     * @param archiveFormat (optional) format of archive. If <code>null</code> archive format will be auto-detected.
     * @param inFile        input file path
     * @return implementation of {@link IInArchive} which represents opened archive.
     * @throws SevenZipException    7-Zip or 7-Zip-JBinding error occur. Use {@link SevenZipException#printStackTraceExtended()} to get
     *                              stack traces of this SevenZipException and of the all thrown 'cause by' exceptions.
     * @throws NullPointerException is thrown, if inStream is null
     * @see #openInArchive(ArchiveFormat, String, IArchiveOpenCallback)
     */

//    public static IInArchive openInArchive(ArchiveFormat archiveFormat, String inFile) throws SevenZipException {
//        return openInArchive(archiveFormat, inFile, new DummyOpenArchiveCallback());
//    }


    /**
     * Open archive of type <code>archiveFormat</code> from the input stream <code>inStream</code> using 'archive open
     * call back' listener <code>archiveOpenCallback</code>. To open archive from the file, use
     * {@link RandomAccessFileInStream}.
     *
     * @param archiveFormat       format of archive
     * @param inStream            input stream to open archive from
     * @param archiveOpenCallback archive open call back listener to use. You can optionally implement {@link ICryptoGetTextPassword} to
     *                            specify password to use.
     * @return implementation of {@link IInArchive} which represents opened archive.
     * @throws SevenZipException    7-Zip or 7-Zip-JBinding error occur. Use {@link SevenZipException#printStackTraceExtended()} to get
     *                              stack traces of this SevenZipException and of the all thrown 'cause by' exceptions.
     * @throws NullPointerException is thrown, if inStream is null
     * @see #openInArchive(ArchiveFormat, IInStream, IArchiveOpenCallback)
     * @see #openInArchive(ArchiveFormat, IInStream, String)
     */
    public static IInArchive openInArchive(ArchiveFormat archiveFormat, IInStream inStream,
                                           IArchiveOpenCallback archiveOpenCallback) throws SevenZipException {
        IArchiveOpenCallback openArchiveCallbackToUse = archiveOpenCallback;
        if (openArchiveCallbackToUse == null) {
            // TODO Test this!
            openArchiveCallbackToUse = new DummyOpenArchiveCallback();
        }
        return callNativeOpenArchive(archiveFormat, inStream, openArchiveCallbackToUse);
    }

    /**
     * Open archive of type <code>archiveFormat</code> from the input stream <code>inStream</code> using 'archive open
     * call-back' listener <code>archiveOpenCallback</code>. To open archive from the file, use
     * {@link RandomAccessFileInStream}.
     *
     * @param archiveFormat   format of archive
     * @param inStream        input stream to open archive from
     * @param passwordForOpen password to use. Warning: this password will not be used to extract item from archive but only to open
     *                        archive. (7-zip format supports encrypted filename)
     * @return implementation of {@link IInArchive} which represents opened archive.
     * @throws SevenZipException    7-Zip or 7-Zip-JBinding error occur. Use {@link SevenZipException#printStackTraceExtended()} to get
     *                              stack traces of this SevenZipException and of the all thrown 'cause by' exceptions.
     * @throws NullPointerException is thrown, if inStream is null
     * @see #openInArchive(ArchiveFormat, IInStream)
     * @see #openInArchive(ArchiveFormat, IInStream, IArchiveOpenCallback)
     */
    public static IInArchive openInArchive(ArchiveFormat archiveFormat, IInStream inStream, String passwordForOpen)
            throws SevenZipException {
        if (archiveFormat != null) {
            return callNativeOpenArchive(archiveFormat, inStream, new ArchiveOpenCryptoCallback(passwordForOpen));
        }
        return callNativeOpenArchive(null, inStream, new ArchiveOpenCryptoCallback(passwordForOpen));
    }

    /**
     * Open archive of type <code>archiveFormat</code> from the input stream <code>inStream</code>. To open archive from
     * the file, use {@link RandomAccessFileInStream}.
     *
     * @param archiveFormat (optional) format of archive. If <code>null</code> archive format will be auto-detected.
     * @param inStream      input stream to open archive from
     * @return implementation of {@link IInArchive} which represents opened archive.
     * @throws SevenZipException    7-Zip or 7-Zip-JBinding error occur. Use {@link SevenZipException#printStackTraceExtended()} to get
     *                              stack traces of this SevenZipException and of the all thrown 'cause by' exceptions.
     * @throws NullPointerException is thrown, if inStream is null
     * @see #openInArchive(ArchiveFormat, IInStream)
     * @see #openInArchive(ArchiveFormat, IInStream, String)
     */
    public static IInArchive openInArchive(ArchiveFormat archiveFormat, IInStream inStream) throws SevenZipException {
        if (archiveFormat != null) {
            return callNativeOpenArchive(archiveFormat, inStream, new DummyOpenArchiveCallback());
        }
        return callNativeOpenArchive(null, inStream, new DummyOpenArchiveCallback());
    }


    private static void throwInitException(String message) throws SevenZipNativeInitializationException {
        throwInitException(null, message);
    }

    private static void throwInitException(Exception exception, String message)
            throws SevenZipNativeInitializationException {
        throw new SevenZipNativeInitializationException("Error loading SevenZipJBinding native library into JVM: "
                + message + " [You may also try different SevenZipJBinding initialization methods "
                + "'net.sf.sevenzipjbinding.SevenZip.init*()' in order to solve this problem] ", exception);
    }

    private static IInArchive callNativeOpenArchive(ArchiveFormat archiveFormat, IInStream inStream,
                                                    IArchiveOpenCallback archiveOpenCallback) throws SevenZipException {
        if (inStream == null) {
            throw new NullPointerException("SevenZip.callNativeOpenArchive(...): inStream parameter is null");
        }
        return nativeOpenArchive(archiveFormat, inStream, archiveOpenCallback);
    }

    private static native IInArchive nativeOpenArchive(ArchiveFormat archiveFormat, IInStream inStream,
                                                       IArchiveOpenCallback archiveOpenCallback) throws SevenZipException;

//    private static native IInArchive nativeOpenArchive2(ArchiveFormat archiveFormat, String file,
//                                                        IArchiveOpenCallback archiveOpenCallback) throws SevenZipException;

    private static native void nativeCreateArchive(OutArchiveImpl<?> outArchiveImpl, ArchiveFormat archiveFormat)
            throws SevenZipException;

    private static native String nativeInitSevenZipLibrary() throws SevenZipNativeInitializationException;

    private static native int nativeGetVersionMajor();

    private static native int nativeGetVersionMinor();

    private static native int nativeGetVersionBuild();

    private static native String nativeGetVersionVersion();

    private static native String nativeGetVersionDate();

    private static native String nativeGetVersionCopyright();

    /**
     * Return information about native 7-Zip engine.
     *
     * @return Version
     */
    public static Version getSevenZipVersion() {

        Version version = new Version();

        version.major = nativeGetVersionMajor();
        version.minor = nativeGetVersionMinor();
        version.build = nativeGetVersionBuild();
        version.version = nativeGetVersionVersion();
        version.date = nativeGetVersionDate();
        version.copyright = nativeGetVersionCopyright();

        return version;
    }

    /**
     * Return version of the 7-Zip-JBinding.
     *
     * @return version of the 7-Zip-JBinding
     */
    public static String getSevenZipJBindingVersion() {
        return SEVENZIPJBINDING_VERSION;
    }

    /**
     * Create a new Zip archive.
     *
     * @return an out-archive object initialized to create the new Zip archive
     * @throws SevenZipException 7-Zip or 7-Zip-JBinding error occur. Use {@link SevenZipException#printStackTraceExtended()} to get
     *                           stack traces of this SevenZipException and of the all thrown 'cause by' exceptions.
     * @see IOutCreateArchiveZip
     */
    public static IOutCreateArchiveZip openOutArchiveZip() throws SevenZipException {
        return (IOutCreateArchiveZip) openOutArchiveIntern(ArchiveFormat.ZIP);
    }

    /**
     * Create a new 7z archive.
     *
     * @return an out-archive object initialized to create the new 7z archive
     * @throws SevenZipException 7-Zip or 7-Zip-JBinding error occur. Use {@link SevenZipException#printStackTraceExtended()} to get
     *                           stack traces of this SevenZipException and of the all thrown 'cause by' exceptions.
     * @see IOutCreateArchive7z
     */
    public static IOutCreateArchive7z openOutArchive7z() throws SevenZipException {
        return (IOutCreateArchive7z) openOutArchiveIntern(ArchiveFormat.SEVEN_ZIP);
    }

    /**
     * Create a new 7z archive.
     *
     * @return an out-archive object initialized to create the new 7z archive
     * @throws SevenZipException 7-Zip or 7-Zip-JBinding error occur. Use {@link SevenZipException#printStackTraceExtended()} to get
     *                           stack traces of this SevenZipException and of the all thrown 'cause by' exceptions.
     * @see IOutCreateArchiveTar
     */
    public static IOutCreateArchiveTar openOutArchiveTar() throws SevenZipException {
        return (IOutCreateArchiveTar) openOutArchiveIntern(ArchiveFormat.TAR);
    }

    /**
     * Create a new BZip2 archive.
     *
     * @return an out-archive object initialized to create the new BZip2 archive
     * @throws SevenZipException 7-Zip or 7-Zip-JBinding error occur. Use {@link SevenZipException#printStackTraceExtended()} to get
     *                           stack traces of this SevenZipException and of the all thrown 'cause by' exceptions.
     * @see IOutCreateArchiveBZip2
     */
    public static IOutCreateArchiveBZip2 openOutArchiveBZip2() throws SevenZipException {
        return (IOutCreateArchiveBZip2) openOutArchiveIntern(ArchiveFormat.BZIP2);
    }

    /**
     * Create a new GZip archive.
     *
     * @return an out-archive object initialized to create the new GZip archive
     * @throws SevenZipException 7-Zip or 7-Zip-JBinding error occur. Use {@link SevenZipException#printStackTraceExtended()} to get
     *                           stack traces of this SevenZipException and of the all thrown 'cause by' exceptions.
     * @see IOutCreateArchiveGZip
     */
    public static IOutCreateArchiveGZip openOutArchiveGZip() throws SevenZipException {
        return (IOutCreateArchiveGZip) openOutArchiveIntern(ArchiveFormat.GZIP);
    }

    /**
     * Create a new Xz archive.
     *
     * @return an out-archive object initialized to create the new Xz archive
     * @throws SevenZipException 7-Zip or 7-Zip-JBinding error occur. Use {@link SevenZipException#printStackTraceExtended()} to get
     *                           stack traces of this SevenZipException and of the all thrown 'cause by' exceptions.
     * @see IOutCreateArchiveXz
     */
    public static IOutCreateArchiveXz openOutArchiveXz() throws SevenZipException {
        return (IOutCreateArchiveXz) openOutArchiveIntern(ArchiveFormat.XZ);
    }
    /**
     * Create a new Zstd archive.
     *
     * @return an out-archive object initialized to create the new Xz archive
     * @throws SevenZipException 7-Zip or 7-Zip-JBinding error occur. Use {@link SevenZipException#printStackTraceExtended()} to get
     *                           stack traces of this SevenZipException and of the all thrown 'cause by' exceptions.
     * @see IOutCreateArchiveXz
     */
    public static IOutCreateArchiveZstd openOutArchiveZstd() throws SevenZipException {
        return (IOutCreateArchiveZstd) openOutArchiveIntern(ArchiveFormat.ZSTD);
    }

    /**
     * Create a new archive of type <code>archiveFormat</code>.
     *
     * @param archiveFormat archive format of the new archive
     * @return an out-archive object initialized to create the new archive
     * @throws SevenZipException 7-Zip or 7-Zip-JBinding error occur. Use {@link SevenZipException#printStackTraceExtended()} to get
     *                           stack traces of this SevenZipException and of the all thrown 'cause by' exceptions.
     * @see IOutCreateArchiveZip
     */
    @SuppressWarnings("unchecked")
    public static IOutCreateArchive<IOutItemAllFormats> openOutArchive(ArchiveFormat archiveFormat)
            throws SevenZipException {
        return (IOutCreateArchive<IOutItemAllFormats>) openOutArchiveIntern(archiveFormat);
    }

    private static OutArchiveImpl<?> openOutArchiveIntern(ArchiveFormat archiveFormat) throws SevenZipException {
        if (!archiveFormat.isOutArchiveSupported()) {
            throw new IllegalStateException("Archive format '" + archiveFormat + "' doesn't support archive creation.");
        }

        //不使用反射
        OutArchiveImpl<?> outArchiveImpl;
        Class<? extends OutArchiveImpl<?>> outArchiveImplementation = archiveFormat.getOutArchiveImplementation();
        if (outArchiveImplementation == OutArchiveXzImpl.class) {
            outArchiveImpl = new OutArchiveXzImpl();
        } else if (outArchiveImplementation == OutArchiveZipImpl.class) {
            outArchiveImpl = new OutArchiveZipImpl();
        } else if (outArchiveImplementation == OutArchive7zImpl.class) {
            outArchiveImpl = new OutArchive7zImpl();
        } else if (outArchiveImplementation == OutArchiveTarImpl.class) {
            outArchiveImpl = new OutArchiveTarImpl();
        } else if (outArchiveImplementation == OutArchiveGZipImpl.class) {
            outArchiveImpl = new OutArchiveGZipImpl();
        } else if (outArchiveImplementation == OutArchiveBZip2Impl.class) {
            outArchiveImpl = new OutArchiveBZip2Impl();
        }  else if (outArchiveImplementation == OutArchiveZstdImpl.class) {
            outArchiveImpl = new OutArchiveZstdImpl();
        } else {
            throw new IllegalStateException("Internal error: Can't create new instance of the class "
                    + outArchiveImplementation + " using default constructor.");
        }

        nativeCreateArchive(outArchiveImpl, archiveFormat);
        return outArchiveImpl;
    }

    private static class DummyOpenArchiveCallback implements IArchiveOpenCallback, ICryptoGetTextPassword {
        /**
         * {@inheritDoc}
         */

        public boolean setCompleted(long files, long bytes) {
            return true;
        }

        /**
         * {@inheritDoc}
         */

        public boolean setTotal(long files, long bytes) {
            return true;
        }

        /**
         * {@inheritDoc}
         */

        public String cryptoGetTextPassword() throws SevenZipException {
            throw new SevenZipException("No password was provided for opening protected archive.");
        }
    }

    private static final class ArchiveOpenCryptoCallback implements IArchiveOpenCallback, ICryptoGetTextPassword {
        private final String passwordForOpen;

        public ArchiveOpenCryptoCallback(String passwordForOpen) {
            this.passwordForOpen = passwordForOpen;
        }

        public boolean setCompleted(long files, long bytes) {
            return true;
        }

        public boolean setTotal(long files, long bytes) {
            return true;
        }

        public String cryptoGetTextPassword() throws SevenZipException {
            return passwordForOpen;
        }
    }
}
