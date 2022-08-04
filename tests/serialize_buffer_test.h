/*	EQEMu: Everquest Server Emulator
	Copyright (C) 2001-2013 EQEMu Development Team (http://eqemulator.net)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY except by those people which sell it, which
	are required to give you total support for your newly bought product;
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR
	A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#pragma once

#include "cppunit/cpptest.h"
#include "../common/serialize_buffer.h"

class SerializeBufferTest : public Test::Suite
{
	using TestFunction = void(SerializeBufferTest::*)();

public:
	SerializeBufferTest()
	{
		TEST_ADD(SerializeBufferTest::SerializeArray)
		TEST_ADD(SerializeBufferTest::SerializeBasicTypes)
		TEST_ADD(SerializeBufferTest::SerializeBytes)
		TEST_ADD(SerializeBufferTest::SerializeComplexTypes)
		TEST_ADD(SerializeBufferTest::SerializeStrings)
	}

private:
	struct ComplexSerializableType
	{
		std::string stringValue;
		int number;
		bool flag;

		ComplexSerializableType(const std::string& stringValue_, int number_, bool flag_)
			: stringValue(stringValue_), number(number_), flag(flag_)
		{}

		void Serialize(SerializeBuffer& buffer) const
		{
			buffer.WriteString(stringValue);
			buffer.Write(number);
			buffer.Write(flag);
		}
	};


	void SerializeArray()
	{
		// Test writing array of primitive types
		{
			SerializeBuffer buffer;

			int values[10] = {
				1, 51, 21, 23, 6123, 12, 2, 365, 1235, 1
			};

			buffer.WriteArray(values, 10);

			const uint8_t rawBuffer[] = {
				0x0a, 0x00, 0x00, 0x00,
				0x01, 0x00, 0x00, 0x00,	0x33, 0x00, 0x00, 0x00,
				0x15, 0x00, 0x00, 0x00,	0x17, 0x00, 0x00, 0x00,
				0xeb, 0x17, 0x00, 0x00,	0x0c, 0x00, 0x00, 0x00,
				0x02, 0x00, 0x00, 0x00,	0x6d, 0x01, 0x00, 0x00,
				0xd3, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
			};

			TEST_ASSERT(buffer.length() == sizeof(rawBuffer));
			TEST_ASSERT(memcmp(buffer.buffer(), rawBuffer, buffer.length()) == 0);
		}

		// Test writing array of more complex types
		{
			SerializeBuffer buffer;

			ComplexSerializableType complexValues[3] = {
				{ "Hello", 42, true },
				{ "World", 15, false },
				{ "eSUN511AD87IZIubGo3T", 0x13762162, true }
			};

			buffer.WriteArray(complexValues, 3);

			const uint8_t rawBuffer[] = {
				0x03, 0x00, 0x00, 0x00,
				0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x00, 0x2a, 0x00,
				0x00, 0x00, 0x01, 0x57, 0x6f, 0x72, 0x6c, 0x64,
				0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x65, 0x53,
				0x55, 0x4e, 0x35, 0x31, 0x31, 0x41, 0x44, 0x38,
				0x37, 0x49, 0x5a, 0x49, 0x75, 0x62, 0x47, 0x6f,
				0x33, 0x54, 0x00, 0x62, 0x21, 0x76, 0x13, 0x01
			};

			TEST_ASSERT(buffer.length() == sizeof(rawBuffer));
			TEST_ASSERT(memcmp(buffer.buffer(), rawBuffer, buffer.length()) == 0);
		}
	}

	void SerializeBasicTypes()
	{
		// Test writing integer types
		{
			SerializeBuffer buffer;

			buffer.WriteUInt8(1);
			buffer.WriteUInt16(2);
			buffer.WriteUInt32(3);
			buffer.WriteUInt64(4);
			buffer.WriteInt8(-1);
			buffer.WriteInt16(-2);
			buffer.WriteInt32(-3);
			buffer.WriteInt64(-4);
			buffer.WriteBool(true);

			const uint8_t rawBuffer[] = {
				0x01, 0x02, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
				0xfe, 0xff, 0xfd, 0xff, 0xff, 0xff, 0xfc, 0xff,
				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01,
			};

			TEST_ASSERT(buffer.length() == sizeof(rawBuffer));
			TEST_ASSERT(memcmp(buffer.buffer(), rawBuffer, buffer.length()) == 0);
		}

		// Test writing floating point types
		{
			SerializeBuffer buffer;

			buffer.WriteFloat(1.0f);
			buffer.WriteDouble(2.0);

			const uint8_t rawBuffer[] = {
				0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x40
			};

			TEST_ASSERT(buffer.length() == sizeof(rawBuffer));
			TEST_ASSERT(memcmp(buffer.buffer(), rawBuffer, buffer.length()) == 0);
		}
	}

	void SerializeBytes()
	{
		SerializeBuffer buffer;

		uint8_t data[] = {
			0x8b, 0x78, 0x38, 0xbe, 0xf0, 0xfc, 0xf3, 0x02,
			0xa8, 0x55, 0x57, 0xe9, 0xea, 0x3f, 0x0f, 0xa6,
			0xa1, 0xb7, 0x11, 0x87
		};

		buffer.WriteBytes(data, sizeof(data));

		TEST_ASSERT(buffer.length() == sizeof(data));
		TEST_ASSERT(memcmp(buffer.buffer(), data, buffer.length()) == 0);
	}

	void SerializeComplexTypes()
	{
		// Serialize a complex type
		{
			SerializeBuffer buffer;

			ComplexSerializableType complexValue {
				"eSUN511AD87IZIubGo3T", 0x13762162, true
			};

			buffer.Write(complexValue);

			const uint8_t rawBuffer[] = {
				0x65, 0x53, 0x55, 0x4e, 0x35, 0x31, 0x31, 0x41,
				0x44, 0x38, 0x37, 0x49, 0x5a, 0x49, 0x75, 0x62,
				0x47, 0x6f, 0x33, 0x54, 0x00, 0x62, 0x21, 0x76, 0x13, 0x01
			};

			TEST_ASSERT(buffer.length() == sizeof(rawBuffer));
			TEST_ASSERT(memcmp(buffer.buffer(), rawBuffer, buffer.length()) == 0);
		}

		// Serialize another SerializationBuffer
		{
			SerializeBuffer buffer1;

			buffer1.WriteUInt8(1);
			buffer1.WriteUInt32(0x19283744);


			SerializeBuffer buffer2;
			buffer2.WriteLengthString("The quick brown fox jumps over the lazy dog");
			buffer2.Write(buffer1);

			const uint8_t rawBuffer[] = {
				0x2b, 0x00, 0x00, 0x00,
				0x54, 0x68, 0x65, 0x20, 0x71, 0x75, 0x69, 0x63,
				0x6b, 0x20, 0x62, 0x72, 0x6f, 0x77, 0x6e, 0x20,
				0x66, 0x6f, 0x78, 0x20, 0x6a, 0x75, 0x6d, 0x70,
				0x73, 0x20, 0x6f, 0x76, 0x65, 0x72, 0x20, 0x74,
				0x68, 0x65, 0x20, 0x6c, 0x61, 0x7a, 0x79, 0x20,
				0x64, 0x6f, 0x67,
				0x01, 0x44, 0x37, 0x28, 0x19
			};

			TEST_ASSERT(buffer2.length() == sizeof(rawBuffer));
			TEST_ASSERT(memcmp(buffer2.buffer(), rawBuffer, buffer2.length()) == 0);
		}
	}

	void SerializeStrings()
	{
		// Test writing raw string
		{
			SerializeBuffer buffer;

			const char* const testString = "0123456789";
			buffer.WriteString(testString);

			const uint8_t rawBuffer[] = {
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x00
			};

			TEST_ASSERT(buffer.length() == sizeof(rawBuffer));
			TEST_ASSERT(memcmp(buffer.buffer(), rawBuffer, buffer.length()) == 0);
		}

		// Test writing string_view
		{
			SerializeBuffer buffer;

			std::string_view testString = "0123456789";
			buffer.WriteString(testString);

			const uint8_t rawBuffer[] = {
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x00
			};

			TEST_ASSERT(buffer.length() == sizeof(rawBuffer));
			TEST_ASSERT(memcmp(buffer.buffer(), rawBuffer, buffer.length()) == 0);
		}

		// Test writing length-prefixed raw string
		{
			SerializeBuffer buffer;

			const char* const testString = "Hello World!";
			constexpr int testLength = 4;
			buffer.WriteLengthString(testLength, testString);

			const uint8_t rawBuffer[] = {
				0x04, 0x00, 0x00, 0x00, 0x48, 0x65, 0x6c, 0x6c,
			};

			TEST_ASSERT(buffer.length() == sizeof(rawBuffer));
			TEST_ASSERT(memcmp(buffer.buffer(), rawBuffer, buffer.length()) == 0);
		}

		// Test writing length-prefixed string view
		{
			SerializeBuffer buffer;

			std::string_view testString = "Hello World!";
			buffer.WriteLengthString(testString);

			const uint8_t rawBuffer[] = {
				0x0c, 0x00, 0x00, 0x00, 0x48, 0x65, 0x6c, 0x6c,
				0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21,
			};

			TEST_ASSERT(buffer.length() == sizeof(rawBuffer));
			TEST_ASSERT(memcmp(buffer.buffer(), rawBuffer, buffer.length()) == 0);
		}

		{
			SerializeBuffer buffer;

			constexpr int testLength = 4;
			std::string_view testString{ "Hello World!", testLength };
			buffer.WriteLengthString(testString);

			const uint8_t rawBuffer[] = {
				0x04, 0x00, 0x00, 0x00, 0x48, 0x65, 0x6c, 0x6c,
			};

			TEST_ASSERT(buffer.length() == sizeof(rawBuffer));
			TEST_ASSERT(memcmp(buffer.buffer(), rawBuffer, buffer.length()) == 0);
		}
	}

};
