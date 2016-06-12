#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
char* encrypte( char* subText, int a, int b , int part){
	int i, x, encryptedNum;
	for(i = 0; i < part ; i++){
			x = subText[i] - 97;
			encryptedNum = ((a * x) + b ) % 26;
			subText[i] = (encryptedNum + 97);
	}
    return subText;
}
int main(int argc, char** argv){

	int my_rank, p ,mod = 0 ,part, length , a = 5, b = 7, count , i;
	char* text;
	char* subText;
	char* reminder;
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if(my_rank == 0){

        text = malloc(1000 * sizeof(char));
		printf("%s\n", "Enter your message");
		scanf ( " %[^\n]s",text);
		length = strlen(text);
		
		printf("Text length %d\n" , length);
		printf("%s\n", " ---------------------------------------------------------------		");
		
		part = length / p;
		mod = length % p;

		if( mod != 0){

            reminder = malloc(mod * sizeof(char));
            count = 0;
            for( i = strlen(text)-mod ; i < strlen(text) ; i++){
                reminder[count] = text[i];
                count = count + 1;

            }

          reminder = encrypte( reminder , a , b, mod);
            for( i = 0 ; i < mod ; i++){
                // printf("reminder %c \n" ,  reminder[i] );
                  text[i+ (strlen(text)-mod)] = reminder[i];
                 // printf("text[i] %c \n" ,  text[i+ (strlen(text)-mod)] );
            }

		}
	}

	MPI_Bcast(&part, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	subText = malloc(part * sizeof(char));
	
	MPI_Scatter(text,part, MPI_CHAR, subText, part, MPI_CHAR, 0, MPI_COMM_WORLD);
	
	subText = encrypte(subText, a, b, part);
	
	MPI_Gather(subText,part, MPI_CHAR, text, part, MPI_CHAR, 0, MPI_COMM_WORLD);

	if(my_rank == 0){

		printf("%s\n", "<< Encrypted Text >>");
        printf("%s%s%s\n" , "   \"", text, "\"");
		
		int x, encryptedNum;
		for( i = 0 ; i < strlen(text) ; i++){
            if( text[i] == '['){
                text[i] = ' ';
                 continue;
            }
            x = text[i] - 97;
            if( x>=2 && x <= 6)
            encryptedNum = (b-x)*a;
            else if( x == 0 )
                encryptedNum = 9;
            else if( x == 1)
                encryptedNum = 4;
           	else
                encryptedNum = (21 * (x - b )) % 26;
            
            text[i] = encryptedNum + 97;
		}

		printf("%s\n", " ---------------------------------------------------------------		");
		printf("%s\n", "<< Decrypted Text >>");
		printf("%s%s%s\n", "   \"", text, "\"");
		free(text);
	}

	free(subText);
	if( mod !=0 && my_rank == 0)
        free(reminder);
	MPI_Finalize();
	return 0;
}