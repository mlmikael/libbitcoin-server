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
#include <bitcoin/server/service/fetch_x.hpp>

#include <bitcoin/server/config/configuration.hpp>
#include <bitcoin/server/service/util.hpp>

namespace libbitcoin {
namespace server {

using namespace bc::blockchain;

// fetch_history stuff

bool unwrap_fetch_history_args(wallet::payment_address& address,
    uint32_t& from_height, const incoming_message& request)
{
    const data_chunk& data = request.data();

    if (data.size() != 1 + short_hash_size + 4)
    {
        log::error(LOG_SERVICE)
            << "Incorrect data size for .fetch_history";
        return false;
    }

    auto deserial = make_deserializer(data.begin(), data.end());
    uint8_t version_byte = deserial.read_byte();
    short_hash hash = deserial.read_short_hash();
    from_height = deserial.read_4_bytes_little_endian();
    BITCOIN_ASSERT(deserial.iterator() == data.end());

    address = wallet::payment_address(hash, version_byte);
    return true;
}

void send_history_result(const code& ec,
    const block_chain::history& history, const incoming_message& request,
    queue_send_callback queue_send)
{
    constexpr size_t row_size = 1 + 36 + 4 + 8;
    data_chunk result(4 + row_size * history.size());
    auto serial = make_serializer(result.begin());
    write_error_code(serial, ec);

    for (const block_chain::history_row& row : history)
    {
        BITCOIN_ASSERT(row.height <= max_uint32);
        auto row_height32 = static_cast<uint32_t>(row.height);

        if (row.kind == block_chain::point_kind::output)
            serial.write_byte(0);
        else // if (row.kind == point_kind::spend)
            serial.write_byte(1);

        data_chunk raw_point = row.point.to_data();
        serial.write_data(raw_point);
        serial.write_4_bytes_little_endian(row_height32);
        serial.write_8_bytes_little_endian(row.value);
    }

    BITCOIN_ASSERT(serial.iterator() == result.end());

    // TODO: Slows down queries!
    //log::debug(LOG_SERVICE)
    //    << "*.fetch_history() finished. Sending response.";

    outgoing_message response(request, result);
    queue_send(response);
}

// fetch_transaction stuff

bool unwrap_fetch_transaction_args(hash_digest& tx_hash,
    const incoming_message& request)
{
    const auto& data = request.data();

    if (data.size() != 32)
    {
        log::error(LOG_SERVICE)
            << "Incorrect data size for *.fetch_transaction";
        return false;
    }

    auto deserial = make_deserializer(data.begin(), data.end());
    tx_hash = deserial.read_hash();
    return true;
}

void transaction_fetched(const code& ec,
    const chain::transaction& tx, const incoming_message& request,
    queue_send_callback queue_send)
{
    data_chunk result(4 + tx.serialized_size());
    auto serial = make_serializer(result.begin());
    write_error_code(serial, ec);
    BITCOIN_ASSERT(serial.iterator() == result.begin() + 4);

    data_chunk tx_data = tx.to_data();
    serial.write_data(tx_data);
    BITCOIN_ASSERT(serial.iterator() == result.end());

    log::debug(LOG_REQUEST)
        << "blockchain.fetch_transaction() finished. Sending response.";

    outgoing_message response(request, result);
    queue_send(response);
}

} // namespace server
} // namespace libbitcoin
