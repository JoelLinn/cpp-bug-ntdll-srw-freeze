#include <assert.h>
#include <stdio.h>

#include <Windows.h>

HANDLE mutex;

HANDLE thread_a;
HANDLE thread_b;

// thread main
DWORD WINAPI main_spin(LPCSTR ident) {
    int i = 0;
    while (i < 10) {
        {
            assert(WaitForSingleObject(mutex, INFINITE) == WAIT_OBJECT_0);
            printf("%s spinning... %d\n", ident, ++i);
            ReleaseMutex(mutex);
        }
        Sleep(100);
    }
    return 0;
}

int main()
{
    SetThreadDescription(GetCurrentThread(), L"main");

    mutex = CreateMutexA(NULL, FALSE, "global_critical_region");
    assert(mutex);

    thread_a = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)main_spin, "A", 0, NULL);
    assert(thread_a);
    SetThreadDescription(thread_a, L"main_spin_A");

    thread_b = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)main_spin, "B", 0, NULL);
    assert(thread_b);
    SetThreadDescription(thread_b, L"main_spin_B");

    Sleep(500);

    {
        assert(WaitForSingleObject(mutex, INFINITE) == WAIT_OBJECT_0);
        printf("I'm in danger...\n");
        Sleep(500);
        // Suspend thread 'a' which is trying to lock the mutex_
        SuspendThread(thread_a);
        ReleaseMutex(mutex);
    }

    WaitForSingleObject(thread_b, INFINITE);
    TerminateThread(thread_a, 0);

    CloseHandle(thread_a);
    CloseHandle(thread_b);

    CloseHandle(mutex);

    return 0;
}
