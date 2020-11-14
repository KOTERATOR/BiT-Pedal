////////////////////////////////////////////////////////////////////////////////
///
/// Classes for easy reading & writing of WAV sound files.
///
/// For big-endian CPU, define BIG_ENDIAN during compile-time to correctly
/// parse the WAV files with such processors.
/// 
/// Admittingly, more complete WAV reader routines may exist in public domain, but 
/// the reason for 'yet another' one is that those generic WAV reader libraries are
/// exhaustingly large and cumbersome! Wanted to have something simpler here, i.e. 
/// something that's not already larger than rest of the SoundTouch/SoundStretch program...
///
/// Author        : Copyright (c) Olli Parviainen
/// Author e-mail : oparviai 'at' iki.fi
/// SoundTouch WWW: http://www.surina.net/soundtouch
///
////////////////////////////////////////////////////////////////////////////////
//
// License :
//
//  SoundTouch audio processing library
//  Copyright (c) Olli Parviainen
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////

#ifndef WAVFILE_H
#define WAVFILE_H

#include <stdio.h>
#include <SD.h>

#ifndef uint
typedef unsigned int uint;
#endif           


/// WAV audio file 'riff' section header
typedef struct 
{
    char riff_char[4];
    uint package_len;
    char wave[4];
} WavRiff;

/// WAV audio file 'format' section header
typedef struct 
{
    char  fmt[4];
    unsigned int   format_len;
    unsigned short fixed;
    unsigned short channel_number;
    unsigned int   sample_rate;
    unsigned int   byte_rate;
    unsigned short byte_per_sample;
    unsigned short bits_per_sample;
} WavFormat;

/// WAV audio file 'fact' section header
typedef struct 
{
    char fact_field[4];
    uint fact_len;
    uint fact_sample_len;
} WavFact;

/// WAV audio file 'data' section header
typedef struct 
{
    char  data_field[4];
    uint  data_len;
} WavData;


/// WAV audio file header
typedef struct 
{
    WavRiff   riff;
    WavFormat format;
    WavFact   fact;
    WavData   data;
} WavHeader;


/// Base class for processing WAV audio files.
class WavFileBase
{
private:
    /// Conversion working buffer;
    char *convBuff;
    int convBuffSize;

protected:
    WavFileBase();
    virtual ~WavFileBase();

    /// Get pointer to conversion buffer of at min. given size
    void *getConvBuffer(int sizeByte);
};


/// Class for reading WAV audio files.
class WavInFile : protected WavFileBase
{
private:
    /// File pointer.
    File file;

    /// Position within the audio stream
    long position;

    /// Counter of how many bytes of sample data have been read from the file.
    long dataRead;

    /// WAV header information
    WavHeader header;

    /// Init the WAV file stream
    void init();

    /// Read WAV file headers.
    /// \return zero if all ok, nonzero if file format is invalid.
    int readWavHeaders();

    /// Checks WAV file header tags.
    /// \return zero if all ok, nonzero if file format is invalid.
    int checkCharTags() const;

    /// Reads a single WAV file header block.
    /// \return zero if all ok, nonzero if file format is invalid.
    int readHeaderBlock();

    /// Reads WAV file 'riff' block
    int readRIFFBlock();

public:
    /// Constructor: Opens the given WAV file. If the file can't be opened,
    /// throws 'runtime_error' exception.
    WavInFile(const char *filename);

    WavInFile(File file);

    /// Destructor: Closes the file.
    ~WavInFile();

    /// Rewind to beginning of the file
    void rewind();

    /// Get sample rate.
    uint getSampleRate() const;

    /// Get number of bits per sample, i.e. 8 or 16.
    uint getNumBits() const;

    /// Get sample data size in bytes. Ahem, this should return same information as 
    /// 'getBytesPerSample'...
    uint getDataSizeInBytes() const;

    /// Get total number of samples in file.
    uint getNumSamples() const;

    /// Get number of bytes per audio sample (e.g. 16bit stereo = 4 bytes/sample)
    uint getBytesPerSample() const;
    
