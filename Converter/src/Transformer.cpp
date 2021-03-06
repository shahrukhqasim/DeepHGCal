/*
 * Transform.cpp
 *
 *  Created on: 6 Aug 2017
 *      Author: user
 */

#include "../include/Transformer.h"
#include <numeric>
#include <map>
#include <stack>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <string>
#include <limits>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include "TString.h"
#define LAYER_NUM 52

using namespace std;

Transformer::Transformer(vector<float> *rechit_x, vector<float> *rechit_y, vector<float> *rechit_z, converter* c)
{
	cout << "dx:" << endl;
    _DX_per_layer = this->find_dx(rechit_x, c, "exe/dx.csv");
	cout << "dy:" << endl;
    _DY_per_layer = this->find_dx(rechit_y, c, "exe/dy.csv");

	//each value is the origins coordinate in the respective layer and axis

	cout << "origins_x:" << endl;
	_origin_x = get_layer_origins(rechit_x, c, "exe/originx.csv");
	cout << "origins_y:" << endl;
	_origin_y = get_layer_origins(rechit_y, c, "exe/originy.csv");

	hits_outfile = *(c->outfilename_) + "_hits";

	cout << "hits outfile:" << hits_outfile << endl;

	ofstream outfile;
	outfile.open("exe/"+ hits_outfile +".csv", ios::out | ios::trunc );
	outfile.close();

}


vector<float> Transformer::abs_transform(float x, float y, float z, int layer ) const
{
	ofstream outfile;
	outfile.open("exe/"+hits_outfile+".csv", ios::out | ios::app );

	float a = x;
	float b = y;

	float dx=0,dy=0;

	if (layer <= 28)
		{
        	dx = 0.41235;
			dy = 0.71415;
		}
	else if (layer >= 29 ) // && layer <= 40
		{
			dx = 0.56235;
			dy = 0.9739;
		}



	int row = int(round(y /  dy));

	a = (row % 2 == 0) ? a :  (a - dx);

	vector<float> ret = {a, b};


	outfile << layer << "," << x << "," << y << "," << z << "," << a << "," << b << endl;
	outfile.close();


//	cout << "x = " << to_string(x) << ", a = " << to_string(a) << ", y = " << to_string(y) <<
//			", b = " << to_string(b) << ", layer = " << to_string(layer) << endl;

	return ret;
}

vector<float> Transformer::rel_transform(float x, float y, float z,int layer) const
{
	float dx=get_dx(x,y,layer),dy=get_dy(x,y,layer);


	ofstream outfile;
	outfile.open("exe/"+hits_outfile+".csv", ios::out | ios::app );

	float a = x;
	float b = y - fmod(y,dy);

	int row = int(round(y /  dy));


	a = (row % 2 == 0) ? a :  (a - dx);

	vector<float> ret = {a, b};


	outfile << layer << "," << x << "," << y << "," << z << "," << a << "," << b << endl;
	outfile.close();



	return ret;
}

vector<float> Transformer::get_layer_origins(vector<float> *rechit, converter* c, string name)
{

	ofstream outfile;
	outfile.open(name, ios::out | ios::trunc );

	vector<float> origins;
    map<int, vector<float>> hits_by_layer = get_hits_by_layer(rechit, c);

	for (int i_layer = 0; i_layer < LAYER_NUM; i_layer++)
	{
    	if(hits_by_layer.find(i_layer) != hits_by_layer.end())
    	{
    		vector<float> layer = hits_by_layer[i_layer];

    		if (layer.size() > 0){
    			float o = accumulate(layer.begin(), layer.end(), 0.0) / layer.size();
    			origins.push_back(o);
    			outfile << i_layer << "," << o << endl;

    		}
    		else
    		{
//    			cout << "layer " << to_string(i_layer) << " is empty" << endl;
    			origins.push_back(0);
    			outfile << i_layer << "," << 0 << endl;
    		}
    	}
    	else
    	{
    		origins.push_back(0);
			outfile << i_layer << "," << 0 << endl;

    	}
	}

//	for (auto i: origins)
//	      std::cout << to_string(i) << ", ";
//	cout << "" << endl;

    return origins;
}


