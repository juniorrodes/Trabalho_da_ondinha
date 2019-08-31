#include<stdlib.h>
#include<stdio.h>
#include<conio.h>
#include<string.h>
#include<locale.h>
#include<string.h>
//#include"C:\Program Files (x86)\FMOD SoundSystem\FMOD Studio API Windows\api\core\inc\fmod.h"
#define HEADER 44
typedef struct{
    int chunkID;
    int chunkSize;
    int format;
    int subChunk1ID;
    int subChunk1Size;
    short int audioFormat;
    short int numChannels;
    int sampleRate;
    int byteRate;
    short int blockAlign;
    short int bitsPerSample;
    int subChunk2ID;
    int subChunk2Size;
}fileHeader;

int headerReader(fileHeader*, FILE*);
int fileCut(FILE*,FILE* , int, int, fileHeader*);
int copyHeaderToNewFile(FILE*, FILE*);
int checkBlock(char*, int);
void menu(){
    system("cls");
    puts("1 - Exibir infos do arquivo.");
    puts("2 - Cortar o arquivo.");
    puts("3 - Sair.");
}
void displayInfo(int, int, char);
void stringConcatenate(char*, char*);
void getTime(int*, int*);


int main (void){

    setlocale(LC_ALL, "");
    fileHeader cabecalho;
    FILE *originalFile, *file_x;
    char fileName[100], fileXName[100];
    puts("Entre com o nome do arquivo");
    gets(fileName);
    stringConcatenate(fileName, fileXName);
    if((originalFile = fopen(strcat(fileName, ".wav") ,"rb")) == NULL){
        perror("Aconteceu o seguinte erro: ");
        return -1;
    }
    if((file_x = fopen(strcat(fileXName, "_x.wav"), "wb")) == NULL){
        perror("Aconteceu o seguinte erro ao criar o arquivo. ");
        return -1;
    }
    switch (headerReader(&cabecalho, originalFile)){
    case -1:
        puts("Houve um erro na leitura do arquivo");
    case -2:
        puts("Este arquivo não tem RIFF.");
        return -2;
        break;
    case -3:
        puts("Este arquivo não é do tipo WAVE.");
        return -3;
        break;
    case -4:
        puts("Este arquivo não tem fmt ");
        return -4;
        break;
    default:
        puts("Arquivos abertos com sucesso, aperte qualquer tecla para avançar.");
        break;
    }
    getch();
    int ctrl;
    do{
        menu();
        ctrl = getch() - '0';
        switch(ctrl){
        case 1:
            displayInfo(cabecalho.chunkID, sizeof(int), 'C');
            displayInfo(cabecalho.chunkSize, sizeof(int), 'I');
            displayInfo(cabecalho.sampleRate, sizeof(int), 'I');
            displayInfo(cabecalho.byteRate, sizeof(int), 'I');
            puts("\nPressione qualquer tecla para continuar.");
            getch();
            break;
        case 2:
            puts("Entre com o tempo desejado para o corte em segundos.(inical e final)");
            int initialTime, finalTime;
            getTime(&initialTime, &finalTime);
            switch(fileCut(originalFile, file_x, initialTime, finalTime, &cabecalho)){
            case -1:
                puts("Houve algum erro com a letirura do arquivo");
                break;
            case -2:
                puts("Houve algum erro na escrita no arquivo");
                break;
            default:
                puts("Corte completo, aperte qualquer tecla para continuar");
                break;
                getch();
            }
            break;
        }
    }while(ctrl != 3);
    fclose(originalFile);
    fclose(file_x);

}
void getTime(int *initialTime, int *finalTime){
    do{
        if(*initialTime >= *finalTime){
            system("cls");
            puts("Insira primeiro o tempo inicial e depois o final.");
        }else if(*finalTime > 220){
            system("cls");
            puts("insira um tempo final menor que 220 segundos");
        }
        scanf("%d", initialTime);
        scanf("%d", finalTime);
    }while((*initialTime >= *finalTime) || (*finalTime > 220));

}

void stringConcatenate(char *str, char *newstr){
    int i = 0;
    while(*(str + i) != '\0'){
        *(newstr + i) = *(str + i);
        i++;
        if(*(str + i) == '\0'){
            *(newstr + i) = '\0';
        }
    }
}

int GetIntValue(char *arr, int size){  // transforma um little endian em um big endian e atribui o resultado em um inteiro
    int i = size - 1; // define quantos ciclos serão necessarios
    int value = 0;
    while(i >= 0 && (*(arr + i) == 0)){ //testa para descobrir o primeiro valor não nulo e sua posição
        i--;
    }
    for(int j = 0; j <= i; j++){
        value += (*(arr + i - j) & 0xFF) << ((i - j) * 8); // atribui os valovres do vetor para a variavel value
        //printf("value: %X | info[%d]: %02X\n", value, j, *(arr + i - j));
    }
    return value; // retorna o valor de value
}

