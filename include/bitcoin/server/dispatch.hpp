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
#ifndef LIBBITCOIN_SERVER_DISPATCH_HPP
#define LIBBITCOIN_SERVER_DISPATCH_HPP

#include <iostream>
#include <bitcoin/server/define.hpp>

namespace libbitcoin {
namespace server {

/**
 * Console return codes, positive values are domain-specific.
 */
enum console_result : int
{
    failure = -1,
    okay = 0,
    not_started = 1
};

/**
 * Dispatch from the command line.
 */
console_result BCS_API dispatch(int argc, const char* argv[], std::istream&,
    std::ostream& output, std::ostream& error);

} // namespace server
} // namespace libbitcoin

#endif
