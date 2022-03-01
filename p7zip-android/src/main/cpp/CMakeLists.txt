cmake_minimum_required(VERSION 3.6)
project(p7zip-jni)


set(P7ZIP_SRC p7zip_16.02)

include_directories(${P7ZIP_SRC}/CPP/myWindows)
include_directories(${P7ZIP_SRC}/CPP/)
include_directories(${P7ZIP_SRC}/CPP/include_windows)

include_directories(jbinding-cpp)
include_directories(jbinding-cpp/PlatformUnix/)

#include_directories(${P7ZIP_SRC}/C/zstd)
#include_directories(${P7ZIP_SRC}/C/zstdmt)

add_definitions(
        -DANDROID
        -DANDROID_NDK
        #        -D_NO_EXCEPTIONS
        #        -Wl,--fatal-warnings
        #        -fshort-wchar
        -fexceptions
        -DNDEBUG
        -D_REENTRANT
        -DENV_UNIX
        -DBREAK_HANDLER
        -DUNICODE
        -D_UNICODE
        -DUNIX_USE_WIN_FILE)


find_package(Threads)
if (CMAKE_USE_PTHREADS_INIT)
    set(HAVE_PTHREADS Yes)
endif ()
if (HAVE_WCHAR_H)
    option(HAVE_UNICODE_WCHAR "Enable Unicode Support" YES)
    mark_as_advanced(HAVE_UNICODE_WCHAR)
else (HAVE_WCHAR_H)
    set(HAVE_UNICODE_WCHAR 0)
endif (HAVE_WCHAR_H)

set(ZSTD_SRC
        #zstd
        "${P7ZIP_SRC}/C/zstd/entropy_common.c"
        "${P7ZIP_SRC}/C/zstd/error_private.c"
        "${P7ZIP_SRC}/C/zstd/fse_compress.c"
        "${P7ZIP_SRC}/C/zstd/fse_decompress.c"
        "${P7ZIP_SRC}/C/zstd/huf_compress.c"
        "${P7ZIP_SRC}/C/zstd/huf_decompress.c"
        "${P7ZIP_SRC}/C/zstd/xxhash.c"
        "${P7ZIP_SRC}/C/zstd/zstd_ddict.c"
        "${P7ZIP_SRC}/C/zstd/zstd_common.c"
        "${P7ZIP_SRC}/C/zstd/zstd_compress.c"
        "${P7ZIP_SRC}/C/zstd/zstd_compress_sequences.c"
        "${P7ZIP_SRC}/C/zstd/zstd_compress_literals.c"
        "${P7ZIP_SRC}/C/zstd/zstd_decompress.c"
        "${P7ZIP_SRC}/C/zstd/zstd_decompress_block.c"
        "${P7ZIP_SRC}/C/zstd/zstd_double_fast.c"
        "${P7ZIP_SRC}/C/zstd/zstd_fast.c"
        "${P7ZIP_SRC}/C/zstd/zstd_lazy.c"
        "${P7ZIP_SRC}/C/zstd/zstd_ldm.c"
        "${P7ZIP_SRC}/C/zstd/zstd_opt.c"
        "${P7ZIP_SRC}/C/zstd/hist.c"
        "${P7ZIP_SRC}/C/zstdmt/zstdmt_common.c"
        "${P7ZIP_SRC}/C/zstdmt/zstdmt_compress.c"
        "${P7ZIP_SRC}/C/zstdmt/zstdmt_decompress.c"

        #zstd handler
        "${P7ZIP_SRC}/CPP/7zip/Archive/ZstdHandler.cpp"

        #zstd
        "${P7ZIP_SRC}/CPP/7zip/Compress/ZstdDecoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/ZstdEncoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/ZstdRegister.cpp"
        )

