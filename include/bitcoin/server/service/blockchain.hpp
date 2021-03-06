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
#ifndef LIBBITCOIN_SERVER_BLOCKCHAIN_HPP
#define LIBBITCOIN_SERVER_BLOCKCHAIN_HPP

#include <bitcoin/server/define.hpp>
#include <bitcoin/server/service/util.hpp>

namespace libbitcoin {
namespace server {

class server_node;

void BCS_API blockchain_fetch_history(server_node& node,
    const incoming_message& request, queue_send_callback queue_send);

void BCS_API blockchain_fetch_transaction(server_node& node,
    const incoming_message& request, queue_send_callback queue_send);

void BCS_API blockchain_fetch_last_height(server_node& node,
    const incoming_message& request, queue_send_callback queue_send);

void BCS_API blockchain_fetch_block_header(server_node& node,
    const incoming_message& request, queue_send_callback queue_send);

////void BCS_API blockchain_fetch_block_transaction_hashes(server_node& node,
////    const incoming_message& request, queue_send_callback queue_send);

void BCS_API blockchain_fetch_transaction_index(server_node& node,
    const incoming_message& request, queue_send_callback queue_send);

void BCS_API blockchain_fetch_spend(server_node& node,
    const incoming_message& request, queue_send_callback queue_send);

void BCS_API blockchain_fetch_block_height(server_node& node,
    const incoming_message& request, queue_send_callback queue_send);

void BCS_API blockchain_fetch_stealth(server_node& node,
    const incoming_message& request, queue_send_callback queue_send);

} // namespace server
} // namespace libbitcoin

#endif

