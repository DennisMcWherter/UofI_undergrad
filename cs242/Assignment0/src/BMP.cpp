/**
 * src/BMP.cpp
 * Bitmap interface
 *
 * Author: Dennis J. McWherter, Jr.
 * 21 January 2012
 */

#include "../include/BMP.h"

using namespace std;

/**
 * Constructor
 */
BMP::BMP(const char* inFile, const char* outFile, unsigned dimX, unsigned dimY)
  : _inFile(inFile), _outFile(outFile), _dimX(dimX), _dimY(dimY)
{
  if(inFile == NULL || outFile == NULL)
    throw "Could not initialize BMP: NULL values detected\n";
}

/**
 * Destructor (empty)
 */
BMP::~BMP()
{
}

/**
 * writeHeaders
 * Write the BMP headers
 *
 * @param outStream - Output file
 *
 * @return void
 */
void BMP::writeHeaders(ofstream& outStream)
{
  // Magic word
  _bmpHeader.magic[0] = 'B';
  _bmpHeader.magic[1] = 'M';

  // File size
  _bmpHeader.fsize = 54 + (_dimX * _dimY);

  // Offset
  _bmpHeader.offset = 54; // Total size of header

  // Setup DIB header
  _dibHeader.header_size = 108;

  _dibHeader.width  = _dimX;
  _dibHeader.height = _dimY;

  _dibHeader.planes = 1;

  _dibHeader.bitspp = 32;

  _dibHeader.compress = 0;

  _dibHeader.bbytes = 32;

  _dibHeader.hres = 2835;
  _dibHeader.vres = 2835;

  _dibHeader.ncolors = _dibHeader.nimpcolors = 0;

  // Color mask
  _dibHeader.alphaMask = 0x000000ff;
  _dibHeader.redMask   = 0x0000ff00;
  _dibHeader.greenMask = 0x00ff0000;
  _dibHeader.blueMask  = 0xff000000;
}