    /// Get number of audio channels in the file (1=mono, 2=stereo)
    uint getNumChannels() const;

    /// Get the audio file length in milliseconds
    uint getLengthMS() const;

    /// Returns how many milliseconds of audio have so far been read from the file
    ///
    /// \return elapsed duration in milliseconds
    uint getElapsedMS() const;

    /// Reads audio samples from the WAV file. This routine works only for 8 bit samples.
    /// Reads given number of elements from the file or if end-of-file reached, as many 
    /// elements as are left in the file.
    ///
    /// \return Number of 8-bit integers read from the file.
    int read(unsigned char *buffer, int maxElems);

    /// Reads audio samples from the WAV file to 16 bit integer format. Reads given number 
    /// of elements from the file or if end-of-file reached, as many elements as are 
    /// left in the file.
    ///
    /// \return Number of 16-bit integers read from the file.
    int read(short *buffer,     ///< Pointer to buffer where to read data.
             int maxElems       ///< Size of 'buffer' array (number of array elements).
             );

    /// Reads audio samples from the WAV file to floating point format, converting 
    /// sample values to range [-1,1[. Reads given number of elements from the file
    /// or if end-of-file reached, as many elements as are left in the file.
    /// Notice that reading in float format supports 8/16/24/32bit sample formats.
    ///
    /// \return Number of elements read from the file.
    int read(float *buffer,     ///< Pointer to buffer where to read data.
             int maxElems       ///< Size of 'buffer' array (number of array elements).
             );

    /// Check end-of-file.
    ///
    /// \return Nonzero if end-of-file reached.
    int eof();
};


/// Class for writing WAV audio files.
class WavOutFile : protected WavFileBase
{
private:
    /// Pointer to the WAV file
    FILE *fptr;

    /// WAV file header data.
    WavHeader header;

    /// Counter of how many bytes have been written to the file so far.
    int bytesWritten;

    /// Fills in WAV file header information.
    void fillInHeader(const uint sampleRate, const uint bits, const uint channels);

    /// Finishes the WAV file header by supplementing information of amount of
    /// data written to file etc
    void finishHeader();

    /// Writes the WAV file header.
    void writeHeader();

public:
    /// Constructor: Creates a new WAV file. Throws a 'runtime_error' exception 
    /// if file creation fails.
    WavOutFile(const char *fileName,    ///< Filename
               int sampleRate,          ///< Sample rate (e.g. 44100 etc)
               int bits,                ///< Bits per sample (8 or 16 bits)
               int channels             ///< Number of channels (1=mono, 2=stereo)
               );

    WavOutFile(FILE *file, int sampleRate, int bits, int channels);

    /// Destructor: Finalizes & closes the WAV file.
    ~WavOutFile();

    /// Write data to WAV file. This function works only with 8bit samples. 
    /// Throws a 'runtime_error' exception if writing to file fails.
    void write(const unsigned char *buffer, ///< Pointer to sample data buffer.
               int numElems                 ///< How many array items are to be written to file.
               );

    /// Write data to WAV file. Throws a 'runtime_error' exception if writing to
    /// file fails.
    void write(const short *buffer,     ///< Pointer to sample data buffer.
               int numElems             ///< How many array items are to be written to file.
               );

    /// Write data to WAV file in floating point format, saturating sample values to range
    /// [-1..+1[. Throws a 'runtime_error' exception if writing to file fails.
    void write(const float *buffer,     ///< Pointer to sample data buffer.
               int numElems             ///< How many array items are to be written to file.
               );
};

 ////////////////////////////////////////////////////////////////////////////////
///
/// Classes for easy reading & writing of WAV sound files. 
///
/// For big-endian CPU, define _BIG_ENDIAN_ during compile-time to correctly
/// parse the WAV files with such processors.
/// 
/// Admittingly, more complete WAV reader routines may exist in public domain,
/// but the reason for 'yet another' one is that those generic WAV reader 
/// libraries are exhaustingly large and cumbersome! Wanted to have something
/// simpler here, i.e. something that's not already larger than rest of the
/// SoundTouch/SoundStretch program...
///
/// Author        : Copyright (c) Olli Parviainen
/// Author e-mail : oparviai 'at' iki.fi
/// SoundTouch WWW: http://www.surina.net/soundtouch
///
////////////////////////////////////////////////////////////////////////////////
//
// License :
//
//  SoundTouch audio processing library
//  Copyright (c) Olli Parviainen
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string>
#include <sstream>
#include <cstring>
#include <assert.h>
#include <limits.h>

