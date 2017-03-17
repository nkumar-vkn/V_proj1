
#include "project.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>


//int *gen_PPat(pattern_t *pat, int bit_pos, int pat_start){
int *gen_PPat(int (*pat)[2], int bit_pos, int pat_start){
	int i; static int PPat[2];
	int parlen = 2;
	PPat[0] = 0; PPat[1] = 0;
	for (i=pat_start; (i<(pat_start+2))&&(i<parlen) ; i++){
		//generate PPat
		PPat[0]=PPat[0]<<2;
//		switch(pat->in.[pat_start][bit_pos])
		switch(pat[i][bit_pos]){
			case 0: PPat[0]|=0; break;
			case 1: PPat[0]|=3; break;
			case 2: PPat[0]|=1; break;
			default: assert(0);
		}
	}
	PPat[1] = (pat_start+2 < parlen)? 0 : ((pat_start+2) - parlen)*2;
	return PPat;
}	


int main()
{

	int (*pat)[2];
	int val[2][2];
	val[0][0] = 0;
	val[0][1] = 1;
	val[1][0] = 1;
	val[1][1] = 2;
   pat = val;

	int *ans = gen_PPat (pat,0,1);
	printf ("\nanswer is %d, %d\n",ans[0],ans[1]);
	/*
	int *k;
	int val = 3;
	k = &val;
	printf ("k=%d",*k);
	*/
}
	
