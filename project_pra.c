#include "project_pra.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>


/* Macro Definitions */

#define evaluate(i) \
  { \
    switch( ckt->gate[i].type ) { \
    case PI: \
      break; \
    case PO: \
    case BUF: \
      gate_val[i].out_val_A = gate_val[i].in_val_A[0]; \
      gate_val[i].out_val_B = gate_val[i].in_val_B[0]; \
      break; \
    case PO_GND: \
      gate_val[i].out_val_A = 0x00; \
      gate_val[i].out_val_B = 0x00; \
      break; \
    case PO_VCC: \
      gate_val[i].out_val_A = 0xFFFFFFFF; \
      gate_val[i].out_val_B = 0xFFFFFFFF; \
      break; \
    case INV: \
      gate_val[i].out_val_A = ~gate_val[i].in_val_B[0]; \
      gate_val[i].out_val_B = ~gate_val[i].in_val_A[0]; \
      break; \
    case AND: \
      gate_val[i].out_val_A = gate_val[i].in_val_A[0] & gate_val[i].in_val_A[1]; \
      gate_val[i].out_val_B = gate_val[i].in_val_B[0] & gate_val[i].in_val_B[1]; \
      break; \
    case NAND: \
      gate_val[i].out_val_A = ~(gate_val[i].in_val_B[0] & gate_val[i].in_val_B[1]); \
      gate_val[i].out_val_B = ~(gate_val[i].in_val_A[0] & gate_val[i].in_val_A[1]); \
      break; \
    case OR: \
      gate_val[i].out_val_A = gate_val[i].in_val_A[0] | gate_val[i].in_val_A[1]; \
      gate_val[i].out_val_B = gate_val[i].in_val_B[0] | gate_val[i].in_val_B[1]; \
      break; \
    case NOR: \
      gate_val[i].out_val_A = ~(gate_val[i].in_val_B[0] | gate_val[i].in_val_B[1]); \
      gate_val[i].out_val_B = ~(gate_val[i].in_val_A[0] | gate_val[i].in_val_A[1]); \
      break; \
    default: \
      assert(0); \
    } \
  }

