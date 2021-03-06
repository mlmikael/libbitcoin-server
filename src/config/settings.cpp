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
#include <bitcoin/server/config/settings.hpp>

#include <cstdint>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <bitcoin/node.hpp>
#include <bitcoin/server/config/parser.hpp>
#include <bitcoin/server/server_node.hpp>

// Define after boost asio, see stackoverflow.com/a/9750437/1172329.
#ifdef _MSC_VER
    #include <shlobj.h>
    #include <windows.h>
#else
    #include <pwd.h>
    #include <unistd.h>
#endif

namespace libbitcoin {
namespace server {

using boost::filesystem::path;
using namespace boost::program_options;
using namespace bc::config;
using namespace bc::network;

static std::string system_config_directory()
{
#ifdef _MSC_VER
    char directory[MAX_PATH];
    const auto result = SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL,
        SHGFP_TYPE_CURRENT, directory);
    if (SUCCEEDED(result))
        return directory;
    return "";
#else
    // This symbol must be defined at compile for this project.
    // Therefore do not move this definition into libbitcoin.
    return std::string(SYSCONFDIR);
#endif
}

static path default_config_path()
{
    // This subdirectory and file name must stay in sync with the path
    // for the sample distributed via the build.
    return path(system_config_directory()) / "libbitcoin" / "bs.cfg";
}

// TODO: localize descriptions.
const options_description parser::load_options()
{
    options_description description("options");
    description.add_options()
    (
        BS_CONFIG_VARIABLE,
        value<path>(&settings.file),
        "The path to the configuration settings file."
    )
    (
        BS_HELP_VARIABLE ",h",
        value<bool>(&settings.help)->default_value(false)->zero_tokens(),
        "Get list of options for this command."
    )
    (
        "mainnet,m",
        value<bool>(&settings.mainnet)->default_value(false)->zero_tokens(),
        "Initialize mainnet in the configured directory."
    )
    (
        BS_SETTINGS_VARIABLE ",s",
        value<bool>(&settings.settings)->default_value(false)->zero_tokens(),
        "Display the loaded configuration settings."
    )
    (
        "testnet,t",
        value<bool>(&settings.testnet)->default_value(false)->zero_tokens(),
        "Initialize testnet in the configured directory."
    )
    (
        BS_VERSION_VARIABLE ",v",
        value<bool>(&settings.version)->default_value(false)->zero_tokens(),
        "Get version information."
    );

    return description;
}

const positional_options_description parser::load_arguments()
{
    positional_options_description description;
    return description
        .add(BS_CONFIG_VARIABLE, 1);
}

// TODO: localize descriptions.
const options_description parser::load_environment()
{
    options_description description("environment");
    description.add_options()
    (
        // For some reason po requires this to be a lower case name.
        // The case must match the other declarations for it to compose.
        // This composes with the cmdline options and inits to system path.
        BS_CONFIG_VARIABLE,
        value<path>(&settings.file)->composing()
            ->default_value(default_config_path()),
        "The path to the configuration settings file."
    );

    return description;
}

// TODO: localize descriptions.
const options_description parser::load_settings()
{
    options_description description("settings");
    description.add_options()
    /* [network] */
    (
        "network.threads",
        value<uint32_t>(&settings.network.threads)->
            default_value(p2p::mainnet.threads),
        "The number of threads in the network threadpool, defaults to 4."
    )
    (
        "network.identifier",
        value<uint32_t>(&settings.network.identifier)->
        default_value(p2p::mainnet.identifier),
        "The magic number for message headers, defaults to 3652501241."
    )
    (
        "network.inbound_port",
        value<uint16_t>(&settings.network.inbound_port)->
            default_value(p2p::mainnet.inbound_port),
        "The port for incoming connections, defaults to 8333."
    )
    (
        "network.inbound_connection_limit",
        value<uint32_t>(&settings.network.inbound_connection_limit)->
            default_value(p2p::mainnet.inbound_connection_limit),
        "The maximum number of incoming network connections, defaults to 8."
    )
    (
        "network.outbound_connections",
        value<uint32_t>(&settings.network.outbound_connections)->
            default_value(p2p::mainnet.outbound_connections),
        "The maximum number of outgoing network connections, defaults to 8."
    )
    (
        "network.connect_attempts",
        value<uint16_t>(&settings.network.connect_attempts)->
            default_value(p2p::mainnet.connect_attempts),
        "The number of times to attempt contacting a node, defaults to 0 (forever)."
    )
    (
        "network.connect_timeout_seconds",
        value<uint32_t>(&settings.network.connect_timeout_seconds)->
            default_value(p2p::mainnet.connect_timeout_seconds),
        "The time limit for connection establishment, defaults to 5."
    )
    (
        "network.channel_handshake_seconds",
        value<uint32_t>(&settings.network.channel_handshake_seconds)->
            default_value(p2p::mainnet.channel_handshake_seconds),
        "The time limit to complete the connection handshake, defaults to 30."
    )
    (
        "network.channel_revival_minutes",
        value<uint32_t>(&settings.network.channel_revival_minutes)->
            default_value(p2p::mainnet.channel_revival_minutes),
        "The time between blocks that initiates a request, defaults to 5."
    )
    (
        "network.channel_heartbeat_minutes",
        value<uint32_t>(&settings.network.channel_heartbeat_minutes)->
            default_value(p2p::mainnet.channel_heartbeat_minutes),
        "The time between ping messages, defaults to 5."
    )
    (
        "network.channel_inactivity_minutes",
        value<uint32_t>(&settings.network.channel_inactivity_minutes)->
            default_value(p2p::mainnet.channel_inactivity_minutes),
        "The inactivity time limit for any connection, defaults to 30."
    )
    (
        "network.channel_expiration_minutes",
        value<uint32_t>(&settings.network.channel_expiration_minutes)->
            default_value(p2p::mainnet.channel_expiration_minutes),
        "The maximum age limit for an outbound connection, defaults to 90."
    )
    (
        "network.channel_germination_seconds",
        value<uint32_t>(&settings.network.channel_germination_seconds)->
            default_value(p2p::mainnet.channel_germination_seconds),
        "The maximum time limit for obtaining seed addresses, defaults to 30."
    )
    (
        "network.host_pool_capacity",
        value<uint32_t>(&settings.network.host_pool_capacity)->
            default_value(p2p::mainnet.host_pool_capacity),
        "The maximum number of peer hosts in the pool, defaults to 1000."
    )
    (
        "network.relay_transactions",
        value<bool>(&settings.network.relay_transactions)->
            default_value(p2p::mainnet.relay_transactions),
        "Request that peers relay transactions, defaults to true."
    )
    (
        "network.hosts_file",
        value<path>(&settings.network.hosts_file)->
            default_value(p2p::mainnet.hosts_file),
        "The peer hosts cache file path, defaults to 'hosts.cache'."
    )
    (
        "network.debug_file",
        value<path>(&settings.network.debug_file)->
            default_value(p2p::mainnet.debug_file),
        "The debug log file path, defaults to 'debug.log'."
    )
    (
        "network.error_file",
        value<path>(&settings.network.error_file)->
            default_value(p2p::mainnet.error_file),
        "The error log file path, defaults to 'error.log'."
    )
    (
        "network.self",
        value<config::authority>(&settings.network.self)->
            multitoken()->default_value(p2p::mainnet.self),
        "The advertised public address of this node, defaults to none."
    )
    (
        "network.blacklist",
        value<config::authority::list>(&settings.network.blacklists)->
            multitoken()->default_value(p2p::mainnet.blacklists),
        "IP address to disallow as a peer, multiple entries allowed."
    )
    (
        "network.seed",
        value<config::endpoint::list>(&settings.network.seeds)->
            multitoken()->default_value(p2p::mainnet.seeds),
        "A seed node for initializing the host pool, multiple entries allowed."
    )

    /* [blockchain] */
    (
        "blockchain.threads",
        value<uint32_t>(&settings.chain.threads)->
            default_value(BLOCKCHAIN_THREADS),
        "The number of threads in the blockchain threadpool, defaults to 6."
    )
    (
        "blockchain.block_pool_capacity",
        value<uint32_t>(&settings.chain.block_pool_capacity)->
            default_value(BLOCKCHAIN_BLOCK_POOL_CAPACITY),
        "The maximum number of orphan blocks in the pool, defaults to 50."
    )
    (
        "blockchain.history_start_height",
        value<uint32_t>(&settings.chain.history_start_height)->
            default_value(BLOCKCHAIN_HISTORY_START_HEIGHT),
        "The history index start height, defaults to 0."
    )
    (
        "blockchain.use_testnet_rules",
        value<bool>(&settings.chain.use_testnet_rules)->
            default_value(BLOCKCHAIN_TESTNET_RULES_MAINNET),
        "Use testnet rules for determination of work required, defaults to false."
    )
    (
        "blockchain.database_path",
        value<path>(&settings.chain.database_path)->
            default_value(BLOCKCHAIN_DATABASE_PATH),
        "The blockchain database directory, defaults to 'blockchain'."
    )
    (
        "blockchain.checkpoint",
        value<config::checkpoint::list>(&settings.chain.checkpoints)->
            multitoken()->default_value(BLOCKCHAIN_CHECKPOINTS_MAINNET),
        "A hash:height checkpoint, multiple entries allowed."
    )

    /* [node] */
    (
        "node.threads",
        value<uint32_t>(&settings.node.threads)->
            default_value(NODE_THREADS),
        "The number of threads in the node threadpool, defaults to 4."
    )
    (
        "node.transaction_pool_capacity",
        value<uint32_t>(&settings.node.transaction_pool_capacity)->
            default_value(NODE_TRANSACTION_POOL_CAPACITY),
        "The maximum number of transactions in the pool, defaults to 2000."
    )
    (
        "node.peer",
        value<config::endpoint::list>(&settings.node.peers)->
            multitoken()->default_value(NODE_PEERS),
        "Persistent host:port to augment discovered hosts, multiple entries allowed."
    )

    /* [server] */
    (
        "server.query_endpoint",
        value<endpoint>(&settings.server.query_endpoint)->
            default_value(SERVER_QUERY_ENDPOINT),
        "The query service endpoint, defaults to 'tcp://*:9091'."
    )
    (
        "server.heartbeat_endpoint",
        value<endpoint>(&settings.server.heartbeat_endpoint)->
            default_value(SERVER_HEARTBEAT_ENDPOINT),
        "The heartbeat service endpoint, defaults to 'tcp://*:9092'."
    )
    (
        "server.block_publish_endpoint",
        value<endpoint>(&settings.server.block_publish_endpoint)->
            default_value(SERVER_BLOCK_PUBLISH_ENDPOINT),
        "The block publishing service endpoint, defaults to 'tcp://*:9093'."
    )
    (
        "server.transaction_publish_endpoint",
        value<endpoint>(&settings.server.transaction_publish_endpoint)->
            default_value(SERVER_TRANSACTION_PUBLISH_ENDPOINT),
        "The transaction publishing service endpoint, defaults to 'tcp://*:9094'."
    )
    (
        "server.publisher_enabled",
        value<bool>(&settings.server.publisher_enabled)->
            default_value(SERVER_PUBLISHER_ENABLED),
        "Enable the block and transaction publishing endpoints, defaults to true."
    )
    (
        "server.queries_enabled",
        value<bool>(&settings.server.queries_enabled)->
            default_value(SERVER_QUERIES_ENABLED),
        "Enable the query and heartbeat endpoints, defaults to true."
    )
    (
        "server.log_requests",
        value<bool>(&settings.server.log_requests)->
            default_value(SERVER_LOG_REQUESTS),
        "Write service requests to the log, defaults to false."
    )
    (
        "server.polling_interval_seconds",
        value<uint32_t>(&settings.server.polling_interval_seconds)->
            default_value(SERVER_POLLING_INTERVAL_SECONDS),
        "The query polling interval in seconds, defaults to 1."
    )
    (
        "server.heartbeat_interval_seconds",
        value<uint32_t>(&settings.server.heartbeat_interval_seconds)->
            default_value(SERVER_HEARTBEAT_INTERVAL_SECONDS),
        "The heartbeat interval in seconds, defaults to 5."
    )
    (
        "server.subscription_expiration_minutes",
        value<uint32_t>(&settings.server.subscription_expiration_minutes)->
            default_value(SERVER_SUBSCRIPTION_EXPIRATION_MINUTES),
        "The subscription expiration time, defaults to 10 minutes."
    )
    (
        "server.subscription_limit",
        value<uint32_t>(&settings.server.subscription_limit)->
            default_value(SERVER_SUBSCRIPTION_LIMIT),
        "The maximum number of subscriptions, defaults to 100000000."
    )
    (
        "server.certificate_file",
        value<path>(&settings.server.certificate_file)->
            default_value(SERVER_CERTIFICATE_FILE),
        "The path to the ZPL-encoded server private certificate file."
    )
    (
        "server.client_certificates_path",
        value<path>(&settings.server.client_certificates_path)->
            default_value(SERVER_CLIENT_CERTIFICATES_PATH),
        "The directory for ZPL-encoded client public certificate files, allows anonymous clients if not set."
    )
    (
        "server.whitelist",
        value<config::authority::list>(&settings.server.whitelists)->
            multitoken()->default_value(SERVER_WHITELISTS),
        "Allowed client IP address, all clients allowed if none set, multiple entries allowed."
    );

    return description;
}

} // namespace server
} // namespace libbitcoin

