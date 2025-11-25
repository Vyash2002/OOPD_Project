#ifndef CHANNEL_H
#define CHANNEL_H

// Maximum users per channel — adjust if needed
#define CHANNEL_MAX_USERS 256

class UserDevice;

class Channel {
private:
    int index;
    int maxUsers;
    UserDevice* users[CHANNEL_MAX_USERS];  
    int userCount;

public:
    Channel(int indexValue, int maxUsersValue);

    int getIndex() const;
    int getMaxUsers() const;
    int getCurrentUserCount() const;

    bool addUser(UserDevice* user);  // false if full

    // Return raw array pointer (NO STL)
    UserDevice** getUsers();

    // For safe iteration
    int getUserCount() const;
};

#endif // CHANNEL_H
