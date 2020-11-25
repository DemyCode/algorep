#pragma once

#include <string>

#include "rpc/message.hh"
#include "rpc/rpc-query.hh"

class Controller
{
public:
    /** @brief Constructor
     */
    Controller();

    /** @brief Run the controller
     */
    void run();

private:
    /** @brief print the help message
     */
    static void print_help();

    /** @brief print the list of ranks
     */
    static void list_ranks();

    /** @brief send a message to the destination_rank
     ** @param destination_rank rank to send the message to
     ** @param message_type type of the message
     ** @param command command of the message, "empty" by default
     */
    void send_message(int destination_rank,
                      const Message::MESSAGE_TYPE& message_type,
                      const std::string& command = "empty") const;

    /** @brief handle the entry command
     ** @param tokens list of parsed tokens
     ** @param line unparsed line
     */
    void handle_entry(const std::vector<std::string>& tokens, const std::string& line);

    /** @brief handle the set_speed command
     ** @param tokens list of parsed tokens
     */
    void handle_set_speed(const std::vector<std::string>& tokens);

    /** @brief handle the crash command
     ** @param tokens list of parsed tokens
     */
    void handle_crash(const std::vector<std::string>& tokens);

    /** @brief handle the start command
     ** @param tokens list of parsed tokens
     */
    void handle_start(const std::vector<std::string>& tokens);

    /** @brief handle the recover command
     ** @param tokens list of parsed tokens
     */
    void handle_recover(const std::vector<std::string>& tokens);

    /** @brief handle the stop command
     ** @param tokens list of parsed tokens
     */
    void handle_stop(const std::vector<std::string>& tokens);

    /** @brief handle the wait_finish command
     ** @param tokens list of parsed tokens
     */
    void handle_wait_finish(const std::vector<std::string>& tokens);

    /** @brief handle the get_state command
     ** @param tokens list of parsed tokens
     */
    void handle_get_state(const std::vector<std::string>& tokens) const;

    /** @brief handle the sleep command
     ** @param tokens list of parsed tokens
     */
    static void handle_sleep(const std::vector<std::string>& tokens);

    /** @brief handle the timeout command
     ** @param tokens list of parsed tokens
     */
    void handle_timeout(const std::vector<std::string>& tokens);

    /** @brief handle the print_local_log command
     ** @param tokens list of parsed tokens
     */
    void handle_print_local_log(const std::vector<std::string>& tokens);

    /// Controller timeout
    const float timeout_;
};