set(P7ZIP_SRC_FILES
        "${P7ZIP_SRC}/C/7zCrc.c"
        "${P7ZIP_SRC}/C/7zCrcOpt.c"
        "${P7ZIP_SRC}/C/7zStream.c"
        "${P7ZIP_SRC}/C/Aes.c"
        "${P7ZIP_SRC}/C/Alloc.c"
        "${P7ZIP_SRC}/C/Bcj2.c"
        "${P7ZIP_SRC}/C/Bcj2Enc.c"
        "${P7ZIP_SRC}/C/Bra.c"
        "${P7ZIP_SRC}/C/Bra86.c"
        "${P7ZIP_SRC}/C/BraIA64.c"
        "${P7ZIP_SRC}/C/BwtSort.c"
        "${P7ZIP_SRC}/C/CpuArch.c"
        "${P7ZIP_SRC}/C/Delta.c"
        "${P7ZIP_SRC}/C/HuffEnc.c"
        "${P7ZIP_SRC}/C/LzFind.c"
        "${P7ZIP_SRC}/C/LzFindMt.c"
        "${P7ZIP_SRC}/C/Lzma2Dec.c"
        "${P7ZIP_SRC}/C/Lzma2Enc.c"
        "${P7ZIP_SRC}/C/LzmaDec.c"
        "${P7ZIP_SRC}/C/LzmaEnc.c"
        "${P7ZIP_SRC}/C/MtCoder.c"
        "${P7ZIP_SRC}/C/Ppmd7.c"
        "${P7ZIP_SRC}/C/Ppmd7Dec.c"
        "${P7ZIP_SRC}/C/Ppmd7Enc.c"
        "${P7ZIP_SRC}/C/Ppmd8.c"
        "${P7ZIP_SRC}/C/Ppmd8Dec.c"
        "${P7ZIP_SRC}/C/Ppmd8Enc.c"
        "${P7ZIP_SRC}/C/Sha1.c"
        "${P7ZIP_SRC}/C/Sha256.c"
        "${P7ZIP_SRC}/C/Sort.c"
        "${P7ZIP_SRC}/C/Threads.c"
        "${P7ZIP_SRC}/C/Xz.c"
        "${P7ZIP_SRC}/C/XzCrc64.c"
        "${P7ZIP_SRC}/C/XzCrc64Opt.c"
        "${P7ZIP_SRC}/C/XzDec.c"
        "${P7ZIP_SRC}/C/XzEnc.c"
        "${P7ZIP_SRC}/C/XzIn.c"
        "${P7ZIP_SRC}/C/AesOpt.c"
        #rar
        "${P7ZIP_SRC}/C/Blake2s.c"


        #编译顺序影响archiveformts顺序，把常用的排前面未知格式打开时效率更高

        #issue tar文件里有7z文件，先用7z打开则先可能被判断是否为7z，所以tar要放前面

        #tar可能和其他格式冲突，所以先判断tar
        "${P7ZIP_SRC}/CPP/7zip/Archive/Tar/TarHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Tar/TarHandlerOut.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Tar/TarHeader.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Tar/TarIn.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Tar/TarOut.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Tar/TarUpdate.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Tar/TarRegister.cpp"

        #zip
        "${P7ZIP_SRC}/CPP/7zip/Archive/Zip/ZipAddCommon.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Zip/ZipHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Zip/ZipHandlerOut.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Zip/ZipIn.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Zip/ZipItem.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Zip/ZipOut.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Zip/ZipUpdate.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Zip/ZipRegister.cpp"


        #rar
        "${P7ZIP_SRC}/CPP/7zip/Compress/Rar1Decoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/Rar2Decoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/Rar3Decoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/Rar3Vm.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/Rar5Decoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/RarCodecsRegister.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Rar/Rar5Handler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Rar/RarHandler.cpp"

        #7z
        "${P7ZIP_SRC}/CPP/7zip/Archive/7z/7zCompressionMode.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/7z/7zDecode.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/7z/7zEncode.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/7z/7zExtract.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/7z/7zFolderInStream.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/7z/7zHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/7z/7zHandlerOut.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/7z/7zHeader.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/7z/7zIn.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/7z/7zOut.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/7z/7zProperties.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/7z/7zSpecStream.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/7z/7zUpdate.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/7z/7zRegister.cpp"

        #gz
        "${P7ZIP_SRC}/CPP/7zip/Archive/GzHandler.cpp"
        #bz2
        "${P7ZIP_SRC}/CPP/7zip/Archive/Bz2Handler.cpp"
        #xz
        "${P7ZIP_SRC}/CPP/7zip/Archive/XzHandler.cpp"

        #deb
        "${P7ZIP_SRC}/CPP/7zip/Archive/ArHandler.cpp"

        #rpm
        "${P7ZIP_SRC}/CPP/7zip/Archive/RpmHandler.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Archive/Cab/CabBlockInStream.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Cab/CabHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Cab/CabHeader.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Cab/CabIn.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Cab/CabRegister.cpp"



        "${P7ZIP_SRC}/CPP/7zip/Archive/Common/CoderMixer2.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Common/DummyOutStream.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Common/FindSignature.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Common/HandlerOut.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Common/InStreamWithCRC.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Common/ItemNameUtils.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Common/MultiStream.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Common/OutStreamWithCRC.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Common/OutStreamWithSha1.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Common/ParseProperties.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Archive/DeflateProps.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/LzmaHandler.cpp"
        #        "${P7ZIP_SRC}/CPP/7zip/Archive/SplitHandler.cpp"


        "${P7ZIP_SRC}/CPP/7zip/Archive/ZHandler.cpp"


        "${P7ZIP_SRC}/CPP/7zip/Archive/CpioHandler.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Archive/ElfHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/PeHandler.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Archive/ExtHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/HfsHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/XarHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/ArjHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/LzhHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/NtfsHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/FatHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/VdiHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/VhdHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/VmdkHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/SquashfsHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/SwfHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/DmgHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/FatHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/CramfsHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/UefiHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/QcowHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/MbrHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/FlvHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/GptHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/PpmdHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/IhexHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/MachoHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/MubHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/MslzHandler.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Archive/Udf/UdfHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Udf/UdfIn.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Archive/Chm/ChmHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Chm/ChmIn.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Archive/HandlerCont.cpp"


        #iso
        "${P7ZIP_SRC}/CPP/7zip/Archive/Iso/IsoHandler.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Iso/IsoHeader.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Iso/IsoIn.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Archive/Iso/IsoRegister.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Common/CWrappers.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/CreateCoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/FilterCoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/InBuffer.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/InOutTempBuffer.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/LimitedStreams.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/MemBlocks.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/MethodId.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/MethodProps.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/OffsetStream.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/OutBuffer.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/OutMemStream.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/ProgressMt.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/ProgressUtils.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/PropId.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/StreamBinder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/StreamObjects.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/StreamUtils.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/UniqBlocks.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Common/VirtThread.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Compress/BZip2Crc.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/BZip2Decoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/BZip2Encoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/BZip2Register.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Compress/Bcj2Coder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/Bcj2Register.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/BcjCoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/BcjRegister.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Compress/ZlibDecoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/ZlibEncoder.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Compress/BranchMisc.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/BranchRegister.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Compress/BitlDecoder.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Compress/ByteSwap.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Compress/CopyCoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/CopyRegister.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Compress/Deflate64Register.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/DeflateDecoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/DeflateEncoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/DeflateRegister.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/DeltaFilter.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Compress/ImplodeDecoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/ImplodeHuffmanDecoder.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Compress/LzOutWindow.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/Lzma2Decoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/Lzma2Encoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/Lzma2Register.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/LzmaDecoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/LzmaEncoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/LzmaRegister.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Compress/LzxDecoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/LzhDecoder.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Compress/PpmdDecoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/PpmdEncoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/PpmdRegister.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/PpmdZip.cpp"

        "${P7ZIP_SRC}/CPP/7zip/Compress/QuantumDecoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/ShrinkDecoder.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Compress/ZDecoder.cpp"


        "${P7ZIP_SRC}/CPP/7zip/Crypto/7zAes.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Crypto/7zAesRegister.cpp"

        #rar
        "${P7ZIP_SRC}/CPP/7zip/Crypto/Rar5Aes.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Crypto/RarAes.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Crypto/Rar20Crypto.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Crypto/HmacSha256.cpp"


        "${P7ZIP_SRC}/CPP/7zip/Crypto/HmacSha1.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Crypto/MyAes.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Crypto/MyAesReg.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Crypto/Pbkdf2HmacSha1.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Crypto/RandGen.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Crypto/WzAes.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Crypto/ZipCrypto.cpp"
        "${P7ZIP_SRC}/CPP/7zip/Crypto/ZipStrong.cpp"

        #${P7ZIP_SRC}/        "CPP/7zip/UI/Common/ArchiveCommandLine.cpp"
        #        "${P7ZIP_SRC}/CPP/7zip/UI/Common/ArchiveExtractCallback.cpp"
        #        "${P7ZIP_SRC}/CPP/7zip/UI/Common/ArchiveOpenCallback.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Common/Bench.cpp"
        #        "${P7ZIP_SRC}/CPP/7zip/UI/Common/DefaultName.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Common/EnumDirItems.cpp"
        #        "${P7ZIP_SRC}/CPP/7zip/UI/Common/Extract.cpp"
        #        "${P7ZIP_SRC}/CPP/7zip/UI/Common/ExtractingFilePath.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Common/HashCalc.cpp"
        "${P7ZIP_SRC}/CPP/7zip/UI/Common/LoadCodecs.cpp"
        #        "${P7ZIP_SRC}/CPP/7zip/UI/Common/OpenArchive.cpp"
        "${P7ZIP_SRC}/CPP/7zip/UI/Common/PropIDUtils.cpp"

        #        "${P7ZIP_SRC}/CPP/7zip/UI/Common/SetProperties.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Common/SortUtils.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Common/TempFiles.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Common/Update.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Common/UpdateAction.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Common/UpdateCallback.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Common/UpdatePair.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Common/UpdateProduce.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Console/BenchCon.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Console/ConsoleClose.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Console/ExtractCallbackConsole.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Console/HashCon.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Console/List.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Console/Main.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Console/MainAr.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Console/OpenCallbackConsole.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Console/PercentPrinter.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Console/UpdateCallbackConsole.cpp"
        #${P7ZIP_SRC}/        "CPP/7zip/UI/Console/UserInputUtils.cpp"
        "${P7ZIP_SRC}/CPP/Common/CRC.cpp"
        #        "${P7ZIP_SRC}/CPP/Common/CommandLineParser.cpp"
        "${P7ZIP_SRC}/CPP/Common/CrcReg.cpp"
        "${P7ZIP_SRC}/CPP/Common/IntToString.cpp"
        #        "${P7ZIP_SRC}/CPP/Common/ListFileUtils.cpp"
        "${P7ZIP_SRC}/CPP/Common/MyString.cpp"
        "${P7ZIP_SRC}/CPP/Common/MyVector.cpp"
        "${P7ZIP_SRC}/CPP/Common/MyWindows.cpp"
        "${P7ZIP_SRC}/CPP/Common/Sha1Reg.cpp"
        "${P7ZIP_SRC}/CPP/Common/Sha256Reg.cpp"
        "${P7ZIP_SRC}/CPP/Common/MyXml.cpp"
        "${P7ZIP_SRC}/CPP/Common/MyMap.cpp"
        #        "${P7ZIP_SRC}/CPP/Common/StdOutStream.cpp"
        "${P7ZIP_SRC}/CPP/Common/StringConvert.cpp"
        "${P7ZIP_SRC}/CPP/Common/StringToInt.cpp"
        "${P7ZIP_SRC}/CPP/Common/UTFConvert.cpp"
        "${P7ZIP_SRC}/CPP/Common/Wildcard.cpp"
        "${P7ZIP_SRC}/CPP/Common/XzCrc64Reg.cpp"

        "${P7ZIP_SRC}/CPP/Windows/ErrorMsg.cpp"
        "${P7ZIP_SRC}/CPP/Windows/FileDir.cpp"
        "${P7ZIP_SRC}/CPP/Windows/FileFind.cpp"
        "${P7ZIP_SRC}/CPP/Windows/FileIO.cpp"
        "${P7ZIP_SRC}/CPP/Windows/FileName.cpp"
        "${P7ZIP_SRC}/CPP/Windows/PropVariant.cpp"
        "${P7ZIP_SRC}/CPP/Windows/PropVariantConv.cpp"
        "${P7ZIP_SRC}/CPP/Windows/PropVariantUtils.cpp"
        "${P7ZIP_SRC}/CPP/Windows/Synchronization.cpp"
        "${P7ZIP_SRC}/CPP/Windows/System.cpp"
        "${P7ZIP_SRC}/CPP/Windows/TimeUtils.cpp"
        #        "${P7ZIP_SRC}/CPP/myWindows/myAddExeFlag.cpp"
        #        "${P7ZIP_SRC}/CPP/myWindows/mySplitCommandLine.cpp"
        "${P7ZIP_SRC}/CPP/myWindows/wine_date_and_time.cpp"

        )


