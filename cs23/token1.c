#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
        FILE *fp;
        char buf[80];
        int n = 0;

        fp = fopen("s_2_03_045.s", "r");

        if(fp==NULL) {
                fprintf(stderr, "cant find file\n");
                exit(1);
        }

        while(fgets(buf, sizeof(buf), fp) != NULL) {
                n += token(buf);
        }

        fclose(fp);
        printf("Number of token: %d\n", n);
}

int token(char *a) {
        int n = 0;
        char *word;
        for(word = strtok(a, " \t\n"); word != NULL;) {
		n++;
                word = strtok(NULL, " \t\n");
        }
        return(n);
}
