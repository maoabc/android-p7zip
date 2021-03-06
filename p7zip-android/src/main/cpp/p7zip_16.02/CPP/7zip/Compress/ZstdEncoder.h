// (C) 2016 Tino Reichardt

#define ZSTD_STATIC_LINKING_ONLY
#include "../../../C/Alloc.h"
#include "../../../C/Threads.h"
#include "../../../C/zstd/zstd.h"
#include "../../../C/zstdmt/zstdmt.h"

#include "../../Common/Common.h"
#include "../../Common/MyCom.h"
#include "../ICoder.h"
#include "../Common/StreamUtils.h"

#ifndef EXTRACT_ONLY
namespace NCompress {
namespace NZSTD {

struct CProps
{
  CProps() { clear (); }
  void clear ()
  {
    memset(this, 0, sizeof (*this));
    _ver_major = ZSTD_VERSION_MAJOR;
    _ver_minor = ZSTD_VERSION_MINOR;
    _level = 3;
  }

  Byte _ver_major;
  Byte _ver_minor;
  Byte _level;
  Byte _reserved[2];
};

class CEncoder:
  public ICompressCoder,
  public ICompressSetCoderProperties,
  public ICompressWriteCoderProperties,
  public CMyUnknownImp
{
  CProps _props;

  UInt64 _processedIn;
  UInt64 _processedOut;
  UInt32 _inputSize;
  UInt32 _numThreads;

  ZSTDMT_CCtx *_ctx;
  HRESULT ErrorOut(size_t code);

public:
  MY_UNKNOWN_IMP2 (ICompressSetCoderProperties, ICompressWriteCoderProperties)
  STDMETHOD (Code)(ISequentialInStream *inStream, ISequentialOutStream *outStream, const UInt64 *inSize, const UInt64 *outSize, ICompressProgressInfo *progress);
  STDMETHOD (SetCoderProperties)(const PROPID *propIDs, const PROPVARIANT *props, UInt32 numProps);
  STDMETHOD (WriteCoderProperties)(ISequentialOutStream *outStream);
  STDMETHODIMP SetNumberOfThreads(UInt32 numThreads);

  CEncoder();
  virtual ~CEncoder();
};

}}
#endif