using namespace std;

static const char riffStr[] = "RIFF";
static const char waveStr[] = "WAVE";
static const char fmtStr[]  = "fmt ";
static const char factStr[] = "fact";
static const char dataStr[] = "data";

//////////////////////////////////////////////////////////////////////////////
//
// Helper functions for swapping byte order to correctly read/write WAV files 
// with big-endian CPU's: Define compile-time definition _BIG_ENDIAN_ to
// turn-on the conversion if it appears necessary. 
//
// For example, Intel x86 is little-endian and doesn't require conversion,
// while PowerPC of Mac's and many other RISC cpu's are big-endian.

#ifdef BYTE_ORDER
    // In gcc compiler detect the byte order automatically
    #if BYTE_ORDER == BIG_ENDIAN
        // big-endian platform.
        #define _BIG_ENDIAN_
    #endif
#endif
    
#ifdef _BIG_ENDIAN_
    // big-endian CPU, swap bytes in 16 & 32 bit words

    // helper-function to swap byte-order of 32bit integer
    static inline int _swap32(int &dwData)
    {
        dwData = ((dwData >> 24) & 0x000000FF) | 
               ((dwData >> 8)  & 0x0000FF00) | 
               ((dwData << 8)  & 0x00FF0000) | 
               ((dwData << 24) & 0xFF000000);
        return dwData;
    }   

    // helper-function to swap byte-order of 16bit integer
    static inline short _swap16(short &wData)
    {
        wData = ((wData >> 8) & 0x00FF) | 
                ((wData << 8) & 0xFF00);
        return wData;
    }

    // helper-function to swap byte-order of buffer of 16bit integers
    static inline void _swap16Buffer(short *pData, int numWords)
    {
        int i;

        for (i = 0; i < numWords; i ++)
        {
            pData[i] = _swap16(pData[i]);
        }
    }

#else   // BIG_ENDIAN
    // little-endian CPU, WAV file is ok as such

    // dummy helper-function
    static inline int _swap32(int &dwData)
    {
        // do nothing
        return dwData;
    }   

    // dummy helper-function
    static inline short _swap16(short &wData)
    {
        // do nothing
        return wData;
    }

    // dummy helper-function
    static inline void _swap16Buffer(short *pData, int numBytes)
    {
        // do nothing
    }

#endif  // BIG_ENDIAN


//////////////////////////////////////////////////////////////////////////////
//
// Class WavFileBase
//

WavFileBase::WavFileBase()
{
    convBuff = NULL;
    convBuffSize = 0;
}


WavFileBase::~WavFileBase()
{
    delete[] convBuff;
    convBuffSize = 0;
}


/// Get pointer to conversion buffer of at min. given size
void *WavFileBase::getConvBuffer(int sizeBytes)
{
    if (convBuffSize < sizeBytes)
    {
        delete[] convBuff;

        convBuffSize = (sizeBytes + 15) & -8;   // round up to following 8-byte bounday
        convBuff = new char[convBuffSize];
    }
    return convBuff;
}


//////////////////////////////////////////////////////////////////////////////
//
// Class WavInFile
//

WavInFile::WavInFile(const char *fileName)
{
    // Try to open the file for reading
    file = SD.open(fileName);
    if (!file) 
    {
        // didn't succeed
        String msg = "Error : Unable to open file \"";
        msg += fileName;
        msg += "\" for reading.";
        Serial.println(msg);
    }

    init();
}


WavInFile::WavInFile(File file)
{
    // Try to open the file for reading
    this->file = file;
    if (!file) 
    {
        // didn't succeed
        String msg = "Error : Unable to access input stream for reading";
        Serial.println(msg);
    }

    init();
}


