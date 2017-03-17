
#include "project.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

/* Macro Definitions */
#define compute_INV(result,val) result = (~val)

#define compute_AND(result,val1,val2) result = (val1 & val2)

#define compute_NAND(result,val1,val2) result = (~(val1 & val2))

#define compute_OR(result,val1,val2) result = (val1 | val2)

#define compute_NOR(result,val1,val2) result = (~(val1 | val2))

#define extract(result,val,valid_pats,pos) result = ( ( (3<<((valid_pats-1-pos)*2) & val) >> ((valid_pats-1-pos)*2) ) & 3 )

#define evaluate(gate) \
  { \
    switch( gate.type ) { \
    case PI: \
      break; \
    case PO: \
    case BUF: \
      gate.out_val = gate.in_val[0]; \
      break; \
    case PO_GND: \
      gate.out_val = LOGIC_0; \
      break; \
    case PO_VCC: \
      gate.out_val = LOGIC_1; \
      break; \
    case INV: \
      compute_INV(gate.out_val,gate.in_val[0]); \
      break; \
    case AND: \
      compute_AND(gate.out_val,gate.in_val[0],gate.in_val[1]); \
      break; \
    case NAND: \
      compute_NAND(gate.out_val,gate.in_val[0],gate.in_val[1]); \
      break; \
    case OR: \
      compute_OR(gate.out_val,gate.in_val[0],gate.in_val[1]); \
      break; \
    case NOR: \
      compute_NOR(gate.out_val,gate.in_val[0],gate.in_val[1]); \
      break; \
    default: \
      assert(0); \
    } \
  }

/*************************************************************************
                            USER DEFINED 
*************************************************************************/
int gen_PPat(pattern_t *pat, int pat_start, int bit_pos){ //Generates 32 bit ip with atmost 16 patterns
	int i; static int PPat;
	int parlen = pat->len;
	PPat = 0;
	for (i=pat_start; (i<(pat_start+16))&&(i<parlen) ; i++){
		//generate PPat
		PPat=PPat<<2;
		switch(pat->in[i][bit_pos]){
			case 0: PPat|=0; break;
			case 1: PPat|=3; break;
			case 2: PPat|=1; break;
			default: assert(0);
		}
	}
	//printf ("\nGererated 32 bit pattern is %d\n",PPat);
	return PPat;
}

/*************************************************************************

Function:  three_val_fault_simulate

Purpose:  This function performs fault simulation on 3-valued input patterns.

pat.out[][] is filled with the fault-free output patterns corresponding to
the input patterns in pat.in[][].

Return:  List of faults that remain undetected.

*************************************************************************/

