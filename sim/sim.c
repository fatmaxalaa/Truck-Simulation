#include<stdio.h>
#include<stdlib.h>

//The 3 lines below mean that you can assign 7, 6 and 5 values to the loading time, weighing time and the travel time while taking in inputs
#define n1 7		
#define n2 6
#define n3 5

struct input_values {
	int loading_time[n1];
	int weighing_time[n2];
	int travel_time[n3];
}values;

int a = 0, b = 0, c = 0;

struct sys_state {
	int lq;		//Loader Queue
	int l;		//Loader
	int wq;		//Weighing Queue
	int w;		//Weighing Scale
}state;


struct dump_truck {
	int id;			//Truck number
	int loading_time;
	int weighing_time;
	int travel_time;
}truck[7];


//Represents a node in the Loader Queue, which is implemented as a Singly-linked list
struct loader_queue {
	int id;					//Truck number
	struct loader_queue* next_ptr;
}*l_start = NULL, * l_temp = NULL, * l_current = NULL;


//This represents any of the 2 loader slots or the weighing scale
struct slot {
	int id;				//Truck number
	struct slot* next_ptr;
}*l1 = NULL, * l2 = NULL, * w1 = NULL;


//Represents a node in the Weighing Queue, which is implemented as a Singly-linked list
struct weighing_queue {
	int id;					//Truck number
	struct weighing_queue* next_ptr;
}*w_start = NULL, * w_temp = NULL, * w_current = NULL;


//The Future Event List stores all the upcoming events
//There are 3 types of events namely End Loading(EL), End Weighing(EW) and Arrival at Loader Queue(ALQ)
struct fel {
	enum event_notices { ALQ, EL, EW } event;
	int event_time;					//Time at which that event will be completed
	int dt;
	struct fel* next_ptr;
}*fel_start = NULL, * fel_temp = NULL, * fel_current = NULL, * fel_prcd = NULL;


int lbusy_time = 0;		//Total Busy Time of both the loaders
int wbusy_time = 0;		//Total Busy Time of the Weighing Scale
int clock = 0;			//Maintains the clock time
const char* eventnames[] = { "ALQ", "EL", "EW" };
int prev_clock, prev_loaderstate, prev_weighscalestate;


//This function is used give the loading time, weighing time and arrival time as inputs
void init()
{
	int i;
	printf("Enter the loading time values:\t");
	for (i = 0; i < n1; i++)
		scanf_s("%d", &values.loading_time[i]);
	printf("Enter the weighing time values:\t");
	for (i = 0; i < n2; i++)
		scanf_s("%d", &values.weighing_time[i]);
	printf("Enter the travel time values:\t");
	for (i = 0; i < n3; i++)
		scanf_s("%d", &values.travel_time[i]);
	printf("\n");
}


int set_input(int q)
{
	if (q == 0)
	{
		if (a == n1)
			a = 0;
		return(values.loading_time[a++]);
	}
	else if (q == 1)
	{
		if (b == n2)
			b = 0;
		return(values.weighing_time[b++]);
	}
	else
	{
		if (c == n3)
			c = 0;
		return(values.travel_time[c++]);
	}
}


