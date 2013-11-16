/**
 * include/BMP.h
 * Bitmap interface
 *
 * Author: Dennis J. McWherter, Jr.
 * 21 January 2012
 */

#ifndef _BMP_H__
#define _BMP_H__

#include <fstream>

class BMP
{
public:
  BMP(const char* inFile, const char* outFile, unsigned dimX, unsigned dimY);
  virtual ~BMP();

private:
  void writeHeaders(std::ofstream& outStream);

  // BMP Header
  struct bmp_header {
    unsigned char magic[2];
    unsigned fsize;
    unsigned short c1;
    unsigned short c2;
    unsigned offset;
  } _bmpHeader;

  // DIB Header
  struct dib_header {
    unsigned header_size;
    int width;
    int height;
    unsigned short planes;
    unsigned short bitspp;
    unsigned compress;
    unsigned bbytes;
    int hres;
    int vres;
    unsigned ncolors;
    unsigned nimpcolors;
    unsigned redMask;
    unsigned greenMask;
    unsigned blueMask;
    unsigned alphaMask;
  } _dibHeader;

  // Member variables
  const char* _inFile;
  const char* _outFile;
  unsigned _dimX, _dimY;
};

#endif /* _BMP_H__ */
