#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
int main(int argc, char *argv[])
{
	int fuzzCount = 500;
	char cBuf[250] , *charArray;
	char fileName[40]; 
	int x=0;
	int status, ret, i, j, retCode, arraySize;
	int firstInt, secondInt;
	FILE *fileptr,*fout,*fout_1;
	char *buff;
	long filelen;
	int val,loc;
	time_t t;	
	srand((unsigned) time(&t));  											// randomization of the seed
	
	for(i=0; i<fuzzCount; i++){  
				
		fout = fopen("./test.jpg", "wb");									//file output
		fileptr = fopen("./cross.jpg", "rb"); 						 		//opening file
		fseek(fileptr, 0, SEEK_END);          								//jumping to the end of the file
		filelen = ftell(fileptr);            							 	//current byte offset in the file
		rewind(fileptr);
		buff = (char *)malloc((filelen+1)*sizeof(char)); 
		fread(buff, filelen, 1, fileptr);  								 	//reading file cross.jpg
		buff[filelen]='\0';													//buff has the image and terminated with a null character
		fclose(fileptr); 													//file closed

		//Replacing random location with random values.

		for(i=0; i<2; i++)
		{
			loc=rand()%filelen;
			val=rand()%256;
			buff[loc]=val;
			buff[loc+1]=val;
			buff[loc+2]=val;
			buff[loc+3]=val;
			loc=rand()%filelen;
			val=rand()%512;
			buff[loc]=0;
			buff[loc+1]=0;
			buff[loc+2]=0;
			buff[loc+3]=0;
			buff[loc+4]=val;
		}
		 
	
		fwrite (buff,sizeof(char),filelen,fout);	
		int final=fputs(buff,fout);
		sprintf(cBuf, "./jpgbmp test.jpg temp.bmp");
		fclose(fout);	
		ret = system(cBuf);
		wait(&status); 
		retCode = WEXITSTATUS(ret); 
		if ( retCode == 128+11 || retCode ==128+6)  							// the target code caused segmentation fault (11) or 
																				//Abort (6)
		                                          
			{
				x++;
				sprintf(fileName, "crashed-%d.jpg", x);
				fout_1 = fopen(fileName, "wb");
				fwrite(buff,sizeof(char),filelen,fout_1);
				fclose(fout_1);
				printf("File name is : %s \n",fileName);
				fflush(stdout);
			}
		free(buff); 															// release memory for repeated testing!
	}
   return 0;
}
