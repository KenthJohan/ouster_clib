/**
 * Copyright (c) 2022, Ouster, Inc.
 * All rights reserved.
 */

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <random>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "ouster_client/client.h"
#include "ouster_client/impl/build.h"
#include "ouster_client/lidar_scan.h"
#include "ouster_client/types.h"
#include "ouster_viz/point_viz.h"
#include <Eigen/Geometry>

using namespace ouster;

const size_t N_SCANS = 5;
const size_t UDP_BUF_SIZE = 65536;

void FATAL(const char* msg) {
    std::cerr << msg << std::endl;
    std::exit(EXIT_FAILURE);
}


typedef struct
{
    ouster::viz::PointViz * viz;
} render_args_t;


void* render_thread(void* arg)
{
    render_args_t * a = (render_args_t *)arg;
    a->viz->running(true);
    a->viz->visible(true);
    while(1)
    {
        // send updates to be rendered. This method is thread-safe
        a->viz->update();
        a->viz->run_once();
        if (a->viz->running() == false){break;}
    }
    a->viz->visible(false);
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}




void runcv(cv::Ptr<cv::SimpleBlobDetector> detector, cv::Mat& a, std::vector<cv::KeyPoint>& keypoints, cv::Mat& b)
{
    // Detect blobs.
    detector->detect(a, keypoints);
    //printf("keypoints %i\n", keypoints.size());
    // Draw detected blobs as red circles.
    // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
    cv::drawKeypoints(a, keypoints, b, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DEFAULT );
}





void scan_to_cvmat(LidarScan& scan, ouster::sensor::ChanField f, cv::Mat& dst)
{
    //https://docs.opencv.org/4.x/d0/daf/group__core__eigen.html#gaa79981ae2c2dbeea3ccf788d3a851a74
    Eigen::Ref<img_t<uint32_t>> img = scan.field(f);
    cv::Mat a(img.rows(), img.cols(), CV_32SC1, img.data());
    a.copyTo(dst);

    /*
    Eigen::Matrix<uint32_t, -1, -1, Eigen::RowMajor> img1 = scan.field(ouster::sensor::ChanField::RANGE);
    dst.create(();)
    cv::Mat A(img1.rows(), img1.cols(), CV_32SC1, img1.data());
    cv::resize
    */
}









