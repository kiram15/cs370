#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>


struct Process{
    int Process_ID;
    int Arrival_Time;
    int Burst_Duration;
    int Priority;
    int TurnAroundTime;
    int WaitTime;
};
//**********************************************************
struct Gantt
{
  int Process_ID;
  int Arrival_Time;
  int End_Time;
};

void File_Content(struct  Process Processes[], int number);
void FCFS(struct Process Processes[], int number);
void SJFP(struct Process Processes[], int number);
void PRIORITY(struct Process Processes[], int number);
void RR(struct Process Processes[], int number, int quantum);
void Display_Output(struct  Process Processes[],int number,struct Gantt Rows[],int count, char* filename);
void sortByBurst(struct Process arrived[], int arrivedTotal);
void sortByPriority(struct Process arrived[], int arrivedTotal);
void deleteFinished(struct Process *arrived, int totalArrived);

int main(int argc, char **argv)
{

    int number_of_pross = 0;

    	if (argc != 2)
    	{
       		printf("Incorrect number of arguments.\n");
       	 	return -1;
    	}

		FILE* file1 = fopen(argv[1], "r");

  while(!feof(file1))
  {
  char  ch = fgetc(file1);
    if(ch == '\n')
    {
      number_of_pross++;
    }
  }
        struct Process Processes[number_of_pross];

        fseek(file1, 0, SEEK_SET);


        number_of_pross=0;


		while(fscanf(file1, "%d,%d,%d,%d",&Processes[number_of_pross].Process_ID,&Processes[number_of_pross].Arrival_Time,
						&Processes[number_of_pross].Burst_Duration,&Processes[number_of_pross].Priority) != EOF	)
       {
			number_of_pross++;
        }
        fclose(file1);

            File_Content(Processes,number_of_pross);
            FCFS(Processes,number_of_pross);
            SJFP(Processes,number_of_pross);
            PRIORITY(Processes,number_of_pross);
	
        return 0;
}
//--------------------------------------------------------
void File_Content(struct Process Processes[], int number)
{
    int i;
    printf("PROCESS ID\tARRIVAL TIME\tBURST DURATION\tPRIORITY\n");
    for(i=0;i<number;i++)
    {

        printf("%d\t\t%d\t\t%d\t\t%d\n",Processes[i].Process_ID,Processes[i].Arrival_Time,Processes[i].Burst_Duration,Processes[i].Priority);

    }
}
//---------------------------------------------------------------
void Display_Output(struct  Process Processes[],int number,struct Gantt Rows[],int count, char *filename)
{
      FILE* file;
      int i;
      file=fopen(filename,"w");
      fprintf(file,"PROCESS ID\tARRIVAL TIME\tEND TIME\n");

      for(i=0;i<count;i++)
      {
        fprintf(file,"%10d%10d%10d\n",Rows[i].Process_ID,Rows[i].Arrival_Time,Rows[i].End_Time);
      }

      float avgWait=0;
      float avgTurnAround=0;
      for(i=0;i<number;i++)
      {
          avgWait+=Processes[i].WaitTime;
          avgTurnAround+=Processes[i].TurnAroundTime;
      }
	  
      fprintf(file,"Average Wait Time=%f\n",avgWait/number);
      fprintf(file,"TurnAround Time=%f\n",avgTurnAround/number);
      // Assumption: the value of count recieved is greater than exact number of rows in the Gantt chart by 1. Dont forget to adjust the value if yours is not so.
      fprintf(file,"Throughput=%f\n",((float)number/Rows[count-1].End_Time));
      fclose(file);

}
//------------------------------------------------------------

/*
 Note: Use function Display_Output to display the result. The Processes is the
 updated Processes with their respective waittime, turnAround time.
 Use the array of Gantt Struct to mimic the behaviour of Gantt Chart. Rows is the base name of the same array.
 number is the number of processes which if not changed during processing, is taken care. count is the number of rows in the Gantt chart. 
 
 Number of Processes will not exceed 1000. Create and array of Gantt chart which can accomodate atleast 1000 process.
 */
// -------------FCFS----------------------------------------

