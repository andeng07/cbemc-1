#ifndef PERMISSION_UTILS_HPP
#define PERMISSION_UTILS_HPP

#include <stdint.h>

class PermissionUtility {
public:
    static bool hasPermission(uint8_t userPermission, uint8_t requiredPermission) {
        return (userPermission & requiredPermission) != 0;
    }

    static uint8_t grant(uint8_t userPermission, uint8_t permissionToAdd) {
        return userPermission | permissionToAdd;
    }

    static uint8_t revoke(uint8_t userPermission, uint8_t permissionToRemove) {
        return userPermission & (~permissionToRemove);
    }
};

#endif
