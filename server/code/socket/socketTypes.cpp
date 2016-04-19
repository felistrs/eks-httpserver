#include <mutex>

#include "socketTypes.h"
#include "../utils/logger.h"


namespace server {


static std::map<connection_handler, connection_descriptor> g_connection_descriptors =
        std::map<connection_handler, connection_descriptor>();

static connection_handler g_handler_counter = 0;
static std::mutex g_lock_connection_descriptors;


std::pair<connection_handler, connection_descriptor*> generateConnectionHandler()
{
    std::unique_lock<std::mutex> lock(g_lock_connection_descriptors);

    bool found = false;
    connection_handler handler = CONNECTION_HANDLER_INVALID;
    connection_descriptor* descr = nullptr;

    // Search for empty slot
    connection_handler h_start = g_handler_counter;

    for (;;)
    {
        ++g_handler_counter;

        if (g_handler_counter == CONNECTION_HANDLER_INVALID)
            ++g_handler_counter;

        if (g_handler_counter == h_start)
            break;

        auto it = g_connection_descriptors.find(g_handler_counter);
        if (it == g_connection_descriptors.end())
        {
            found = true;
            break;
        }
    }

    // New descriptor
    if (found )
    {
        handler = g_handler_counter;
        g_connection_descriptors[handler] = connection_descriptor();
        descr = &g_connection_descriptors[handler];
    }

    return std::make_pair(handler, descr);
}

void releaseConnectionHandler(connection_handler handler)
{
    std::unique_lock<std::mutex> lock(g_lock_connection_descriptors);

    if (handler != CONNECTION_HANDLER_INVALID)
    {
        auto it = g_connection_descriptors.find(handler);
        if (it != g_connection_descriptors.end())
            g_connection_descriptors.erase(it);
    }
}

bool testIfAllHandlersReleased()
{
    std::unique_lock<std::mutex> lock(g_lock_connection_descriptors);

    return g_connection_descriptors.empty();
}

connection_descriptor* GetConnectionDescriptor(connection_handler handler)
{
    std::unique_lock<std::mutex> lock(g_lock_connection_descriptors);

    auto it = g_connection_descriptors.find(handler);
    if (it != g_connection_descriptors.end())
        return &(it->second);
    else
    {
        error("Request for bad handler");
        return nullptr;
    }
}


}
