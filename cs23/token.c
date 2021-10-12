#include <stdio.h>
#include <string.h>

int main() {
        FILE *fp;
        char buf[80][10];
	char str[80][10];
        int j, n = 0;

        fp = fopen("s_2_03_045.s", "r");

        if(fp==NULL) {
                fprintf(stderr, "can't find file!\n");
                exit(1);
        }

        while(fgets(buf, sizeof(buf), fp) != NULL) {
                token(buf, str);
        }

        fclose(fp);
        printf("Number of token: %d\n", n);
}

int token(char *a, char b[][10]) {
        int tmp = 0, n = 0;
	char *word;
        for(word = strtok(a, " \t\n"); word != NULL;) {
		strcpy(b[tmp],word);
		word = strtok(NULL, " \t\n");
		tmp++;
                n++;
	}
        return(n);
}
