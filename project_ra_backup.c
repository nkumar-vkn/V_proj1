#include "project_ra.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>


/* Macro Definitions */

#define compute_INV(result,val) \
  { \
    switch (val) { \
    case LOGIC_0: \
      result = LOGIC_1; \
      break; \
    case LOGIC_1: \
      result = LOGIC_0; \
      break; \
    case LOGIC_X: \
      result = LOGIC_X; \
      break; \
    default: \
      assert(0); \
    } \
  }

#define compute_AND(result,val1,val2) \
    switch (val1) { \
    case LOGIC_0: \
      result = LOGIC_0; \
      break; \
    case LOGIC_1: \
      result = val2; \
      break; \
    case LOGIC_X: \
      if ( val2 == LOGIC_0 ) result = LOGIC_0; \
      else result = LOGIC_X; \
      break; \
    default: \
      assert(0); \
    } 

#define compute_OR(result,val1,val2) \
  { \
    switch (val1) { \
    case LOGIC_1: \
      result = LOGIC_1; \
      break; \
    case LOGIC_0: \
      result = val2; \
      break; \
    case LOGIC_X: \
      if ( val2 == LOGIC_1 ) result = LOGIC_1; \
      else result = LOGIC_X; \
      break; \
    default: \
      assert(0); \
    } \
  }

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
      compute_AND(gate.out_val,gate.in_val[0],gate.in_val[1]); \
      compute_INV(gate.out_val,gate.out_val); \
      break; \
    case OR: \
      compute_OR(gate.out_val,gate.in_val[0],gate.in_val[1]); \
      break; \
    case NOR: \
      compute_OR(gate.out_val,gate.in_val[0],gate.in_val[1]); \
      compute_INV(gate.out_val,gate.out_val); \
      break; \
    default: \
      assert(0); \
    } \
  }


void update_inp_fault_list(circuit_t *ckt, gate_fault_t *gate_flt, int i, int no_of_inp) {
  //printf("Hello ");
  int j;
  int copy_index = ckt->gate[i].fanin[0];
  int total = gate_flt[copy_index].out_fault;

  for (j = 0; j < total; j++) {
    gate_flt[i].in_fault_list[0][j].gate_index = gate_flt[copy_index].out_fault_list[j].gate_index;
    gate_flt[i].in_fault_list[0][j].input_index = gate_flt[copy_index].out_fault_list[j].input_index;
    gate_flt[i].in_fault_list[0][j].type = gate_flt[copy_index].out_fault_list[j].type;
    gate_flt[i].in_fault_list[0][j].status = gate_flt[copy_index].out_fault_list[j].status;
  }
  if (ckt->gate[i].in_val[0] != LOGIC_X) {
    gate_flt[i].in_fault_list[0][total].gate_index = i;
    gate_flt[i].in_fault_list[0][total].input_index = 0;
    gate_flt[i].in_fault_list[0][total].type = (ckt->gate[i].in_val[0]) ? S_A_0 : S_A_1;
    gate_flt[i].in_fault_list[0][total].status = DET;
    gate_flt[i].in_fault[0] = total + 1;
  }

  if (no_of_inp == 2) {
    copy_index = ckt->gate[i].fanin[1];
    total = gate_flt[copy_index].out_fault;
    for (j = 0; j < total; j++) {
      gate_flt[i].in_fault_list[1][j].gate_index = gate_flt[copy_index].out_fault_list[j].gate_index;
      gate_flt[i].in_fault_list[1][j].input_index = gate_flt[copy_index].out_fault_list[j].input_index;
      gate_flt[i].in_fault_list[1][j].type = gate_flt[copy_index].out_fault_list[j].type;
      gate_flt[i].in_fault_list[1][j].status = gate_flt[copy_index].out_fault_list[j].status;
    }
    if (ckt->gate[i].in_val[1] != LOGIC_X) {
      gate_flt[i].in_fault_list[1][total].gate_index = i;
      gate_flt[i].in_fault_list[1][total].input_index = 1;
      gate_flt[i].in_fault_list[1][total].type = (ckt->gate[i].in_val[1]) ? S_A_0 : S_A_1;
      gate_flt[i].in_fault_list[1][total].status = DET;
      gate_flt[i].in_fault[1] = total + 1;
    }
  }

}

