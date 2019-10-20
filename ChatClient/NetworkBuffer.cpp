
#include "NetworkBuffer.hpp"
#include <algorithm>

NetworkBuffer::NetworkBuffer(std::size_t size)
{
	for (std::size_t i = 0; i < size; ++i)
		_buffer.push_back(0u);
	readIndex = 0;
	writeIndex = 0;
}

NetworkBuffer::NetworkBuffer(std::size_t size, char* data)
{
	for (std::size_t i = 0; i < size; ++i)
		_buffer.push_back(data[i]);
	readIndex = 0;
	writeIndex = size;
}

void NetworkBuffer::Clear()
{
	std::size_t size = _buffer.size();
	_buffer.clear();
	for (std::size_t i = 0; i < size; ++i)
		_buffer.push_back(0u);
	readIndex = 0;
	writeIndex = 0;
}

char* NetworkBuffer::Data()
{
	char* arr = new char[_buffer.size()];
	for (std::size_t i = 0; i < _buffer.size(); ++i)
	{
		arr[i] = (char)_buffer[i];
	}
	return arr;
}

void NetworkBuffer::writeUInt32LE(std::size_t index, uint32_t value)
{
	char A = value >> 24;
	char B = value >> 16;
	char C = value >> 8;
	char D = value >> 0;

	_buffer[index + 0] = A;
	_buffer[index + 1] = B;
	_buffer[index + 2] = C;
	_buffer[index + 3] = D;
}

void NetworkBuffer::writeUInt32LE(uint32_t value)
{
	char A = value >> 24;
	char B = value >> 16;
	char C = value >> 8;
	char D = value >> 0;

	_buffer[writeIndex++] = A;
	_buffer[writeIndex++] = B;
	_buffer[writeIndex++] = C;
	_buffer[writeIndex++] = D;
}

void NetworkBuffer::writeString(std::size_t index, std::string value)
{
	for (std::size_t i = 0; i < value.length(); ++i)
	{
		_buffer[index++] = value[i];
	}
}

void NetworkBuffer::writeString(std::string value)
{
	for (std::size_t i = 0; i < value.length(); ++i)
	{
		_buffer[writeIndex++] = value[i];
	}
}

uint32_t NetworkBuffer::readUInt32LE(std::size_t index)
{
	uint32_t swapped = 0;

	swapped |= _buffer[index + 3] << 24;
	swapped |= _buffer[index + 2] << 16;
	swapped |= _buffer[index + 1] << 8;
	swapped |= _buffer[index + 0] << 0;

	return swapped;
}

std::uint32_t NetworkBuffer::readUInt32LE()
{
	uint32_t swapped = 0;

	swapped |= _buffer[readIndex++] << 0;
	swapped |= _buffer[readIndex++] << 8;
	swapped |= _buffer[readIndex++] << 16;
	swapped |= _buffer[readIndex++] << 24;

	return swapped;
}

std::string NetworkBuffer::readString(std::size_t index, std::size_t length)
{
	std::string s = "";
	for (std::size_t i = index + length; i > index; --i)
	{
		s += _buffer[i];
	}
	return s;
}

std::string NetworkBuffer::readString(std::size_t length)
{
	std::string s = "";
	readIndex += length - 1;
	for (std::size_t i = 0; i < length; ++i)
	{
		s += _buffer[readIndex--];
	}
	readIndex += length + 1;
	return s;
}

//Big Endian

void NetworkBuffer::writeUInt32BE(std::size_t index, uint32_t value)
{
	char A = value >> 0;
	char B = value >> 8;
	char C = value >> 16;
	char D = value >> 24;

	_buffer[index + 0] = A;
	_buffer[index + 1] = B;
	_buffer[index + 2] = C;
	_buffer[index + 3] = D;
}

void NetworkBuffer::writeUInt32BE(uint32_t value)
{
	char A = value >> 0;
	char B = value >> 8;
	char C = value >> 16;
	char D = value >> 24;

	_buffer[writeIndex++] = A;
	_buffer[writeIndex++] = B;
	_buffer[writeIndex++] = C;
	_buffer[writeIndex++] = D;
}

void NetworkBuffer::writeStringBE(std::size_t index, std::string value)
{
	std::reverse(value.begin(), value.end());
	for (std::size_t i = 0; i < value.length(); ++i)
	{
		_buffer[index++] = value[i];
	}
}

void NetworkBuffer::writeStringBE(std::string value)
{
	std::reverse(value.begin(), value.end());
	for (std::size_t i = 0; i < value.length(); ++i)
	{
		_buffer[writeIndex++] = value[i];
	}
}

uint32_t NetworkBuffer::readUInt32BE(std::size_t index)
{
	uint32_t swapped = 0;

	swapped |= _buffer[index + 3] << 0;
	swapped |= _buffer[index + 2] << 8;
	swapped |= _buffer[index + 1] << 16;
	swapped |= _buffer[index + 0] << 24;

	return swapped;
}

std::uint32_t NetworkBuffer::readUInt32BE()
{
	uint32_t swapped = 0;

	swapped |= _buffer[readIndex++] << 24;
	swapped |= _buffer[readIndex++] << 16;
	swapped |= _buffer[readIndex++] << 8;
	swapped |= _buffer[readIndex++] << 0;

	return swapped;
}

std::string NetworkBuffer::readStringBE(std::size_t index, std::size_t length)
{
	std::string s = "";
	for (std::size_t i = index + length; i > index; --i)
	{
		s += _buffer[i];
	}
	return s;
}

std::string NetworkBuffer::readStringBE(std::size_t length)
{
	std::string s = "";
	readIndex += length - 1;
	for (std::size_t i = 0; i < length; ++i)
	{
		s += _buffer[readIndex--];
	}
	readIndex += length + 1;
	return s;
}