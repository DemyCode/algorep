#pragma once

#include <string>

#include "json/json.hpp"
#include "rpc/rpc.hh"

class Entry
{
public:
    using json = nlohmann::json;

    /** @brief classic constructor
     ** @param term term of the entry
     ** @param command command of the entry
     */
    Entry(int term, std::string command);

    /** @brief json serialized constructor
     ** @param serialized_json the parsed json
     */
    Entry(const json& serialized_json);

    /** @brief to parse constructor
     ** @param serialized the un-parsed string
     */
    Entry(const std::string& serialized);

    /** @brief serialize the message
     * @return the serialized message
     */
    json serialize_message() const;

    /// The term of the entry
    const int term_;
    /// The command of the entry
    const std::string command_;
};