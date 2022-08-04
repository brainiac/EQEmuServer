#pragma once

#include <cstring>
#include <cassert>
#include <cstdint>
#include <string>

namespace detail
{
	template <typename, typename T>
	struct has_serialize {
		static_assert(
			std::integral_constant<T, false>::value,
			"Second template parameter needs to be of function type.");
	};

	// specialization that does the checking
	template <typename C, typename Ret, typename... Args>
	struct has_serialize<C, Ret(Args...)> {
	private:
		template <typename T>
		static constexpr auto check_serialize(T*)
			-> typename std::is_same<
			decltype(std::declval<T>().Serialize(std::declval<Args>()...)),
			Ret>::type;

		template <typename>
		static constexpr std::false_type check_serialize(...);

		using type = decltype(check_serialize<C>(0));

	public:
		static constexpr bool value = type::value;
	};
}

class SerializeBuffer
{
public:
	SerializeBuffer() {}

	explicit SerializeBuffer(size_t size)
		: m_capacity(size)
	{
		m_buffer = new uint8_t[size];
		memset(m_buffer, 0, size);
	}

	SerializeBuffer(const SerializeBuffer& rhs)
		: m_buffer(new uint8_t[rhs.m_capacity]), m_capacity(rhs.m_capacity), m_pos(rhs.m_pos)
	{
		memcpy(m_buffer, rhs.m_buffer, rhs.m_capacity);
	}

	SerializeBuffer& operator=(const SerializeBuffer& rhs)
	{
		if (this != &rhs) {
			delete[] m_buffer;
			m_buffer = new uint8_t[rhs.m_capacity];
			m_capacity = rhs.m_capacity;
			m_pos = rhs.m_pos;
			memcpy(m_buffer, rhs.m_buffer, m_capacity);
		}
		return *this;
	}

	SerializeBuffer(SerializeBuffer&& rhs) noexcept
		: m_buffer(rhs.m_buffer), m_capacity(rhs.m_capacity), m_pos(rhs.m_pos)
	{
		rhs.m_buffer = nullptr;
		rhs.m_capacity = 0;
		rhs.m_pos = 0;
	}

	SerializeBuffer& operator=(SerializeBuffer&& rhs) noexcept
	{
		if (this != &rhs) {
			delete[] m_buffer;

			m_buffer = rhs.m_buffer;
			m_capacity = rhs.m_capacity;
			m_pos = rhs.m_pos;

			rhs.m_buffer = nullptr;
			rhs.m_capacity = 0;
			rhs.m_pos = 0;
		}
		return *this;
	}

	~SerializeBuffer() { delete[] m_buffer; }

	// Write a single pod data type
	template <typename T>
	std::enable_if_t<std::is_pod_v<T> && !detail::has_serialize<T, void(SerializeBuffer&)>::value, void>
		Write(T value)
	{
		EnsureCapacity(sizeof(T));

		*(T*)(m_buffer + m_pos) = value;
		m_pos += sizeof(T);
	}

	// Write overload enabled for objects that have a Serialize function
	// Function should have the form: void Serialize(SerializeBuffer&) const;
	template <typename T>
	std::enable_if_t<detail::has_serialize<T, void(SerializeBuffer&)>::value, void> Write(const T& obj)
	{
		obj.Serialize(*this);
	}

	void WriteUInt8(uint8_t value) { Write(value); }
	void WriteUInt16(uint16_t value) { Write(value); }
	void WriteUInt32(uint32_t value) { Write(value); }
	void WriteUInt64(uint64_t value) { Write(value); }
	void WriteInt8(int8_t value) { Write(value); }
	void WriteInt16(int16_t value) { Write(value); }
	void WriteInt32(int32_t value) { Write(value); }
	void WriteInt64(int64_t value) { Write(value); }
	void WriteFloat(float value) { Write(value); }
	void WriteDouble(double value) { Write(value); }
	void WriteBool(bool value) { Write(value); }
	void WriteByte(uint8_t value) { Write(value); }

	// Write array of pod values
	template <typename T>
	std::enable_if_t<std::is_pod_v<T> && !detail::has_serialize<T, void(SerializeBuffer&)>::value, void>
		WriteArray(const T* array, size_t elements)
	{
		size_t size = sizeof(T) * elements;
		EnsureCapacity(size + sizeof(uint32_t));

		// Write length
		*(uint32_t*)(m_buffer + m_pos) = (uint32_t)elements;
		m_pos += sizeof(uint32_t);

		memcpy(m_buffer + m_pos, array, size);
		m_pos += size;
	}

	// Write array of objects that have a Serialize function
	template <typename T>
	std::enable_if_t<detail::has_serialize<T, void(SerializeBuffer&)>::value, void>
		WriteArray(const T* array, size_t elements)
	{
		size_t size = (sizeof(T) * elements) + sizeof(uint32_t);
		EnsureCapacity(size);

		// Write length
		*(uint32_t*)(m_buffer + m_pos) = (uint32_t)elements;
		m_pos += sizeof(uint32_t);

		// Write length
		for (size_t i = 0; i < elements; ++i)
		{
			Write(array[i]);
		}
	}

	// Write out an array of bytes directly into the buffer.
	void WriteBytes(const uint8_t* data, size_t length)
	{
		EnsureCapacity(length);

		memcpy(m_buffer + m_pos, data, length);
		m_pos += length;
	}

	// Write out a null-terminated string into the buffer
	void WriteString(std::string_view str)
	{
		EnsureCapacity(str.length() + 1);

		// Write bytes
		memcpy(m_buffer + m_pos, str.data(), str.length());
		m_pos += str.length();

		// Write terminator
		m_buffer[m_pos++] = 0;
	}

	// Write a length-prefixed string into the buffer
	void WriteLengthString(std::string_view str)
	{
		WriteLengthString(str.length(), str.data());
	}

	// Write a length-prefixed string into the buffer
	void WriteLengthString(size_t len, const char* data)
	{
		EnsureCapacity(len + sizeof(uint32_t));

		// Write length
		*(uint32_t *)(m_buffer + m_pos) = (uint32_t)len;
		m_pos += sizeof(uint32_t);

		// Write bytes
		memcpy(m_buffer + m_pos, data, len);
		m_pos += len;
	}

	// Serialize another SerializeBuffer into this one.
	void Serialize(SerializeBuffer& other) const
	{
		assert(&other != this);

		other.WriteBytes(buffer(), length());
	}

	void Reset();

	size_t size() const { return m_pos; }
	size_t length() const { return size(); }
	size_t capacity() const { return m_capacity; }
	const uint8_t* buffer() const { return m_buffer; }

	friend class BasePacket;

private:
	void EnsureCapacity(size_t amount)
	{
		if (m_pos + amount > m_capacity)
			Grow(m_capacity + amount);
	}

	void Grow(size_t new_size);

	uint8_t* m_buffer = nullptr;
	size_t m_capacity = 0;
	size_t m_pos = 0;
};

inline bool operator==(const SerializeBuffer& lhs, const SerializeBuffer& rhs) {
	if (lhs.size() != rhs.size()) {
		return false;
	}

	return memcmp(lhs.buffer(), rhs.buffer(), lhs.size()) == 0;
}
