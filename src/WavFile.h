#pragma once
#include <SD.h>

class WavFile
{
public:
    File file;
    uint16_t channels, audioFormat, blockAlign, bitsPerSample;
    uint32_t chunkId, chunkSize, format, subchunk1ID, subchunk1Size, sampleRate, byteRate, subchunk2ID, subchunk2Size;

    uint32_t read4bytes()
    {
        uint8_t buf[4];
        file.readBytes(buf, 4);
        uint32_t dest = 0;
        dest = ((uint32_t)buf[3]) << 24 | ((uint32_t)buf[2]) << 16 | ((uint32_t)buf[1]) << 8 | ((uint32_t)buf[0]);
        return dest;
    }

    uint16_t read2bytes()
    {
        uint8_t buf[2];
        file.readBytes(buf, 2);
        uint16_t dest = 0;
        dest = ((uint16_t)buf[1]) << 8 | ((uint16_t)buf[0]);
        return dest;
    }

    WavFile()
    {
        
    }

    void init(const char * path)
    {
        if (!SD.begin())
        {
            Serial.println("SD BEGIN FAILED");
        }
        file = SD.open(path);
        Serial.println(file.size());

        chunkId = read4bytes();
        chunkSize = read4bytes();
        format = read4bytes();

        subchunk1ID = read4bytes();
        subchunk1Size = read4bytes();
        audioFormat = read2bytes();
        channels = read2bytes();
        sampleRate = read4bytes();
        byteRate = read4bytes();
        blockAlign = read2bytes();
        bitsPerSample = read2bytes();

        subchunk2ID = read4bytes();
        subchunk2Size = read4bytes();

        Serial.printf("Audio Format = %d\r\nChannels = %d\r\nSample Rate = %d\r\nByte Rate = %d\r\nBlock Align = %d\r\nBits Per Sample = %d\r\n",
        audioFormat, channels, sampleRate, byteRate, blockAlign, bitsPerSample);
    }

    void read(uint16_t * samples, size_t count)
    {
        size_t blockCount = 0;
        for (int i = 0; i < count; i++)
        {
            if (blockCount == 4)
            {
                blockCount = 0;
                //samples
            }
            blockCount++;
        }
    }
};