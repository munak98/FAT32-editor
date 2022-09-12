# FAT32-editor
Repositório para desenvolvimento do trabalho final da disciplina Fundamentos de Sistemas Operacionais,  da Universidade de Brasília. (2022)

# Setup
Atualize a linha 17 da função main com o *path* do pen drive conectado ao seu computador.
Compile o projeto com `make` no diretório raiz e execute com `sudo ./exec` (é preciso executar com permissão para acessar e editar o pen drive)

# Help
O programa mostra um prompt de comando que permite navegar pelos diretórios do pen drive e fazer modificações nos estados dos arquivos.
Os seguintes comandos são aceitos pelo prompt:

- `ls` -> mostra os arquivos no diretório atual; flags `--hidden` (para mostrar também os arquivos ocultos) ou `--deleted` (para mostrar também os arquivos deletados)
  
 - `cat [nome do arquivo]` -> mostra o conteúdo do arquivo
  
- `cd [nome do diretorio]` -> muda o diretório corrente

- `delete [nome do arquivo]` -> deleta o arquivo

- `undelete [nome do arquivo]` -> restaura o arquivo

- `hide [nome do arquivo]` -> oculta o arquivo

- `unhide [nome do arquivo oculto]` -> desoculta o arquivo

# Observações

O nome de arquivos e diretórios deve ser sempre o nome final, o programa não aceita *paths*.
Para desocultar um arquivo, deve ser utilizado o seu nome oculto, ou seja, incluindo o '.' (por exemplo, ".arquivo.txt")