fault_list_t *three_val_fault_simulate(ckt,pat,undetected_flist)
     circuit_t *ckt;
     pattern_t *pat;
     fault_list_t *undetected_flist;	
{
	int p;  /* looping variable for pattern number */
	int i;
	fault_list_t *fptr, *prev_fptr;
	int detected_flag;

  /*************************/
  /* fault-free simulation */
  /*************************/

  /* loop through all patterns */
     //printf("\nStarting faultfree sim\n");
	int p_16;
     for ( p_16 = 0; ((p_16*16) < pat->len) ; p_16++) {
          /* initialize all gate values to UNDEFINED */
     	for (i = 0; i < ckt->ngates; i++) {
     		ckt->gate[i].in_val[0] = UNDEFINED;
     		ckt->gate[i].in_val[1] = UNDEFINED;
     		ckt->gate[i].out_val = UNDEFINED;
     	}
     	/* assign primary input values for pattern */
     	 for (i = 0; i < ckt->npi; i++) {
     	 	int PPat = gen_PPat (pat,(p_16*16),i);
     	 	ckt->gate[ckt->pi[i]].out_val = PPat;
     	 }
     	 /* evaluate all gates */
     	 for (i = 0; i < ckt->ngates; i++) {
     	 	/* get gate input values */
     		 switch ( ckt->gate[i].type ) {
     		 /* gates with no input terminal */
     		 case PI:
     		 case PO_GND:
     		 case PO_VCC:
    			 	break;
     		 /* gates with one input terminal */
     		 case INV:
     		 case BUF:
     		 case PO:
			 	ckt->gate[i].in_val[0] = ckt->gate[ckt->gate[i].fanin[0]].out_val;
				break;
     		 /* gates with two input terminals */
     		 case AND:
     		 case NAND:
     		 case OR:
     		 case NOR:
			 	ckt->gate[i].in_val[0] = ckt->gate[ckt->gate[i].fanin[0]].out_val;
			 	ckt->gate[i].in_val[1] = ckt->gate[ckt->gate[i].fanin[1]].out_val;
			 	break;
     		 default:
			 	assert(0);
     		 }
     		 /* compute gate output value */
     		 evaluate(ckt->gate[i]);
     	  }//gates loop end			
    		  /* put fault-free primary output values into pat data structure */
     	  int valid_pats = ((p_16*16)+16 < pat->len)? 16 : (16 - (((p_16*16)+16) - pat->len));
		  printf ("\nValid_pats for p_16 = %d is %d",p_16,valid_pats);
		  int j,PO;
     	  for (i = 0; i < ckt->npo; i++) {
    		  	for (j=0; j<valid_pats; j++){
				extract(PO,ckt->gate[ckt->po[i]].out_val,valid_pats,j);
				//printf ("PO = %d\n",PO);
				printf ("\n i = %d, j= %d",(p_16*16)+j,i);
				switch (PO){
				case 0: pat->out[((p_16*16)+j)][i] = 0;break;
				case 3: pat->out[((p_16*16)+j)][i] = 1;break;
				case 1: pat->out[((p_16*16)+j)][i] = 2;break;
				case 2: pat->out[((p_16*16)+j)][i] = 2;break;
				default: assert(0);
				}
			}
		 }	
	//printf ("\nFF evaluation complete for p_16 = %d",p_16);
	/*	
	printf ("\nEvaluation complete for p_16 = %d\n",p_16);
	printf ("Evaluation is result = %d\n",ckt->gate[ckt->po[0]].out_val);
	printf ("Evaluation for gate 0 = %d\n",ckt->gate[0].out_val);
	printf ("Evaluation for gate 1 = %d\n",ckt->gate[1].out_val);
	printf ("Evaluation for gate 2 ip A = %d\n",ckt->gate[2].in_val[0]);
	printf ("Evaluation for gate 2 in B = %d\n",ckt->gate[2].in_val[1]);
	printf ("Evaluation for gate 2 = %d\n",ckt->gate[2].out_val);
	printf ("Evaluation for gate 3 = %d\n",ckt->gate[3].out_val);
	*/	
     }//pattern loop
  //printf ("\nFaultfree simulation conplete\n");
  /********************/
  /* fault simulation */
  /********************/

  /* loop through all undetected faults */
  prev_fptr = (fault_list_t *)NULL;
  for (fptr=undetected_flist; fptr != (fault_list_t *)NULL; fptr=fptr->next) {
     /* loop through all patterns */
  	detected_flag = FALSE;
	int p_16;
     for ( p_16 = 0; ((p_16*16) < pat->len) && !detected_flag; p_16++) {
     	/* initialize all gate values to UNDEFINED */
     	for (i = 0; i < ckt->ngates; i++) {
			ckt->gate[i].in_val[0] = UNDEFINED;
			ckt->gate[i].in_val[1] = UNDEFINED;
			ckt->gate[i].out_val = UNDEFINED;
      	}

     	/* assign primary input values for pattern */
      	for (i = 0; i < ckt->npi; i++) {
      		int PPat = gen_PPat (pat,(p_16*16),i);
	 		//printf ("\nAssigning ip pattern for i = %d is %d\n",i,PPat);
      		ckt->gate[ckt->pi[i]].out_val = PPat;
      	}

      	/* evaluate all gates */
 		for (i = 0; i < ckt->ngates; i++) {
			/* get gate input values */
			switch ( ckt->gate[i].type ) {
			  /* gates with no input terminal */
			case PI:
			case PO_GND:
			case PO_VCC:
			  break;
			  /* gates with one input terminal */
			case INV:
			case BUF:
			case PO:
			  ckt->gate[i].in_val[0] = ckt->gate[ckt->gate[i].fanin[0]].out_val;
			  break;
			  /* gates with two input terminals */
			case AND:
			case NAND:
			case OR:
			case NOR:
			  ckt->gate[i].in_val[0] = ckt->gate[ckt->gate[i].fanin[0]].out_val;
			  ckt->gate[i].in_val[1] = ckt->gate[ckt->gate[i].fanin[1]].out_val;
			  break;
			default:
			  assert(0);
			}
			/* check if faulty gate */
			if ( i == fptr->gate_index ) {
			  /* check if fault at input */
			  if ( fptr->input_index >= 0 ) {
			    /* inject fault */
			    if ( fptr->type == S_A_0 ) {
			      ckt->gate[i].in_val[fptr->input_index] = 0; //changed
			    }
			    else { /* S_A_1 */
			      ckt->gate[i].in_val[fptr->input_index] = -1;
			    }
			    /* compute gate output value */
			    evaluate(ckt->gate[i]);
			  }
			  else { /* fault at output */
			    evaluate(ckt->gate[i]);
			    /* inject fault */
			    if ( fptr->type == S_A_0 ) {
			      ckt->gate[i].out_val = 0;
			    }
			    else { /* S_A_1 */
			      ckt->gate[i].out_val = -1;
			    }
			  }
			}
			else { /* not faulty gate */
			  /* compute gate output value */
			  evaluate(ckt->gate[i]);
			}
	 	}//gate loop
		/*
	 	printf ("\nEvaluation complete for p_16 = %d\n",p_16);
	 	printf ("Evaluation is result = %d\n",ckt->gate[ckt->po[0]].out_val);
	 	printf ("Evaluation for gate 0 = %d\n",ckt->gate[0].out_val);
	 	printf ("Evaluation for gate 1 = %d\n",ckt->gate[1].out_val);
	 	printf ("Evaluation for gate 2 ip A = %d\n",ckt->gate[2].in_val[0]);
	 	printf ("Evaluation for gate 2 in B = %d\n",ckt->gate[2].in_val[1]);
	 	printf ("Evaluation for gate 2 = %d\n",ckt->gate[2].out_val);
	 	printf ("Evaluation for gate 3 = %d\n",ckt->gate[3].out_val);
		*/
	 	/* check if fault detected */
     	int valid_pats = ((p_16*16)+16 < pat->len)? 16 : (16 - (((p_16*16)+16) - pat->len));
  		int j;unsigned int PO;
  		for (i = 0; i < ckt->npo; i++) {
  		  	for (j=0; j<valid_pats; j++){//can break this loop early
  		   		extract(PO,ckt->gate[ckt->po[i]].out_val,valid_pats,j);
  		    	 	if ( (PO == LOGIC_0_n) && ( pat->out[((p_16*16)+j)][i] == LOGIC_1) ){
  		    	 		detected_flag = TRUE;
  		    	 		break;
  		    	 	}
  		    	 	if ( (PO == LOGIC_1_n) && ( pat->out[((p_16*16)+j)][i] == LOGIC_0) ){
  		    	 		detected_flag = TRUE;
  		    	 		break;
  		    	 	}
  		     }
  		}
    }//pattern loop - also breaks with DETECTED FLAG
    if ( detected_flag ) {
      /* remove fault from undetected fault list */
    	 if ( prev_fptr == (fault_list_t *)NULL ) {
	 /* if first fault in fault list, advance head of list pointer */
	 	undetected_flist = fptr->next;
      }
      else { /* if not first fault in fault list, then remove link */
	 	prev_fptr->next = fptr->next;
      }
    }
    else { /* fault remains undetected, keep on list */
    	 prev_fptr = fptr;
    }
  }//fault loop
  return(undetected_flist);
}
