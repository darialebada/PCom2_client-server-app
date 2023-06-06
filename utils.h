#ifndef __UTILS_h__
#define __UTILS_h__

#include <vector>
#include <queue>

#define MAXC 128
#define BUFLEN 1551
#define MAX_MSG 1501
#define MAX_ID 11
#define MAX_IP 13
#define TOPIC_LEN 50
#define TYPE_LEN 12
#define MAX_DATA_TYPE 11
#define MAXB 256

// message type
#define SUBSCRIBE 1
#define UNSUBSCRIBE 2

struct subscriber_t {
    char id[11];
    int fd;
    bool conn;
    std::vector<struct topic_t> topics;
    std::queue<char *> q;
};

struct topic_t {
    char topic[TOPIC_LEN];
    int sf;
};

struct __attribute__ ((__packed__)) message_t {
    int type;
    char topic[TOPIC_LEN];
    int sf;
};

struct udp_message_t {
    char ip[MAX_IP];
    uint32_t port;
    char topic[TOPIC_LEN + 1];
    char type[MAX_DATA_TYPE];
    char payload[MAX_MSG];
    char msg[BUFLEN];
};

#endif // __UTILS_h__