/// Init the WAV file stream
void WavInFile::init()
{
    int hdrsOk;

    // assume file stream is already open
    assert(file);

    // Read the file headers
    hdrsOk = readWavHeaders();
    Serial.println(hdrsOk);
    if (hdrsOk != 0) 
    {
        // Something didn't match in the wav file headers 
    }

    // sanity check for format parameters
    if ((header.format.channel_number < 1)  || (header.format.channel_number > 9) ||
        (header.format.sample_rate < 4000)  || (header.format.sample_rate > 192000) ||
        (header.format.byte_per_sample < 1) || (header.format.byte_per_sample > 320) ||
        (header.format.bits_per_sample < 8) || (header.format.bits_per_sample > 32))
    {
        // Something didn't match in the wav file headers 
    }

    dataRead = 0;
}


WavInFile::~WavInFile()
{
    if (file) file.close();
    //file = NULL;
}


void WavInFile::rewind()
{
    int hdrsOk;
    file.seek(0);
    hdrsOk = readWavHeaders();
    assert(hdrsOk == 0);
    dataRead = 0;
}


int WavInFile::checkCharTags() const
{
    // header.format.fmt should equal to 'fmt '
    if (memcmp(fmtStr, header.format.fmt, 4) != 0) return -1;
    // header.data.data_field should equal to 'data'
    if (memcmp(dataStr, header.data.data_field, 4) != 0) return -1;

    return 0;
}


int WavInFile::read(unsigned char *buffer, int maxElems)
{
    int numBytes;
    uint afterDataRead;

    // ensure it's 8 bit format
    if (header.format.bits_per_sample != 8)
    {
    }
    assert(sizeof(char) == 1);

    numBytes = maxElems;
    afterDataRead = dataRead + numBytes;
    if (afterDataRead > header.data.data_len) 
    {
        // Don't read more samples than are marked available in header
        numBytes = (int)header.data.data_len - (int)dataRead;
        assert(numBytes >= 0);
    }

    assert(buffer);
    numBytes = (int)file.read(buffer, numBytes);
    dataRead += numBytes;

    return numBytes;
}


int WavInFile::read(short *buffer, int maxElems)
{
    unsigned int afterDataRead;
    int numBytes;
    int numElems = 0;

    assert(buffer);
    switch (header.format.bits_per_sample)
    {
        case 8:
        {
            // 8 bit format
            unsigned char *temp = (unsigned char*)getConvBuffer(maxElems);
            int i;

            numElems = read(temp, maxElems);
            // convert from 8 to 16 bit
            for (i = 0; i < numElems; i ++)
            {
                buffer[i] = (short)(((short)temp[i] - 128) * 256);
            }
            break;
        }

        case 16:
        {
            // 16 bit format

            assert(sizeof(short) == 2);

            numBytes = maxElems * 2;
            afterDataRead = dataRead + numBytes;
            if (afterDataRead > header.data.data_len) 
            {
                // Don't read more samples than are marked available in header
                numBytes = (int)header.data.data_len - (int)dataRead;
                assert(numBytes >= 0);
            }

            numBytes = (int)file.read(buffer, numBytes);
            dataRead += numBytes;
            numElems = numBytes / 2;

            // 16bit samples, swap byte order if necessary
            _swap16Buffer((short *)buffer, numElems);
            break;
        }

        default:
        {
            stringstream ss;
            ss << "\nOnly 8/16 bit sample WAV files supported in integer compilation. Can't open WAV file with ";
            ss << (int)header.format.bits_per_sample;
            ss << " bit sample format. ";
        }
    };

    return numElems;
}


