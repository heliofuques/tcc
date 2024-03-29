#define RG_TX_REQ ((volatile unsigned char *) (0x11001000))
#define RG_TX_REQ_ACK ((volatile unsigned char *) (0x11001001))
#define RG_TX_WRITE_HEADER ((volatile unsigned char *) (0x11001002))
#define RG_TX_WRITE_DATA ((volatile unsigned char *) (0x11001004))

// @brief: Require permission to peripheral to send a message.
// Blocking method.
void requireToSend()
{
    printf("RNS: Requiring write permission\n");
    (*(char*) RG_TX_REQ) = 1;
    while(*RG_TX_REQ_ACK != 1);
    printf("RNS: Ready to send!\n");
}

// @brief: Send given message filling the header with parameter sender and size.
void sendMessage(int size, int sender,char* data)
{
    int i = 0;
    // Write header
    *RG_TX_WRITE_HEADER = sender;
    *RG_TX_WRITE_HEADER = size;
    
    for(i = 0; i < size; i++ )
    {
       *RG_TX_WRITE_DATA = data[i]; 
    }
    printf("RNS: Sent a message!\n");
}