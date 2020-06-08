#include <assert.h>
#include <stdio.h>

#include <Windows.h>

SRWLOCK rwlock;

HANDLE thread_a;
HANDLE thread_b;

// thread main
DWORD WINAPI main_spin(LPCSTR ident) {
    int i = 0;
    while (i < 10) {
        {
            AcquireSRWLockExclusive(&rwlock);
            printf("%s spinning... %d\n", ident, ++i);
            ReleaseSRWLockExclusive(&rwlock);
        }
        Sleep(100);
    }
    return 0;
}

int main()
{
    SetThreadDescription(GetCurrentThread(), L"main");

    InitializeSRWLock(&rwlock);

    thread_a = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)main_spin, "A", 0, NULL);
    assert(thread_a);
    SetThreadDescription(thread_a, L"main_spin_A");

    thread_b = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)main_spin, "B", 0, NULL);
    assert(thread_b);
    SetThreadDescription(thread_b, L"main_spin_B");

    Sleep(500);

    {
        AcquireSRWLockExclusive(&rwlock);
        printf("I'm in danger...\n");
        Sleep(500);
        // Suspend thread 'a' which is trying to lock the mutex_
        SuspendThread(thread_a);
        ReleaseSRWLockExclusive(&rwlock);
    }

    WaitForSingleObject(thread_b, INFINITE);
    TerminateThread(thread_a, 0);

    CloseHandle(thread_a);
    CloseHandle(thread_b);

    return 0;
}
