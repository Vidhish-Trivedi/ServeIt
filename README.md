# ServeIt
- A naive client-server data-store application which uses an event loop (polling) to achieve non-blocking I/O on the server-side, offers command-based get, set and delete (del).
- The program establishes a client-server connection through sockets and allows for multiple clients to connect simultaneously to the server by implementing an event loop which "polls" the clients to determine which connections are "ready".

## Protocol
- In order to process multiple requests from a client, the application makes use of a simple protocol to split requests apart from the TCP byte stream.
- The length of the request is declared at the beginning of each request.
- Format:

| len | msg1 | len | msg2 | ... |
| --- | ---- | --- | ---- | --- |

- The protocol consists of a 4-byte integer indicating the length of the request, and then the variable-length request follows.

## IO Design
- There are multiple ways to deal with concurrent connections in server-side network programs, such as multi-threading, forking, and event loops. Forking creates new processes for each client connection and multi-threading creates new threads.
- Keeping in mind the overhead of processes and thread, this application implements an event loop which uses polling and non-blocking IO and runs on a single thread.

## Features
- With the server running, you can perform the following operations:
    * Set `our_value` as value to `our_key`
    ```bash
    ./client set our_key our_val
    ```
    * Get value associated with `our_key`
    ```bash
    ./client get our_key
    ```
    * Delete (key, value) pair associated with `our_key`
    ```bash
    ./client del our_key
    ```

## Requirements and Running the Project
- This section is yet to be added.

## Authors

- [@Vidhish-Trivedi](https://github.com/Vidhish-Trivedi)

## License

[MIT](https://choosealicense.com/licenses/mit/)
