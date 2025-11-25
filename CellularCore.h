#ifndef CELLULARCORE_H
#define CELLULARCORE_H

class CellularCore {
private:
    int maxMessagesCapacity;     // total messages core can handle
    int overheadPer100Messages;  // overhead per 100 user messages

public:
    CellularCore();
    CellularCore(int maxMessagesCapacity, int overheadPer100Messages);

    void setMaxMessagesCapacity(int cap);
    void setOverheadPer100Messages(int overhead);

    int getMaxMessagesCapacity() const;
    int getOverheadPer100Messages() const;

    // Computes max users core can support, given messages per user
    int computeMaxUsers(int messagesPerUser) const;
};

#endif // CELLULARCORE_H
