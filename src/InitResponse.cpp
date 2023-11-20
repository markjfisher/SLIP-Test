#include "InitResponse.h"

InitResponse::InitResponse(const uint8_t request_sequence_number, const uint8_t status) : Response(request_sequence_number, status) {}

std::vector<uint8_t> InitResponse::serialize() const
{
	std::vector<uint8_t> data;
	data.push_back(this->get_request_sequence_number());
	data.push_back(this->get_status());
	return data;
}