/// Read data in float format. Notice that when reading in float format 
/// 8/16/24/32 bit sample formats are supported
int WavInFile::read(float *buffer, int maxElems)
{
    unsigned int afterDataRead;
    int numBytes;
    int numElems;
    int bytesPerSample;

    assert(buffer);

    bytesPerSample = header.format.bits_per_sample / 8;
    if ((bytesPerSample < 1) || (bytesPerSample > 4))
    {
        stringstream ss;
        ss << "\nOnly 8/16/24/32 bit sample WAV files supported. Can't open WAV file with ";
        ss << (int)header.format.bits_per_sample;
        ss << " bit sample format. ";
    }

    numBytes = maxElems * bytesPerSample;
    afterDataRead = dataRead + numBytes;
    if (afterDataRead > header.data.data_len) 
    {
        // Don't read more samples than are marked available in header
        numBytes = (int)header.data.data_len - (int)dataRead;
        assert(numBytes >= 0);
    }

    // read raw data into temporary buffer
    char *temp = (char*)getConvBuffer(numBytes);
    //char *temp = new char[numBytes];
    numBytes = (int)file.read(temp, numBytes);
    dataRead += numBytes;

    numElems = numBytes / bytesPerSample;

    // swap byte ordert & convert to float, depending on sample format
    switch (bytesPerSample)
    {
        case 1:
        {
            unsigned char *temp2 = (unsigned char*)temp;
            double conv = 1.0 / 128.0;
            for (int i = 0; i < numElems; i ++)
            {
                buffer[i] = (float)(temp2[i] * conv - 1.0);
            }
            break;
        }

        case 2:
        {
            short *temp2 = (short*)temp;
            double conv = 1.0 / 32768.0;
            for (int i = 0; i < numElems; i ++)
            {
                short value = temp2[i];
                buffer[i] = (float)(_swap16(value) * conv);
            }
            break;
        }

        case 3:
        {
            char *temp2 = (char *)temp;
            double conv = 1.0 / 8388608.0;
            for (int i = 0; i < numElems; i ++)
            {
                int value = temp2[i*3] << 16 | temp2[i*3 + 1] << 8 | temp2[i*3 + 2]; //*((int*)temp2);
                value = _swap32(value) & 0x00ffffff;             // take 24 bits
                value |= (value & 0x00800000) ? 0xff000000 : 0;  // extend minus sign bits
                buffer[i] = (float)(value * conv);
                //temp2 += 3;
            }
            break;
        }

        case 4:
        {
            int *temp2 = (int *)temp;
            double conv = 1.0 / 2147483648.0;
            assert(sizeof(int) == 4);
            for (int i = 0; i < numElems; i ++)
            {
                int value = temp2[i];
                buffer[i] = (float)(_swap32(value) * conv);
            }
            break;
        }
    }
    //delete[] temp;
    return numElems;
}


int WavInFile::eof()
{
    // return true if all data has been read or file eof has reached
    return (dataRead == header.data.data_len || !file.available());
}


// test if character code is between a white space ' ' and little 'z'
static int isAlpha(char c)
{
    return (c >= ' ' && c <= 'z') ? 1 : 0;
}


// test if all characters are between a white space ' ' and little 'z'
static int isAlphaStr(const char *str)
{
    char c;

    c = str[0];
    while (c) 
    {
        if (isAlpha(c) == 0) return 0;
        str ++;
        c = str[0];
    }

    return 1;
}


int WavInFile::readRIFFBlock()
{
    if (file.read(&(header.riff), sizeof(WavRiff)) != 12) return -1;

    // swap 32bit data byte order if necessary
    _swap32((int &)header.riff.package_len);

    // header.riff.riff_char should equal to 'RIFF');
    if (memcmp(riffStr, header.riff.riff_char, 4) != 0) return -1;
    // header.riff.wave should equal to 'WAVE'
    if (memcmp(waveStr, header.riff.wave, 4) != 0) return -1;

    return 0;
}


