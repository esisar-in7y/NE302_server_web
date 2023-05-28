// Host
// User-Agent
// Accept
// Content-Type
// Content-Length
// Authorization
// Referer
// Cache-Control
// If-Modified-Since
// Range
// Origin 

typedef struct {
    char* host;
    //char* user_agent;
    char* accept;
    char* content_type;
    char* content_length;
    //char* authorization;
    char* referer;
    //char* cache_control;
    //char* if_modified_since;
    char* range;
    //char* origin; // Proxy 
    char* connection;
} requestClient;

// Status Line
// Content-Type
// Content-Length
// Location
// Cache-Control
// Date
// Server
// ETag 
// Last-Modified
// Connection
// Content-Encoding
// Keep-Alive
// Transfer-Encoding

typedef struct {
    int status_code;
    char* status_line;
    char* content_type;
    char* content_length;
    char* location;
    char* cache_control;
    char* date;
    char* server;
    char* etag;
    char* last_modified;
    char* connection;
    char* content_encoding;
    char* keep_alive;
    char* transfer_encoding;
} responseServer;