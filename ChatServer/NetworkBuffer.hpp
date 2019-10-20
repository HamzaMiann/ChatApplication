#pragma once
#include <vector>
#include <string>

/*
@author		Hamza Mian, Brandon Becker
@descr		Buffer class to convert endianness of data to big endian
*/
class NetworkBuffer
{
public:

	// Members
	size_t readIndex;
	size_t writeIndex;
	std::vector<uint8_t> _buffer;

	// Constructor
	NetworkBuffer(size_t size);
	NetworkBuffer(size_t size, char* data);

	// Misc Methods
	void Clear();
	char* Data();

	// UInt
	void writeUInt32LE(size_t index, uint32_t value);
	void writeUInt32LE(uint32_t value);
	uint32_t readUInt32LE(size_t index);
	uint32_t readUInt32LE();

	// Int
	void writeInt32LE(size_t index, int32_t value) { this->writeUInt32LE(index, value); }
	void writeInt32LE(int32_t value) { this->writeUInt32LE(value); }
	int32_t readInt32LE(size_t index) { return this->readUInt32LE(index); }
	int32_t readInt32LE() { return this->readUInt32LE(); }

	// String
	void writeString(size_t index, std::string value);
	void writeString(std::string value);
	std::string readString(size_t index, size_t length);
	std::string readString(size_t length);

	//**************************************
	//Big Endian

	// UInt
	void writeUInt32BE(size_t index, uint32_t value);
	void writeUInt32BE(uint32_t value);
	uint32_t readUInt32BE(size_t index);
	uint32_t readUInt32BE();

	// Int
	void writeInt32BE(size_t index, int32_t value) { this->writeUInt32BE(index, value); }
	void writeInt32BE(int32_t value) { this->writeUInt32BE(value); }
	int32_t readInt32BE(size_t index) { return this->readUInt32BE(index); }
	int32_t readInt32BE() { return this->readUInt32BE(); }

	// String
	void writeStringBE(size_t index, std::string value);
	void writeStringBE(std::string value);
	std::string readStringBE(size_t index, size_t length);
	std::string readStringBE(size_t length);
};