int checkBlock(char *info, int infoToCompare){
    if((*info != ((infoToCompare >> 24) & 0xff)) || (*(info + 1) != ((infoToCompare >> 16) & 0xff)) || (*(info + 2) != ((infoToCompare >> 8) & 0xff)) || (*(info + 3) != (infoToCompare  & 0xff)))
        return -1;
    else
        return 0;
}

int headerReader(fileHeader *cabecalho, FILE *ogFile){
    rewind(ogFile);
    char info[4];
    if((fread(info, sizeof(char), 4, ogFile)) != 4){
        return -1;
    }
    if(checkBlock(info, 0x52494646) != 0){
        return -2;
    }
    cabecalho->chunkID = GetIntValue(info, sizeof(int));
    memset(info, 0, sizeof(info));
    if((fread(info, sizeof(char), 4, ogFile)) != 4){
        return -1;
    }
    cabecalho->chunkSize = GetIntValue(info, sizeof(int)) + 8;
    memset(info, 0, sizeof(info));
    if((fread(info, sizeof(char), 4, ogFile)) != 4){
        return -1;
    }
    if(checkBlock(info, 0x57415645) != 0){
        return -3;
    }
    cabecalho->format = GetIntValue(info, sizeof(int));
    memset(info, 0, sizeof(info));
    if((fread(info, sizeof(char), 4, ogFile)) != 4){
        return -1;
    }
    if(checkBlock(info, 0x666D7420) != 0){
        return -4;
    }
    cabecalho->subChunk1ID = GetIntValue(info, sizeof(int));
    memset(info, 0, sizeof(info));
    if((fread(info, sizeof(char), 4, ogFile)) != 4){
        return -1;
    }
    cabecalho->subChunk1Size = GetIntValue(info, sizeof(int));
    memset(info, 0, sizeof(info));
    if((fread(info, sizeof(char), 4, ogFile)) != 4){
        return -1;
    }
    cabecalho->audioFormat = GetIntValue(info, sizeof(int)/2);
    cabecalho->numChannels = GetIntValue(&info[2], sizeof(int)/2);
    memset(info, 0, sizeof(info));
    if((fread(info, sizeof(char), 4, ogFile)) != 4){
        return -1;
    }
    cabecalho->sampleRate = GetIntValue(info, sizeof(int));
    memset(info, 0, sizeof(info));
    if((fread(info, sizeof(char), 4, ogFile)) != 4){
        return -1;
    }
    cabecalho->byteRate = GetIntValue(info, sizeof(int));
    memset(info, 0, sizeof(info));
    if((fread(info, sizeof(char), 4, ogFile)) != 4){
        return -1;
    }
    cabecalho->blockAlign = GetIntValue(info, sizeof(int)/2);
    cabecalho->bitsPerSample = GetIntValue(&info[2], sizeof(int)/2);
    memset(info, 0, sizeof(info));

    if((fread(info, sizeof(char), 4, ogFile)) != 4){
        return -1;
    }
    cabecalho->subChunk2ID = GetIntValue(info, sizeof(int));
    memset(info, 0, sizeof(info));
    if((fread(info, sizeof(char), 4, ogFile)) != 4){
        return -1;
    }
    cabecalho->subChunk2Size = GetIntValue(info, sizeof(int));
    memset(info, 0, sizeof(info));
    return 0;
}

void displayInfo(int infoInt, int _size, char type){
    char infoChar[_size];
    switch(type){
    case 'C':
        for(int i = 0; i < _size; i++){
            infoChar[i] = (infoInt >> 8 * i) & 0xFF;
            printf("%c", infoChar[i]);
        }
        break;
    case 'I':
        printf("%d", infoInt);
    }
    puts("");
}

int copyHeaderToNewFile(FILE* originalFile, FILE* newFile){
    char info;
    rewind(originalFile);
    for(int i = 0; i < HEADER; i++){
        if(fread(&info, sizeof(char), 1, originalFile) != 1)
            return -1;

        if(fwrite(&info, sizeof(char), 1, newFile) != 1)
            return -1;
    }
    return 0;
}

int fileCut(FILE* originalFile, FILE* file_x, int init, int fin, fileHeader* cabecalho){
    char info;
    int i = 0;
    rewind(originalFile);
    rewind(file_x);
    copyHeaderToNewFile(originalFile, file_x);
    fseek(originalFile, init * cabecalho->byteRate, SEEK_SET);
    while(i < (fin - init) * cabecalho->byteRate){
        if(fread(&info, sizeof(char), 1, originalFile) != 1)
            return -1;
        if(fwrite(&info, sizeof(char), 1, file_x) != 1)
            return -2;
        i++;
    }
    return 0;
}
