
#include "eq_guid.h"
#include "serialize_buffer.h"
#include "http/httplib.h"

//============================================================================

void EqGuid::Serialize(SerializeBuffer& buffer) const
{
	buffer.Write(guid);
}

//============================================================================

EqItemGuid::EqItemGuid()
{
	Clear();
}

EqItemGuid::EqItemGuid(EqGuid playerGuid, uint32_t itemId)
{
	Set(playerGuid, itemId);
}

EqItemGuid::EqItemGuid(std::string_view encodedString)
{
	Set(encodedString);
}

EqItemGuid::EqItemGuid(const EqItemGuid& other)
	: m_data(other.m_data)
{
}

EqItemGuid::EqItemGuid(EqItemGuid&& other) noexcept
	: m_data(std::move(other.m_data))
{
}

EqItemGuid& EqItemGuid::operator=(const EqItemGuid& other)
{
	m_data = other.m_data;
	return *this;
}

EqItemGuid& EqItemGuid::operator=(EqItemGuid&& other) noexcept
{
	if (this != &other)
		m_data = std::move(other.m_data);
	return *this;
}

void EqItemGuid::Clear()
{
	m_data.resize(GUID_LENGTH);

	// Initialize the buffer to "0000000000000000" with two null terminators.
	uint64_t* data = reinterpret_cast<uint64_t*>(&m_data[0]);
	data[0] = 0x3030303030303030;
	data[1] = 0x3030303030303030;
	m_data[16] = 0;
	m_data[17] = 0;
}

bool EqItemGuid::IsEmpty() const
{
	// Guid is empty if all 16 bytes are '0' (0x30)
	const uint64_t* data = reinterpret_cast<const uint64_t*>(&m_data[0]);

	return data[0] == 0x3030303030303030 && data[1] == 0x3030303030303030;
}

void EqItemGuid::Set(EqGuid playerGuid, uint32_t itemId)
{
	char buffer[12] = { 0 };
	memcpy(buffer, &playerGuid.guid, 8);
	memcpy(buffer + 8, &itemId, 4);

	Set(httplib::detail::base64_encode(std::string_view{ buffer, 12 }));
}

void EqItemGuid::Set(std::string_view encodedString)
{
	m_data = encodedString;
	m_data.resize(GUID_LENGTH);
	m_data[16] = 0;
	m_data[17] = 0;
}

bool EqItemGuid::IsEqual(const EqItemGuid& other) const
{
	const uint64_t* lhs = reinterpret_cast<const uint64_t*>(&m_data[0]);
	const uint64_t* rhs = reinterpret_cast<const uint64_t*>(&other.m_data[0]);

	return lhs[0] == rhs[0] && lhs[1] == rhs[1];
}

void EqItemGuid::Serialize(SerializeBuffer& buffer) const
{
	// Writes 17 bytes
	buffer.WriteString(std::string_view{ m_data }.substr(0, 16));
}

//============================================================================
