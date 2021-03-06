/** Copyright 2013. All rights reserved.
* Author: Jordi Frias (jfrias99@gmail.com)
*
*	  Universitat Autònoma de Barcelona (UAB)
*
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License as published by the Free Software
* Foundation; either version 3 of the License, or any later version.
*
* This software is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
* PARTICULAR PURPOSE. See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with
* this software; if not, write to the Free Software Foundation, Inc., 51 Franklin
* Street, Fifth Floor, Boston, MA 02110-1301, USA 
*
*/

#include <iostream>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <pcl/point_cloud.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_types.h>

using namespace std;
using namespace pcl;

struct mRGB
{
	int r, g, b;
	
	mRGB(int r, int g, int b) : r(r), g(g), b(b) {}
};

struct ViewerStruct
{
    pcl::visualization::PCLVisualizer* viewer;
    PointCloud<PointXYZRGB>::Ptr cl1;
    PointCloud<PointXYZRGB>::Ptr cl2;

    bool flag;

    ViewerStruct() : flag(true) {}
};


void keyboardEventOccurred (const pcl::visualization::KeyboardEvent &event, void* viewerStruct)
{
    ViewerStruct* ptr = (ViewerStruct*)viewerStruct;

    if (event.getKeySym () == "a" && event.keyDown ())
    {
        if(ptr->flag==true)
        {
           ptr->viewer->removePointCloud("Cloud");
           ptr->viewer->addPointCloud(ptr->cl2,"Cloud");
           ptr->flag=false;
        }
        else
        {
            ptr->viewer->removePointCloud("Cloud");
            ptr->viewer->addPointCloud(ptr->cl1,"Cloud");
            ptr->flag=true;
        }
    }


}

int main (int argc, char** argv)
{
	/*You must introduce name of PLY image, example: "./planar_detector image3D.ply"*/
	if(argc != 2)
	{
		std::cout<< "Invalid number of arguments."<< std::endl;
		return -1;
	}

	pcl::PointCloud<pcl::PointXYZRGBL>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZRGBL>);
	pcl::io::loadPCDFile (argv[1], *cloud);

	pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloudRGB (new pcl::PointCloud<pcl::PointXYZRGB>);
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloudLabels (new pcl::PointCloud<pcl::PointXYZRGB>);

	vector<mRGB> colors;
	colors.push_back(mRGB(128, 64, 128));
	colors.push_back(mRGB(0, 0, 192));
	colors.push_back(mRGB(128, 128, 0));

	for(int i=0; i<cloud->size(); i++)
	{
		PointXYZRGBL pt = cloud->at(i);
		PointXYZRGB ptRGB;
		ptRGB.x = pt.x;
		ptRGB.y = pt.y;
		ptRGB.z = pt.z;
		ptRGB.r = pt.r;
		ptRGB.g = pt.g;
		ptRGB.b = pt.b;

		mRGB color = colors[pt.label];
		PointXYZRGB ptLabel;
		ptLabel.x = pt.x;
		ptLabel.y = pt.y;
		ptLabel.z = pt.z;
		ptLabel.r = color.r;
		ptLabel.g = color.g;
		ptLabel.b = color.b;

		cloudRGB->push_back(ptRGB);
		cloudLabels->push_back(ptLabel);
	}


	//We set the viewer in position to see what happens better.
	pcl::visualization::PCLVisualizer viewer("Cloud Viewer");
	viewer.setCameraPosition(0.0,0.0,0.0,0.0,0.0,1.0,0.0,-6.0,1.0);
	cloudRGB->sensor_orientation_.w() = 1.0;
	cloudRGB->sensor_orientation_.x() = 0.0;
	cloudLabels->sensor_orientation_.w() = 1.0;
	cloudLabels->sensor_orientation_.x() = 0.0;

	ViewerStruct viewerStruct;
	viewerStruct.cl1 = cloudRGB;
	viewerStruct.cl2 = cloudLabels;
	viewerStruct.viewer = &viewer;
	viewer.addPointCloud(viewerStruct.cl1, "Cloud");
	viewer.registerKeyboardCallback(keyboardEventOccurred, (void*) (&viewerStruct));

	//Mandatory to show viewer.
	viewer.spin();

	viewer.close();
	return (0);
}
