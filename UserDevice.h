#ifndef USERDEVICE_H
#define USERDEVICE_H

class UserDevice {
private:
    int id;
    int messagesRequired;

public:
    UserDevice();
    UserDevice(int id, int messagesRequired);

    int getId() const;
    int getMessagesRequired() const;
};

#endif // USERDEVICE_H
