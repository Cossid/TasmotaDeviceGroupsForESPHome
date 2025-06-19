#if defined(USE_ESP_IDF)

#include "WiFiUdp.h"
#include <fcntl.h>
#include <netdb.h>

#include "esp_wifi.h"
#include "esp_netif.h"

// Default buffer size for UDP packets
#define DEFAULT_BUFFER_SIZE 1024

WiFiUDP::WiFiUDP() : sock_fd(-1), is_connected(false), buffer(nullptr), 
                     buffer_size(0), data_length(0), read_position(0) {
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
}

WiFiUDP::~WiFiUDP() {
    stop();
    if (buffer) {
        free(buffer);
        buffer = nullptr;
    }
}

bool WiFiUDP::initSocket() {
    if (sock_fd >= 0) {
        close(sock_fd);
    }
    
    sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_fd < 0) {
        printf("Failed to create UDP socket: %s\n", strerror(errno));
        return false;
    }
    
    return setSocketOptions();
}

bool WiFiUDP::setSocketOptions() {
    // Set socket to non-blocking mode
    int flags = fcntl(sock_fd, F_GETFL, 0);
    if (flags < 0) {
        printf("Failed to get socket flags: %s\n", strerror(errno));
        return false;
    }
    
    if (fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        printf("Failed to set socket non-blocking: %s\n", strerror(errno));
        return false;
    }
    
    // Allow address reuse
    int opt = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        printf("Failed to set SO_REUSEADDR: %s\n", strerror(errno));
        return false;
    }
    
    return true;
}

