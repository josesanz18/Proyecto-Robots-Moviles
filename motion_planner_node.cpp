/***********************************************
*                                              *
*      motion_planner_node.cpp                 *
*                                              *
*      Jesus Savage                            *
*      Diego Cordero                           *
*                                              *
*              Bio-Robotics Laboratory         *
*              UNAM, 17-2-2020                 *
*                                              *
*                                              *
************************************************/

#include "ros/ros.h"
#include "../utilities/simulator_structures.h"
#include "../utilities/random.h"
#include "motion_planner_utilities.h"
#include "../state_machines/light_follower.h"
#include "../state_machines/sm_avoidance.h"
#include "../state_machines/sm_avoidance_destination.h"
#include "../state_machines/sm_destination.h"
#include "../state_machines/user_sm.h"
#include "../state_machines/user_sm0.h"
#include "../state_machines/user_sm1.h"
#include "../state_machines/user_sm2.h"
#include "../state_machines/dijkstra.h"
#include "../state_machines/dfs.h"
#include "clips_ros/SimuladorRepresentation.h"
#include "../behaviors/oracle.h"
#include "../action_planner/action_planner.h"
#include "simulator/MotionPlanner.h"
#include "simulator/ActionPlanner.h"



int main(int argc ,char **argv)
{

    float lidar_readings[512];
    float light_readings[8];
    int i;
    int tmp;
    int sensor;
    int est_sig;
    int q_light;
    int q_inputs;
    int flagOnce;
    int flg_finish;
    int mini_sm=1;
    int cta_steps;
    int flg_result;
    int flg_noise=0;
    float result;
    float final_x;
    float final_y;
    float intensity;
    float max_advance;
    float max_turn_angle;
    float noise_advance;
    float noise_angle;
    char path[100];
    char object_name[20];
    movement movements;
    step steps[200];
    step graph_steps[200];
    char str1[300];
    char str2[300];
    Actions plan;
    std::string ss;
    char ROS_System[300];
    char action[300];
    char room[300];
    char zone[300];
    static float x,y,z=0.0;
    int action_goto=0;
    int flg_executing_goto=0;



    // ROS directives
    // Initializes Node Name
    ros::init(argc ,argv ,"simulator_motion_planner_node");
    // Node handle declaration for communication with ROS system
    ros::NodeHandle n;

//    ros::Rate loop_rate(10);


    ros::Subscriber params_sub = n.subscribe("simulator_parameters_pub",0, parametersCallback);
    ros::Subscriber sub = n.subscribe("/scan", 10, laserCallback);

    // It subscribes to the action planner messages
    // Function msgCallbackActionPlanner is in motion_planner_utilities.h
    ros::Subscriber params_act = n.subscribe("action_planner_msg", 100, msgCallbackActionPlanner);

    // Declare publisher, create publisher motion_planner_msg 
    ros::Publisher motion_planner_msg  = n.advertise<simulator::MotionPlanner>("motion_planner_msg", 100);
    simulator::MotionPlanner msg_mot;

    //ros::Publisher action_planner_msg  = n.advertise<simulator::ActionPlanner>("action_planner_msg", 100);
    //simulator::ActionPlanner msg_mot;

    SimuladorRepresentation::setNodeHandle(&n);
    ros::Rate r(20);

    // it sets the environment's path
    strcpy(path,"./src/simulator/src/data/");


    printf("\n\n             MOTION PLANNER \n________________________________\n");



    while( ros::ok()  )
    {
        flagOnce = 1;
        cta_steps = 0;
        mini_sm =1;

        while( params.run )
        {
            // it gets sensory data
            ros::spinOnce();

            if (!params.useRealRobot)
            {
                get_light_values(&intensity,light_readings); // function in ~/catkin_ws/src/simulator/src/motion_planner/motion_planner_utilities.h

                get_lidar_values(lidar_readings,params.robot_x,
                                 params.robot_y,params.robot_theta,params.useRealRobot); // function in ~/catkin_ws/src/simulator/src/motion_planner/motion_planner_utilities.h
            }
            else
            {
                get_light_values_RealRobot(&intensity,light_readings); // function in ~/catkin_ws/src/simulator/src/motion_planner/motion_planner_utilities.h
                for( i = 0; i < 512; i++)
                    lidar_readings[i] = lasers[i];
            }

            // it quantizes the sensory data
            q_light = quantize_light(light_readings); // function in ~/catkin_ws/src/simulator/src/motion_planner/motion_planner_utilities.h
            
            if(params.noise )
                q_inputs = quantize_laser_noise(lidar_readings,params.laser_num_sensors,params.laser_value); // function in ~/catkin_ws/src/simulator/src/motion_planner/motion_planner_utilities.h
            else
                q_inputs = quantize_laser(lidar_readings,params.laser_num_sensors,params.laser_value); // function in ~/catkin_ws/src/simulator/src/motion_planner/motion_planner_utilities.h


            max_advance = params.robot_max_advance;
            max_turn_angle = params.robot_turn_angle;

            switch ( params.behavior)
            {

            case 1:
                // This function sends light sensory data to a function that follows a light source and it issues
                // the actions that the robot needs to perfom.
                // It is located in ~/catkin_ws/src/simulator/src/state_machines/light_follower.h
                flg_result = light_follower(intensity, light_readings,&movements,max_advance,max_turn_angle);
                if(flg_result == 1) stop();
                break;

            case 2:
                // This function sends light sensory data to an state machine that follows a light source and it issues
                // the actions that the robot needs to perfom.
                // It is located in ~/catkin_ws/src/simulator/src/state_machines/sm_destination.h
                if(flagOnce)
                {
                    est_sig = 1;
                    flagOnce = 0;
                }
                flg_result = sm_destination(intensity,q_light,&movements,&est_sig,params.robot_max_advance,params.robot_turn_angle);
                if(flg_result == 1) stop();

                break;

            case 3:
                // This function sends quantized sensory data to an state machine that avoids obstacles and it issues
                // the actions that the robot needs to perfom.
                // It is located in ~/catkin_ws/src/simulator/src/state_machines/sm_avoidance.h
                if(flagOnce)
                {
                    est_sig = 0;
                    flagOnce = 0;
                }
                sm_avoid_obstacles(q_inputs,&movements,&est_sig ,params.robot_max_advance ,params.robot_turn_angle);
                break;

            case 4:
                // This function sends quantized sensory data to an state machine that follows a light source and avoids obstacles
                // and it issues the actions that the robot needs to perfom.
                // It is located in ~/catkin_ws/src/simulator/src/state_machines/sm_avoidance_destination.h
                if(flagOnce)
                {
                    est_sig = 0;
                    flagOnce = 0;
                }
                flg_result=sm_avoidance_destination(intensity,q_light,q_inputs,&movements,&est_sig,
                                                    params.robot_max_advance ,params.robot_turn_angle);

                if(flg_result == 1) stop();
                break;

            case 5:
                // This function sends the intensity and the quantized sensory data to a Clips node and it receives the actions that
                // the robot needs to perfom to avoid obstacles and reach a light source according to first order logic
                // It is located in ~/catkin_ws/src/simulator/src/behaviors/oracle.h
                result=oracle_clips(intensity,q_light,q_inputs,&movements,max_advance ,max_turn_angle);
                if(result == 1.0) stop();
                break;


            case 6:
                // it finds a path from the origen to a destination using depth first search
                if(flagOnce)
                {
                    for(i = 0; i < 200; i++) steps[i].node = -1;

                    // it finds a path from the origen to a destination using first search
                    dfs(params.robot_x ,params.robot_y ,params.light_x ,params.light_y ,params.world_name,steps);
                    print_algorithm_graph (steps);
                    i = 0;
                    final_x = params.light_x;
                    final_y = params.light_y;
                    set_light_position(steps[i].x,steps[i].y);
                    printf("New Light %d: x = %f  y = %f \n",i,steps[i].x,steps[i].y);
                    flagOnce = 0;
                    flg_finish=0;
                    est_sig = 0;
                    movements.twist=0.0;
                    movements.advance =0.0;
                }
                else
                {
                    //flg_result=sm_avoidance_destination(intensity,q_light,q_inputs,&movements,&est_sig,                                                        //params.robot_max_advance ,params.robot_turn_angle);
                    flg_result = oracle_clips(intensity,q_light,q_inputs,&movements,max_advance ,max_turn_angle);

                    if(flg_result == 1)
                    {
                        if(flg_finish == 1)
                            stop();
                        else
                        {
                            if(steps[i].node != -1)
                            {
                                set_light_position(steps[i].x,steps[i].y);
                                printf("New Light %d: x = %f  y = %f \n",i,steps[i].x,steps[i].y);
                                printf("Node %d\n",steps[i].node);
                                i++;
                                //printf("type a number \n");
                                //scanf("%d",&tmp);
                            }
                            else
                            {
                                set_light_position(final_x,final_y);
                                printf("New Light %d: x = %f  y = %f \n",i,final_x,final_y);
                                flg_finish = 1;
                            }
                        }
                    }
                }

                break;

            case 7:
                if(flagOnce)
                {
                    for(i = 0; i < 200; i++)steps[i].node=-1;
                    // it finds a path from the origen to a destination using the Dijkstra algorithm
                    dijkstra(params.robot_x ,params.robot_y ,params.light_x ,params.light_y ,params.world_name,steps);
                    print_algorithm_graph (steps);
                    i=0;
                    final_x=params.light_x;
                    final_y= params.light_y;
                    set_light_position(steps[i].x,steps[i].y);
                    printf("New Light %d: x = %f  y = %f \n",i,steps[i].x,steps[i].y);
                    flagOnce = 0;
                    flg_finish=0;
                    est_sig = 0;
                    movements.twist=0.0;
                    movements.advance =0.0;
                }
                else
                {
                    flg_result=sm_avoidance_destination(intensity,q_light,q_inputs,&movements,&est_sig,
                                                        params.robot_max_advance ,params.robot_turn_angle);

                    if(flg_result == 1)
                    {
                        if(flg_finish == 1) stop();
                        else
                        {
                            if(steps[i].node != -1)
                            {
                                set_light_position(steps[i].x,steps[i].y);
                                printf("New Light %d: x = %f  y = %f \n",i,steps[i].x,steps[i].y);
                                printf("Node %d\n",steps[i].node);
                                i++;
                                //printf("type a number \n");
                                //scanf("%d",&tmp);
                            }
                            else
                            {
                                set_light_position(final_x,final_y);
                                printf("New Light %d: x = %f  y = %f \n",i,final_x,final_y);
                                flg_finish=1;
                            }
                        }
                    }
                }
                break;

            case 8:
                // Here it goes your code for selection 8
                if(flagOnce)
                {
                    est_sig = 0;
                    flagOnce = 0;
                }
                flg_result=user_sm0(intensity,light_readings, lidar_readings, params.laser_num_sensors,params.laser_value,                        q_light,q_inputs,&movements,&est_sig ,params.robot_max_advance ,params.robot_turn_angle,params.robot_x,params.robot_y,cta_steps++, params.light_x, params.light_y);
		if(flg_result == 1)
                    stop();
                break;

            case 9:

                flg_result=light_follower(intensity, light_readings,&movements,max_advance,max_turn_angle);
                if(flg_result == 1)
                    set_light_position(.5,.25);

                break;

            
            case 10:

		//action_planner(params.robot_x, params.robot_y,params.robot_theta,&movements);
		//stop();
		movements.twist=0.0;
                movements.advance =0.0;
		//sleep(1);
		//params_act.behavior
		//while(action_data == 0){
		//	sleep(0.1);
		//}
		printf("action_data %d flg_execution_goto %d\n",action_data,flg_executing_goto);

		if(action_data != 0){
		    if(flg_executing_goto == 0){

			printf("Movement to be executed action_data %d %s\n",action_data,Action_Planner);

			//ACT-PLN plan 1 goto bedroom deposit
			sscanf(Action_Planner,"%s %s %s %s",ROS_System,str1,str2,action);

			sprintf(plan.action_plan[action_data],"%s DONE",action);
			//ss=plan.action_plan[action_data];
			msg_mot.motion=plan.action_plan[action_data];

			action_goto=exe_action(action,Action_Planner,params.robot_x,params.robot_y,params.robot_theta,&movements);

			if(action_goto == 0){
				printf("not goto command");
			}
			else if(action_goto ==1){

				params.behavior = 7;
				params.light_x = movements.advance;
				params.light_y = movements.twist;
				printf("Find Dijkstra path to %f %f\n",params.light_x,params.light_y);

				for(i = 0; i < 200; i++)steps[i].node=-1;
	                    // it finds a path from the origen to a destination using the Dijkstra algorithm
                    		dijkstra(params.robot_x ,params.robot_y ,params.light_x ,params.light_y ,params.world_name,steps);
                    		print_algorithm_graph (steps);
                    		i=0;
                    		final_x=params.light_x;
                    		final_y= params.light_y;
                    		set_light_position(steps[i].x,steps[i].y);
                    		printf("New Light %d: x = %f  y = %f \n",i,steps[i].x,steps[i].y);
                    		flagOnce = 0;
                    		flg_finish=0;
                    		est_sig = 0;
                    		movements.twist=0.0;
                    		movements.advance =0.0;
				flg_executing_goto = 1;
				ros::spinOnce();


			}


		}
		else{

                    flg_result=user_sm0(intensity,light_readings, lidar_readings, params.laser_num_sensors,params.laser_value,                        q_light,q_inputs,&movements,&est_sig ,params.robot_max_advance ,params.robot_turn_angle,params.robot_x,params.robot_y,cta_steps++, params.light_x, params.light_y);

                    if(flg_result == 1)
                    {
                        if(flg_finish == 1) {
				//stop();
				flg_executing_goto = 0;
                                //action_data = 0;
                                msg_mot.stamp = ros::Time::now();                   // Save current time in the stamp of 'msg'
                                msg_mot.data = action_data;                        // Save the the 'count' value in the data of 'msg'
                                msg_mot.data = cta_steps;                        // Save the the 'count' value in the data of 'msg'
                                msg_mot.motion=Action_Planner;
                                ROS_INFO("MP send msg = %d", msg_mot.stamp.sec);       // Print the 'stamp.sec' message
                                ROS_INFO("MP send msg = %d", msg_mot.stamp.nsec);      // Print the 'stamp.nsec' message
                                ROS_INFO("MP send msg = %d", msg_mot.data);            // Print the 'data' message
                                motion_planner_msg.publish(msg_mot);                // Publishes motion_planner message
                                ROS_INFO("send msg motion = %s", msg_mot.motion.c_str());           // Print the 'data' message
				flg_executing_goto = 0;

			}
                        else
                        {
                            if(steps[i].node != -1)
                            {
                                set_light_position(steps[i].x,steps[i].y);
                                printf("New Light %d: x = %f  y = %f \n",i,steps[i].x,steps[i].y);
                                printf("Node %d\n",steps[i].node);
                                i++;
                                //printf("type a number \n");
                                //scanf("%d",&tmp);
                            }
                            else
                            {
                                set_light_position(final_x,final_y);
                                printf("New Light %d: x = %f  y = %f \n",i,final_x,final_y);
                                flg_finish=1;
				//flg_executing_goto = 0;
                            }
                        }
                    }
                  }



		}



                break;

            case 11:
                movements.twist = 0;
                movements.advance = .05;
                if (mini_sm==1)
                {

                    mini_sm++;
                }
                else if(mini_sm==2)
                {
                    //object_interaction(GRASP,"blockA");
                    mini_sm++;
                }
                else if(mini_sm==3)
                {
                    mini_sm++;
                }
                else if(mini_sm==4)
                {

                    mini_sm++;
                }
                else if(mini_sm==5)
                {
                    mini_sm++;
                }

                
                

                break;

             default:
                    printf(" ******* SELECTION NO DEFINED *******\n");
                    movements.twist = 3.1416/4;
                    movements.advance = .03;
                break;
            }

            ros::spinOnce();
            printf("\n\n             MOTION PLANNER \n________________________________\n");
            printf("Light: x = %f  y = %f \n",params.light_x,params.light_y);
            printf("Robot: x = %f  y = %f \n",params.robot_x,params.robot_y);
            printf("Step: %d \n",cta_steps++);
            printf("Movement: twist: %f advance: %f \n" ,movements.twist ,movements.advance );

            flg_noise = params.noise;

            move_robot(movements.twist,movements.advance,lidar_readings);
            ros::spinOnce();
            new_simulation = 0;
            r.sleep();
        }
        ros::spinOnce();
        r.sleep();
    }
}
