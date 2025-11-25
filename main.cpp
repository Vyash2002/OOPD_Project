// main.cpp
// No STL, no <string>, no <cstdio>, no <unistd.h>
// Uses basicIO (basicIO.h/.cpp) and syscall3 defined in basicIO.cpp
// Uses pthreads for threading.

#include "IO.h"
#include "ProtocolConfig.h"
#include "CellularCore.h"
#include "SimulationScenario.h"
#include "basicIO.h"
#include <pthread.h>

extern basicIO io;
extern "C" long syscall3(long number, long arg1, long arg2, long arg3);

// raw syscall numbers for linux x86-64
#define SYS_READ   0
#define SYS_OPEN   2
#define SYS_CLOSE  3

// CSV and network constants
#define CSV_MAX_LINE    256
#define CSV_MAX_USERS   1000
#define CSV_MAX_NET_LEN 8

// Network indices: 0 -> 2G, 1 -> 3G, 2 -> 4G, 3 -> 5G
#define NET_COUNT 4

// Channel counts and capacity per channel per network (adjust here if needed)
static const int NET_CHANNEL_COUNT[NET_COUNT] = {5, 5, 10, 10};
static const int NET_CAPACITY[NET_COUNT]      = {16,32,30,30};

// queue for lines (single-producer, single-consumer)
#define Q_SIZE 256
struct LineQueue {
    char lines[Q_SIZE][CSV_MAX_LINE];
    int head;
    int tail;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
};
static LineQueue g_queue;

// file buffer for raw syscalls
static long g_fd = -1;
static char g_buf[512];
static int g_bufPos = 0;
static int g_bufEnd = 0;

// results: arrays populated by consumer thread (consumer writes; main reads after join)
static int results_userId[CSV_MAX_USERS];
static int results_netIndex[CSV_MAX_USERS];  // 0..3 for networks, -1 if ignored
static int results_channel[CSV_MAX_USERS];   // -1 => dropped
static int results_count = 0;
static int net_load[NET_COUNT];              // per-network total assigned

// control
static int producer_done = 0;

// -------- queue helpers ----------
static void queue_init(LineQueue* q) {
    q->head = q->tail = q->count = 0;
    pthread_mutex_init(&q->lock, 0);
    pthread_cond_init(&q->not_empty, 0);
    pthread_cond_init(&q->not_full, 0);
}
static void queue_destroy(LineQueue* q) {
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->not_empty);
    pthread_cond_destroy(&q->not_full);
}
static void queue_push(LineQueue* q, const char* line) {
    pthread_mutex_lock(&q->lock);
    while (q->count == Q_SIZE) pthread_cond_wait(&q->not_full, &q->lock);
    // copy line
    int i = 0;
    while (i < CSV_MAX_LINE-1 && line[i] != '\0') { q->lines[q->tail][i] = line[i]; ++i; }
    q->lines[q->tail][i] = '\0';
    q->tail = (q->tail + 1) % Q_SIZE;
    q->count++;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}
static int queue_pop(LineQueue* q, char* dest) {
    pthread_mutex_lock(&q->lock);
    while (q->count == 0) {
        if (producer_done) { pthread_mutex_unlock(&q->lock); return 0; }
        pthread_cond_wait(&q->not_empty, &q->lock);
    }
    int i = 0;
    while (i < CSV_MAX_LINE-1 && q->lines[q->head][i] != '\0') { dest[i] = q->lines[q->head][i]; ++i; }
    dest[i] = '\0';
    q->head = (q->head + 1) % Q_SIZE;
    q->count--;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);
    return 1;
}

// -------- raw syscall file helpers ----------
static long refill_buffer() {
    long r = syscall3(SYS_READ, g_fd, (long)g_buf, (long)sizeof(g_buf));
    if (r <= 0) { g_bufPos = g_bufEnd = 0; return 0; }
    g_bufPos = 0; g_bufEnd = (int)r;
    return r;
}
static int get_next_char(char* out) {
    if (g_bufPos >= g_bufEnd) {
        long r = refill_buffer();
        if (r <= 0) return 0;
    }
    *out = g_buf[g_bufPos++];
    return 1;
}
static int read_line_from_file(char* dest) {
    int len = 0; char c; int any = 0;
    while (len < CSV_MAX_LINE - 1) {
        if (!get_next_char(&c)) break;
        any = 1;
        if (c == '\n') break;
        if (c == '\r') continue;
        dest[len++] = c;
    }
    if (!any) return 0;
    dest[len] = '\0';
    return 1;
}

