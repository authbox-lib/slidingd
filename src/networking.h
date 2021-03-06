#ifndef NETWORKING_H
#define NETWORKING_H
#include <pthread.h>
#include "config.h"

// Network configuration struct
typedef struct hlld_networking hlld_networking;
typedef struct conn_info hlld_conn_info;

#define EXTRACT_PROTO_ERROR (-2)
#define EXTRACT_NO_DATA (-1)

/**
 * Initializes the networking interfaces
 * @arg config Takes the server configuration
 * @arg mgr The manager to pass up to the connection handlers
 * @arg netconf Output. The configuration for the networking stack.
 */
int init_networking(struct hlld_config *config, void *mgr, hlld_networking **netconf_out);

/**
 * Entry point for the main thread to start accepting
 * @arg netconf The configuration for the networking stack.
 * @arg should_run A flag checked to see if we should run
 * @arg threads The list of worker threads
 */
void enter_main_loop(hlld_networking *netconf, int *should_run, pthread_t *threads);

/**
 * Entry point for threads to join the networking
 * stack. This method blocks indefinitely until the
 * network stack is shutdown.
 * @arg netconf The configuration for the networking stack.
 */
void start_networking_worker(hlld_networking *netconf);

/**
 * Shuts down all the connections
 * and listeners and prepares to exit.
 * @arg netconf The config for the networking stack.
 * @arg threads A list of worker threads
 */
int shutdown_networking(hlld_networking *netconf, pthread_t *threads);

/*
 * Connection related methods. These are exposed so
 * that the connection handlers can manipulate the buffers.
 */

/**
 * Sends a response to a client.
 * @arg conn The client connection
 * @arg response_buffers A list of response buffers to send
 * @arg buf_sizes A list of the buffer sizes
 * @arg num_bufs The number of response buffers
 * @return 0 on success.
 */
int send_client_response(hlld_conn_info *conn, char **response_buffers, int *buf_sizes, int num_bufs);

/**
 * This method is used to conveniently extract commands from the
 * command buffer. It scans up to a terminator, and then sets the
 * buf to the start of the buffer, and buf_len to the length
 * of the buffer. The output param should_free indicates that
 * the caller should free the buffer pointed to by buf when it is finished.
 * This method consumes the bytes from the underlying buffer, freeing
 * space for later reads.
 * @arg conn The client connection
 * @arg terminator The terminator charactor to look for. Included in buf.
 * @arg buf Output parameter, sets the start of the buffer.
 * @arg buf_len Output parameter, the length of the buffer.
 * @arg arg_count Output parameter, the number of arguments.
 * @arg should_free Output parameter, should the buffer be freed by the caller.
 * @return 0 on success, -1 if the terminator is not found.
 */
int extract_command(hlld_conn_info *conn, char **args, int *arg_lens, int max_args, int *arg_count, int *free_arg);

#endif
