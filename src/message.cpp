typedef struct message_t
{
    unsigned char mac[6];
    long int time;
    float humidity;
    float temp;
};