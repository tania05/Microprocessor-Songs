/**********************************************************************************************************************
File: messaging.h                                                                

Description:
This is a messaging .h file new source code header file.
**********************************************************************************************************************/

#ifndef __MESSAGING_H
#define __MESSAGING_H


/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
/* G_u32MessagingFlags */
#define _MESSAGING_TX_QUEUE_FULL        (u32)0x00000001
#define _DEQUEUE_GOT_NULL               (u32)0x00000002
#define _DEQUEUE_MSG_NOT_FOUND          (u32)0x00000004
  
/* Tx buffer allocation: be cognisant of RAM usage when selecting the two parameters below */
#define TX_QUEUE_SIZE                   (u8)25         /* Number of messages allowed in the queue */
#define MAX_TX_MESSAGE_LENGTH           (u16)100       /* Max bytes in message payload */
#define STATUS_QUEUE_SIZE               (u8)50         /* Number of message statusi to maintain */

#define MSG_STATUS_COMPLETE_TIME        (u32)1000      /* Max time in ms that a message status can sit in the status queue in a COMPLETE state */
#define MSG_STATUS_WAITING_TIME         (u32)1000      /* Max time in ms that a message can sit in the queue in a WAITING state */
#define MSG_STATUS_TIMEOUT_TIME         (u32)1500      /* Max time in ms that a message status can sit in the status queue in a TIMEOUT state */
#define MSG_STATUS_CLEANING_TIME        (u32)1000      /* Time in ms between cleaning the message queue */


/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/
typedef enum {EMPTY = 0, WAITING, SENDING, COMPLETE, TIMEOUT, ABANDONED, NOT_FOUND = 0xff} MessageStateType;

/* Message struct for data messages */
typedef struct
{
  u32 u32Token;                         /* Unigue token for this message */
  u32 u32Size;                          /* Size of the data payload in bytes */
  u8 pu8Message[MAX_TX_MESSAGE_LENGTH]; /* Data payload array */
  void* psNextMessage;                  /* Pointer to next message */
} MessageType;

typedef struct
{
  bool bFree;                           /* TRUE if message slot is available */
  MessageType Message;                  /* The slot's message */
} MessageSlot;

typedef struct
{
  u32 u32Token;                         /* Unigue token for this message; a token is never 0 */
  MessageStateType eState;              /* State of the message */
  u32 u32Timestamp;                     /* Time the message status was last posted */          
} MessageStatus;


/**********************************************************************************************************************
* Function Declarations
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions */
/*--------------------------------------------------------------------------------------------------------------------*/
MessageStateType QueryMessageStatus(u32 u32Token_);


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions */
/*--------------------------------------------------------------------------------------------------------------------*/
void MessagingInitialize(void);

u32 QueueMessage(u32 u32MessageSize_, u8* pu8MessageData_, MessageType** pTargetQueue_);
void DeQueueMessage(MessageType** pTargetQueue_);

void UpdateMessageStatus(u32 u32Token_, MessageStateType eNewState_);


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions */
/*--------------------------------------------------------------------------------------------------------------------*/
static void AddNewMessageStatus(u32 u32Token_);


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
void MessagingIdle(void);             
void MessagingError(void);         


#endif /* __MESSAGING_H */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/
