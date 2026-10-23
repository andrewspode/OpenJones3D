#include <unity_fixture.h>

#include <stdint.h>
#include <string.h>

#include <sith/Gameplay/sithEvent.h>

// The free event buffer table and its helpers are not exposed by the header
extern SithEvent aEventBuffer[256];
extern int aFreeEventBufferIdxs[256];
extern size_t numFreeEventBuffers;
void sithEvent_ResetFreeBufferTable(void);
SithEvent* sithEvent_Create(void);

TEST_GROUP(sithEvent);

TEST_SETUP(sithEvent)
{}

TEST_TEAR_DOWN(sithEvent)
{}

// The table lists the buffers in reverse order, so that sithEvent_Create hands out the buffers 0, 1, 2... in turn.
TEST(sithEvent, ResetFreeBufferTableListsAllBuffers)
{
    sithEvent_ResetFreeBufferTable();

    TEST_ASSERT_EQUAL_UINT(256, numFreeEventBuffers);
    for ( int i = 0; i < 256; ++i )
    {
        TEST_ASSERT_EQUAL_INT(255 - i, aFreeEventBufferIdxs[i]);
    }

    for ( size_t i = 0; i < 256; ++i )
    {
        TEST_ASSERT_EQUAL_PTR(&aEventBuffer[i], sithEvent_Create());
    }
    TEST_ASSERT_EQUAL_UINT(0, numFreeEventBuffers);
}

// The original also stored -1 behind the last table entry (one past the end of the array); that store is gone.
TEST(sithEvent, ResetFreeBufferTableStaysInsideTable)
{
    // The word behind the table is whatever the linker placed there (another variable or padding). Put a marker
    // in it and check that the marker survives, unless the word is the buffer count, which the function sets.
    int* pBehind = aFreeEventBufferIdxs + 256;
    if ( (void*)pBehind == (void*)&numFreeEventBuffers )
    {
        TEST_IGNORE_MESSAGE("The buffer count follows the table, an overwrite cannot be told from its update");
    }

    int saved = *pBehind;
    *pBehind  = 0x5A5A5A5A;
    sithEvent_ResetFreeBufferTable();
    int after = *pBehind;
    *pBehind  = saved;

    TEST_ASSERT_EQUAL_HEX32(0x5A5A5A5A, after);
}

TEST_GROUP_RUNNER(sithEvent)
{
    RUN_TEST_CASE(sithEvent, ResetFreeBufferTableListsAllBuffers);
    RUN_TEST_CASE(sithEvent, ResetFreeBufferTableStaysInsideTable);
}
