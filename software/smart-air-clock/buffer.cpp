#include "buffer.h"

// 240 metingen = 2 uur bij een uploadinterval van 30 s (ongeveer 7 kB RAM)
#define BUFFER_CAPACITY 240

static BufferedMeasurement items[BUFFER_CAPACITY];
static int head = 0;    // index van de oudste meting
static int count = 0;

void bufferPush(const BufferedMeasurement& m)
{
    if(count == BUFFER_CAPACITY)
    {
        // Vol: oudste overschrijven
        head = (head + 1) % BUFFER_CAPACITY;
        count--;
    }

    items[(head + count) % BUFFER_CAPACITY] = m;
    count++;
}

bool bufferPeek(BufferedMeasurement& m)
{
    if(count == 0)
    {
        return false;
    }

    m = items[head];
    return true;
}

void bufferPop()
{
    if(count == 0)
    {
        return;
    }

    head = (head + 1) % BUFFER_CAPACITY;
    count--;
}

int bufferCount()
{
    return count;
}