void update_output_fault_list (circuit_t *ckt, gate_fault_t *gate_flt, int i) {
  
  void input2_list() {
    int j, k, insert_flag = 1;
    for (j = 0; j < gate_flt[i].in_fault[1]; j++) {
      insert_flag = 1;
      for (k = 0; k < gate_flt[i].in_fault[0]; k++) {
	if (gate_flt[i].in_fault_list[1][j].status == UNDET)
	  break;
	if ((gate_flt[i].in_fault_list[1][j].gate_index == gate_flt[i].in_fault_list[0][k].gate_index) &&
	    (gate_flt[i].in_fault_list[1][j].input_index == gate_flt[i].in_fault_list[0][k].input_index) &&
	    (gate_flt[i].in_fault_list[1][j].type == gate_flt[i].in_fault_list[0][k].type)) {
	  insert_flag = 0;
	  break;
	}
      }
      if (insert_flag) {
	gate_flt[i].out_fault_list[gate_flt[i].out_fault].gate_index = gate_flt[i].in_fault_list[1][j].gate_index;
	gate_flt[i].out_fault_list[gate_flt[i].out_fault].input_index = gate_flt[i].in_fault_list[1][j].input_index;
	gate_flt[i].out_fault_list[gate_flt[i].out_fault].type = gate_flt[i].in_fault_list[1][j].type;
	gate_flt[i].out_fault_list[gate_flt[i].out_fault++].status = gate_flt[i].in_fault_list[1][j].status;
      }
    }
  }
  
  void x_input2_list() {
    int j, k, insert_flag = 1;
    for (j = 0; j < gate_flt[i].in_fault[1]; j++) {
      insert_flag = 1;
      for (k = 0; k < gate_flt[i].in_fault[0]; k++) {
	if ((gate_flt[i].in_fault_list[1][j].gate_index == gate_flt[i].in_fault_list[0][k].gate_index) &&
	    (gate_flt[i].in_fault_list[1][j].input_index == gate_flt[i].in_fault_list[0][k].input_index) &&
	    (gate_flt[i].in_fault_list[1][j].type == gate_flt[i].in_fault_list[0][k].type)) {
	  insert_flag = 0;
	  //printf ("Removed insert_flag\n");
	  break;
	}
      }
      if (insert_flag) {
	gate_flt[i].out_fault_list[gate_flt[i].out_fault].gate_index = gate_flt[i].in_fault_list[1][j].gate_index;
	gate_flt[i].out_fault_list[gate_flt[i].out_fault].input_index = gate_flt[i].in_fault_list[1][j].input_index;
	gate_flt[i].out_fault_list[gate_flt[i].out_fault].type = gate_flt[i].in_fault_list[1][j].type;
	gate_flt[i].out_fault_list[gate_flt[i].out_fault++].status = UNDET;
      }
    }
  }
  
  void union_list() {
    int j, k, insert_flag = 1;
    for (j = 0; j < gate_flt[i].in_fault[0]; j++) {
	gate_flt[i].out_fault_list[j].gate_index = gate_flt[i].in_fault_list[0][j].gate_index;
	gate_flt[i].out_fault_list[j].input_index = gate_flt[i].in_fault_list[0][j].input_index;
	gate_flt[i].out_fault_list[j].type = gate_flt[i].in_fault_list[0][j].type;
	gate_flt[i].out_fault_list[j].status = gate_flt[i].in_fault_list[0][j].status;
    }
   gate_flt[i].out_fault = j;
   for (j = 0; j < gate_flt[i].in_fault[1]; j++) {
      insert_flag = 1;
      for (k = 0; k < gate_flt[i].in_fault[0]; k++) {
	if ((gate_flt[i].in_fault_list[1][j].gate_index == gate_flt[i].in_fault_list[0][k].gate_index) &&
	    (gate_flt[i].in_fault_list[1][j].input_index == gate_flt[i].in_fault_list[0][k].input_index) &&
	    (gate_flt[i].in_fault_list[1][j].type == gate_flt[i].in_fault_list[0][k].type)) {
	  if (gate_flt[i].out_fault_list[k].status == UNDET)
	    gate_flt[i].out_fault_list[k].status = gate_flt[i].in_fault_list[1][j].status;
	  insert_flag = 0;
	  break;
	}
      }
      if (insert_flag) {
	gate_flt[i].out_fault_list[gate_flt[i].out_fault].gate_index = gate_flt[i].in_fault_list[1][j].gate_index;
	gate_flt[i].out_fault_list[gate_flt[i].out_fault].input_index = gate_flt[i].in_fault_list[1][j].input_index;
	gate_flt[i].out_fault_list[gate_flt[i].out_fault].type = gate_flt[i].in_fault_list[1][j].type;
	gate_flt[i].out_fault_list[gate_flt[i].out_fault++].status = gate_flt[i].in_fault_list[1][j].status;
      }
    }
  }
  
  void x_union_list() {
    int j, k, insert_flag = 1;
    for (j = 0; j < gate_flt[i].in_fault[0]; j++) {
	gate_flt[i].out_fault_list[j].gate_index = gate_flt[i].in_fault_list[0][j].gate_index;
	gate_flt[i].out_fault_list[j].input_index = gate_flt[i].in_fault_list[0][j].input_index;
	gate_flt[i].out_fault_list[j].type = gate_flt[i].in_fault_list[0][j].type;
	gate_flt[i].out_fault_list[j].status = UNDET;
    }
   gate_flt[i].out_fault = j;
   x_input2_list();
  }
  
  void input1_list() {
    int j, k, insert_flag = 1;
    for (j = 0; j < gate_flt[i].in_fault[0]; j++) {
      insert_flag = 1;
      for (k = 0; k < gate_flt[i].in_fault[1]; k++) {
	if (gate_flt[i].in_fault_list[0][j].status == UNDET)
	  break;
	if ((gate_flt[i].in_fault_list[1][k].gate_index == gate_flt[i].in_fault_list[0][j].gate_index) &&
	    (gate_flt[i].in_fault_list[1][k].input_index == gate_flt[i].in_fault_list[0][j].input_index) &&
	    (gate_flt[i].in_fault_list[1][k].type == gate_flt[i].in_fault_list[0][j].type)) {
	  insert_flag = 0;
	  break;
	}
      }
      if (insert_flag) {
	gate_flt[i].out_fault_list[gate_flt[i].out_fault].gate_index = gate_flt[i].in_fault_list[0][j].gate_index;
	gate_flt[i].out_fault_list[gate_flt[i].out_fault].input_index = gate_flt[i].in_fault_list[0][j].input_index;
	gate_flt[i].out_fault_list[gate_flt[i].out_fault].type = gate_flt[i].in_fault_list[0][j].type;
	gate_flt[i].out_fault_list[gate_flt[i].out_fault++].status = gate_flt[i].in_fault_list[0][j].status;
      }
    }
  }
  void intersect_list() {
    //printf ("Intersect\n");
    int j, k, l = 0;
    for (j = 0; j < gate_flt[i].in_fault[0]; j++) {
      for (k = 0; k < gate_flt[i].in_fault[1]; k++) {
	if ((gate_flt[i].in_fault_list[1][k].gate_index == gate_flt[i].in_fault_list[0][j].gate_index) &&
	    (gate_flt[i].in_fault_list[1][k].input_index == gate_flt[i].in_fault_list[0][j].input_index) &&
	    (gate_flt[i].in_fault_list[1][k].type == gate_flt[i].in_fault_list[0][j].type)) {
	  //printf ("Intersection Passed\n");
	  gate_flt[i].out_fault_list[l].gate_index = gate_flt[i].in_fault_list[0][j].gate_index;
	  gate_flt[i].out_fault_list[l].input_index = gate_flt[i].in_fault_list[0][j].input_index;
	  gate_flt[i].out_fault_list[l].type = gate_flt[i].in_fault_list[0][j].type;
	  gate_flt[i].out_fault_list[l++].status = (gate_flt[i].in_fault_list[0][j].status == UNDET) ? UNDET : gate_flt[i].in_fault_list[1][k].status;
	  break;
	}
      }
    }
    gate_flt[i].out_fault = l;
  }
  void input_list() {
    int j;
    for (j = 0; j < gate_flt[i].in_fault[0]; j++) {
	gate_flt[i].out_fault_list[j].gate_index = gate_flt[i].in_fault_list[0][j].gate_index;
	gate_flt[i].out_fault_list[j].input_index = gate_flt[i].in_fault_list[0][j].input_index;
	gate_flt[i].out_fault_list[j].type = gate_flt[i].in_fault_list[0][j].type;
	gate_flt[i].out_fault_list[j].status = gate_flt[i].in_fault_list[0][j].status;
    }
    gate_flt[i].out_fault = j;
  }


  if ((ckt->gate[i].in_val[0] == LOGIC_X) || (ckt->gate[i].in_val[1] == LOGIC_X))
    x_union_list();
  else if(ckt->gate[i].type == AND) {
    if (ckt->gate[i].out_val == LOGIC_1)
      union_list();
    else if (ckt->gate[i].in_val[0] == LOGIC_1)
      input2_list();
    else if (ckt->gate[i].in_val[1] == LOGIC_1)
      input1_list();
    else
      intersect_list();
  }
  else if (ckt->gate[i].type == OR) {
    if (ckt->gate[i].out_val == LOGIC_0)
      union_list();
    else if (ckt->gate[i].in_val[0] == LOGIC_0)
      input2_list();
    else if (ckt->gate[i].in_val[1] == LOGIC_0)
      input1_list();
    else
      intersect_list();
  }
  else if (ckt->gate[i].type == NAND) {
    if (ckt->gate[i].out_val == LOGIC_0)
      union_list();
    else if (ckt->gate[i].in_val[0] == LOGIC_1)
      input2_list();
    else if (ckt->gate[i].in_val[1] == LOGIC_1)
      input1_list();
    else
      intersect_list();
  }
  else if (ckt->gate[i].type == NOR) {
    if (ckt->gate[i].out_val == LOGIC_1)
      union_list();
    else if (ckt->gate[i].in_val[0] == LOGIC_0)
      input2_list();
    else if (ckt->gate[i].in_val[1] == LOGIC_0)
      input1_list();
    else
      intersect_list();
  }
  else if (ckt->gate[i].type == INV) {
    input_list();
  }
  else if (ckt->gate[i].type == BUF) {
    input_list();
  }
  
  if (ckt->gate[i].out_val != LOGIC_X) {
    gate_flt[i].out_fault_list[gate_flt[i].out_fault].gate_index = i;
    gate_flt[i].out_fault_list[gate_flt[i].out_fault].input_index = -1;
    gate_flt[i].out_fault_list[gate_flt[i].out_fault].type = (ckt->gate[i].out_val) ? S_A_0 : S_A_1;
    gate_flt[i].out_fault_list[gate_flt[i].out_fault++].status = DET;
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
  //printf ("Hello\n");
  int p;  /* looping variable for pattern number */
  int i, j;
  fault_list_t *fptr, *prev_fptr;
  int detected_flag;
  static gate_fault_t gate_flt[MAX_GATES];

  /*************************/
  /* fault-free simulation */
  /*************************/

  /* loop through all patterns */
  for (p = 0; p < pat->len; p++) {
    printf ("\nPattern %d: ",p);
    /* initialize all gate values to UNDEFINED */
    for (i = 0; i < ckt->ngates; i++) {
      ckt->gate[i].in_val[0] = UNDEFINED;
      ckt->gate[i].in_val[1] = UNDEFINED;
      ckt->gate[i].out_val = UNDEFINED;
      //initialize the no. of faults to 0 for all gates.
      gate_flt[ckt->gate[i].index].in_fault[0] = 0;
      gate_flt[ckt->gate[i].index].in_fault[1] = 0;
      gate_flt[ckt->gate[i].index].out_fault = 0;
    }

    /* assign primary input values for pattern */
    for (i = 0; i < ckt->npi; i++) {
      printf ("%d ", pat->in[p][i]);
      ckt->gate[ckt->pi[i]].out_val = pat->in[p][i];
    }
    printf("\n");

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
        update_inp_fault_list(ckt, gate_flt, i, 1);
	break;
      /* gates with two input terminals */
      case AND:
      case NAND:
      case OR:
      case NOR:
	ckt->gate[i].in_val[0] = ckt->gate[ckt->gate[i].fanin[0]].out_val;
	ckt->gate[i].in_val[1] = ckt->gate[ckt->gate[i].fanin[1]].out_val;
        update_inp_fault_list(ckt, gate_flt, i, 2);
	break;
      default:
	assert(0);
      }
      /* compute gate output value */
      evaluate(ckt->gate[i]);
      update_output_fault_list(ckt, gate_flt, i);
    }
    
    // Update Undetected Fault List by removing faults at POs
    for (i = 0; i < ckt->npo; i++) {
      pat->out[p][i] = ckt->gate[ckt->po[i]].out_val;
      for (j = 0; j < gate_flt[ckt->po[i]].in_fault[0]; j++) {
	if (gate_flt[ckt->po[i]].in_fault_list[0][j].status == UNDET)
	  continue;
/*	int g_ind = gate_flt[ckt->po[i]].in_fault_list[0][j].gate_index;
	int i_ind = gate_flt[ckt->po[i]].in_fault_list[0][j].input_index;
	int typ = gate_flt[ckt->po[i]].in_fault_list[0][j].type;
	int st = gate_flt[ckt->po[i]].in_fault_list[0][j].status;
	printf ("Fault Gate: %d, Gate Name: %s, Fault Pin: %d, Fault Type: %d Fault Status: %d\n",
		g_ind, ckt->gate[g_ind].name, i_ind, typ, st);*/
	prev_fptr = (fault_list_t *)NULL;
        for (fptr = undetected_flist; fptr != (fault_list_t *)NULL; fptr=fptr->next) {
	  if ((gate_flt[ckt->po[i]].in_fault_list[0][j].gate_index == fptr->gate_index) &&
	      (gate_flt[ckt->po[i]].in_fault_list[0][j].input_index == fptr->input_index) &&
	      (gate_flt[ckt->po[i]].in_fault_list[0][j].type == fptr->type)) {
	    //Remove from fault list
	    if (prev_fptr == (fault_list_t *)NULL)
	      undetected_flist = fptr->next;
	    else
	      prev_fptr->next = fptr->next;
	    break;
	  }
	  prev_fptr = fptr;
	}
      }
    }
    
  }
