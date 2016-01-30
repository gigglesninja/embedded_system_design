#ifndef EMBEDDED_LAB_H
#define	EMBEDDED_LAB_H

#include <string.h>

typedef struct
{
    char *psz_name;
    char *psz_netID;
    uint8_t u8_teamID;
    uint8_t u8_memberID;
} CAN_ID;

#define NUM_OF_IDS 10 // 8 Students + 1 TA + 1 Professor

#define teamIDMask   0b0000011100000000
#define memberIDMask 0b0000000011100000
#define typeIDMask   0b0000000000011111
#define TEAM_ID_SHIFT_AMOUNT   8
#define MEMBER_ID_SHIFT_AMOUNT 5
#define TYPE_ID_SHIFT_AMOUNT   0

// Message Types
#define MSG_TYPE_POLL           0
#define MSG_TYPE_UNUSED         1
#define MSG_TYPE_TEMPERATURE    2
#define MSG_TYPE_POTENTIOMETER  3
#define MSG_TYPE_FREQUENCY      4
#define MSG_TYPE_AMPLITUDE      5
#define MSG_TYPE_DUTYCYCLE      6
#define MSG_TYPE_LEDS           7

// CAN ID Table
const CAN_ID aCANID_IDs[NUM_OF_IDS] = {
//"Name",            "netID",    teamID,    memberID,   // Array Index     // MSG ID
 {"J.D. Ford",       "jdf249",   1,         1 },        // 0                120
 {"Wyatt Gamble",    "jwg254",   1,         2 },        // 1                140
 {"Andrew Hegman",   "arh426",   1,         3 },        // 2                160
 {"James Tate",      "jct322",   1,         4 },        // 3                180
 {"Blake Brockner",  "bab334",   2,         1 },        // 4                200
 {"Alex Campbell",   "ajc269",   2,         2 },        // 5                220
 {"John Corn",       "jc1485",   2,         3 },        // 6                240
 {"George Hilliard", "gh403",    2,         4 },        // 7                260
 {"Drew Taylor",     "rat56",    3,         1 },        // 8                280
 {"J.W. Bruce",      "jwb",      3,         2 }         // 9                300
};

#define MY_ID             3 // Look above for your array index
#define MY_NAME           (aCANID_IDs[MY_ID].psz_name)
#define MY_NETID          (aCANID_IDs[MY_ID].psz_netID)
#define MY_TEAM_ID        (aCANID_IDs[MY_ID].u8_teamID)
#define MY_MEMBER_ID      (aCANID_IDs[MY_ID].u8_memberID)
#define MY_MSG_ID(type)   ((MY_TEAM_ID << TEAM_ID_SHIFT_AMOUNT) | (MY_MEMBER_ID << MEMBER_ID_SHIFT_AMOUNT) | (type & typeIDMask))

uint16_t calcMsgID(uint8_t u8_arrayIndex)
{
    return (aCANID_IDs[u8_arrayIndex].u8_teamID << TEAM_ID_SHIFT_AMOUNT) | (aCANID_IDs[u8_arrayIndex].u8_memberID << MEMBER_ID_SHIFT_AMOUNT);
}

uint8_t stripTeamID(uint16_t u16_MSG_ID)
{
    return (u16_MSG_ID & teamIDMask)    >> TEAM_ID_SHIFT_AMOUNT;
}

uint8_t stripMemberID(uint16_t u16_MSG_ID)
{
    return (u16_MSG_ID & memberIDMask)  >> MEMBER_ID_SHIFT_AMOUNT;
}

uint8_t stripTypeID(uint16_t u16_MSG_ID)
{
    return (u16_MSG_ID & typeIDMask)    >> TYPE_ID_SHIFT_AMOUNT;
}

int8_t getArrayIndexFromMsgID(uint16_t u16_MSG_ID)
{
    int8_t i8_arrayIndex = -1; // Default - Not Found (-1)
    uint8_t u8_arrayIndex;
	
    for (u8_arrayIndex = 0; u8_arrayIndex < NUM_OF_IDS; u8_arrayIndex++)
	{
        if (calcMsgID(u8_arrayIndex) == (u16_MSG_ID & (teamIDMask | memberIDMask)))
		{
            i8_arrayIndex = u8_arrayIndex;
            break;
        }
    }
	
    return i8_arrayIndex;
}

int8_t getNameFromMsgID(uint16_t u16_MSG_ID, char *psz_name, uint8_t u8_len)
{
    int8_t i8_arrayIndex;
	
    i8_arrayIndex = getArrayIndexFromMsgID(u16_MSG_ID);
    if (i8_arrayIndex != -1) // If the Message ID was found
	{
        strncpy(psz_name, aCANID_IDs[i8_arrayIndex].psz_name, u8_len);
        return 0;
    }
	
    return -1;
}

#endif	/* EMBEDDED_LAB_H */
