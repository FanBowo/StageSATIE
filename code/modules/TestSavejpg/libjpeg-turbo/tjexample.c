/*
 * Copyright (C)2011-2012, 2014-2015, 2017, 2019 D. R. Commander.
 *                                               All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the libjpeg-turbo Project nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS",
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This program demonstrates how to compress, decompress, and transform JPEG
 * images using the TurboJPEG C API
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <turbojpeg.h>


#ifdef _WIN32
#define strcasecmp  stricmp
#define strncasecmp  strnicmp
#endif

#define THROW(action, message) { \
  printf("ERROR in line %d while %s:\n%s\n", __LINE__, action, message); \
  retval = -1;  goto bailout; \
}

#define THROW_TJ(action)  THROW(action, tjGetErrorStr2(tjInstance))

#define THROW_UNIX(action)  THROW(action, strerror(errno))

#define DEFAULT_SUBSAMP  TJSAMP_444
#define DEFAULT_QUALITY  95


const char *subsampName[TJ_NUMSAMP] = {
  "4:4:4", "4:2:2", "4:2:0", "Grayscale", "4:4:0", "4:1:1"
};

const char *colorspaceName[TJ_NUMCS] = {
  "RGB", "YCbCr", "GRAY", "CMYK", "YCCK"
};

tjscalingfactor *scalingFactors = NULL;
int numScalingFactors = 0;


/* DCT filter example.  This produces a negative of the image. */
//this function was used to work as a self defined filter of erach pixel
static int customFilter(short *coeffs, tjregion arrayRegion,
                        tjregion planeRegion, int componentIndex,
                        int transformIndex, tjtransform *transform)
{
  int i;

  for (i = 0; i < arrayRegion.w * arrayRegion.h; i++)
    coeffs[i] = -coeffs[i];

  return 0;
}


static void usage(char *programName)
{
  int i;

  printf("\nUSAGE: %s <Input image> <Output image> [options]\n\n",
         programName);

  printf("Input and output images can be in Windows BMP or PBMPLUS (PPM/PGM) format.  If\n");
  printf("either filename ends in a .jpg extension, then the TurboJPEG API will be used\n");
  printf("to compress or decompress the image.\n\n");

  printf("Compression Options (used if the output image is a JPEG image)\n");
  printf("--------------------------------------------------------------\n\n");

  printf("-subsamp <444|422|420|gray> = Apply this level of chrominance subsampling when\n");
  printf("     compressing the output image.  The default is to use the same level of\n");
  printf("     subsampling as in the input image, if the input image is also a JPEG\n");
  printf("     image, or to use grayscale if the input image is a grayscale non-JPEG\n");
  printf("     image, or to use %s subsampling otherwise.\n\n",
         subsampName[DEFAULT_SUBSAMP]);

  printf("-q <1-100> = Compress the output image with this JPEG quality level\n");
  printf("     (default = %d).\n\n", DEFAULT_QUALITY);

  printf("Decompression Options (used if the input image is a JPEG image)\n");
  printf("---------------------------------------------------------------\n\n");

  printf("-scale M/N = Scale the input image by a factor of M/N when decompressing it.\n");
  printf("(M/N = ");
  for (i = 0; i < numScalingFactors; i++) {
    printf("%d/%d", scalingFactors[i].num, scalingFactors[i].denom);
    if (numScalingFactors == 2 && i != numScalingFactors - 1)
      printf(" or ");
    else if (numScalingFactors > 2) {
      if (i != numScalingFactors - 1)
        printf(", ");
      if (i == numScalingFactors - 2)
        printf("or ");
    }
  }
  printf(")\n\n");

  printf("-hflip, -vflip, -transpose, -transverse, -rot90, -rot180, -rot270 =\n");
  printf("     Perform one of these lossless transform operations on the input image\n");
  printf("     prior to decompressing it (these options are mutually exclusive.)\n\n");

  printf("-grayscale = Perform lossless grayscale conversion on the input image prior\n");
  printf("     to decompressing it (can be combined with the other transform operations\n");
  printf("     above.)\n\n");

  printf("-crop WxH+X+Y = Perform lossless cropping on the input image prior to\n");
  printf("     decompressing it.  X and Y specify the upper left corner of the cropping\n");
  printf("     region, and W and H specify the width and height of the cropping region.\n");
  printf("     X and Y must be evenly divible by the MCU block size (8x8 if the input\n");
  printf("     image was compressed using no subsampling or grayscale, 16x8 if it was\n");
  printf("     compressed using 4:2:2 subsampling, or 16x16 if it was compressed using\n");
  printf("     4:2:0 subsampling.)\n\n");

  printf("General Options\n");
  printf("---------------\n\n");

  printf("-fastupsample = Use the fastest chrominance upsampling algorithm available in\n");
  printf("     the underlying codec.\n\n");

  printf("-fastdct = Use the fastest DCT/IDCT algorithms available in the underlying\n");
  printf("     codec.\n\n");

  printf("-accuratedct = Use the most accurate DCT/IDCT algorithms available in the\n");
  printf("     underlying codec.\n\n");

  exit(1);
}