/*
  printf ("Fanout: %d Fanout Gates: %d %d\n",ckt->gate[5].num_fanout, ckt->gate[5].fanout[0], ckt->gate[5].fanout[1]);
  printf ("Fanout: %d Fanout Gates: %d %d %d\n",ckt->gate[38].num_fanout, ckt->gate[38].fanout[0], ckt->gate[38].fanout[1], ckt->gate[38].fanout[2]);
  printf ("Fanout: %d Fanout Gates: %d %d %d\n",ckt->gate[56].num_fanout, ckt->gate[56].in_val[0], ckt->gate[56].in_val[1], ckt->gate[56].fanout[0]);
  printf ("Fanout: %d Fanout Gates: %d %d %d %d\n",ckt->gate[128].num_fanout, ckt->gate[128].in_val[0], ckt->gate[128].in_val[1], ckt->gate[128].fanout[0], ckt->gate[128].fanout[1]);
  printf ("Fanout: %d Fanout Gates: %d %d %d\n",ckt->gate[144].num_fanout, ckt->gate[144].in_val[0], ckt->gate[144].in_val[1], ckt->gate[144].fanout[0]);
  printf ("Fanout: %d Fanout Gates: %d %d %d\n",ckt->gate[157].num_fanout, ckt->gate[157].in_val[0], ckt->gate[157].in_val[1], ckt->gate[157].fanout[0]);
  printf ("Fanout: %d Fanout Gates: %d %d %d %d\n",ckt->gate[159].num_fanout, ckt->gate[159].in_val[0], ckt->gate[159].in_val[1], ckt->gate[159].type, ckt->gate[159].fanout[0]);
  printf ("Fanout: %d Fanout Gates: %d %d %d %d\n",ckt->gate[160].num_fanout, ckt->gate[160].in_val[0], ckt->gate[160].in_val[1], ckt->gate[160].type, ckt->gate[160].fanout[0]);
  printf ("Fanin 0: %d Fanin Gates: %d %d %d %d\n",ckt->gate[56].fanin[0], ckt->gate[160].in_val[0], ckt->gate[160].in_val[1], ckt->gate[160].type, ckt->gate[160].fanout[0]);
  printf ("Fanin 0: %d Fanin Gates: %d %d %d %d\n",ckt->gate[128].fanin[0], ckt->gate[160].in_val[0], ckt->gate[160].in_val[1], ckt->gate[160].type, ckt->gate[160].fanout[0]);
  printf ("Fanin: %d %d Fanin Gates: %d %d %d %d\n",ckt->gate[20].fanin[0], ckt->gate[20].fanin[1], ckt->gate[20].in_val[0], ckt->gate[20].in_val[1], ckt->gate[20].type, ckt->gate[20].fanout[0]);
  printf ("Fanin: %d %d Fanin Gates: %d %d %d %d\n",ckt->gate[118].fanin[0], ckt->gate[118].fanin[1], ckt->gate[118].in_val[0], ckt->gate[118].in_val[1], ckt->gate[118].type, ckt->gate[118].fanout[0]);
  printf ("Fanin: %d %d Fanin Gates: %d %d %d %d\n",ckt->gate[94].fanin[0], ckt->gate[94].fanin[1], ckt->gate[94].in_val[0], ckt->gate[94].in_val[1], ckt->gate[94].type, ckt->gate[94].fanout[0]);
  printf ("Fanin: %d %d Fanin Gates: %d %d %d %d\n",ckt->gate[95].fanin[0], ckt->gate[95].fanin[1], ckt->gate[95].in_val[0], ckt->gate[95].in_val[1], ckt->gate[95].type, ckt->gate[95].fanout[0]);
  printf ("Fanin: %d %d Fanin Gates: %d %d %d %d\n",ckt->gate[88].fanin[0], ckt->gate[88].fanin[1], ckt->gate[88].in_val[0], ckt->gate[88].in_val[1], ckt->gate[88].type, ckt->gate[88].fanout[0]);
  printf ("Fanin: %d %d Fanin Gates: %d %d %d %d\n",ckt->gate[78].fanin[0], ckt->gate[78].fanin[1], ckt->gate[78].in_val[0], ckt->gate[78].in_val[1], ckt->gate[78].type, ckt->gate[78].fanout[0]);
 
printf ("Change 128 0\n");
    for (j = 0; j < gate_flt[128].in_fault[0]; j++) {
	int g_ind = gate_flt[128].in_fault_list[0][j].gate_index;
	int i_ind = gate_flt[128].in_fault_list[0][j].input_index;
	int typ = gate_flt[128].in_fault_list[0][j].type;
	int st = gate_flt[128].in_fault_list[0][j].status;
	printf ("Fault Gate: %d, Gate Name: %s, Fault Pin: %d, Fault Type: %d Fault Status: %d\n",
		g_ind, ckt->gate[g_ind].name, i_ind, typ, st);
    }
printf ("Change 128 1\n");
    for (j = 0; j < gate_flt[128].in_fault[1]; j++) {
	int g_ind = gate_flt[128].in_fault_list[1][j].gate_index;
	int i_ind = gate_flt[128].in_fault_list[1][j].input_index;
	int typ = gate_flt[128].in_fault_list[1][j].type;
	int st = gate_flt[128].in_fault_list[1][j].status;
	printf ("Fault Gate: %d, Gate Name: %s, Fault Pin: %d, Fault Type: %d Fault Status: %d\n",
		g_ind, ckt->gate[g_ind].name, i_ind, typ, st);
    }
printf ("Change 94 1\n");
    for (j = 0; j < gate_flt[94].in_fault[1]; j++) {
	int g_ind = gate_flt[94].in_fault_list[1][j].gate_index;
	int i_ind = gate_flt[94].in_fault_list[1][j].input_index;
	int typ = gate_flt[94].in_fault_list[1][j].type;
	int st = gate_flt[94].in_fault_list[1][j].status;
	printf ("Fault Gate: %d, Gate Name: %s, Fault Pin: %d, Fault Type: %d Fault Status: %d\n",
		g_ind, ckt->gate[g_ind].name, i_ind, typ, st);
    }
printf ("Change 94 0\n");
    for (j = 0; j < gate_flt[94].in_fault[0]; j++) {
	int g_ind = gate_flt[94].in_fault_list[0][j].gate_index;
	int i_ind = gate_flt[94].in_fault_list[0][j].input_index;
	int typ = gate_flt[94].in_fault_list[0][j].type;
	int st = gate_flt[94].in_fault_list[0][j].status;
	printf ("Fault Gate: %d, Gate Name: %s, Fault Pin: %d, Fault Type: %d Fault Status: %d\n",
		g_ind, ckt->gate[g_ind].name, i_ind, typ, st);
    }
printf ("Change 118 1\n");
    for (j = 0; j < gate_flt[118].in_fault[1]; j++) {
	int g_ind = gate_flt[118].in_fault_list[1][j].gate_index;
	int i_ind = gate_flt[118].in_fault_list[1][j].input_index;
	int typ = gate_flt[118].in_fault_list[1][j].type;
	int st = gate_flt[118].in_fault_list[1][j].status;
	printf ("Fault Gate: %d, Gate Name: %s, Fault Pin: %d, Fault Type: %d Fault Status: %d\n",
		g_ind, ckt->gate[g_ind].name, i_ind, typ, st);
    }
printf ("Change 118 0\n");
    for (j = 0; j < gate_flt[118].in_fault[0]; j++) {
	int g_ind = gate_flt[118].in_fault_list[0][j].gate_index;
	int i_ind = gate_flt[118].in_fault_list[0][j].input_index;
	int typ = gate_flt[118].in_fault_list[0][j].type;
	int st = gate_flt[118].in_fault_list[0][j].status;
	printf ("Fault Gate: %d, Gate Name: %s, Fault Pin: %d, Fault Type: %d Fault Status: %d\n",
		g_ind, ckt->gate[g_ind].name, i_ind, typ, st);
    }
printf ("Change 118 -1\n");
    for (j = 0; j < gate_flt[118].out_fault; j++) {
	int g_ind = gate_flt[118].out_fault_list[j].gate_index;
	int i_ind = gate_flt[118].out_fault_list[j].input_index;
	int typ = gate_flt[118].out_fault_list[j].type;
	int st = gate_flt[118].out_fault_list[j].status;
	printf ("Fault Gate: %d, Gate Name: %s, Fault Pin: %d, Fault Type: %d Fault Status: %d\n",
		g_ind, ckt->gate[g_ind].name, i_ind, typ, st);
    }
*/  
  return(undetected_flist);

  /********************/
  /* fault simulation */
  /********************/

  /* loop through all undetected faults */
  prev_fptr = (fault_list_t *)NULL;
  for (fptr=undetected_flist; fptr != (fault_list_t *)NULL; fptr=fptr->next) {
    printf ("Fault Gate: %d, Fault Pin: %d, Fault Type: %d\n",fptr->gate_index, fptr->input_index, fptr->type);
    /* loop through all patterns */
    detected_flag = FALSE;
    for (p = 0; (p < pat->len) && !detected_flag; p++) {
      /* initialize all gate values to UNDEFINED */
      for (i = 0; i < ckt->ngates; i++) {
	ckt->gate[i].in_val[0] = UNDEFINED;
	ckt->gate[i].in_val[1] = UNDEFINED;
	ckt->gate[i].out_val = UNDEFINED;
      }
      /* assign primary input values for pattern */
      for (i = 0; i < ckt->npi; i++) {
	ckt->gate[ckt->pi[i]].out_val = pat->in[p][i];
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
	      ckt->gate[i].in_val[fptr->input_index] = LOGIC_0;
	    }
	    else { /* S_A_1 */
	      ckt->gate[i].in_val[fptr->input_index] = LOGIC_1;
	    }
	    /* compute gate output value */
	    evaluate(ckt->gate[i]);
	  }
	  else { /* fault at output */
	    evaluate(ckt->gate[i]);
	    /* inject fault */
	    if ( fptr->type == S_A_0 ) {
	      ckt->gate[i].out_val = LOGIC_0;
	    }
	    else { /* S_A_1 */
	      ckt->gate[i].out_val = LOGIC_1;
	    }
	  }
	}
	else { /* not faulty gate */
	  /* compute gate output value */
	  evaluate(ckt->gate[i]);
	}
      }
      /* check if fault detected */
      for (i = 0; i < ckt->npo; i++) {
	if ( (ckt->gate[ckt->po[i]].out_val == LOGIC_0)
	     && ( pat->out[p][i] == LOGIC_1) )
	  detected_flag = TRUE;
	else if ( (ckt->gate[ckt->po[i]].out_val == LOGIC_1)
	     && ( pat->out[p][i] == LOGIC_0) )
	  detected_flag = TRUE;
      }
    }
  }

/*
    if (detected_flag) {
      if ( prev_fptr == (fault_list_t *)NULL )
	undetected_flist = fptr->next;
      else
	prev_fptr->next = fptr->next;
    }
    else
      prev_fptr = fptr;
  }
  return(undetected_flist);
*/
}

