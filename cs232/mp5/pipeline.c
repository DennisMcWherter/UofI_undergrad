#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct element {
  double x, y, z;
  float a, b;
  int c, d;
} element_t;


//----------------------------------------------//
// BEGIN code you don't have to look at			//
//----------------------------------------------//
const double RADIUS = 0x3fffffff * .00001;
const double RADIUS_SQUARED = 2 * RADIUS * RADIUS;

typedef struct vp {
  double ax, ay, az;
  double bx, by, bz;
} vp_t;

bool 
cull(element_t *e1, element_t *e2) {
  // make work function #1 
  double x_delta = fabs(e1->x - e2->x);
  double y_delta = fabs(e1->y - e2->y);
  double z_delta = fabs(e1->z - e2->z);
  double distance = x_delta*x_delta + y_delta*y_delta + z_delta*z_delta;
  return distance > RADIUS_SQUARED;
}  

void 
transform(element_t *e, vp_t *vp) {
  // make work function #2
  e->a = (vp->ax * e->x) + (vp->ay * e->y) + (vp->az * e->z);
  e->b = (vp->bx * e->x) + (vp->by * e->y) + (vp->bz * e->z);
}

void 
update(element_t *e1, element_t *e2) {
  // make work function #3
  if (e1->a > e2->a) {
	 e1->c += 100;
  } else {
	 e1->c = (int) (e1->c * .9);
  }

  if (e1->b > e2->b) {
	 e1->d += (int) (e1->c * .1);
  } else {
	 e1->d = -e1->d;
  }
}

void
initialize_element(element_t *e) {
  e->x = random() * .00001;
  e->y = random() * .00001;
  e->z = random() * .00001;
  e->c = random();
  e->d = random(); 
}

vp_t *
get_a_vp() {
  vp_t *vp = (vp_t *)malloc(sizeof(vp_t));
  vp->ax =  .05;  vp->ay = -.17;  vp->az =  .49; 
  vp->bx =  .91;  vp->by =  .33;  vp->bz = -.82; 
  return vp;
}
//----------------------------------------------//
// END code you don't have to look at  			//
//----------------------------------------------//

const unsigned NUM_ELEMENTS = 200000;
const unsigned NUM_STEPS = 1000;

int
main(int argc, char **argv) {

  // allocate 4 times as many elements as we need (sparse-ish data)
  element_t * elements = (element_t *)malloc(NUM_ELEMENTS * 4 * sizeof(element_t));  

  // allocate two arrays of pointers to elements
  unsigned num_elements = NUM_ELEMENTS;
  element_t **list = (element_t **)malloc(num_elements * sizeof(element_t *));  
  element_t **list2 = (element_t **)malloc(num_elements * sizeof(element_t *));  

  // point list entries at random elements
  for (int i = 0 ; i < num_elements ; i ++) {
	 int index = random() % (NUM_ELEMENTS * 4);
	 list[i] = &elements[index];
	 initialize_element(list[i]);
  }
  
  vp_t *vp = get_a_vp();  

  // iteratively process the elements
  for (int step = 0 ; step < NUM_STEPS ; step ++) {

	 // pick two random elements
	 element_t *e_cull = list[random() % num_elements];
	 element_t *e_update = list[random() % num_elements];

	 // select those elements close to the element of interest
	 unsigned num_elements2 = 0;
	 for (int i = 0 ; i < num_elements ; i ++) {
		if (!cull(list[i], e_cull)) {
		  list2[num_elements2] = list[i];
		  num_elements2 ++;
		}
	 }

	 for (int i = 0 ; i < num_elements2 ; i ++) {
		transform(list2[i], vp);
	 }

	 // update based on the update element
	 for (int i = 0 ; i < num_elements2 ; i ++) {
		update(list2[i], e_update);
	 }
  }
}

