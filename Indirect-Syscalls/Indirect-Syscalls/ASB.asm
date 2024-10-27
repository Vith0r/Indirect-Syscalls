EXTERN SSNtOpenProcess:DWORD
EXTERN AddrNtOpenProcess:QWORD

EXTERN SSNtAllocateVirtualMemory:DWORD
EXTERN AddrNtAllocateVirtualMemory:QWORD

EXTERN SSNtWriteVirtualMemory:DWORD
EXTERN AddrNtWriteVirtualMemory:QWORD

EXTERN SSNtCreateThreadEx:DWORD
EXTERN AddrNtCreateThreadEx:QWORD

EXTERN SSNtWaitForSingleObject:DWORD
EXTERN AddrNtWaitForSingleObject:QWORD

.CODE

NtOpenProcess PROC
    mov r10, rcx
    mov eax, SSNtOpenProcess
    jmp QWORD PTR [AddrNtOpenProcess]
NtOpenProcess ENDP


NtAllocateVirtualMemory PROC
    mov r10, rcx
    mov eax, SSNtAllocateVirtualMemory
    jmp QWORD PTR [AddrNtAllocateVirtualMemory]
NtAllocateVirtualMemory ENDP


NtWriteVirtualMemory PROC
    mov r10, rcx
    mov eax, SSNtWriteVirtualMemory
    jmp QWORD PTR [AddrNtWriteVirtualMemory]
NtWriteVirtualMemory ENDP


NtCreateThreadEx PROC
    mov r10, rcx
    mov eax, SSNtCreateThreadEx
    jmp QWORD PTR [AddrNtCreateThreadEx]
NtCreateThreadEx ENDP


NtWaitForSingleObject PROC
    mov r10, rcx
    mov eax, SSNtWaitForSingleObject
    jmp QWORD PTR [AddrNtWaitForSingleObject]
NtWaitForSingleObject ENDP

END