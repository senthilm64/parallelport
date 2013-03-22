#include<stdio.h>
#include<unistd.h>
int main()
{
  unsigned char byte,dummy;
  FILE *f;
  f=fopen("/dev/pport","w");
  setvbuf(f,&dummy,_IONBF,1);
  byte=1;
  while(1)
    {
      printf("Byte is %d\n",byte);
      fwrite(&byte,1,1,f);
      sleep(10);
      byte<<=1;
      if(byte==0)
	byte+=1;
}
  fclose(f);
  return 0;
}