bool WiFiUDP::begin(uint16_t port) {
    if (!initSocket()) {
        return false;
    }
    
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(port);
    
    if (bind(sock_fd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        printf("Failed to bind UDP socket to port %d: %s\n", port, strerror(errno));
        close(sock_fd);
        sock_fd = -1;
        return false;
    }
    
    is_connected = true;
    return true;
}

bool WiFiUDP::beginMulticast(uint16_t port, const char* multicast_ip, const char* interface_ip) {
    if (!initSocket()) {
        return false;
    }
    
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(port);
    
    if (bind(sock_fd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        printf("Failed to bind UDP socket to port %d: %s\n", port, strerror(errno));
        close(sock_fd);
        sock_fd = -1;
        return false;
    }
    
    // Join multicast group
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(multicast_ip);
    mreq.imr_interface.s_addr = inet_addr(interface_ip);
    
    if (setsockopt(sock_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        printf("Failed to join multicast group: %s\n", strerror(errno));
        close(sock_fd);
        sock_fd = -1;
        return false;
    }
    
    is_connected = true;
    return true;
}

bool WiFiUDP::beginMulticast(const IPAddress& multicast_ip, uint16_t port) {
    if (!initSocket()) {
        return false;
    }
    
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(port);
    
    if (bind(sock_fd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        printf("Failed to bind UDP socket to port %d: %s\n", port, strerror(errno));
        close(sock_fd);
        sock_fd = -1;
        return false;
    }
    
    // Join multicast group
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = htonl((multicast_ip[0] << 24) | (multicast_ip[1] << 16) | (multicast_ip[2] << 8) | multicast_ip[3]);
    mreq.imr_interface.s_addr = INADDR_ANY;  // Use default interface
    
    if (setsockopt(sock_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        printf("Failed to join multicast group: %s\n", strerror(errno));
        close(sock_fd);
        sock_fd = -1;
        return false;
    }
    
    is_connected = true;
    return true;
}

void WiFiUDP::stop() {
    if (sock_fd >= 0) {
        close(sock_fd);
        sock_fd = -1;
    }
    is_connected = false;
    
    if (buffer) {
        free(buffer);
        buffer = nullptr;
        buffer_size = 0;
    }
    data_length = 0;
    read_position = 0;
}

bool WiFiUDP::beginPacket(const char* ip, uint16_t port) {
    if (sock_fd < 0) {
        if (!initSocket()) {
            return false;
        }
    }
    
    remote_addr.sin_addr.s_addr = inet_addr(ip);
    remote_addr.sin_port = htons(port);
    
    return true;
}

bool WiFiUDP::beginPacket(uint32_t ip, uint16_t port) {
    if (sock_fd < 0) {
        if (!initSocket()) {
            return false;
        }
    }
    
    remote_addr.sin_addr.s_addr = htonl(ip);
    remote_addr.sin_port = htons(port);
    
    return true;
}

bool WiFiUDP::beginPacket(const IPAddress& ip, uint16_t port) {
    if (sock_fd < 0) {
        if (!initSocket()) {
            return false;
        }
    }
    
    remote_addr.sin_addr.s_addr = htonl((ip[0] << 24) | (ip[1] << 16) | (ip[2] << 8) | ip[3]);
    remote_addr.sin_port = htons(port);
    
    return true;
}

bool WiFiUDP::endPacket() {
    if (sock_fd < 0) {
        return false;
    }
    
    ssize_t sent = sendto(sock_fd, buffer, data_length, 0,
                         (struct sockaddr*)&remote_addr, sizeof(remote_addr));
    
    if (sent < 0) {
        printf("Failed to send UDP packet: %s\n", strerror(errno));
        return false;
    }
    
    // Reset buffer for next packet
    data_length = 0;
    read_position = 0;
    
    return true;
}

size_t WiFiUDP::write(uint8_t byte) {
    if (!buffer) {
        buffer = (char*)malloc(DEFAULT_BUFFER_SIZE);
        buffer_size = DEFAULT_BUFFER_SIZE;
        if (!buffer) {
            return 0;
        }
    }
    
    if (data_length >= buffer_size) {
        // Resize buffer if needed
        size_t new_size = buffer_size * 2;
        char* new_buffer = (char*)realloc(buffer, new_size);
        if (!new_buffer) {
            return 0;
        }
        buffer = new_buffer;
        buffer_size = new_size;
    }
    
    buffer[data_length++] = byte;
    return 1;
}

size_t WiFiUDP::write(const uint8_t* data, size_t size) {
    if (!buffer) {
        buffer = (char*)malloc(DEFAULT_BUFFER_SIZE);
        buffer_size = DEFAULT_BUFFER_SIZE;
        if (!buffer) {
            return 0;
        }
    }
    
    // Ensure buffer is large enough
    while (data_length + size > buffer_size) {
        size_t new_size = buffer_size * 2;
        char* new_buffer = (char*)realloc(buffer, new_size);
        if (!new_buffer) {
            return 0;
        }
        buffer = new_buffer;
        buffer_size = new_size;
    }
    
    memcpy(buffer + data_length, data, size);
    data_length += size;
    return size;
}

size_t WiFiUDP::write(const char* str) {
    return write((const uint8_t*)str, strlen(str));
}

int WiFiUDP::parsePacket() {
    if (sock_fd < 0) {
        return 0;
    }
    
    // Allocate buffer if not already done
    if (!buffer) {
        buffer = (char*)malloc(DEFAULT_BUFFER_SIZE);
        buffer_size = DEFAULT_BUFFER_SIZE;
        if (!buffer) {
            return 0;
        }
    }
    
    struct sockaddr_in sender_addr;
    socklen_t sender_len = sizeof(sender_addr);
    
    ssize_t received = recvfrom(sock_fd, buffer, buffer_size - 1, MSG_DONTWAIT,
                               (struct sockaddr*)&sender_addr, &sender_len);
    
    if (received > 0) {
        data_length = received;
        read_position = 0;
        buffer[data_length] = '\0';  // Null-terminate for string operations
        
        // Store sender info for remoteIP() and remotePort()
        remote_addr = sender_addr;
        
        return data_length;
    }
    
    return 0;
}

int WiFiUDP::read() {
    if (read_position >= data_length) {
        return -1;
    }
    return (int)(unsigned char)buffer[read_position++];
}

int WiFiUDP::read(uint8_t* data, size_t size) {
    if (read_position >= data_length) {
        return -1;
    }
    
    size_t available = data_length - read_position;
    size_t to_read = (size < available) ? size : available;
    
    memcpy(data, buffer + read_position, to_read);
    read_position += to_read;
    
    return to_read;
}

int WiFiUDP::read(char* data, size_t size) {
    return read((uint8_t*)data, size);
}

int WiFiUDP::peek() {
    if (read_position >= data_length) {
        return -1;
    }
    return (int)(unsigned char)buffer[read_position];
}

void WiFiUDP::flush() {
    // For UDP, flush doesn't really apply, but we can clear the receive buffer
    data_length = 0;
    read_position = 0;
}


IPAddress WiFiUDP::remoteIP() {
    uint32_t ip = ntohl(remote_addr.sin_addr.s_addr);
    return IPAddress((ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
}

uint16_t WiFiUDP::remotePort() {
    return ntohs(remote_addr.sin_port);
}

bool WiFiUDP::connected() {
    return is_connected && sock_fd >= 0;
}

void WiFiUDP::setTimeout(int timeout_ms) {
    if (sock_fd >= 0) {
        struct timeval tv;
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    }
}

uint16_t WiFiUDP::localPort() {
    if (sock_fd < 0) {
        return 0;
    }
    
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    if (getsockname(sock_fd, (struct sockaddr*)&addr, &len) < 0) {
        return 0;
    }
    return ntohs(addr.sin_port);
}

const char* WiFiUDP::localIP() {
    static char ip_str[16];
    
    // Get the default network interface (usually WiFi STA)
    esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (netif == nullptr) {
        // Try to get any available network interface
        esp_netif_t* netif_list[4]; // Use a reasonable default size
        int netif_count = esp_netif_get_nr_of_ifs();
        if (netif_count > 0 && netif_count <= 4) {
            // Get the first available interface
            esp_netif_t* default_netif = esp_netif_get_default_netif();
            if (default_netif != nullptr) {
                netif = default_netif;
            } else {
                // Fallback: try to get any interface
                esp_netif_t* current_netif = nullptr;
                esp_netif_t* temp_netif = esp_netif_next(nullptr);
                if (temp_netif != nullptr) {
                    netif = temp_netif;
                }
            }
        }
    }
    
    if (netif != nullptr) {
        esp_netif_ip_info_t ip_info;
        if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
            inet_ntop(AF_INET, &ip_info.ip.addr, ip_str, sizeof(ip_str));
            return ip_str;
        }
    }
    
    return "0.0.0.0";
}

int WiFiUDP::available() {
    return data_length - read_position;
}

const char* WiFiUDP::ipToString(uint32_t ip) {
    static char ip_str[16];
    struct in_addr addr;
    addr.s_addr = htonl(ip);
    inet_ntop(AF_INET, &addr, ip_str, sizeof(ip_str));
    return ip_str;
}

#endif  // USE_ESP_IDF 