void FCFS(struct Process Processes[], int number)
{	
	struct Process FCFSQueue[number];
	for (int i = 0; i < number; i++) {
		FCFSQueue[i] = Processes[i];
	}
	
	for(int i = 1; i < number; i++) {
		for(int j = 0; j < number-i; j++) {
	    	if(FCFSQueue[j].Arrival_Time > FCFSQueue[j+1].Arrival_Time) {
	        	struct Process temp = FCFSQueue[j];
	            FCFSQueue[j] = FCFSQueue[j+1];
	            FCFSQueue[j+1] = temp;
	         }
			 if (FCFSQueue[j].Arrival_Time == FCFSQueue[j+1].Arrival_Time) {
				 //sort by process IDs if same arrival
				 if (FCFSQueue[j].Process_ID > FCFSQueue[j+1].Process_ID) {
	 	        	struct Process temp = FCFSQueue[j];
	 	            FCFSQueue[j] = FCFSQueue[j+1];
	 	            FCFSQueue[j+1] = temp;
				 }
			 }
	    }
	}
	
	struct Gantt FCFSGantt[number];
	int runCount = 0;
	for(int i = 0; i < number; i++) {
		FCFSGantt[i].Process_ID = FCFSQueue[i].Process_ID;
		FCFSGantt[i].Arrival_Time = runCount;
		runCount += FCFSQueue[i].Burst_Duration;
		FCFSGantt[i].End_Time = runCount;
	}
	
	//WaitTime
	Processes[0].WaitTime = 0; //first process doesn't wait 
	for (int i = 1; i < number; i++) {
		Processes[i].WaitTime = FCFSGantt[i].Arrival_Time - FCFSQueue[i].Arrival_Time;
	}
	
	//TurnAroundTime
	for (int i = 0; i < number; i++) {
		Processes[i].TurnAroundTime = Processes[i].WaitTime + Processes[i].Burst_Duration;
	}

	char FCFS[] = "FCFS";
	Display_Output(Processes,number,FCFSGantt,number,FCFS);
	
}	

//---------------------SJFP---------------------------------------
void SJFP(struct Process Processes[],int number)
{
	int clock = 0; 
	int totalBurst = 0; 
	
	for (int i = 0; i < number; i++) {
		totalBurst += Processes[i].Burst_Duration;
	}
	struct Process arrived[2000];
    struct Gantt gantt[totalBurst+15];
	
	int totalArrived = 0; 
	int totalGantt = 0;
	int previousArrived = 0;
	
    while (clock < totalBurst) {
		int arrivalNum = 0; 
		for (int i = 0; i < number; i++) {
			if (Processes[i].Arrival_Time == clock) {
				arrivalNum++; //how many processes have arrived
			}
		}
				
		struct Process newArrivals[arrivalNum];
		int k = 0;
		for (int i = 0; i < number; i++) {
			if (Processes[i].Arrival_Time == clock) {
				newArrivals[k] = Processes[i];
				k++;
			}
		}
		previousArrived = totalArrived;
		totalArrived += arrivalNum; 

        //NO NEW ARRIVALS
		if (arrivalNum == 0) { 
            sortByBurst(arrived, totalArrived);
			
            //CHECK IF BURST DURATION = 0
            if (arrived[0].Burst_Duration == 0) {
                for (int i = 0; i < number; i++) {
                    if (arrived[0].Process_ID == Processes[i].Process_ID) {
                        Processes[i].TurnAroundTime = clock - Processes[i].Arrival_Time;
						Processes[i].WaitTime = clock - Processes[i].Arrival_Time - Processes[i].Burst_Duration;
                    }
                }
                deleteFinished(arrived, totalArrived);		
                totalArrived--;
			}
			arrived[0].Burst_Duration--;

			//if its the same as the one before
			if (totalGantt > 0) {
				if ((gantt[totalGantt-1].Process_ID) == arrived[0].Process_ID) {					
					gantt[totalGantt-1].End_Time++;
				}
				//different process
				else {				
					gantt[totalGantt].Process_ID = arrived[0].Process_ID;
	                gantt[totalGantt].Arrival_Time = clock;
	                gantt[totalGantt].End_Time = clock+1;
					totalGantt++;
				}	
			}		
			clock++;
		}
		
		//NEW ARRIVALS
		else {
			if (arrivalNum > 0){
				int j = 0;
				//adding new arrivals into the list 
				if (totalArrived == arrivalNum) {
					for (int i = 0; i < totalArrived; i++) {
						arrived[i] = newArrivals[i];
					}
				}
				
				//adding new arrivals into the list 
		        for (int i = (previousArrived); i < totalArrived; i++) {
		            arrived[i] = newArrivals[j];
		            j++;
		        }
                sortByBurst(arrived, totalArrived);
            }

            //CHECK IF BURST DURATION = 0
            if (arrived[0].Burst_Duration == 0) {
                for (int i = 0; i < number; i++) {
                    if (arrived[0].Process_ID == Processes[i].Process_ID) {
                        Processes[i].TurnAroundTime = clock - Processes[i].Arrival_Time;
						Processes[i].WaitTime = (clock - Processes[i].Arrival_Time) - Processes[i].Burst_Duration;
                    }
                }
                deleteFinished(arrived, totalArrived);
                totalArrived--;
            } 
			arrived[0].Burst_Duration--;

			//if it's the first one
			if (gantt[0].Process_ID == 0) {
				gantt[0].Process_ID = arrived[0].Process_ID;
			    gantt[0].Arrival_Time = clock;
			    gantt[0].End_Time = clock+1;
				totalGantt++;
			}

			else if (totalGantt > 0) {
                //if its the same as the one before
				if ((gantt[totalGantt-1].Process_ID) == arrived[0].Process_ID) {
					gantt[totalGantt-1].End_Time++;
				}
				//different process
				else {				
					gantt[totalGantt].Process_ID = arrived[0].Process_ID;
	                gantt[totalGantt].Arrival_Time = clock;
	                gantt[totalGantt].End_Time = clock+1;
					totalGantt++;
					}
				}
				clock++;
	   		}
		}   	 	
		
		int i = 0;
		while (arrived[i].Process_ID != 0) {
            Processes[i].TurnAroundTime = clock - Processes[i].Arrival_Time;
			Processes[i].WaitTime = clock - Processes[i].Arrival_Time - Processes[i].Burst_Duration;
			i++;
		}

		for (int i = 0; i < number; i++) {
			for (int i = 0; i < totalArrived; i++) {
				if (Processes[i].Process_ID == arrived[i].Process_ID) {
					Processes[i].WaitTime = arrived[i].Arrival_Time - Processes[i].Arrival_Time;
				}

			}
		}
		
 
    char SJFP[] = "SJFP";
	Display_Output(Processes,number,gantt,totalGantt,SJFP);
}
	