int WavInFile::readHeaderBlock()
{
    char label[5];
    string sLabel;

    // lead label string
    if (file.read(label, 4) !=4) return -1;
    label[4] = 0;

    if (isAlphaStr(label) == 0) return -1;    // not a valid label

    // Decode blocks according to their label
    if (strcmp(label, fmtStr) == 0)
    {
        int nLen, nDump;

        // 'fmt ' block 
        memcpy(header.format.fmt, fmtStr, 4);

        // read length of the format field
        if (file.read(&nLen, sizeof(int)) != 4) return -1;
        // swap byte order if necessary
        _swap32(nLen);

        // calculate how much length differs from expected 
        nDump = nLen - ((int)sizeof(header.format) - 8);

        // verify that header length isn't smaller than expected structure
        if ((nLen < 0) || (nDump < 0)) return -1;

        header.format.format_len = nLen;

        // if format_len is larger than expected, read only as much data as we've space for
        if (nDump > 0)
        {
            nLen = sizeof(header.format) - 8;
        }

        // read data
        if (file.read(&(header.format.fixed), nLen) != nLen) return -1;

        // swap byte order if necessary
        _swap16((short &)header.format.fixed);            // short int fixed;
        _swap16((short &)header.format.channel_number);   // short int channel_number;
        _swap32((int &)header.format.sample_rate);        // int sample_rate;
        _swap32((int &)header.format.byte_rate);          // int byte_rate;
        _swap16((short &)header.format.byte_per_sample);  // short int byte_per_sample;
        _swap16((short &)header.format.bits_per_sample);  // short int bits_per_sample;

        // if format_len is larger than expected, skip the extra data
        if (nDump > 0)
        {
            file.seek(file.position() + nDump);
        }

        return 0;
    }
    else if (strcmp(label, factStr) == 0)
    {
        int nLen, nDump;

        // 'fact' block 
        memcpy(header.fact.fact_field, factStr, 4);

        // read length of the fact field
        if (file.read(&nLen, sizeof(int)) != 4) return -1;
        // swap byte order if necessary
        _swap32(nLen);

        // calculate how much length differs from expected
        nDump = nLen - ((int)sizeof(header.fact) - 8);

        // verify that fact length isn't smaller than expected structure
        if ((nLen < 0) || (nDump < 0)) return -1;

        header.fact.fact_len = nLen;

        // if format_len is larger than expected, read only as much data as we've space for
        if (nDump > 0)
        {
            nLen = sizeof(header.fact) - 8;
        }

        // read data
        if (file.read(&(header.fact.fact_sample_len), nLen) != nLen) return -1;

        // swap byte order if necessary
        _swap32((int &)header.fact.fact_sample_len);    // int sample_length;

        // if fact_len is larger than expected, skip the extra data
        if (nDump > 0)
        {
            file.seek(file.position() + nDump);
        }

        return 0;
    }
    else if (strcmp(label, dataStr) == 0)
    {
        // 'data' block
        memcpy(header.data.data_field, dataStr, 4);
        if (file.read(&(header.data.data_len), sizeof(uint)) != 4) return -1;

        // swap byte order if necessary
        _swap32((int &)header.data.data_len);

        return 1;
    }
    else
    {
        uint len, i;
        uint temp;
        // unknown block

        // read length
        if (file.read(&len, sizeof(len)) != 4) return -1;
        // scan through the block
        for (i = 0; i < len; i ++)
        {
            if (file.read(&temp, 1) != 1) return -1;
            if (!file.available()) return -1;   // unexpected eof
        }
    }
    return 0;
}


int WavInFile::readWavHeaders()
{
    int res;

    memset(&header, 0, sizeof(header));

    Serial.println("RIFF");
    res = readRIFFBlock();
    if (res) return 1;
    // read header blocks until data block is found
    do
    {
        // read header blocks
        Serial.println("HEADER");
        res = readHeaderBlock();
        if (res < 0) return 1;  // error in file structure
    } while (res == 0);
    // check that all required tags are legal
    Serial.println("CHAR TAGS");
    return checkCharTags();
}


uint WavInFile::getNumChannels() const
{
    return header.format.channel_number;
}


uint WavInFile::getNumBits() const
{
    return header.format.bits_per_sample;
}


uint WavInFile::getBytesPerSample() const
{
    return getNumChannels() * getNumBits() / 8;
}


uint WavInFile::getSampleRate() const
{
    return header.format.sample_rate;
}


uint WavInFile::getDataSizeInBytes() const
{
    return header.data.data_len;
}


uint WavInFile::getNumSamples() const
{
    if (header.format.byte_per_sample == 0) return 0;
    if (header.format.fixed > 1) return header.fact.fact_sample_len;
    return header.data.data_len / (unsigned short)header.format.byte_per_sample;
}


