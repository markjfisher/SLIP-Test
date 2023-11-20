﻿#pragma once

#include <vector>
#include <cstdint>
#include <array>

#include "Request.h"
#include "Response.h"

class ReadRequest : public Request
{
public:
	ReadRequest(uint8_t request_sequence_number, uint8_t sp_unit);
	std::vector<uint8_t> serialize() const override;
	std::unique_ptr<Response> deserialize(const std::vector<uint8_t>& data) const override;

	const std::array<uint8_t, 2>& get_byte_count() const;
	void set_byte_count_from_ptr(const uint8_t* ptr, size_t offset);

	const std::array<uint8_t, 3>& get_address() const;
	void set_address_from_ptr(const uint8_t * ptr, size_t offset);


private:
	std::array<uint8_t, 2> byte_count_;
	std::array<uint8_t, 3> address_;
};
