/********************************************************
 *                                                      *
 *                                                      *
 *      user_sm1.h			          	*
 *                                                      *
 *							*
 *		FI-UNAM					*
 *		17-2-2019                               *
 *                                                      *
 ********************************************************/
//#include "../state_machines/light_follower.h"

//float qhx=0,qhy=0, qlx=0, qly=0;
//float maxintensity = 0;
//int stephit = 0;
//bool rodeo = false;

int sensorhit = 0,sensor = 0;

void user_sm(float intensity, float *light_values, float *observations, int size, float laser_value, int  dest, int obs ,
					movement *movements  ,int *next_state ,float Mag_Advance ,float max_twist, float position_x, float position_y, int step, float light_x, float light_y)
{

 int state = *next_state;
 int i;

 printf("intensity %f\n",intensity);
 printf("quantized destination %d\n",dest);
 printf("quantized obs %d\n",obs);

 for(int i = 0; i < 8; i++)
        printf("light_values[%d] %f\n",i,light_values[i]);
 for (int i = 0; i < size ; i++ ) 
         printf("laser observations[%d] %f\n",i,observations[i]);


 switch ( state ) {

        case 0:
		               
		if (obs == 0){
                        // there is not obstacle
			sensorhit = 0, sensor = 0;
                        int flg_result=light_follower(intensity, light_values,movements,Mag_Advance,max_twist);
		        if(flg_result == 1)stop();
                        //printf("Present State: %d FORWARD\n", state);
                        *next_state = 0;
                }
		else 
		{
			*movements=generate_output(STOP,Mag_Advance,max_twist);
			*next_state = 1;
		}
                break;

        case 1: 
		{
	 	for(int i = 1; i < 20; i++) 
	 	{	
		    if( observations[i] < observations[sensorhit])
			sensorhit = i;
	 	}
	 	
	 	if(sensorhit > 10)
		{
			sensorhit = -(20 - sensorhit);
			movements->twist =  (sensorhit * 3.1415 / 20 );//  + sensor * 3.1415 / 16;

		}
		if(sensorhit==0)
		{
			movements->advance = Mag_Advance/2;
		}
		else
			movements->twist = (sensorhit * 3.1415 / 20  );//  + sensor * 3.1415 / 16;
		*next_state = 0;	 	
		}
		

                break;

	case 2:
		{
		for(int i = 1; i < 8; i++) 
	 	{
		    if( light_values[i] > light_values[sensor])
			sensor = i;
	 	}
		if(sensor > 4)
		   sensor = -(8 - sensor);
		movements->twist =  sensor * 3.1415 / 16;
	 	//movements->advance = Mag_Advance/2;
		*next_state = 0;
		}
		break;

	default:
		{
			//printf("State %d light follower used ", state);
			//*movements=generate_output(STOP,Mag_Advance,max_twist);
			//*next_state = 0;
		}
                break;

                
 }

 printf("Next State: %d\n", *next_state);
 printf("sensorHit %d\n", sensorhit);
 printf("sensor %d\n", sensor);
 

}




                 