int main(int argc, char **argv)
{
  tjscalingfactor scalingFactor = { 1, 1 };
  int outSubsamp = -1, outQual = -1;
  tjtransform xform;
  int flags = 0;
  int width, height;
  char *inFormat, *outFormat;//array of input and output format control instruction
  FILE *jpegFile = NULL;//jpeg file pointer 
  unsigned char *imgBuf = NULL, *jpegBuf = NULL;//input and output pixel data buffer 
  int retval = 0, i, pixelFormat = TJPF_UNKNOWN;//pixel format, TJPF_RGB, TJPF_BGR, etc
  tjhandle tjInstance = NULL;

/* tjGetScalingFactors
Returns a list of fractional scaling factors that the JPEG decompressor in this implementation of TurboJPEG supports.
Parameters
    numscalingfactors	pointer to an integer variable that will receive the number of elements in the list
Returns
    a pointer to a list of fractional scaling factors, or NULL if an error is encountered (see tjGetErrorStr2().) 
*/
  if ((scalingFactors = tjGetScalingFactors(&numScalingFactors)) == NULL)
    THROW_TJ("getting scaling factors");
  memset(&xform, 0, sizeof(tjtransform));

  if (argc < 3)
    usage(argv[0]);

  /* Parse arguments. */
  for (i = 3; i < argc; i++) {
	  //"-scale M/N = Scale the input image by a factor of M/N when decompressing it.\n"
    if (!strncasecmp(argv[i], "-sc", 3) && i < argc - 1) {
      int match = 0, temp1 = 0, temp2 = 0, j;

      if (sscanf(argv[++i], "%d/%d", &temp1, &temp2) < 2)//get M,N scale and save them in temp1/temp2
        usage(argv[0]);
      for (j = 0; j < numScalingFactors; j++) {
        if ((double)temp1 / (double)temp2 == (double)scalingFactors[j].num /
                                             (double)scalingFactors[j].denom) {
          scalingFactor = scalingFactors[j];
          match = 1;
          break;
        }
      }
      if (match != 1)
        usage(argv[0]);
    } else if (!strncasecmp(argv[i], "-su", 3) && i < argc - 1) {
      i++;
      if (!strncasecmp(argv[i], "g", 1))
        outSubsamp = TJSAMP_GRAY;
      else if (!strcasecmp(argv[i], "444"))
        outSubsamp = TJSAMP_444;
      else if (!strcasecmp(argv[i], "422"))
        outSubsamp = TJSAMP_422;
      else if (!strcasecmp(argv[i], "420"))
        outSubsamp = TJSAMP_420;
      else
        usage(argv[0]);
    } else if (!strncasecmp(argv[i], "-q", 2) && i < argc - 1) {
      outQual = atoi(argv[++i]);
      if (outQual < 1 || outQual > 100)
        usage(argv[0]);
    } else if (!strncasecmp(argv[i], "-g", 2))
		//"-grayscale = Perform lossless grayscale conversion on the input image prior\n"
      xform.options |= TJXOPT_GRAY;
    else if (!strcasecmp(argv[i], "-hflip"))
      xform.op = TJXOP_HFLIP;
    else if (!strcasecmp(argv[i], "-vflip"))
      xform.op = TJXOP_VFLIP;
    else if (!strcasecmp(argv[i], "-transpose"))
      xform.op = TJXOP_TRANSPOSE;
    else if (!strcasecmp(argv[i], "-transverse"))
      xform.op = TJXOP_TRANSVERSE;
    else if (!strcasecmp(argv[i], "-rot90"))
      xform.op = TJXOP_ROT90;
    else if (!strcasecmp(argv[i], "-rot180"))
      xform.op = TJXOP_ROT180;
    else if (!strcasecmp(argv[i], "-rot270"))
      xform.op = TJXOP_ROT270;
    else if (!strcasecmp(argv[i], "-custom"))
      xform.customFilter = customFilter;
    else if (!strncasecmp(argv[i], "-c", 2) && i < argc - 1) {
/*
printf("-crop WxH+X+Y = Perform lossless cropping on the input image prior to\n");
  printf("     decompressing it.  X and Y specify the upper left corner of the cropping\n");
  printf("     region, and W and H specify the width and height of the cropping region.\n");
  printf("     X and Y must be evenly divible by the MCU block size (8x8 if the input\n");
  printf("     image was compressed using no subsampling or grayscale, 16x8 if it was\n");
  printf("     compressed using 4:2:2 subsampling, or 16x16 if it was compressed using\n");
  printf("     4:2:0 subsampling.)\n\n");

*/
      if (sscanf(argv[++i], "%dx%d+%d+%d", &xform.r.w, &xform.r.h, &xform.r.x,
                 &xform.r.y) < 4 ||
          xform.r.x < 0 || xform.r.y < 0 || xform.r.w < 1 || xform.r.h < 1)
        usage(argv[0]);
      xform.options |= TJXOPT_CROP;
    } else if (!strcasecmp(argv[i], "-fastupsample")) {
      printf("Using fast upsampling code\n");
      flags |= TJFLAG_FASTUPSAMPLE;
    } else if (!strcasecmp(argv[i], "-fastdct")) {
      printf("Using fastest DCT/IDCT algorithm\n");
      flags |= TJFLAG_FASTDCT;
    } else if (!strcasecmp(argv[i], "-accuratedct")) {
      printf("Using most accurate DCT/IDCT algorithm\n");
      flags |= TJFLAG_ACCURATEDCT;
    } else usage(argv[0]);
  }

  /* Determine input and output image formats based on file extensions. */
  inFormat = strrchr(argv[1], '.');
  outFormat = strrchr(argv[2], '.');
  if (inFormat == NULL || outFormat == NULL || strlen(inFormat) < 2 ||
      strlen(outFormat) < 2)
    usage(argv[0]);
  inFormat = &inFormat[1];
  outFormat = &outFormat[1];

  if (!strcasecmp(inFormat, "jpg")) {
    /* Input image is a JPEG image.  Decompress and/or transform it. */
    long size;
    int inSubsamp, inColorspace;
    int doTransform = (xform.op != TJXOP_NONE || xform.options != 0 ||
                       xform.customFilter != NULL);
    unsigned long jpegSize;

    /* Read the JPEG file into memory. */
    if ((jpegFile = fopen(argv[1], "rb")) == NULL)
      THROW_UNIX("opening input file");
    if (fseek(jpegFile, 0, SEEK_END) < 0 || ((size = ftell(jpegFile)) < 0) ||
        fseek(jpegFile, 0, SEEK_SET) < 0)
      THROW_UNIX("determining input file size");
    if (size == 0)
      THROW("determining input file size", "Input file contains no data");
    jpegSize = (unsigned long)size;
    if ((jpegBuf = (unsigned char *)tjAlloc(jpegSize)) == NULL)
      THROW_UNIX("allocating JPEG buffer");
    if (fread(jpegBuf, jpegSize, 1, jpegFile) < 1)
      THROW_UNIX("reading input file");
    fclose(jpegFile);  jpegFile = NULL;

    if (doTransform) {
      /* Transform it. */
      unsigned char *dstBuf = NULL;  /* Dynamically allocate the JPEG buffer */
      unsigned long dstSize = 0;

      if ((tjInstance = tjInitTransform()) == NULL)
        THROW_TJ("initializing transformer");
      xform.options |= TJXOPT_TRIM;
      if (tjTransform(tjInstance, jpegBuf, jpegSize, 1, &dstBuf, &dstSize,
                      &xform, flags) < 0)
        THROW_TJ("transforming input image");
      tjFree(jpegBuf);
      jpegBuf = dstBuf;
      jpegSize = dstSize;
    } else {
      if ((tjInstance = tjInitDecompress()) == NULL)
        THROW_TJ("initializing decompressor");
    }

    if (tjDecompressHeader3(tjInstance, jpegBuf, jpegSize, &width, &height,
                            &inSubsamp, &inColorspace) < 0)
      THROW_TJ("reading JPEG header");

    printf("%s Image:  %d x %d pixels, %s subsampling, %s colorspace\n",
           (doTransform ? "Transformed" : "Input"), width, height,
           subsampName[inSubsamp], colorspaceName[inColorspace]);

    if (!strcasecmp(outFormat, "jpg") && doTransform &&
        scalingFactor.num == 1 && scalingFactor.denom == 1 && outSubsamp < 0 &&
        outQual < 0) {
      /* Input image has been transformed, and no re-compression options
         have been selected.  Write the transformed image to disk and exit. */
      if ((jpegFile = fopen(argv[2], "wb")) == NULL)
        THROW_UNIX("opening output file");
      if (fwrite(jpegBuf, jpegSize, 1, jpegFile) < 1)
        THROW_UNIX("writing output file");
      fclose(jpegFile);  jpegFile = NULL;
      goto bailout;
    }

    /* Scaling and/or a non-JPEG output image format and/or compression options
       have been selected, so we need to decompress the input/transformed
       image. */
    width = TJSCALED(width, scalingFactor);
    height = TJSCALED(height, scalingFactor);
    if (outSubsamp < 0)
      outSubsamp = inSubsamp;

    pixelFormat = TJPF_BGRX;
    if ((imgBuf = (unsigned char *)tjAlloc(width * height *
                                           tjPixelSize[pixelFormat])) == NULL)
      THROW_UNIX("allocating uncompressed image buffer");

    if (tjDecompress2(tjInstance, jpegBuf, jpegSize, imgBuf, width, 0, height,
                      pixelFormat, flags) < 0)
      THROW_TJ("decompressing JPEG image");
    tjFree(jpegBuf);  jpegBuf = NULL;
    tjDestroy(tjInstance);  tjInstance = NULL;
  } else {
    /* Input image is not a JPEG image.  Load it into memory. */
    if ((imgBuf = tjLoadImage(argv[1], &width, 1, &height, &pixelFormat,
                              0)) == NULL)
/*
Parameters
    filename	name of a file containing an uncompressed image in Windows BMP or PBMPLUS (PPM/PGM) format
    width	pointer to an integer variable that will receive the width (in pixels) of the uncompressed image
    align	row alignment of the image buffer to be returned (must be a power of 2.) For instance, setting this parameter to 4 will cause all rows in the image buffer to be padded to the nearest 32-bit boundary, and setting this parameter to 1 will cause all rows in the image buffer to be unpadded.
    height	pointer to an integer variable that will receive the height (in pixels) of the uncompressed image
    pixelFormat	pointer to an integer variable that specifies or will receive the pixel format of the uncompressed image buffer. The behavior of tjLoadImage() will vary depending on the value of *pixelFormat passed to the function:

        TJPF_UNKNOWN : The uncompressed image buffer returned by the function will use the most optimal pixel format for the file type, and *pixelFormat will contain the ID of this pixel format upon successful return from the function.
        TJPF_GRAY : Only PGM files and 8-bit BMP files with a grayscale colormap can be loaded.
        TJPF_CMYK : The RGB or grayscale pixels stored in the file will be converted using a quick & dirty algorithm that is suitable only for testing purposes (proper conversion between CMYK and other formats requires a color management system.)
        Other pixel formats : The uncompressed image buffer will use the specified pixel format, and pixel format conversion will be performed if necessary.

    flags	the bitwise OR of one or more of the flags.
*/
      THROW_TJ("loading input image");
    if (outSubsamp < 0) {
      if (pixelFormat == TJPF_GRAY)
        outSubsamp = TJSAMP_GRAY;
      else
        outSubsamp = TJSAMP_444;
    }
    printf("Input Image:  %d x %d pixels\n", width, height);
  }

  printf("Output Image (%s):  %d x %d pixels", outFormat, width, height);

  if (!strcasecmp(outFormat, "jpg")) {
    /* Output image format is JPEG.  Compress the uncompressed image. */
    unsigned long jpegSize = 0;

    jpegBuf = NULL;  /* Dynamically allocate the JPEG buffer */

    if (outQual < 0)
      outQual = DEFAULT_QUALITY;
    printf(", %s subsampling, quality = %d\n", subsampName[outSubsamp],
           outQual);

    if ((tjInstance = tjInitCompress()) == NULL)
      THROW_TJ("initializing compressor");
  //The most important step
    if (tjCompress2(tjInstance, imgBuf, width, 0, height, pixelFormat,
                    &jpegBuf, &jpegSize, outSubsamp, outQual, flags) < 0)
/*
Parameters
    handle	a handle to a TurboJPEG compressor or transformer instance
    srcBuf	pointer to an image buffer containing RGB, grayscale, or CMYK pixels to be compressed
    width	width (in pixels) of the source image
    pitch	bytes per line in the source image. Normally, this should be width * tjPixelSize[pixelFormat] if the image is unpadded, or TJPAD(width * tjPixelSize[pixelFormat]) if each line of the image is padded to the nearest 32-bit boundary, as is the case for Windows bitmaps. You can also be clever and use this parameter to skip lines, etc. Setting this parameter to 0 is the equivalent of setting it to width * tjPixelSize[pixelFormat].
    height	height (in pixels) of the source image
    pixelFormat	pixel format of the source image (see Pixel formats.)
    jpegBuf	address of a pointer to an image buffer that will receive the JPEG image. TurboJPEG has the ability to reallocate the JPEG buffer to accommodate the size of the JPEG image. Thus, you can choose to:

        pre-allocate the JPEG buffer with an arbitrary size using tjAlloc() and let TurboJPEG grow the buffer as needed,
        set *jpegBuf to NULL to tell TurboJPEG to allocate the buffer for you, or
        pre-allocate the buffer to a "worst case" size determined by calling tjBufSize(). This should ensure that the buffer never has to be re-allocated (setting TJFLAG_NOREALLOC guarantees that it won't be.)

    If you choose option 1, *jpegSize should be set to the size of your pre-allocated buffer. In any case, unless you have set TJFLAG_NOREALLOC, you should always check *jpegBuf upon return from this function, as it may have changed.
    jpegSize	pointer to an unsigned long variable that holds the size of the JPEG image buffer. If *jpegBuf points to a pre-allocated buffer, then *jpegSize should be set to the size of the buffer. Upon return, *jpegSize will contain the size of the JPEG image (in bytes.) If *jpegBuf points to a JPEG image buffer that is being reused from a previous call to one of the JPEG compression functions, then *jpegSize is ignored.
    jpegSubsamp	the level of chrominance subsampling to be used when generating the JPEG image (see Chrominance subsampling options.)
    jpegQual	the image quality of the generated JPEG image (1 = worst, 100 = best)
    flags	the bitwise OR of one or more of the flags
*/
      THROW_TJ("compressing image");
    tjDestroy(tjInstance);  tjInstance = NULL;

    /* Write the JPEG image to disk. */
    if ((jpegFile = fopen(argv[2], "wb")) == NULL)
      THROW_UNIX("opening output file");
    if (fwrite(jpegBuf, jpegSize, 1, jpegFile) < 1)
      THROW_UNIX("writing output file");
    tjDestroy(tjInstance);  tjInstance = NULL;
    fclose(jpegFile);  jpegFile = NULL;
    tjFree(jpegBuf);  jpegBuf = NULL;
  } else {
    /* Output image format is not JPEG.  Save the uncompressed image
       directly to disk. */
    printf("\n");
    if (tjSaveImage(argv[2], imgBuf, width, 0, height, pixelFormat, 0) < 0)
      THROW_TJ("saving output image");
  }

bailout:
  if (imgBuf) tjFree(imgBuf);
  if (tjInstance) tjDestroy(tjInstance);
  if (jpegBuf) tjFree(jpegBuf);
  if (jpegFile) fclose(jpegFile);
  return retval;
}