void sortByBurst(struct Process arrived[], int arrivedTotal) {
	for (int i = 1; i < arrivedTotal; i++) {
		for (int j = 0; j < arrivedTotal -1; j++) {
			if(arrived[j].Burst_Duration > arrived[j+1].Burst_Duration) {
		       	struct Process temp = arrived[j];
		        arrived[j] = arrived[j+1];
		        arrived[j+1] = temp;
			}
		 	if (arrived[j].Burst_Duration == arrived[j+1].Burst_Duration) {						 				
				if (arrived[j].Arrival_Time > arrived[j+1].Arrival_Time) {
 		 	       struct Process temp = arrived[j];
 		 	       arrived[j] = arrived[j+1];
 		 	       arrived[j+1] = temp;
				}
				if (arrived[j].Arrival_Time == arrived[j+1].Arrival_Time) {
		 	    	if (arrived[j].Process_ID > arrived[j+1].Process_ID) {
				   		struct Process temp = arrived[j];
		 	       		arrived[j] = arrived[j+1];
		 	       		arrived[j+1] = temp;
					}
				}
				
			}
		}
	
	}
}
	

//------------------PRIORITY-------------------------------------
void PRIORITY(struct Process Processes[], int number)
{
	int clock = 0; 
	int totalBurst = 0; 
	
	for (int i = 0; i < number; i++) {
		totalBurst += Processes[i].Burst_Duration;
	}
	struct Process arrived[2000];
    struct Gantt gantt[totalBurst+15];
	
	int totalArrived = 0; 
	int totalGantt = 0;
	int previousArrived = 0;
	
	for (int i = 0; i < totalGantt; i++) {
		gantt[i].Process_ID = 0;
	}
	gantt[0].Process_ID = 0;
	
    while (clock < totalBurst) {
		int arrivalNum = 0; 
		for (int i = 0; i < number; i++) {
			if (Processes[i].Arrival_Time == clock) {
				arrivalNum++; //how many processes have arrived	
			}
		}
		
		//printf("1\n");		
		struct Process newArrivals[arrivalNum];
		int k = 0;
		for (int i = 0; i < number; i++) {
			if (Processes[i].Arrival_Time == clock) {
				newArrivals[k] = Processes[i];
				k++;
			}
		}
		previousArrived = totalArrived;
		totalArrived += arrivalNum; 
	
        //NO NEW ARRIVALS
		if (arrivalNum == 0) { 
            sortByPriority(arrived, totalArrived);

            //CHECK IF BURST DURATION = 0
            if (arrived[0].Burst_Duration == 0) {
                for (int i = 0; i < number; i++) {
                    if (arrived[0].Process_ID == Processes[i].Process_ID) {
                        Processes[i].TurnAroundTime = clock - Processes[i].Arrival_Time;
						Processes[i].WaitTime = clock - Processes[i].Arrival_Time - Processes[i].Burst_Duration;
						
                    }
                }
                deleteFinished(arrived, totalArrived);		
                totalArrived--;
			}
			arrived[0].Burst_Duration--;
			

			//if its the same as the one before
			if (totalGantt > 0) {
				if ((gantt[totalGantt-1].Process_ID) == arrived[0].Process_ID) {					
					gantt[totalGantt-1].End_Time++;
				}
				//different process
				else {				
					gantt[totalGantt].Process_ID = arrived[0].Process_ID;
	                gantt[totalGantt].Arrival_Time = clock;
	                gantt[totalGantt].End_Time = clock+1;
					totalGantt++;
				}	
			}		
			clock++;
		}
		
		//NEW ARRIVALS
		else {
			
			if (arrivalNum > 0){
				int j = 0;
				//adding new arrivals into the list 
				if (totalArrived == arrivalNum) {
					for (int i = 0; i < totalArrived; i++) {
						arrived[i] = newArrivals[i];
					}
				}
				
				//adding new arrivals into the list 
		        for (int i = (previousArrived); i < totalArrived; i++) {
		            arrived[i] = newArrivals[j];
		            j++;
		        }
                sortByPriority(arrived, totalArrived);
            }
			
            //CHECK IF BURST DURATION = 0
            if (arrived[0].Burst_Duration == 0) {
                for (int i = 0; i < number; i++) {
                    if (arrived[0].Process_ID == Processes[i].Process_ID) {
                        Processes[i].TurnAroundTime = clock - Processes[i].Arrival_Time;
						Processes[i].WaitTime = clock - Processes[i].Arrival_Time - Processes[i].Burst_Duration;
						
                    }
                }
                deleteFinished(arrived, totalArrived);
                totalArrived--;
            } 
			arrived[0].Burst_Duration--;
            
			//GANTT CHART STUFF
			//if it's the first one
			if (gantt[0].Process_ID == 0) {
				gantt[0].Process_ID = arrived[0].Process_ID;
			    gantt[0].Arrival_Time = clock;
			    gantt[0].End_Time = clock+1;
				totalGantt++;
			}

			else if (totalGantt > 0) {
                //if its the same as the one before
				if ((gantt[totalGantt-1].Process_ID) == arrived[0].Process_ID) {
					gantt[totalGantt-1].End_Time++;
				}
				//different process
				else {				
					gantt[totalGantt].Process_ID = arrived[0].Process_ID;
	                gantt[totalGantt].Arrival_Time = clock;
	                gantt[totalGantt].End_Time = clock+1;
					totalGantt++;
					}
				}
				clock++;
	   		}
		}
		//calculate wait time 
		int i = 0;
		while (arrived[i].Process_ID != 0) {
            Processes[i].TurnAroundTime = clock - Processes[i].Arrival_Time;
			Processes[i].WaitTime = clock - Processes[i].Arrival_Time - Processes[i].Burst_Duration;
			i++;
		}
		
	Display_Output(Processes,number,gantt,totalGantt,"Priority");
}

