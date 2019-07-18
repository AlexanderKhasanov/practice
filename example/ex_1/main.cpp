#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>


int main(int argc, char **argv)
{
    LONG result;
    SCARDCONTEXT sc_context;

    // initialize pcsc library
    result = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &sc_context);
    if (result != SCARD_S_SUCCESS) {
        printf("%s\n", pcsc_stringify_error(result));
        return 1;
    }
    printf("Connection to PC/SC established\n");

    // calculate required memory size for a list of readers
    DWORD readers_size;

    result = SCardListReaders(sc_context, NULL, 0, &readers_size);
    if (result != SCARD_S_SUCCESS) {
        SCardReleaseContext(sc_context);
        printf("%s\n", pcsc_stringify_error(result));
        return 1;
    }

    // allocate memory and fetch readers list
    LPSTR readers;
    readers = calloc(1, readers_size);

    result = SCardListReaders(sc_context, NULL, readers, &readers_size);
    if (result != SCARD_S_SUCCESS) {
        SCardReleaseContext(sc_context);
        printf("%s\n", pcsc_stringify_error(result));
        return 1;
    }

    // print found readers, it's a "double-null-terminated string"
    printf("Found readers:\n");
    int n = 0;
    for (int i = 0; i < readers_size - 1; ++i) {
        ++n;
        printf("  Reader #%d: %s\n", n, &readers[i]);
        while (readers[++i] != 0);
    }
    printf("total: %i\n", n);

    // relase connection to library
    result = SCardReleaseContext(sc_context);
    if (result != SCARD_S_SUCCESS) {
        printf("%s\n", pcsc_stringify_error(result));
        return 1;
    }
    printf("Connection to PC/SC closed\n");

    return 0;
}