// -------- CSV parse helpers (no STL) ----------
static int parseIntField(const char* s, int* pos) {
    int sign = 1; int val = 0;
    while (s[*pos] && s[*pos] != '-' && (s[*pos]<'0' || s[*pos]>'9')) (*pos)++;
    if (s[*pos] == '-') { sign = -1; (*pos)++; }
    while (s[*pos] >= '0' && s[*pos] <= '9') { val = val*10 + (s[*pos]-'0'); (*pos)++; }
    return sign*val;
}
static void parseStringField(const char* s, int* pos, char* out, int maxLen) {
    int i = 0;
    if (s[*pos] == ',') (*pos)++;
    while (s[*pos] && s[*pos] != ',' && i < maxLen-1) { out[i++] = s[*pos]; (*pos)++; }
    out[i] = '\0';
}

// -------- producer thread: reads file and pushes lines ----------
static void* producer_thread(void* arg) {
    (void)arg;
    const char* fname = "users_1000.csv";
    g_fd = syscall3(SYS_OPEN, (long)fname, 0, 0); // O_RDONLY
    if (g_fd < 0) {
        io.outputstring("ERROR: cannot open users_1000.csv\n");
        pthread_mutex_lock(&g_queue.lock);
        producer_done = 1;
        pthread_cond_broadcast(&g_queue.not_empty);
        pthread_mutex_unlock(&g_queue.lock);
        return 0;
    }
    g_bufPos = g_bufEnd = 0;
    char line[CSV_MAX_LINE];
    while (1) {
        int ok = read_line_from_file(line);
        if (!ok) break;
        if (line[0] == '\0') continue;
        queue_push(&g_queue, line);
    }
    syscall3(SYS_CLOSE, g_fd, 0, 0);
    g_fd = -1;
    pthread_mutex_lock(&g_queue.lock);
    producer_done = 1;
    pthread_cond_broadcast(&g_queue.not_empty);
    pthread_mutex_unlock(&g_queue.lock);
    return 0;
}

// -------- consumer thread: parse lines, assign per-network ----------
static void* consumer_thread(void* arg) {
    (void)arg;
    int i;
    for (i = 0; i < NET_COUNT; ++i) net_load[i] = 0;
    results_count = 0;
    char line[CSV_MAX_LINE];
    while (1) {
        int have = queue_pop(&g_queue, line);
        if (!have) break; // done
        // parse line: user_id,channel,network
        if (line[0] < '0' || line[0] > '9') continue;
        int pos = 0;
        int uid = parseIntField(line, &pos);
        // skip original channel
        while (line[pos] && line[pos] != ',') pos++;
        if (line[pos] == ',') pos++;
        (void)parseIntField(line, &pos);
        // advance to network field
        while (line[pos] && line[pos] != ',') pos++;
        if (line[pos] == ',') pos++;
        char net[CSV_MAX_NET_LEN];
        parseStringField(line, &pos, net, CSV_MAX_NET_LEN);
        // determine network index
        int netIdx = -1;
        if (net[0] == '2') netIdx = 0;
        else if (net[0] == '3') netIdx = 1;
        else if (net[0] == '4') netIdx = 2;
        else if (net[0] == '5') netIdx = 3;
        else netIdx = -1;
        if (netIdx == -1) {
            // unknown network, ignore
            continue;
        }
        // attempt to assign: find first channel with space for that network
        int chCount = NET_CHANNEL_COUNT[netIdx];
        int cap     = NET_CAPACITY[netIdx];
        int assigned = -1;
        if (net_load[netIdx] < chCount * cap) {
            // find the (logical) channel index: simple scheme: assign to next available slot
            // We'll pick the first channel with load < cap by simulating per-channel counts:
            // To avoid per-channel arrays, compute a simple per-channel distribution:
            // But better: maintain per-network per-channel loads — we can store them here local static.
            // For clarity, maintain per-network per-channel loads (static local).
        }
        // We'll implement per-network per-channel loads stored in a static 2D-like array:
        static int perChLoad[NET_COUNT][64]; // safe: max channels <= 64
        // initialize first time (we can check results_count==0 to init)
        static int initialized = 0;
        if (!initialized) {
            initialized = 1;
            int a,b;
            for (a = 0; a < NET_COUNT; ++a) {
                for (b = 0; b < 64; ++b) perChLoad[a][b] = 0;
            }
        }
        // find channel with spare
        int t;
        for (t = 0; t < chCount; ++t) {
            if (perChLoad[netIdx][t] < cap) { assigned = t; break; }
        }
        // store result
        if (results_count < CSV_MAX_USERS) {
            results_userId[results_count] = uid;
            results_netIndex[results_count] = netIdx;
            results_channel[results_count] = assigned;
            results_count++;
        }
        if (assigned == -1) {
            // dropped immediately
            io.outputstring("User ");
            io.outputint(uid);
            io.outputstring(" dropped (no available channel for ");
            if (netIdx==0) io.outputstring("2G");
            else if (netIdx==1) io.outputstring("3G");
            else if (netIdx==2) io.outputstring("4G");
            else io.outputstring("5G");
            io.outputstring(")\n");
        } else {
            perChLoad[netIdx][assigned] += 1;
            net_load[netIdx] += 1;
        }
    }
    return 0;
}

