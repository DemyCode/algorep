#include "string_utils.hh"

#include <iostream>

size_t
split_string(const std::string& str, const std::string& delimiter, std::vector<std::string>& tokens, size_t count)
{
    size_t last_position = 0;
    size_t position = 0;

    while (position < str.size())
    {
        if (count > 0 && tokens.size() >= count)
            break;

        position = str.find(delimiter, last_position);
        if (position == std::string::npos)
            position = str.size();

        if (last_position - position > 0)
            tokens.emplace_back(str.substr(last_position, position - last_position));

        last_position = position + 1;
    }

    return position;
}