void sortByPriority(struct Process arrived[], int arrivedTotal) {
	for (int i = 1; i < arrivedTotal; i++) {
		for (int j = 0; j < arrivedTotal -1; j++) {
			if(arrived[j].Priority > arrived[j+1].Priority) {
		       	struct Process temp = arrived[j];
		        arrived[j] = arrived[j+1];
		        arrived[j+1] = temp;
			}
		 	if (arrived[j].Priority == arrived[j+1].Priority) {						 				
				if (arrived[j].Arrival_Time > arrived[j+1].Arrival_Time) {
 		 	       struct Process temp = arrived[j];
 		 	       arrived[j] = arrived[j+1];
 		 	       arrived[j+1] = temp;
				}
				if (arrived[j].Arrival_Time == arrived[j+1].Arrival_Time) {
		 	    	if (arrived[j].Process_ID > arrived[j+1].Process_ID) {
				   		struct Process temp = arrived[j];
		 	       		arrived[j] = arrived[j+1];
		 	       		arrived[j+1] = temp;
					}
				}	
			}
		}
	}
}


void deleteFinished(struct Process *arrived, int totalArrived) {
    for (int i = 0; i < totalArrived; i++) {
        arrived[i] = arrived[i+1];
        //leaves a blank space at the end, but its ok
    }
}
