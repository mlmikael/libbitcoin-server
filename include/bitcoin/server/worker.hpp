/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin-server.
 *
 * libbitcoin-server is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SERVER_WORKER_HPP
#define LIBBITCOIN_SERVER_WORKER_HPP

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>
#include <boost/date_time.hpp>
#include <czmq++/czmqpp.hpp>
#include <bitcoin/node.hpp>
#include <bitcoin/server/config/settings.hpp>
#include <bitcoin/server/define.hpp>
#include <bitcoin/server/message.hpp>
#include <bitcoin/server/service/util.hpp>

namespace libbitcoin {
namespace server {

/**
 * We don't want to block the originating threads that execute a send
 * as that would slow down requests if they all have to sync access
 * to a single socket.
 *
 * Instead we have a queue (push socket) where send requests are pushed,
 * and then the send_worker is notified. The worker wakes up and pushes
 * all pending requests to the socket.
 */
class BCS_API send_worker
{
public:
    send_worker(czmqpp::context& context);
    void queue_send(const outgoing_message& message);

private:
    czmqpp::context& context_;
};

// TODO: split into class per file.

class BCS_API request_worker
{
public:
    typedef std::function<void(const incoming_message&, queue_send_callback)>
        command_handler;

    request_worker(const settings& settings);

    bool start();
    bool stop();
    void update();
    void attach(const std::string& command, command_handler handler);

private:
    typedef std::unordered_map<std::string, command_handler> command_map;

    void whitelist();
    bool enable_crypto();
    bool create_new_socket();
    void poll();
    void publish_heartbeat();

    czmqpp::context context_;
    czmqpp::socket socket_;
    czmqpp::socket wakeup_socket_;
    czmqpp::socket heartbeat_socket_;
    czmqpp::authenticator authenticate_;

    send_worker sender_;
    command_map handlers_;
    boost::posix_time::ptime deadline_;
    const settings& settings_;
};

} // namespace server
} // namespace libbitcoin

#endif