void main()
{
	int i;
	printf("..........DUMP TRUCK PROBLEM..........\n\n");
	printf("6 dump trucks are used to haul coal from the entrance of a small mine to the railroad.\nThere are two loaders to load the coal into the dump trucks.\nA single weighing scale is then used to weigh the loaded trucks.\n\n");
A: printf("Specify the initial state of the system (State at clock time(t) = 0):\n");
	printf("Enter the number of trucks in the loading queue:\t");
	scanf_s("%d", &state.lq);
	printf("Enter the number of trucks on the loaders:\t");
	scanf_s("%d", &state.l);
	printf("Enter the number of trucks in the weighing queue:\t");
	scanf_s("%d", &state.wq);
	printf("Enter the number of trucks on the weighing scale:\t");
	scanf_s("%d", &state.w);
	printf("\n");
	if ((state.lq >= 0 && state.lq <= 6) && (state.l >= 0 && state.l <= 2) && (state.wq >= 0 && state.wq <= 6) && (state.w == 0 || state.w == 1) && (state.lq + state.l + state.wq + state.w == 6))
		goto B;
	else
	{
		printf("\nThe specified initial state is incorrect. Please provide acceptable inputs.\n\n");
		goto A;
	}
B: for (i = 1; i <= 6; i++)
{
	truck[i].id = i;
	truck[i].loading_time = 0;
	truck[i].weighing_time = 0;
	truck[i].travel_time = 0;
}
for (i = 0; i < state.lq; i++)
{
	printf("Enter the ID of truck %d present in the loading queue:\t", i + 1);
	l_temp = (struct loader_queue*)malloc(sizeof(struct loader_queue));
	scanf_s("%d", &l_temp->id);
	l_temp->next_ptr = NULL;
	if (l_start == NULL)		//If Loader Queue is empty
	{
		l_start = l_temp;
		l_current = l_start;
	}
	else
	{
		l_current->next_ptr = l_temp;
		l_current = l_temp;
	}
	l_temp = NULL;
}
if (state.l >= 1)		//If there is a truck in loader 1
{
	printf("Enter the ID of the truck present on loader L1:\t");
	l1 = (struct slot*)malloc(sizeof(struct slot));
	scanf_s("%d", &l1->id);
	l1->next_ptr = NULL;
}
if (state.l == 2)		//If there are trucks in both the loaders
{
	printf("Enter the ID of the truck present on loader L2:\t");
	l2 = (struct slot*)malloc(sizeof(struct slot));
	scanf_s("%d", &l2->id);
	l2->next_ptr = NULL;
}
for (i = 0; i < state.wq; i++)
{
	printf("Enter the ID of truck %d present in the weighing queue:\t", i + 1);
	w_temp = (struct weighing_queue*)malloc(sizeof(struct weighing_queue));
	scanf_s("%d", &w_temp->id);
	w_temp->next_ptr = NULL;
	if (w_start == NULL)		//If the Weighing Queue is empty
	{
		w_start = w_temp;
		w_current = w_start;
	}
	else
	{
		w_current->next_ptr = w_temp;
		w_current = w_temp;
	}
	w_temp = NULL;
}
if (state.w == 1)		//If there is a truck in the Weighing Scale
{
	printf("Enter the ID of the truck present on the weighing scale W1:\t");
	w1 = (struct slot*)malloc(sizeof(struct slot));
	scanf_s("%d", &w1->id);
	w1->next_ptr = NULL;
}
init();
if (l1 != NULL)		//If loader 1 is not empty
{
	printf("Enter the loading time of DT%d:\t", l1->id);
	printf("%d\n", truck[l1->id].loading_time = set_input(0));
}
if (l2 != NULL)		//If loader 2 is not empty
{
	printf("Enter the loading time of DT%d:\t", l2->id);
	printf("%d\n", truck[l2->id].loading_time = set_input(0));
}
if (w1 != NULL)		//If the weighing scale is not empty
{
	printf("Enter the weighing time of DT%d:\t", w1->id);
	printf("%d\n", truck[w1->id].weighing_time = set_input(1));
}

//Adding events to the Future Event List initially
if (state.l >= 1)		//If loader 1 is occupied by a truck
{
	fel_temp = (struct fel*)malloc(sizeof(struct fel));		//Creating a temporary event
	fel_temp->event = EL;
	fel_temp->event_time = truck[l1->id].loading_time;
	fel_temp->dt = l1->id;
	fel_temp->next_ptr = NULL;
	fel_start = fel_temp;						//Adding the temporary event to the actual list
	fel_current = fel_start;
	fel_temp = NULL;
}
if (state.l == 2)		//If both the loaders are occupied initially
{
	fel_temp = (struct fel*)malloc(sizeof(struct fel));
	fel_temp->event = EL;
	fel_temp->event_time = truck[l2->id].loading_time;
	fel_temp->dt = l2->id;
	fel_temp->next_ptr = NULL;
	if (fel_current->event_time <= fel_temp->event_time)		//Arranging the events in increasing order of their finishing times
	{
		fel_current->next_ptr = fel_temp;
		fel_current = fel_temp;
	}
	else
	{
		fel_temp->next_ptr = fel_current;
		fel_start = fel_temp;
		fel_current = fel_start;
	}
	fel_temp = NULL;
}
if (state.w == 1)		//If a truck is present in the Weighing Scale
{
	fel_temp = (struct fel*)malloc(sizeof(struct fel));
	fel_temp->event = EW;
	fel_temp->event_time = truck[w1->id].weighing_time;
	fel_temp->dt = w1->id;
	fel_temp->next_ptr = NULL;
	if (fel_start == NULL)
	{
		fel_start = fel_temp;
		fel_current = fel_start;
	}
	else
	{
		while (fel_current->event_time <= fel_temp->event_time && fel_current->next_ptr != NULL)		//Traversing to the place where we want to insert the new event in the actual list
		{
			fel_prcd = fel_current;
			fel_current = fel_current->next_ptr;
		}
		if (fel_current == fel_start)
		{
			fel_temp->next_ptr = fel_start;
			fel_start = fel_temp;
			goto E;
		}
		if (fel_current->event_time > fel_temp->event_time)
		{
			fel_prcd->next_ptr = fel_temp;
			fel_temp->next_ptr = fel_current;
			fel_current = fel_start;
		}
		else
		{
			fel_current->next_ptr = fel_temp;
			fel_current = fel_start;
		}
	}
E:
	fel_temp = NULL;
}
printf("\nClock(t)\t\tSystem State\n");
printf("\t\tLQ(t)\tL(t)\tWQ(t)\tW(t)\n");
printf(" %d\t\t%d\t%d\t%d\t%d\t", clock, state.lq, state.l, state.wq, state.w);
printf("\n\t\tLoading Queue:\t");
l_temp = l_start;
while (l_temp != NULL)
{
	printf("DT%d ", l_temp->id);
	l_temp = l_temp->next_ptr;
}
l_temp = NULL;
printf("\n\t\tWeighing Queue:\t");
w_temp = w_start;
while (w_temp != NULL)
{
	printf("DT%d ", w_temp->id);
	w_temp = w_temp->next_ptr;
}
w_temp = NULL;
printf("\n\t\tFuture Event List:\t");
fel_temp = fel_start;
while (fel_temp != NULL)
{
	printf("(%s,%d,DT%d) ", eventnames[fel_temp->event], fel_temp->event_time, fel_temp->dt);
	fel_temp = fel_temp->next_ptr;
}
fel_temp = NULL;
printf("\n\t\tBusy Loading Time:\t%d", lbusy_time);
printf("\n\t\tBusy Weighing Time:\t%d\n\n", wbusy_time);
while (clock <= 100 && fel_start != NULL)		//Printing the Future Event List till clock time is 100
{
	if (fel_start->event == 0)		//If the next event on the Future Event List is ALQ
	{
		prev_clock = clock;
		prev_loaderstate = state.l;
		prev_weighscalestate = state.w;
		clock = fel_start->event_time;
		printf("Enter the loading time for DT%d:\t", fel_start->dt);
		printf("%d\n", truck[fel_start->dt].loading_time = set_input(0));
		if (l_start != NULL)
		{
			l_temp = (struct loader_queue*)malloc(sizeof(struct loader_queue));
			l_current = l_start;
			l_temp->id = fel_start->dt;
			l_temp->next_ptr = NULL;
			while (l_current->next_ptr != NULL)
				l_current = l_current->next_ptr;
			l_current->next_ptr = l_temp;
			state.lq++;
			l_temp = NULL;
		}
		else
		{
			if (l1 != NULL && l2 != NULL)		//If both the loaders are occupied by trucks
			{
				l_start = (struct loader_queue*)malloc(sizeof(struct loader_queue));
				l_start->id = fel_start->dt;
				l_start->next_ptr = NULL;
				fel_start = fel_start->next_ptr;
				state.lq++;
			}
			else if (l1 == NULL)		//If the first loader slot is empty
			{
				l1 = (struct slot*)malloc(sizeof(struct slot));
				l1->id = fel_start->dt;
				l1->next_ptr = NULL;
				fel_temp = (struct fel*)malloc(sizeof(struct fel));
				fel_current = fel_start;
				fel_temp->event = EL;
				fel_temp->event_time = truck[fel_start->dt].loading_time + clock;
				fel_temp->dt = fel_start->dt;
				fel_temp->next_ptr = NULL;
				while (fel_current->event_time <= fel_temp->event_time && fel_current->next_ptr != NULL)
				{
					fel_prcd = fel_current;
					fel_current = fel_current->next_ptr;
				}
				if (fel_current == fel_start)
				{
					fel_temp->next_ptr = fel_start;
					fel_start = fel_temp;
					goto D;
				}
				if (fel_current->event_time > fel_temp->event_time)
				{
					fel_prcd->next_ptr = fel_temp;
					fel_temp->next_ptr = fel_current;
					fel_current = fel_start;
				}
				else
				{
					fel_current->next_ptr = fel_temp;
					fel_current = fel_start;
				}
			D:
				fel_start = fel_start->next_ptr;
				state.l++;
				fel_temp = NULL;
			}
			else
			{
				l2 = (struct slot*)malloc(sizeof(struct slot));
				l2->id = fel_start->dt;
				l2->next_ptr = NULL;
				fel_temp = (struct fel*)malloc(sizeof(struct fel));
				fel_current = fel_start;
				fel_temp->event = EL;
				fel_temp->event_time = truck[fel_start->dt].loading_time + clock;
				fel_temp->dt = fel_start->dt;
				fel_temp->next_ptr = NULL;
				while (fel_current->event_time <= fel_temp->event_time && fel_current->next_ptr != NULL)
				{
					fel_prcd = fel_current;
					fel_current = fel_current->next_ptr;
				}
				if (fel_current == fel_start)
				{
					fel_temp->next_ptr = fel_start;
					fel_start = fel_temp;
					goto C;
				}
				if (fel_current->event_time > fel_temp->event_time)
				{
					fel_prcd->next_ptr = fel_temp;
					fel_temp->next_ptr = fel_current;
					fel_current = fel_start;
				}
				else
				{
					fel_current->next_ptr = fel_temp;
					fel_current = fel_start;
				}
			C:
				fel_start = fel_start->next_ptr;
				state.l++;
				fel_temp = NULL;
			}
		}
		printf("\n\t\tLQ(t)\tL(t)\tWQ(t)\tW(t)\n");
		printf(" %d\t\t%d\t%d\t%d\t%d\t", clock, state.lq, state.l, state.wq, state.w);
		printf("\n\t\tLoading Queue:\t");
		l_temp = l_start;
		while (l_temp != NULL)
		{
			printf("DT%d ", l_temp->id);
			l_temp = l_temp->next_ptr;
		}
		l_temp = NULL;
		printf("\n\t\tWeighing Queue:\t");
		w_temp = w_start;
		while (w_temp != NULL)
		{
			printf("DT%d ", w_temp->id);
			w_temp = w_temp->next_ptr;
		}
		w_temp = NULL;
		printf("\n\t\tFuture Event List:\t");
		fel_temp = fel_start;
		while (fel_temp != NULL)
		{
			printf("(%s,%d,DT%d) ", eventnames[fel_temp->event], fel_temp->event_time, fel_temp->dt);
			fel_temp = fel_temp->next_ptr;
		}
		fel_temp = NULL;
		lbusy_time += (clock - prev_clock) * prev_loaderstate;
		wbusy_time += (clock - prev_clock) * prev_weighscalestate;
		printf("\n\t\tBusy Loading Time:\t%d", lbusy_time);
		printf("\n\t\tBusy Weighing Time:\t%d\n\n", wbusy_time);
	}
	else if (fel_start->event == 1)			//If the next event on the Future Event List is EL
	{
		prev_clock = clock;
		prev_loaderstate = state.l;
		prev_weighscalestate = state.w;
		clock = fel_start->event_time;
		printf("Enter the weighing time for DT%d:\t", fel_start->dt);
		printf("%d\n", truck[fel_start->dt].weighing_time = set_input(1));
		if (w_start != NULL)		//If the Weighing Queue is not empty
		{
			w_temp = (struct weighing_queue*)malloc(sizeof(struct weighing_queue));
			w_current = w_start;
			w_temp->id = fel_start->dt;
			w_temp->next_ptr = NULL;
			while (w_current->next_ptr != NULL)
				w_current = w_current->next_ptr;
			w_current->next_ptr = w_temp;
			state.wq++;
			state.l--;
			w_temp = NULL;
			w_current = NULL;
		}
		else
		{
			if (w1 != NULL)		//If the Weighing Scale is occupied
			{
				w_temp = (struct weighing_queue*)malloc(sizeof(struct weighing_queue));
				w_temp->id = fel_start->dt;
				w_temp->next_ptr = NULL;
				w_start = w_temp;
				state.wq++;
				state.l--;
				w_temp = NULL;
			}
			else
			{
				w1 = (struct slot*)malloc(sizeof(struct slot));
				w1->id = fel_start->dt;
				w1->next_ptr = NULL;
				fel_temp = (struct fel*)malloc(sizeof(struct fel));
				fel_current = fel_start;
				fel_temp->event = EW;
				fel_temp->event_time = truck[fel_start->dt].weighing_time + clock;
				fel_temp->dt = fel_start->dt;
				fel_temp->next_ptr = NULL;
				while (fel_current->event_time <= fel_temp->event_time && fel_current->next_ptr != NULL)
				{
					fel_prcd = fel_current;
					fel_current = fel_current->next_ptr;
				}
				if (fel_current == fel_start)
				{
					fel_temp->next_ptr = fel_start;
					fel_start = fel_temp;
					goto F;
				}
				if (fel_current->event_time > fel_temp->event_time)
				{
					fel_prcd->next_ptr = fel_temp;
					fel_temp->next_ptr = fel_current;
					fel_current = fel_start;
				}
				else
				{
					fel_current->next_ptr = fel_temp;
					fel_current = fel_start;
				}
			F:
				state.w++;
				state.l--;
				fel_temp = NULL;
			}
		}
		if (l1 != NULL && l1->id == fel_start->dt)
			l1 = NULL;
		else if (l2 != NULL && l2->id == fel_start->dt)
			l2 = NULL;
		if (l_start != NULL && l1 == NULL)		//If the first loader is empty and at least one truck is waiting in the Loader Queue
		{
			l1 = (struct slot*)malloc(sizeof(struct slot));
			l1->id = l_start->id;
			l1->next_ptr = NULL;
			printf("Enter the loading time of DT%d:\t", l1->id);
			printf("%d\n", truck[l1->id].loading_time = set_input(0));
			fel_temp = (struct fel*)malloc(sizeof(struct fel));
			fel_current = fel_start;
			fel_temp->event = EL;
			fel_temp->event_time = truck[l_start->id].loading_time + clock;
			fel_temp->dt = l_start->id;
			fel_temp->next_ptr = NULL;
			while (fel_current->event_time <= fel_temp->event_time && fel_current->next_ptr != NULL)
			{
				fel_prcd = fel_current;
				fel_current = fel_current->next_ptr;
			}
			if (fel_current == fel_start)
			{
				fel_temp->next_ptr = fel_start;
				fel_start = fel_temp;
				goto G;
			}
			if (fel_current->event_time > fel_temp->event_time)
			{
				fel_prcd->next_ptr = fel_temp;
				fel_temp->next_ptr = fel_current;
				fel_current = fel_start;
			}
			else
			{
				fel_current->next_ptr = fel_temp;
				fel_current = fel_start;
			}
		G:
			state.lq--;
			state.l++;
			fel_temp = NULL;
		}
		else if (l_start != NULL && l2 == NULL)		//If the second loader is vacant and at least one truck is waiting in the Loader Queue
		{
			l2 = (struct slot*)malloc(sizeof(struct slot));
			l2->id = l_start->id;
			l2->next_ptr = NULL;
			printf("Enter the loading time of DT%d:\t", l2->id);
			printf("%d\n", truck[l2->id].loading_time = set_input(0));
			fel_temp = (struct fel*)malloc(sizeof(struct fel));
			fel_current = fel_start;
			fel_temp->event = EL;
			fel_temp->event_time = truck[l_start->id].loading_time + clock;
			fel_temp->dt = l_start->id;
			fel_temp->next_ptr = NULL;
			while (fel_current->event_time <= fel_temp->event_time && fel_current->next_ptr != NULL)
			{
				fel_prcd = fel_current;
				fel_current = fel_current->next_ptr;
			}
			if (fel_current == fel_start)
			{
				fel_temp->next_ptr = fel_start;
				fel_start = fel_temp;
				goto Z;
			}
			if (fel_current->event_time > fel_temp->event_time)
			{
				fel_prcd->next_ptr = fel_temp;
				fel_temp->next_ptr = fel_current;
				fel_current = fel_start;
			}
			else
			{
				fel_current->next_ptr = fel_temp;
				fel_current = fel_start;
			}
		Z:
			state.lq--;
			state.l++;
			fel_temp = NULL;
		}
		if (l_start != NULL)
			l_start = l_start->next_ptr;
		fel_start = fel_start->next_ptr;
		printf("\n\t\tLQ(t)\tL(t)\tWQ(t)\tW(t)\n");
		printf(" %d\t\t%d\t%d\t%d\t%d\t", clock, state.lq, state.l, state.wq, state.w);
		printf("\n\t\tLoading Queue:\t");
		l_temp = l_start;
		while (l_temp != NULL)
		{
			printf("DT%d ", l_temp->id);
			l_temp = l_temp->next_ptr;
		}
		l_temp = NULL;
		printf("\n\t\tWeighing Queue:\t");
		w_temp = w_start;
		while (w_temp != NULL)
		{
			printf("DT%d ", w_temp->id);
			w_temp = w_temp->next_ptr;
		}
		w_temp = NULL;
		printf("\n\t\tFuture Event List:\t");
		fel_temp = fel_start;
		while (fel_temp != NULL)
		{
			printf("(%s,%d,DT%d) ", eventnames[fel_temp->event], fel_temp->event_time, fel_temp->dt);
			fel_temp = fel_temp->next_ptr;
		}
		fel_temp = NULL;
		lbusy_time += (clock - prev_clock) * prev_loaderstate;
		wbusy_time += (clock - prev_clock) * prev_weighscalestate;
		printf("\n\t\tBusy Loading Time:\t%d", lbusy_time);
		printf("\n\t\tBusy Weighing Time:\t%d\n\n", wbusy_time);
	}
	else		//If the next event in the Future Event List is EW
	{
		prev_clock = clock;
		prev_loaderstate = state.l;
		prev_weighscalestate = state.w;
		clock = fel_start->event_time;
		printf("Enter the travel time for DT%d:\t", fel_start->dt);
		printf("%d\n", truck[fel_start->dt].travel_time = set_input(2));
		fel_temp = (struct fel*)malloc(sizeof(struct fel));
		fel_current = fel_start;
		fel_temp->event = ALQ;
		fel_temp->event_time = truck[fel_start->dt].travel_time + clock;
		fel_temp->dt = fel_start->dt;
		fel_temp->next_ptr = NULL;
		while (fel_current->event_time <= fel_temp->event_time && fel_current->next_ptr != NULL)
		{
			fel_prcd = fel_current;
			fel_current = fel_current->next_ptr;
		}
		if (fel_current == fel_start)
		{
			fel_temp->next_ptr = fel_start;
			fel_start = fel_temp;
			goto Y;
		}
		if (fel_current->event_time > fel_temp->event_time)
		{
			fel_prcd->next_ptr = fel_temp;
			fel_temp->next_ptr = fel_current;
			fel_current = fel_start;
		}
		else
		{
			fel_current->next_ptr = fel_temp;
			fel_current = fel_start;
		}
	Y:
		fel_temp = NULL;
		fel_start = fel_start->next_ptr;
		fel_current = fel_start;
		if (w_start == NULL)		//If the Weiging Queue is empty
		{
			w1 = NULL;
			state.w = 0;
		}
		else
		{
			printf("Enter the weighing time for DT%d:\t", w_start->id);
			printf("%d\n", truck[w_start->id].weighing_time = set_input(1));
			fel_temp = (struct fel*)malloc(sizeof(struct fel));
			fel_temp->event = EW;
			fel_temp->event_time = truck[w_start->id].weighing_time + clock;
			fel_temp->dt = w_start->id;
			fel_temp->next_ptr = NULL;
			while (fel_current->event_time <= fel_temp->event_time && fel_current->next_ptr != NULL)
			{
				fel_prcd = fel_current;
				fel_current = fel_current->next_ptr;
			}
			if (fel_current == fel_start)
			{
				fel_temp->next_ptr = fel_start;
				fel_start = fel_temp;
				goto X;
			}
			if (fel_current->event_time > fel_temp->event_time)
			{
				fel_prcd->next_ptr = fel_temp;
				fel_temp->next_ptr = fel_current;
				fel_current = fel_start;
			}
			else
			{
				fel_current->next_ptr = fel_temp;
				fel_current = fel_start;
			}
		X:
			w_start = w_start->next_ptr;
			state.wq--;
			fel_temp = NULL;
		}
		printf("\n\t\tLQ(t)\tL(t)\tWQ(t)\tW(t)\n");
		printf(" %d\t\t%d\t%d\t%d\t%d\t", clock, state.lq, state.l, state.wq, state.w);
		printf("\n\t\tLoading Queue:\t");
		l_temp = l_start;
		while (l_temp != NULL)
		{
			printf("DT%d ", l_temp->id);
			l_temp = l_temp->next_ptr;
		}
		l_temp = NULL;
		printf("\n\t\tWeighing Queue:\t");
		w_temp = w_start;
		while (w_temp != NULL)
		{
			printf("DT%d ", w_temp->id);
			w_temp = w_temp->next_ptr;
		}
		w_temp = NULL;
		printf("\n\t\tFuture Event List:\t");
		fel_temp = fel_start;
		while (fel_temp != NULL)
		{
			printf("(%s,%d,DT%d) ", eventnames[fel_temp->event], fel_temp->event_time, fel_temp->dt);
			fel_temp = fel_temp->next_ptr;
		}
		fel_temp = NULL;
		lbusy_time += (clock - prev_clock) * prev_loaderstate;
		wbusy_time += (clock - prev_clock) * prev_weighscalestate;
		printf("\n\t\tBusy Loading Time:\t%d", lbusy_time);
		printf("\n\t\tBusy Weighing Time:\t%d\n\n", wbusy_time);
	}
}
}