void delete_fault (circuit_t *ckt, fault_list_t **undetected_flist, fault_list_t **fptr, fault_list_t **prev_fptr) {

  int g_ind = (*fptr)->gate_index;
  int i_ind = (*fptr)->input_index;
  int type = (*fptr)->type;
  int g_search;
  fault_list_t *new_fptr;
  fault_list_t *new_prev_fptr = *prev_fptr;
  /* remove fault from undetected fault list */
  if ( *prev_fptr == (fault_list_t *)NULL ) {
    /* if first fault in fault list, advance head of list pointer */
    *undetected_flist = (*fptr)->next;
  }
  else {
    /* if not first fault in fault list, then remove link */
    (*prev_fptr)->next = (*fptr)->next;
  }

  if ((i_ind >= 0) && (ckt->gate[ckt->gate[g_ind].fanin[i_ind]].num_fanout == 1)) {
    g_search = ckt->gate[g_ind].fanin[i_ind];
    for (new_fptr = (*fptr)->next; new_fptr != (fault_list_t *)NULL; new_fptr = new_fptr->next) {
      //printf ("Gate_index: %d, Input Index: %d, Fault Type: %d\n", new_fptr->gate_index, new_fptr->input_index, new_fptr->type);
      if ((new_fptr->gate_index == g_search) && (new_fptr->input_index == -1) && (new_fptr->type == type)) {
	delete_fault(ckt, undetected_flist, &new_fptr, &new_prev_fptr);
	break;
      }
      else if (new_fptr->gate_index < g_search)
	break;
      else
	new_prev_fptr = new_fptr;
    }
  }

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
  int detected_flag, j, pat_pos, bit_A, bit_B;
  gate_val_t gate_val[MAX_GATES];
  int bit_mask[MAX_PO], corr_out_A[MAX_PO], corr_out_B[MAX_PO], corr_in_A[MAX_PI], corr_in_B[MAX_PI], flt_bit_mask;
  /*************************/
  /* fault-free simulation */
  /*************************/

    //for (i = 0; i < ckt->ngates; i++) {
	//printf ("Gate Index: %d, Gate Type: %d Gate Fanin: %d %d\n", i, ckt->gate[i].type, ckt->gate[i].fanin[0], ckt->gate[i].fanin[1]);
    //}
  /* loop through all patterns */
  for (p = 0; p < (pat->len >> PAT_POW) + 1; p++) {
    //printf ("Pattern LOOP: %d\n", p);
    /* assign primary input values for pattern */
    for (i = 0; i < ckt->npi; i++) {
      pat_pos = (p << PAT_POW);
      for ( j = 0; j < PAT_LEN; j++) {
        switch(pat->in[pat_pos][i]) {
          case LOGIC_0:
            gate_val[ckt->pi[i]].out_val_A &= ~(0x01 << j);
            gate_val[ckt->pi[i]].out_val_B &= ~(0x01 << j);
	    break;
          case LOGIC_1:
            gate_val[ckt->pi[i]].out_val_A |= (0x01 << j);
            gate_val[ckt->pi[i]].out_val_B |= (0x01 << j);
	    break;
          case LOGIC_X:
            gate_val[ckt->pi[i]].out_val_A &= ~(0x01 << j);
            gate_val[ckt->pi[i]].out_val_B |= (0x01 << j);
	    break;
          default:
	    assert(0);
        }
        pat_pos++;
        if (pat_pos >= pat->len)
          break;
      }
      corr_in_A[i] = gate_val[ckt->pi[i]].out_val_A;
      corr_in_B[i] = gate_val[ckt->pi[i]].out_val_B;
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
	gate_val[i].in_val_A[0] = gate_val[ckt->gate[i].fanin[0]].out_val_A;
	gate_val[i].in_val_B[0] = gate_val[ckt->gate[i].fanin[0]].out_val_B;
	break;
      /* gates with two input terminals */
      case AND:
      case NAND:
      case OR:
      case NOR:
	gate_val[i].in_val_A[0] = gate_val[ckt->gate[i].fanin[0]].out_val_A;
	gate_val[i].in_val_B[0] = gate_val[ckt->gate[i].fanin[0]].out_val_B;
	gate_val[i].in_val_A[1] = gate_val[ckt->gate[i].fanin[1]].out_val_A;
	gate_val[i].in_val_B[1] = gate_val[ckt->gate[i].fanin[1]].out_val_B;
	break;
      default:
	assert(0);
      }
      /* compute gate output value */
      evaluate(i);
//	printf ("Gate_ID: %d, Gate_Out_A; %x, Gate_Out_B: %x\n", i, gate_val[i].out_val_A, gate_val[i].out_val_B);
    }
    /* put fault-free primary output values into pat data structure */
    for (i = 0; i < ckt->npo; i++) {
      corr_out_A[i] = gate_val[ckt->po[i]].out_val_A;
      corr_out_B[i] = gate_val[ckt->po[i]].out_val_B;
      bit_mask[i] = 0;
      pat_pos = (p << PAT_POW);
      for (j = 0; j < PAT_LEN; j++) {
        bit_A = gate_val[ckt->po[i]].out_val_A & 0x01;
        bit_B = gate_val[ckt->po[i]].out_val_B & 0x01;
	if ((bit_A == 0) && (bit_B == 0)) {
	  pat->out[pat_pos][i] = LOGIC_0;
	  bit_mask[i] |= (0x01 << j);
	}
	else if ((bit_A == 1) && (bit_B == 1)) {
	  pat->out[pat_pos][i] = LOGIC_1;
	  bit_mask[i] |= (0x01 << j);
	}
	else if ((bit_A == 0) && (bit_B == 1)) {
	  pat->out[pat_pos][i] = LOGIC_X;
	  bit_mask[i] &= ~(0x01 << j);
	}
	//printf ("pat_pos: %d pat_out: %d\n", pat_pos, pat->out[pat_pos][i]);
        gate_val[ckt->po[i]].out_val_A >>= 1;
        gate_val[ckt->po[i]].out_val_B >>= 1;
	pat_pos++;
	if (pat_pos >= pat->len)
	  break;
      }
      //printf ("out_A: %x, out_B: %x, bit_mask: %x\n", corr_out_A[i], corr_out_B[i], bit_mask[i]);
    }
  /********************/
  /* fault simulation */
  /********************/

  /* loop through all undetected faults */
  prev_fptr = (fault_list_t *)NULL;
  for (fptr=undetected_flist; fptr != (fault_list_t *)NULL; fptr=fptr->next) {
    /* loop through all patterns */
    detected_flag = FALSE;
    /* assign primary input values for pattern */
    for (i = 0; i < ckt->npi; i++) {
      gate_val[ckt->pi[i]].out_val_A = corr_in_A[i];
      gate_val[ckt->pi[i]].out_val_B = corr_in_B[i];
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
	  gate_val[i].in_val_A[0] = gate_val[ckt->gate[i].fanin[0]].out_val_A;
	  gate_val[i].in_val_B[0] = gate_val[ckt->gate[i].fanin[0]].out_val_B;
	  break;
	  /* gates with two input terminals */
	case AND:
	case NAND:
	case OR:
	case NOR:
	  gate_val[i].in_val_A[0] = gate_val[ckt->gate[i].fanin[0]].out_val_A;
	  gate_val[i].in_val_A[1] = gate_val[ckt->gate[i].fanin[1]].out_val_A;
	  gate_val[i].in_val_B[0] = gate_val[ckt->gate[i].fanin[0]].out_val_B;
	  gate_val[i].in_val_B[1] = gate_val[ckt->gate[i].fanin[1]].out_val_B;
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
	      gate_val[i].in_val_A[fptr->input_index] = 0x00;
	      gate_val[i].in_val_B[fptr->input_index] = 0x00;
	    }
	    else { /* S_A_1 */
	      gate_val[i].in_val_A[fptr->input_index] = 0xFFFFFFFF;
	      gate_val[i].in_val_B[fptr->input_index] = 0xFFFFFFFF;
	    }
	    /* compute gate output value */
	    evaluate(i);
	  }
	  else { /* fault at output */
	    evaluate(i);
	    /* inject fault */
	    if ( fptr->type == S_A_0 ) {
	      gate_val[i].out_val_A = 0x00;
	      gate_val[i].out_val_B = 0x00;
	    }
	    else { /* S_A_1 */
	      gate_val[i].out_val_A = 0xFFFFFFFF;
	      gate_val[i].out_val_B = 0xFFFFFFFF;
	    }
	  }
	}
	else { /* not faulty gate */
	  /* compute gate output value */
	  evaluate(i);
	}
      }
      /* check if fault detected */
      for (i = 0; i < ckt->npo; i++) {
	flt_bit_mask = ~(gate_val[ckt->po[i]].out_val_A ^ gate_val[ckt->po[i]].out_val_B) & bit_mask[i];
	if ((gate_val[ckt->po[i]].out_val_B & flt_bit_mask) != (corr_out_B[i] & flt_bit_mask)) {
	    detected_flag = TRUE;
	    break;
	}
      }
      if ( detected_flag ) {
        delete_fault(ckt, &undetected_flist, &fptr, &prev_fptr);
      }
      else { /* fault remains undetected, keep on list */
        prev_fptr = fptr;
      }
    }
  }
/*
  for (fptr=undetected_flist; fptr != (fault_list_t *)NULL; fptr=fptr->next) {
    printf ("Fault Gate: %d Input Index: %d Fault Type: %d\n", fptr->gate_index, fptr->input_index, fptr->type);
  }
*/
  return(undetected_flist);
}
