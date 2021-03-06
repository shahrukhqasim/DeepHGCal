/*
 * Transform.h
 *
 *  Created on: 6 Aug 2017
 *      Author: user
 */

#ifndef INCLUDE_TRANSFORMER_H_
#define INCLUDE_TRANSFORMER_H_


#include "../include/converter.h"
#include <vector>


using namespace std;

class Transformer {
public:
	Transformer(vector<float> *rechit_x, vector<float> *rechit_y, vector<float> *rechit_z, converter* c);

	vector<float> abs_transform(float x, float y, float z,int layer) const;

	vector<float> rel_transform(float x, float y, float z,int layer) const;

	void set_seed_origin(float eta, float phi);

	~Transformer() {}


private:
	vector<float> _origin_x;
	vector<float> _origin_y;
	vector<float> _DX_per_layer;
	vector<float> _DY_per_layer;
	vector<float> layer_z = {320.755, 321.505, 322.735, 323.485, 324.715, 325.465, 326.695, 327.445, 328.675,
								   329.425, 330.735, 331.605, 332.915, 333.785, 335.095, 335.965, 337.275, 338.145,
								   339.455, 340.325, 341.775, 342.845, 344.295, 345.365, 346.815, 347.885, 349.335,
								   350.405, 356.335, 361.015, 365.695, 370.375, 375.055, 379.735, 384.415, 389.095,
								   393.775, 398.455, 403.135, 407.815, 424.115, 432.815, 441.515, 450.215, 458.915,
								   467.615, 476.315, 485.015, 493.715, 502.415, 511.115, 519.815};

	vector<float> find_dx(vector<float>* rechit, converter* c, string name="");
	vector<float> get_layer_origins(vector<float> *rechit, converter* c, string name="");
	map<int, vector<float>> get_hits_by_layer(vector<float> *rechit, converter* c);

	float get_x(float theta,float phi, float z);
	float get_y(float theta,float phi, float z);

	string hits_outfile;

	float get_dx(float x, float y, int layer) const;
	float get_dy(float x, float y, int layer) const;
};

#endif /* INCLUDE_TRANSFORMER_H_ */
