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
#ifndef LIBBITCOIN_SERVER_SUBSCRIBE_MANAGER_HPP
#define LIBBITCOIN_SERVER_SUBSCRIBE_MANAGER_HPP

#include <cstdint>
#include <unordered_map>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/server/config/settings.hpp>
#include <bitcoin/server/define.hpp>
#include <bitcoin/server/message.hpp>
#include <bitcoin/server/server_node.hpp>
#include <bitcoin/server/service/util.hpp>

namespace libbitcoin {
namespace server {

enum class subscribe_type
{
    address = 0,
    stealth = 1
};

class BCS_API subscribe_manager
{
public:

    subscribe_manager(server_node& node, const settings& settings);

    void subscribe(const incoming_message& request,
        queue_send_callback queue_send);
    void renew(const incoming_message& request,
        queue_send_callback queue_send);
    void submit(size_t height, const hash_digest& block_hash,
        const chain::transaction& tx);

private:
    struct subscription
    {
        binary_type prefix;
        boost::posix_time::ptime expiry_time;
        data_chunk client_origin;
        queue_send_callback queue_send;
        subscribe_type type;
    };

    typedef std::vector<subscription> subscription_list;

    code add_subscription(const incoming_message& request,
        queue_send_callback queue_send);
    void do_subscribe(const incoming_message& request,
        queue_send_callback queue_send);
    void do_renew(const incoming_message& request,
        queue_send_callback queue_send);
    void do_submit(size_t height, const hash_digest& block_hash,
        const chain::transaction& tx);
    void post_updates(const wallet::payment_address& address,
        size_t height, const hash_digest& block_hash,
        const chain::transaction& tx);
    void post_stealth_updates(uint32_t prefix, size_t height,
        const hash_digest& block_hash, const chain::transaction& tx);
    void sweep_expired();

    dispatcher dispatch_;
    subscription_list subscriptions_;
    const settings& settings_;
};

} // namespace server
} // namespace libbitcoin

#endif
