#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

struct ldstr
{
	int location;
	char name[3];
};
struct label
{
	int location;
	char name[3];
};

int addAnd(int base)
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
	return i;
}
int jmp()
{
	int i = 0;
	char *op;
	op = strtok(NULL, ",");
	i = 0xc000 + (op[1] - 0x30) * 0x40;
	return i;
}
void fill(int PC, char *label, FILE *out)
{
	fprintf(out, "%s %x\n", label, PC);
}
int ld(struct ldstr *ld, int lineCounter)
{
	int i = 0;
	char *op1, *op2;
	op1 = strtok(NULL, ",");
	op2 = strtok(NULL, ",");
	ld->location = lineCounter;
	strcpy(ld->name, op2);
	i = 0x2000 + (op1[1] - 0x30) * 0x200;
	return i;
}

int main(int argc, char *argv[])
{
	struct ldstr ldtable[100];
	struct label labeltable[50];
	int labelno, ldno = 0;
	FILE *file, *symbolTable;
	char *token;
	char line[20] = "";
	int lineCounter = 0;
	int origMem = 0;
	int program[1000];
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
			origMem = (int)strtol(orig, NULL, 16);
			lineCounter = 0;
		}
		else if (strcmp(token, "add") == 0)
		{
			program[lineCounter] = addAnd(0x1000);
			lineCounter++;
		}
		else if (strcmp(token, "and") == 0)
		{
			program[lineCounter] = addAnd(0x5000);
			lineCounter++;
		}
		else if (strcmp(token, "jmp") == 0)
		{
			program[lineCounter] = jmp();
			lineCounter++;
		}
		else if (strcmp(token, "halt") == 0)
		{
			program[lineCounter] = 0xf025;
			lineCounter++;
		}
		else if (strcmp(token, "ld") == 0)
		{
			program[lineCounter] = ld(&ldtable[ldno], lineCounter);
			ldno++;
			lineCounter++;
		}
		else
		{
			char *temp = strtok(NULL, " ");
			if ((temp != NULL) && (strcmp(temp, ".fill") == 0))
			{
				fill(lineCounter, token, symbolTable);
				labeltable[labelno].location = lineCounter;
				strcpy(labeltable[labelno].name, token);
				labelno++;
				lineCounter++;
			}
		}
		memset(line, 0, 20);
	} while (true);
	fclose(file);
	fclose(symbolTable);
	for (int e = 0; e < labelno; e++)
	{
		lineCounter--;
	}
	for (int i = 0; i < ldno; i++)
	{
		for (int e = 0; e < labelno; e++)
		{
			if (strcmp(ldtable[i].name, labeltable[e].name) == 0)
			{
				program[ldtable[i].location] += labeltable[e].location - ldtable[i].location - 1;
			}
		}
	}
	for (int i = 0; i < lineCounter; i++)
	{
		printf("%x\n", program[i]);
	}
}