//Transformer::~Transformer() {
	// TODO Auto-generated destructor stub
//}


vector<float> Transformer::find_dx(vector<float>* rechit, converter* c, string name)
{
	ofstream outfile;
	outfile.open(name, ios::out | ios::trunc );

	//vector of hexagon unit width per layer
    vector<float> dx;

    //maps hits to corresponding layer
    map<int, vector<float>> hits_by_layer = get_hits_by_layer(rechit, c);

//    float epsilon = numeric_limits<float>::epsilon();
//    cout << "epsilon: " << epsilon << endl;

    //for each layer. find hexagon characteristic length
    for (int i_layer = 0; i_layer < LAYER_NUM; i_layer++)
    {
		float diff = numeric_limits<float>::max();

//		cout << "layer" << i_layer << endl;

    	if(hits_by_layer.find(i_layer) != hits_by_layer.end())
    	{
    		vector<float> layer = hits_by_layer[i_layer];

    		sort(layer.begin(), layer.end());

			float tmp_diff = 0;

			for(unsigned int i = 1; i < layer.size(); i++)
			{
				tmp_diff = layer[i] - layer[i-1];

				if (tmp_diff < diff && tmp_diff > 0.41235){
						diff = tmp_diff;
//						cout << diff << ", ";
    		    }
			}
    	}
    	cout << endl;

		dx.push_back(diff);
		outfile << i_layer << "," << diff << endl;
    }

//    cout << "dx size:" << to_string(dx.size()) << endl;
//    for (auto i: dx)
//      std::cout << i << ", ";
//    cout << "" << endl;

    return dx;
}

map<int, vector<float>> Transformer::get_hits_by_layer(vector<float> *rechit, converter* c)
{
	map<int, vector<float>> hits_by_layer;
    Long64_t nentries = c->fChain->GetEntries();
    Long64_t nb;

    //copies hits to their matching entry in the map
    for (Long64_t jentry = 0; jentry < nentries; jentry++) {
        Long64_t ientry = c->LoadTree(jentry);
        if (ientry < 0) break;
        nb = c->fChain->GetEntry(jentry);

        for(size_t i = 0; i < rechit->size(); i++){
        	hits_by_layer[c->rechit_layer->at(i)].push_back(rechit->at(i));
        }
    }

    return hits_by_layer;
}


void Transformer::set_seed_origin(float eta, float phi)
{
	vector<float> seed_x;
	vector<float> seed_y;

	float theta = 2 * atan(exp(-eta));

	for(unsigned int i = 0 ; i < 52; i++)
	{
		seed_x.push_back(get_x(theta, phi, layer_z[i]));
		seed_y.push_back(get_y(theta, phi, layer_z[i]));
	}

	_origin_x = seed_x;
	_origin_y = seed_y;
}


float Transformer::get_x(float theta,float phi, float z)
{
	float over = z*z * pow(sin(theta), 2);
	float below = pow(cos(theta), 2)*(1+pow(tan(phi), 2));
	float x = sqrt(over/below);

	return x;

}

float Transformer::get_y(float theta,float phi, float z)
{
	float x = get_x(theta, phi, z);
	float y = x * tan(phi);

	return y;

}


float Transformer::get_dx(float x, float y, int layer) const
{
	if (layer <=28)
	{
		if (x*x + y*y < 75*75)
			return 0.41235;
		else if ((x*x + y*y >= 75*5) && (x*x + y*y < 120*120))
			return 0.56225;
		else
			return 0.56225;
	}
	else if (layer <=40)
	{
		return 0.56225;
	}
	else
	{
		return 0;
	}
}

float Transformer::get_dy(float x, float y, int layer) const
{
	if (layer <=28)
	{
		if (x*x + y*y < 75*75)
			return 0.71415;
		else if ((x*x + y*y >= 75*75) && (x*x + y*y < 120*120))
			return 0.9739;
		else
			return 0.97395;
	}
	else if (layer <=40)
	{
		return 0.9739;
	}
	else
	{
		return 0;
	}
}


