#include "basicIO.h"

#define MAX_USERS    1000
#define MAX_LINE     256
#define MAX_NET_LEN  8
#define MAX_CHANNELS 5
#define CAP_PER_CHANNEL 16   // 2G: 16 users per 200 kHz

// Simple user record
class CsvUser {
public:
    int  id;
    char network[MAX_NET_LEN];
};

// Global basicIO from your basicIO.h/cpp
extern basicIO io;

// --- Helper: parse integer field from CSV line ---
int parseIntField(const char* line, int* pos) {
    int sign = 1;
    int value = 0;

    // Skip non-digit and non-minus (spaces, etc.)
    while (line[*pos] != 0 && line[*pos] != '-' &&
          (line[*pos] < '0' || line[*pos] > '9')) {
        (*pos)++;
    }

    if (line[*pos] == '-') {
        sign = -1;
        (*pos)++;
    }

    while (line[*pos] >= '0' && line[*pos] <= '9') {
        value = value * 10 + (line[*pos] - '0');
        (*pos)++;
    }

    return sign * value;
}

// --- Helper: parse string field until comma or end ---
void parseStringField(const char* line, int* pos, char* out, int maxLen) {
    int i = 0;

    // Skip starting comma if present
    if (line[*pos] == ',') {
        (*pos)++;
    }

    // Copy until comma or end or maxLen-1
    while (line[*pos] != 0 && line[*pos] != ',' && i < maxLen - 1) {
        out[i] = line[*pos];
        i++;
        (*pos)++;
    }

    out[i] = '\0';
}

// --- Main program ---
int main() {
    CsvUser users[MAX_USERS];
    int userCount = 0;

    int newChannel[MAX_USERS];     // new channel assigned for each user
    int channelLoad[MAX_CHANNELS]; // how many users in each channel

    char line[MAX_LINE];
    int i;

    // Initialize
    for (i = 0; i < MAX_CHANNELS; ++i) {
        channelLoad[i] = 0;
    }

    io.outputstring("Reading CSV from stdin...\n");
    io.outputstring("Format: user_id,channel,network\n");

    while (1) {
        // Read one line (up to newline or EOF)
        io.inputstring(line, MAX_LINE);

        // EOF or empty line => stop
        if (line[0] == '\0') {
            break;
        }

        // Skip header or any non-data line
        // If first character is not digit, ignore
        if (line[0] < '0' || line[0] > '9') {
            continue;
        }

        if (userCount >= MAX_USERS) {
            io.outputstring("Max user limit reached, ignoring extra lines.\n");
            break;
        }

        int pos = 0;

        // Field 1: user_id
        int uid = parseIntField(line, &pos);

        // Field 2: original channel (we read and ignore)
        // Skip comma first if present
        while (line[pos] != 0 && line[pos] != ',') pos++;
        if (line[pos] == ',') pos++;
        /* int oldChannel = parseIntField(line, &pos); */ // Not used

        // Field 3: network
        // Move to next comma if not already
        while (line[pos] != 0 && line[pos] != ',') pos++;
        char net[MAX_NET_LEN];
        if (line[pos] == ',') {
            pos++;
        }
        parseStringField(line, &pos, net, MAX_NET_LEN);

        // For this demo, only process users whose network starts with '2' (2G)
        if (net[0] != '2') {
            // ignore 3G/4G/5G in this simple example
            continue;
        }

        // Store user
        users[userCount].id = uid;
        int j;
        for (j = 0; j < MAX_NET_LEN; ++j) {
            users[userCount].network[j] = net[j];
            if (net[j] == '\0') break;
        }

        userCount++;
    }

    // Now we have all 2G users in `users[0..userCount-1]`
    // Re-allocate them across 5 channels, each with capacity 16

    io.outputstring("Total 2G users read: ");
    io.outputint(userCount);
    io.terminate();

    for (i = 0; i < userCount; ++i) {
        int ch = i / CAP_PER_CHANNEL;  // simple sequential allocation
        if (ch >= MAX_CHANNELS) {
            // no more capacity in this simple example
            newChannel[i] = -1;
        } else {
            newChannel[i] = ch;
            channelLoad[ch]++;
        }
    }

    // Print channel summary
    io.outputstring("\nChannel summary (2G):\n");
    for (i = 0; i < MAX_CHANNELS; ++i) {
        io.outputstring("Channel ");
        io.outputint(i);
        io.outputstring(" has ");
        io.outputint(channelLoad[i]);
        io.outputstring(" users\n");
    }

    // Print per-user assignment
    io.outputstring("\nUser -> Channel mapping:\n");
    for (i = 0; i < userCount; ++i) {
        io.outputstring("User ");
        io.outputint(users[i].id);
        io.outputstring(" assigned to channel ");
        io.outputint(newChannel[i]);
        io.terminate();
    }

    return 0;
}
