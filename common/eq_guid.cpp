
#include "eq_guid.h"
#include "serialize_buffer.h"
#include "http/httplib.h"

//============================================================================

static char characters[64] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
	'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
	'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
	'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
	'u', 'v', 'w', 'x', 'y', 'z', '+', '-'
};

//============================================================================

void EqGuid::Serialize(SerializeBuffer& buffer) const
{
	buffer.Write(guid);
}

//============================================================================

EqItemGuid::EqItemGuid() noexcept
{
	Clear();
}

EqItemGuid::EqItemGuid(EqGuid playerGuid, uint32_t itemId) noexcept
{
	Set(playerGuid, itemId);
}

EqItemGuid::EqItemGuid(std::string_view encodedString) noexcept
{
	Set(encodedString);
}

EqItemGuid::EqItemGuid(const EqItemGuid& other) noexcept
{
	memcpy(m_data, other.m_data, GUID_LENGTH);
}

EqItemGuid::EqItemGuid(EqItemGuid&& other) noexcept
{
	memcpy(m_data, other.m_data, GUID_LENGTH);
}

EqItemGuid& EqItemGuid::operator=(const EqItemGuid& other) noexcept
{
	if (this != &other)
		memcpy(m_data, other.m_data, GUID_LENGTH);
	return *this;
}

EqItemGuid& EqItemGuid::operator=(EqItemGuid&& other) noexcept
{
	if (this != &other)
		memcpy(m_data, other.m_data, GUID_LENGTH);
	return *this;
}

void EqItemGuid::Clear()
{
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
	uint8_t inData[12] = { 0 };
	memcpy(inData, &playerGuid.guid, 8);
	memcpy(inData + 8, &itemId, 4);

	for (int i = 0; i < 4; ++i)
	{
		int inPos = i * 3;
		int outPos = i * 4;

		// 11111111 11111111 11111111
		// 11111100                   0xfc
		//       11 11110000          0x03, 0xf0
		//              1111 11       0x0f, 0xc0
		//                     111111 0x3f
		m_data[outPos]   = characters[ (inData[inPos  ] & 0xFC) >> 2];
		m_data[outPos+1] = characters[((inData[inPos  ] & 0x03) << 4) | ((inData[inPos+1] & 0xf0) >> 4)];
		m_data[outPos+2] = characters[((inData[inPos+1] & 0x0f) << 2) | ((inData[inPos+2] & 0xc0) >> 6)];
		m_data[outPos+3] = characters[ (inData[inPos+2] & 0x3f)];
	}
}

void EqItemGuid::Set(std::string_view encodedString)
{
	if (encodedString.size() >= 16 && encodedString.size() <= 18)
	{
		memcpy(m_data, encodedString.data(), 16);
		m_data[16] = 0;
		m_data[17] = 0;
	}
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
