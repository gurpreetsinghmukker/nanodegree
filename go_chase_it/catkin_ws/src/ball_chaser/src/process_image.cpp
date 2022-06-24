#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
        ROS_ERROR("Failed to call service DriveToTarget");

}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int pixel_position = 0;
    int ball_position = 0;
    bool white_ball_found = false;
    int left_region_boundary = img.width/3;
    int mid_region_boundary = left_region_boundary*2;
    int right_region_boundary = img.width;
    ROS_INFO("IMAGE WIDTH %d", img.width);
    ROS_INFO("IMAGE HEIGHT %d", img.height);
    ROS_INFO("left_region_boundary %d", left_region_boundary);
    ROS_INFO("mid_region_boundary %d", mid_region_boundary);


    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera

    for( int i = 0 ; i < img.height*img.width ; i++ )
    {
        if( img.data[i] == white_pixel )
        {
            ROS_INFO("White pixel found at position %d\n", i);
            white_ball_found = true;
            pixel_position = i;
            break;
        }
    }
    

    ball_position = pixel_position % img.width; 
    ROS_INFO("Ball Position %d \n", ball_position);

// find which regions the ball is in
if (white_ball_found == true)
{

    if( 0 <= ball_position && ball_position< left_region_boundary ) // left region
    {
        // turn to the left
        ROS_INFO("turning left");
        drive_robot(0.0, 0.5);
    }
    else if( left_region_boundary <= ball_position && ball_position< mid_region_boundary ) // mid region
    {
        ROS_INFO("driving straight");
        drive_robot(0.1,0.0);
    }
    else if(  mid_region_boundary <= ball_position && ball_position<= img.width )// right region
    {
        // turn to the right
        ROS_INFO("turning right");
        drive_robot(0, -0.5);
    }
}
else if(white_ball_found == false)
{
    ROS_INFO("no ball found");
    drive_robot(0,0);
}

}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