// -------- main menu and launcher ----------
int main() {
    queue_init(&g_queue);

    IO appIO;
    int running = 1;
    while (running) {
        appIO.println("========================================");
        appIO.println("     CELLULAR NETWORK SIMULATOR");
        appIO.println("========================================");
        appIO.println("1. Simulate 2G");
        appIO.println("2. Simulate 3G");
        appIO.println("3. Simulate 4G");
        appIO.println("4. Simulate 5G");
        appIO.println("5. CSV-based multi-network allocation (multithreaded)");
        appIO.println("0. Exit");

        int choice = appIO.readInt("Enter choice: ");
        if (choice == 0) { running = 0; break; }

        if (choice == 5) {
            // launch producer and consumer
            pthread_t prod, cons;
            producer_done = 0;
            results_count = 0;
            int i;
            for (i=0;i<NET_COUNT;++i) net_load[i]=0;
            if (pthread_create(&prod, 0, producer_thread, 0) != 0) {
                io.outputstring("ERROR: cannot create producer thread\n");
                continue;
            }
            if (pthread_create(&cons, 0, consumer_thread, 0) != 0) {
                io.outputstring("ERROR: cannot create consumer thread\n");
                pthread_mutex_lock(&g_queue.lock);
                producer_done = 1;
                pthread_cond_broadcast(&g_queue.not_empty);
                pthread_mutex_unlock(&g_queue.lock);
                pthread_join(prod, 0);
                continue;
            }
            // wait
            pthread_join(prod, 0);
            pthread_join(cons, 0);

            // print summary per network
            io.outputstring("\n=== CSV allocation summary (multi-network) ===\n");
            int n;
            for (n = 0; n < NET_COUNT; ++n) {
                io.outputstring("\nNetwork ");
                if (n==0) io.outputstring("2G");
                else if (n==1) io.outputstring("3G");
                else if (n==2) io.outputstring("4G");
                else io.outputstring("5G");
                io.outputstring(":\n");
                io.outputstring("  Channels: ");
                io.outputint(NET_CHANNEL_COUNT[n]);
                io.outputstring(", Capacity per channel: ");
                io.outputint(NET_CAPACITY[n]);
                io.outputstring("\n  Total assigned users: ");
                io.outputint(net_load[n]);
                io.terminate();
            }

            io.outputstring("\nAssigned users:\n");
            for (int i = 0; i < results_count; ++i) {
                if (results_channel[i] >= 0) {
                    io.outputstring("User ");
                    io.outputint(results_userId[i]);
                    io.outputstring(" -> ");
                    if (results_netIndex[i]==0) io.outputstring("2G");
                    else if (results_netIndex[i]==1) io.outputstring("3G");
                    else if (results_netIndex[i]==2) io.outputstring("4G");
                    else io.outputstring("5G");
                    io.outputstring(" Channel ");
                    io.outputint(results_channel[i]);
                    io.terminate();
                }
            }
            io.outputstring("\nDropped users:\n");
            int dcount = 0;
            for (i = 0; i < results_count; ++i) {
                if (results_channel[i] < 0) {
                    io.outputstring("User ");
                    io.outputint(results_userId[i]);
                    io.outputstring(" (");
                    if (results_netIndex[i]==0) io.outputstring("2G");
                    else if (results_netIndex[i]==1) io.outputstring("3G");
                    else if (results_netIndex[i]==2) io.outputstring("4G");
                    else io.outputstring("5G");
                    io.outputstring(") dropped\n");
                    dcount++;
                }
            }
            if (dcount == 0) io.outputstring("(none)\n");

            // reset for next run
            results_count = 0;
            for (i=0;i<NET_COUNT;++i) net_load[i]=0;
            producer_done = 0;
            continue;
        }

        // choices 1..4 fall back to existing SimulationScenario single-threaded
        ProtocolConfig cfg;
        if (choice == 1) cfg = ProtocolConfig::make2G();
        else if (choice == 2) cfg = ProtocolConfig::make3G();
        else if (choice == 3) cfg = ProtocolConfig::make4G();
        else if (choice == 4) cfg = ProtocolConfig::make5G();
        else { appIO.println("Invalid choice."); continue; }

        int cap = appIO.readInt("Enter max message capacity of core: ");
        int overhead = appIO.readInt("Enter overhead per 100 messages: ");

        CellularCore core(cap, overhead);
        SimulationScenario scenario(cfg, core);
        scenario.run();
        scenario.printSummary(appIO);

        int chIndex;
        while (1) {
            chIndex = appIO.readInt("Enter channel index (-1 to go back): ");
            if (chIndex == -1) break;
            scenario.printChannelUsers(chIndex, appIO);
        }
    }

    queue_destroy(&g_queue);
    return 0;
}