int main(int argc, char* argv[]) {
    if (argc != 2 && argc != 3) {
        std::cerr
            << "Version: " << ouster::SDK_VERSION_FULL << " ("
            << ouster::BUILD_SYSTEM << ")"
            << "\n\nUsage: client_example <sensor_hostname> [<udp_destination>]"
               "\n\n<udp_destination> is optional: leave blank for "
               "automatic destination detection"
            << std::endl;

        return argc == 1 ? EXIT_SUCCESS : EXIT_FAILURE;
    }



    // Limit ouster_client log statements to "info" and direct the output to log
    // file rather than the console (default).
    sensor::init_logger("info", "ouster.log");

    std::cerr << "Ouster client example " << ouster::SDK_VERSION << std::endl;
    /*
     * The sensor client consists of the network client and a library for
     * reading and working with data.
     *
     * The network client supports reading and writing a limited number of
     * configuration parameters and receiving data without working directly with
     * the socket APIs. See the `client.h` for more details. The minimum
     * required parameters are the sensor hostname/ip and the data destination
     * hostname/ip.
     */
    const std::string sensor_hostname = argv[1];
    const std::string data_destination = (argc == 3) ? argv[2] : "";

    std::cerr << "Connecting to \"" << sensor_hostname << "\"...\n";

    auto handle = sensor::init_client(sensor_hostname, data_destination);
    if (!handle) FATAL("Failed to connect");
    std::cerr << "Connection to sensor succeeded" << std::endl;

    /*
     * Configuration and calibration parameters can be queried directly from the
     * sensor. These are required for parsing the packet stream and calculating
     * accurate point clouds.
     */
    std::cerr << "Gathering metadata..." << std::endl;
    auto metadata = sensor::get_metadata(*handle);

    // Raw metadata can be parsed into a `sensor_info` struct
    sensor::sensor_info info = sensor::parse_metadata(metadata);

    size_t w = info.format.columns_per_frame;
    size_t h = info.format.pixels_per_column;

    ouster::sensor::ColumnWindow column_window = info.format.column_window;

    std::cerr << "  Firmware version:  " << info.fw_rev
              << "\n  Serial number:     " << info.sn
              << "\n  Product line:      " << info.prod_line
              << "\n  Scan dimensions:   " << w << " x " << h
              << "\n  Column window:     [" << column_window.first << ", "
              << column_window.second << "]" << std::endl;


    
    // A ScanBatcher can be used to batch packets into scans
    sensor::packet_format pf = sensor::get_format(info);
    ScanBatcher batch_to_scan(info.format.columns_per_frame, pf);

    // buffer to store raw packet data
    auto packet_buf = std::make_unique<uint8_t[]>(UDP_BUF_SIZE);


    LidarScan scan = LidarScan{w, h, info.format.udp_profile_lidar};
    size_t cloud_size = w*h;
    XYZLut lut = ouster::make_xyz_lut(info);
    ouster::viz::PointViz viz("Viz example");
    ouster::viz::add_default_controls(viz);

    
    // create a point cloud and register it with the visualizer
    //Eigen::Transform<Eigen::double_t, 3, Eigen::Affine> t = Eigen::Transform<Eigen::double_t, 3, Eigen::Affine>::Identity();

    //auto g = a* t;

    /*
    auto a = Eigen::Translation3d(Eigen::Vector3d(1,1,2)).translation();
    auto b = a.matrix();
    */
    std::shared_ptr<ouster::viz::Image> image = std::make_shared<ouster::viz::Image>();
    std::shared_ptr<ouster::viz::Cloud> cloud = std::make_shared<ouster::viz::Cloud>(cloud_size);
    /*
    std::shared_ptr<ouster::viz::Label> labels[2] = 
    {
        std::make_shared<ouster::viz::Label>("Label1", ouster::viz::vec3d{0,0,0}),
        std::make_shared<ouster::viz::Label>("Label2", ouster::viz::vec3d{0,0,0})
    };
    */

    std::shared_ptr<ouster::viz::Cuboid> labels[2] = 
    {
        std::make_shared<ouster::viz::Cuboid>(ouster::viz::identity4d, ouster::viz::vec4f{0,1,1,1}),
        std::make_shared<ouster::viz::Cuboid>(ouster::viz::identity4d, ouster::viz::vec4f{1,0,1,1})
    };
    viz.add(labels[0]);
    viz.add(labels[1]);



    viz.add(cloud);
    viz.add(image);

    image->set_position(-1.0, 1.0, 0.5, 1.0);

    std::random_device rd;
    std::default_random_engine re(rd());
    std::uniform_real_distribution<float> dis2(0.0, 1.0);
    std::vector<float> colors(cloud_size);
    std::generate(colors.begin(), colors.end(), [&]() { return dis2(re); });




    pthread_t ptid;
    render_args_t thread_args = {&viz};
    pthread_create(&ptid, NULL, &render_thread, &thread_args);


    cv::SimpleBlobDetector::Params params;
    params.filterByArea = true;
    params.minArea = 1;
    params.maxArea = 1000;
    //params.filterByColor = true;
    //params.blobColor = 255;
    // Set up the detector with default parameters.
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);


    cv::namedWindow("img_visual_rgb8", cv::WINDOW_FREERATIO);
    cv::resizeWindow("img_visual_rgb8", 2000, 200);

    while(1)
    {
        // wait until sensor data is available
        sensor::client_state st = sensor::poll_client(*handle);

        // check for error status
        if (st & sensor::CLIENT_ERROR)
            FATAL("Sensor client returned error state!");

        // check for lidar data, read a packet and add it to the current batch
        if (st & sensor::LIDAR_DATA) {
            if (!sensor::read_lidar_packet(*handle, packet_buf.get(), pf)) {
                FATAL("Failed to read a packet of the expected size!");
            }

            // batcher will return "true" when the current scan is complete
            if (batch_to_scan(packet_buf.get(), scan)) {
                // retry until we receive a full set of valid measurements
                // (accounting for azimuth_window settings if any)
                if (scan.complete(info.format.column_window))
                {
                    LidarScan::Points p = ouster::cartesian(scan, lut);
                    cloud->set_xyz(p.data());
                    cloud->set_key(colors.data());
                    cv::Mat img_range32;
                    scan_to_cvmat(scan, ouster::sensor::ChanField::RANGE, img_range32);
                    cv::Mat img_range8;
                    cv::normalize(img_range32, img_range8, 0, 255, cv::NORM_MINMAX, CV_8UC1);
                    cv::Mat img_visual_rgb8;

                    std::vector<cv::KeyPoint> keypoints;
                    runcv(detector, img_range8, keypoints, img_visual_rgb8);
                    for(size_t i = 0; i < MIN(keypoints.size(), 2); ++i)
                    {
                        int j = keypoints[i].pt.y * w + keypoints[i].pt.x;
                        Eigen::Vector3d e{p(j, 0), p(j, 1), p(j, 2)};
                        Eigen::Affine3d t{Eigen::Translation3d(Eigen::Vector3d(e))};
                        Eigen::Matrix4d m = t.matrix(); // Option 2
                        ouster::viz::mat4d * a = (ouster::viz::mat4d *)m.data();
                        labels[i]->set_transform(*a);

                        //auto fuck = ;
                        //labels[i]->set_position({p(j, 0), p(j, 1), p(j, 2)});
                        //printf("%f %f %f\n", p(j, 0), p(j, 1), p(j, 2));
                    }
                    cv::imshow("img_visual_rgb8", img_visual_rgb8);
                    cv::pollKey();
                    cv::Mat img_visual_rgb32;
                    cv::normalize(img_visual_rgb8, img_visual_rgb32, 0, 1, cv::NORM_MINMAX, CV_32FC3);
                    image->set_image_rgb(img_visual_rgb32.cols, img_visual_rgb32.rows, (float*)img_visual_rgb32.data);
                }
            }
        }

        // check if IMU data is available (but don't do anything with it)
        if (st & sensor::IMU_DATA) {
            sensor::read_imu_packet(*handle, packet_buf.get(), pf);
        }
        

        
    }





    
    





    return EXIT_SUCCESS;
}