uint WavInFile::getLengthMS() const
{
    double numSamples;
    double sampleRate;

    numSamples = (double)getNumSamples();
    sampleRate = (double)getSampleRate();

    return (uint)(1000.0 * numSamples / sampleRate + 0.5);
}


/// Returns how many milliseconds of audio have so far been read from the file
uint WavInFile::getElapsedMS() const
{
    return (uint)(1000.0 * (double)dataRead / (double)header.format.byte_rate);
}


//////////////////////////////////////////////////////////////////////////////
//
// Class WavOutFile
//

WavOutFile::WavOutFile(const char *fileName, int sampleRate, int bits, int channels)
{
    bytesWritten = 0;
    fptr = fopen(fileName, "wb");
    if (fptr == NULL) 
    {
        string msg = "Error : Unable to open file \"";
        msg += fileName;
        msg += "\" for writing.";
        //pmsg = msg.c_str;
    }

    fillInHeader(sampleRate, bits, channels);
    writeHeader();
}


WavOutFile::WavOutFile(FILE *file, int sampleRate, int bits, int channels)
{
    bytesWritten = 0;
    fptr = file;
    if (fptr == NULL) 
    {
        string msg = "Error : Unable to access output file stream.";
    }

    fillInHeader(sampleRate, bits, channels);
    writeHeader();
}


WavOutFile::~WavOutFile()
{
    finishHeader();
    if (fptr) fclose(fptr);
    fptr = NULL;
}


void WavOutFile::fillInHeader(uint sampleRate, uint bits, uint channels)
{
    // fill in the 'riff' part..

    // copy string 'RIFF' to riff_char
    memcpy(&(header.riff.riff_char), riffStr, 4);
    // package_len unknown so far
    header.riff.package_len = 0;
    // copy string 'WAVE' to wave
    memcpy(&(header.riff.wave), waveStr, 4);

    // fill in the 'format' part..

    // copy string 'fmt ' to fmt
    memcpy(&(header.format.fmt), fmtStr, 4);

    header.format.format_len = 0x10;
    header.format.fixed = 1;
    header.format.channel_number = (short)channels;
    header.format.sample_rate = (int)sampleRate;
    header.format.bits_per_sample = (short)bits;
    header.format.byte_per_sample = (short)(bits * channels / 8);
    header.format.byte_rate = header.format.byte_per_sample * (int)sampleRate;
    header.format.sample_rate = (int)sampleRate;

    // fill in the 'fact' part...
    memcpy(&(header.fact.fact_field), factStr, 4);
    header.fact.fact_len = 4;
    header.fact.fact_sample_len = 0;

    // fill in the 'data' part..

    // copy string 'data' to data_field
    memcpy(&(header.data.data_field), dataStr, 4);
    // data_len unknown so far
    header.data.data_len = 0;
}


void WavOutFile::finishHeader()
{
    // supplement the file length into the header structure
    header.riff.package_len = bytesWritten + sizeof(WavHeader) - sizeof(WavRiff) + 4;
    header.data.data_len = bytesWritten;
    header.fact.fact_sample_len = bytesWritten / header.format.byte_per_sample; 
    
    writeHeader();
}


void WavOutFile::writeHeader()
{
    WavHeader hdrTemp;
    int res;

    // swap byte order if necessary
    hdrTemp = header;
    _swap32((int &)hdrTemp.riff.package_len);
    _swap32((int &)hdrTemp.format.format_len);
    _swap16((short &)hdrTemp.format.fixed);
    _swap16((short &)hdrTemp.format.channel_number);
    _swap32((int &)hdrTemp.format.sample_rate);
    _swap32((int &)hdrTemp.format.byte_rate);
    _swap16((short &)hdrTemp.format.byte_per_sample);
    _swap16((short &)hdrTemp.format.bits_per_sample);
    _swap32((int &)hdrTemp.data.data_len);
    _swap32((int &)hdrTemp.fact.fact_len);
    _swap32((int &)hdrTemp.fact.fact_sample_len);
    
    // write the supplemented header in the beginning of the file
    fseek(fptr, 0, SEEK_SET);
    res = (int)fwrite(&hdrTemp, sizeof(hdrTemp), 1, fptr);
    if (res != 1)
    {
    }

    // jump back to the end of the file
    fseek(fptr, 0, SEEK_END);
}


