#pragma once

#include <cstdint>
#include <string_view>

class SerializeBuffer;

// An EqGuid is a unique identifier composed of the unique id of the world server that
// created it, and a self incrementing identifier for the type of object being identified.
// For example, when creating a new character, we will generate a new EqGuid with the world
// id and the next character id. These EqGuid objects are only unique within entities of the
// same type. For example, a character or guild might have the same guid, but these are
// different types of entities so it shouldn't matter.
union EqGuid
{
	uint64_t guid;

	struct
	{
		uint32_t entityId;
		uint16_t worldId;
		uint16_t reserved;
	};

	EqGuid() : guid(0) {}
	EqGuid(uint16_t worldId_, uint32_t entityId_) : EqGuid() {
        worldId = worldId_;
        entityId = entityId_;
    }

	void Serialize(SerializeBuffer& buffer) const;

	operator uint64_t() const { return guid; }
	explicit operator bool() const { return guid != 0; }
};

inline bool operator==(const EqGuid& lhs, const EqGuid& rhs) { return lhs.guid == rhs.guid; }
inline bool operator!=(const EqGuid& lhs, const EqGuid& rhs) { return lhs.guid != rhs.guid; }

class EqItemGuid
{
public:
	enum { GUID_LENGTH = 18 };

	EqItemGuid() noexcept;
	EqItemGuid(EqGuid playerGuid, uint32_t itemId) noexcept;
	EqItemGuid(std::string_view encodedString) noexcept;

	EqItemGuid(const EqItemGuid& other) noexcept;
	EqItemGuid(EqItemGuid&& other) noexcept;

	EqItemGuid& operator=(const EqItemGuid& other) noexcept;
	EqItemGuid& operator=(EqItemGuid&& other) noexcept;

	void Clear();
	bool IsEmpty() const;
	bool IsEqual(const EqItemGuid& other) const;

	void Set(EqGuid playerGuid, uint32_t itemId);
	void Set(std::string_view sv);

	// Get raw pointer to 18-byte string
	const char* GetEncodedString() const { return m_data; }

	// Serialize as null-terminated string (17-bytes)
	void Serialize(SerializeBuffer& buffer) const;

private:
	char m_data[GUID_LENGTH];
};

inline bool operator==(const EqItemGuid& lhs, const EqItemGuid& rhs) {
	return lhs.IsEqual(rhs);
}
inline bool operator!=(const EqItemGuid& lhs, const EqItemGuid& rhs) {
	return !(lhs == rhs);
}
