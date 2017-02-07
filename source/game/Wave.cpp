/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "Wave.h"
#include "Engine.h"
#include "utils.h"
#include "Trace.h"

Wave::Wave()
{
	memset(&header, 0, sizeof(Header));
	memset(&format, 0, sizeof(FormatChunk));
	memset(&fact, 0, sizeof(FactChunk));
	memset(&data, 0, sizeof(DataChunk));
}

Wave::Wave(const char *filename)
{
	memset(&header, 0, sizeof(Header));
	memset(&format, 0, sizeof(FormatChunk));
	memset(&fact, 0, sizeof(FactChunk));
	memset(&data, 0, sizeof(DataChunk));
	Open(filename);
}

Wave::~Wave()
{
	Close();
}

bool Wave::Open(const char *filename)
{
	Close();

	bytestream file = bytestream_from_file(filename);
	
	if(file.empty())
	{
		Trace("Failed to open wave file: ", filename);
        return false;
	}

	if(file.available() < sizeof(Header))
	{
		Trace("Failed to read wave header: ", filename);
		return false;
	}

	file.read((char*)&header, sizeof(Header));
	
	//read chunks until end of file
	while(file.available())
	{
		if(file.available() < sizeof(ChunkInfo))
		{
			Trace("Invalid wave chunk info: ", filename);
			return false;
		}

		ChunkInfo info;
		file.read(&info, sizeof(ChunkInfo));
		
		uint32_t bytesRead = 0;
		
		if(ChunkCmp(info.type, "fmt "))
		{
			if(file.available() < sizeof(FormatChunk))
			{
				Trace("Invalid format chunk size: ", filename);
				return false;
			}

			file.read(&format, sizeof(FormatChunk));
			bytesRead = sizeof(FormatChunk);
		}
		else if(ChunkCmp(info.type, "junk"))
		{
			file.ignore(info.size);
			bytesRead = info.size;
		}
		else if(ChunkCmp(info.type, "fact"))
		{
			if(file.available() < sizeof(FactChunk))
			{
				Trace("Invalid fact chunk size: ", filename);
				return false;
			}

			file.read(&fact, sizeof(FactChunk));
			bytesRead = sizeof(FactChunk);
		}
		else if(ChunkCmp(info.type, "data"))
		{
			if(file.available() < info.size)
			{
				Trace("Invalid data chunk size: ", filename);
				return false;
			}

			data.size = info.size;
			data.data.reset(new unsigned char[info.size]);

			file.read(data.data.get(), info.size);
			bytesRead = info.size;
		}
		else
		{
			// skip unsupported chunks
			file.ignore(info.size);
			bytesRead = info.size;
		}

		// skip extra data that may be appended to chunk
		if(bytesRead < info.size)
		{
			file.ignore(info.size - bytesRead);
		}

		// skip padding byte is chunk size was odd
		if(info.size % 2)
		{
			file.ignore(1);
		}
    }

	return true;
}

void Wave::Close()
{
	data.data.reset();
}

bool Wave::ChunkCmp(const char type[4], const char *str)
{
	return (type[0] == str[0]
		 && type[1] == str[1]
		 && type[2] == str[2]
		 && type[3] == str[3]);
}

//void Wave::PrintHeader()
//{
//	cout << header.AudioFormat << endl;
//	cout << header.NumOfChan << endl;
//	cout << header.SamplesPerSec << endl;
//	cout << header.bytesPerSec << endl;
//	cout << header.bitsPerSample << endl;
//	cout << header.Subchunk1Size << endl;
//	cout << header.Subchunk2Size << endl << endl;
//}

unsigned long Wave::DataSize()
{
	return data.size;
}

unsigned long Wave::Frequency()
{
	return format.SamplesPerSec;
}

unsigned short Wave::Bitrate()
{
	return format.bitsPerSample;
}

unsigned short Wave::Channels()
{
	return format.NumOfChan;
}

void *Wave::Data()
{
	return data.data.get();
}
