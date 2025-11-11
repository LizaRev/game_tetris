#include <iostream>
#include <bitset>
using namespace std;
const uint8_t CAN_READ = 1 << 0;
const uint8_t CAN_WRITE = 1 << 1;
const uint8_t CAN_DELETE = 1 << 2;
const uint8_t CAN_ADMIN = 1 << 3;

bool hasPermission ( uint8_t userRight, uint8_t permission ){
        return userRight & permission;
}