void WavOutFile::write(const unsigned char *buffer, int numElems)
{
    int res;

    if (header.format.bits_per_sample != 8)
    {
       
    }
    assert(sizeof(char) == 1);

    res = (int)fwrite(buffer, 1, numElems, fptr);
    if (res != numElems) 
    {
    }

    bytesWritten += numElems;
}


void WavOutFile::write(const short *buffer, int numElems)
{
    int res;

    // 16 bit samples
    if (numElems < 1) return;   // nothing to do

    switch (header.format.bits_per_sample)
    {
        case 8:
        {
            int i;
            unsigned char *temp = (unsigned char *)getConvBuffer(numElems);
            // convert from 16bit format to 8bit format
            for (i = 0; i < numElems; i ++)
            {
                temp[i] = (unsigned char)(buffer[i] / 256 + 128);
            }
            // write in 8bit format
            write(temp, numElems);
            break;
        }

        case 16:
        {
            // 16bit format

            // use temp buffer to swap byte order if necessary
            short *pTemp = (short *)getConvBuffer(numElems * sizeof(short));
            memcpy(pTemp, buffer, numElems * 2);
            _swap16Buffer(pTemp, numElems);

            res = (int)fwrite(pTemp, 2, numElems, fptr);

            if (res != numElems) 
            {
            }
            bytesWritten += 2 * numElems;
            break;
        }

        default:
        {
            stringstream ss;
            ss << "\nOnly 8/16 bit sample WAV files supported in integer compilation. Can't open WAV file with ";
            ss << (int)header.format.bits_per_sample;
            ss << " bit sample format. ";
        }
    }
}


/// Convert from float to integer and saturate
inline int saturate(float fvalue, float minval, float maxval)
{
    if (fvalue > maxval) 
    {
        fvalue = maxval;
    } 
    else if (fvalue < minval)
    {
        fvalue = minval;
    }
    return (int)fvalue;
}


void WavOutFile::write(const float *buffer, int numElems)
{
    int numBytes;
    int bytesPerSample;

    if (numElems == 0) return;

    bytesPerSample = header.format.bits_per_sample / 8;
    numBytes = numElems * bytesPerSample;
    void *temp = getConvBuffer(numBytes + 7);   // round bit up to avoid buffer overrun with 24bit-value assignment

    switch (bytesPerSample)
    {
        case 1:
        {
            unsigned char *temp2 = (unsigned char *)temp;
            for (int i = 0; i < numElems; i ++)
            {
                temp2[i] = (unsigned char)saturate(buffer[i] * 128.0f + 128.0f, 0.0f, 255.0f);
            }
            break;
        }

        case 2:
        {
            short *temp2 = (short *)temp;
            for (int i = 0; i < numElems; i ++)
            {
                short value = (short)saturate(buffer[i] * 32768.0f, -32768.0f, 32767.0f);
                temp2[i] = _swap16(value);
            }
            break;
        }

        case 3:
        {
            char *temp2 = (char *)temp;
            for (int i = 0; i < numElems; i ++)
            {
                int value = saturate(buffer[i] * 8388608.0f, -8388608.0f, 8388607.0f);
                *((int*)temp2) = _swap32(value);
                temp2 += 3;
            }
            break;
        }

        case 4:
        {
            int *temp2 = (int *)temp;
            for (int i = 0; i < numElems; i ++)
            {
                int value = saturate(buffer[i] * 2147483648.0f, -2147483648.0f, 2147483647.0f);
                temp2[i] = _swap32(value);
            }
            break;
        }

        default:
            assert(false);
    }

    int res = (int)fwrite(temp, 1, numBytes, fptr);

    if (res != numBytes) 
    {
    }
    bytesWritten += numBytes;
}


#endif
