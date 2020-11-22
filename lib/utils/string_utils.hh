#pragma once

#include <string>
#include <vector>

size_t
split_string(const std::string& str, const std::string& delimiter, std::vector<std::string>& tokens, size_t count = 0);