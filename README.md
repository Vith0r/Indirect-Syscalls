# Indirect Syscalls

⚠️ **Aviso**  
> **Este código foi desenvolvido para fins educativos e de pesquisa em segurança. Não o utilize para propósitos maliciosos.**

## Visão Geral do Projeto

Este projeto demonstra o uso de syscalls indiretos através da biblioteca **ntdll.dll**. para execução de uma shellcode em um processo alvo.

### Como funciona:

1. **RC4 para Descriptografia da Shellcode:**  
   Uma shellcode criptografada é armazenada no código. A função `RC4DEC` utiliza o algoritmo RC4 para descriptografar a shellcode antes de sua execução, impedindo que o código esteja em texto claro na memória.

2. **Recuperação de Syscalls:**  
   O código usa a função `obterSSNeEndereco` para identificar o System Service Number (SSN) e o endereço das syscalls específicas (como `NtOpenProcess`, `NtAllocateVirtualMemory`, etc.) e armazena essas informações para o uso.

3. **Abertura do Processo Alvo:**  
   Após localizar o processo alvo pelo nome, o código utiliza `NtOpenProcess` para obter um handle com as permissões necessárias para execução de código remoto.

4. **Alocação e Escrita de Memória no Processo Alvo:**  
   Utilizando `NtAllocateVirtualMemory`, uma região de memória é alocada no processo alvo, onde a shellcode descriptografada é escrita com `NtWriteVirtualMemory`.

5. **Execução da Shellcode:**  
   Após a escrita da shellcode na memória do processo alvo, `NtCreateThreadEx` é utilizada para iniciar a execução em uma nova thread. `NtWaitForSingleObject` é chamada para aguardar a conclusão da execução.

## Estrutura do Código

### Funções Principais

- **`GetProcessIdByName`**:  
   Função que encontra o ID do processo alvo com base no nome.

- **`RC4DEC`**:  
   Implementa o algoritmo de criptografia RC4 para descriptografar a shellcode antes de sua execução.

- **`obterSSNeEndereco`**:  
   Esta função obtém o System Service Number (SSN) e o endereço da syscall especificada.

### Chamadas Indiretas

As chamadas indiretas são executadas utilizando SSNs e endereços específicos das funções dentro da **ntdll.dll**, e não através de chamadas de API padrão. Isso inclui funções críticas como:

- **`NtOpenProcess`**:  
   Abre o processo alvo e obtém permissões para a execução de código.

- **`NtAllocateVirtualMemory`**:  
   Aloca memória no processo alvo para armazenar a shellcode.

- **`NtWriteVirtualMemory`**:  
   Escreve a shellcode descriptografada na memória do processo alvo.

- **`NtCreateThreadEx`**:  
   Cria uma nova thread no processo alvo, iniciando a execução da shellcode na memória do processo.

- **`NtWaitForSingleObject`**:  
   Aguarda a conclusão da execução da shellcode.

## VirusTotal 4/73
![result](https://i.imgur.com/0SGTpMD.png)

## Vídeo de Demonstração

<a href="https://www.youtube.com/watch?v=csRmxOoYsU8" target="_blank">
    <img src="https://img.youtube.com/vi/csRmxOoYsU8/0.jpg" alt="Assistir vídeo" width="800" />
</a>
