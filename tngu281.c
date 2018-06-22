#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

void addAnd(int base)
{
    int i = 0;
    char *op1, *op2, *op3;
    op1 = strtok(NULL, ",");
    op2 = strtok(NULL, ",");
    op3 = strtok(NULL, ",");
    if (op3[0] != '#')
    {
        i = base + (op1[1] - 0x30) * 0x200 + (op2[1] - 0x30) * 0x40 + (op3[1] - 0x30);
    }
    else
    {
        op3++;
        if (op3[0] == '-')
        {
            op3++;
            i = base + (op1[1] - 0x30) * 0x200 + (op2[1] - 0x30) * 0x40 + 0x40 - atoi(op3);
        }
        else
        {
            i = base + (op1[1] - 0x30) * 0x200 + (op2[1] - 0x30) * 0x40 + atoi(op3) + 0x20;
        }
    }
    printf("%x\n", i);
}
void jmp()
{
    int i = 0;
    char *op;
    op = strtok(NULL, ",");
    i = 0xc000 + (op[1] - 0x30) * 0x40;
    printf("%x\n", i);
}
void fill(int PC, char *label, FILE *out)
{
    fprintf(out, "%s %x\n", label, PC);
}

int main(int argc, char *argv[])
{
    FILE *file, *symbolTable;
    char *token;
    char line[20] = "";
    int lineCounter = 0;
    file = fopen(argv[1], "r");
    symbolTable = fopen("SymbolTable", "w");
    do
    {
        int counter = 0;
        char c = fgetc(file);
        if (c == EOF)
        {
            break;
        }
        while ((c != '\n') && (c != EOF))
        {
            line[counter++] = c;
            c = fgetc(file);
        }
        token = strtok(line, " ");
        if (strcmp(token, ".orig") == 0)
        {
            char *orig = strtok(NULL, " ");
            orig++;
            lineCounter = (int)strtol(orig, NULL, 16);
            lineCounter--;
        }
        else if (strcmp(token, "add") == 0)
        {
            addAnd(0x1000);
        }
        else if (strcmp(token, "and") == 0)
        {
            addAnd(0x5000);
        }
        else if (strcmp(token, "jmp") == 0)
        {
            jmp();
        }
        else if (strcmp(token, "halt") == 0)
        {
            printf("f025\n");
        }
        else
        {
            char *temp = strtok(NULL, " ");
            if ((temp != NULL) && (strcmp(temp, ".fill") == 0))
            {
                fill(lineCounter, token, symbolTable);
            }
        }
        memset(line, 0, 20);
        lineCounter++;
    } while (true);
    fclose(file);
    fclose(symbolTable);
}