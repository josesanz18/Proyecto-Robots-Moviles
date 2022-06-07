/********************************************************
 *                                                      *
 *                                                      *
 *      user_sm0.h			          	*
 *                                                      *
 *							*
 *		FI-UNAM					*
 *		17-2-2019                               *
 *                                                      *
 ********************************************************/
//#include "../state_machines/light_follower.h"


// State Machine 
int user_sm0(float intensity, float *light_values, float *observations, int size, float laser_value, int  dest, int obs ,
					movement *movements  ,int *next_state ,float Mag_Advance ,float max_twist, float position_x, float position_y, int step, float light_x, float light_y)
{

 int state = *next_state;
 int i;
 int result=0;

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
                        int flg_result=light_follower(intensity, light_values,movements,Mag_Advance,max_twist);
		        if(flg_result == 1)result=1;
                        //printf("Present State: %d FORWARD\n", state);
                        *next_state = 0;
                }
                else if (obs == 1){
                        *movements=generate_output(STOP,Mag_Advance,max_twist);
                        //printf("Present State: %d STOP\n", state);

                                *next_state = 1;
                }
		else 
		{
			*movements=generate_output(STOP,Mag_Advance,max_twist);
			*next_state = 5;
		}
                break;

        case 1: // Backward, obstacle in the right
                *movements=generate_output(BACKWARD,Mag_Advance,max_twist);
                //printf("Present State: %d BACKWARD, obstacle RIGHT\n", state);
                *next_state = 2;
                break;

        case 2: // left turn
                *movements=generate_output(LEFT,Mag_Advance,max_twist);
                //printf("Present State: %d TURN LEFT\n", state);
                *next_state = 3;
                break;

	case 3:
		{
			if(obs == 2)
			{
				*next_state = 5;
			}
			else
			{			
				*movements=generate_output(FORWARD,Mag_Advance,max_twist);
				*next_state = 4;
			}
		
		}
		break;
	case 4:
		{
			*movements=generate_output(RIGHT,Mag_Advance,max_twist);
			if(obs == 0)
			{
				*next_state = 0;
			}
			else 
			{				
				*next_state = 1;
			}
		}
		break;
	case 5:
		{
			*movements=generate_output(LEFT,Mag_Advance,max_twist);
			if(obs == 2)
			{
				*next_state = 5;
			}
			else 
			{
				*next_state = 3;
			}
			
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
 return result;

}



                 