set(JBINDING_CPP_FILES
        jbinding-cpp/Debug.cpp
        #   jbinding-cpp/ idd_def.cpp
        jbinding-cpp/JNITools.cpp
        jbinding-cpp/JavaStaticInfo.cpp
        jbinding-cpp/SevenZipJBinding.cpp
        jbinding-cpp/UniversalArchiveOpenCallback.cpp
        jbinding-cpp/UserTrace.cpp
        jbinding-cpp/CodecTools.cpp
        jbinding-cpp/JBindingTools.cpp
        jbinding-cpp/JavaToCPP/JavaToCPPSevenZip.cpp
        jbinding-cpp/JavaToCPP/JavaToCPPInArchiveImpl.cpp
        jbinding-cpp/JavaToCPP/JavaToCPPOutArchiveImpl.cpp
        jbinding-cpp/CPPToJava/CPPToJavaArchiveExtractCallback.cpp
        jbinding-cpp/CPPToJava/CPPToJavaArchiveOpenCallback.cpp
        jbinding-cpp/CPPToJava/CPPToJavaArchiveOpenVolumeCallback.cpp
        jbinding-cpp/CPPToJava/CPPToJavaCryptoGetTextPassword.cpp
        jbinding-cpp/CPPToJava/CPPToJavaInStream.cpp
        jbinding-cpp/CPPToJava/CPPToJavaOutStream.cpp
        jbinding-cpp/CPPToJava/CPPToJavaProgress.cpp
        jbinding-cpp/CPPToJava/CPPToJavaSequentialInStream.cpp
        jbinding-cpp/CPPToJava/CPPToJavaSequentialOutStream.cpp
        jbinding-cpp/CPPToJava/CPPToJavaArchiveUpdateCallback.cpp

        )
#set(MYBINDING mybinding/SevenZip.cpp)

#add_subdirectory(p7zip_16.02)

#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pthread")
#set(CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS} -O2")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11 -fvisibility=hidden")



find_library(log-lib log)

add_library(p7zip-jni SHARED ${JBINDING_CPP_FILES} ${P7ZIP_SRC_FILES} ${MYBINDING} RegisterNative.cxx)

target_link_libraries(p7zip-jni ${log-lib} dl c)
