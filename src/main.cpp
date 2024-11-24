#include <coreinit/screen.h>
#include <coreinit/cache.h>
#include <whb/log_cafe.h>
#include <whb/log_udp.h>
#include <whb/log.h>
#include <whb/proc.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

int main(int argc, char** argv) {
    WHBLogCafeInit();
    WHBLogUdpInit();
    WHBLogPrint("Hello World !");

    WHBProcInit();

    OSScreenInit();

    size_t tvBufferSize = OSScreenGetBufferSizeEx(SCREEN_TV);
    size_t drcBufferSize = OSScreenGetBufferSizeEx(SCREEN_DRC);
    WHBLogPrintf("Will allocate 0x%X bytes for the TV, " \
                 "and 0x%X bytes for the DRC.",
                 tvBufferSize, drcBufferSize);

    void* tvBuffer = memalign(0x100, tvBufferSize);
    void* drcBuffer = memalign(0x100, drcBufferSize);

    if (!tvBuffer || !drcBuffer) {
        WHBLogPrint("Out of memory !");

        if (tvBuffer) free(tvBuffer);
        if (drcBuffer) free(drcBuffer);

        OSScreenShutdown();
        WHBProcShutdown();

        WHBLogPrint("Exiting..");
        WHBLogCafeDeinit();
        WHBLogUdpDeinit();

        return 1;
    }

    OSScreenSetBufferEx(SCREEN_TV, tvBuffer);
    OSScreenSetBufferEx(SCREEN_DRC, drcBuffer);

    OSScreenEnableEx(SCREEN_TV, true);
    OSScreenEnableEx(SCREEN_DRC, true);

    while(WHBProcIsRunning()) {
        OSScreenClearBufferEx(SCREEN_TV, 0x00000000);
        OSScreenClearBufferEx(SCREEN_DRC, 0x00000000);

        OSScreenPutFontEx(SCREEN_TV, 0, 0, "Hello TV !");
        OSScreenPutFontEx(SCREEN_TV, 0, 1, "TV TEXT");

        OSScreenPutFontEx(SCREEN_DRC, 0, 0, "Hello Gamepad");
        OSScreenPutFontEx(SCREEN_DRC, 0, 1, "DRC TEXT");

        DCFlushRange(tvBuffer, tvBufferSize);
        DCFlushRange(drcBuffer, drcBufferSize);

        OSScreenFlipBuffersEx(SCREEN_TV);
        OSScreenFlipBuffersEx(SCREEN_DRC);
    }

    WHBLogPrint("Shutdown console..");

    if (tvBuffer) free(tvBuffer);
    if (drcBuffer) free(drcBuffer);

    OSScreenShutdown();
    WHBProcShutdown();

    WHBLogPrint("Exiting..");
    WHBLogCafeDeinit();
    WHBLogUdpDeinit();

    return 1;
}
