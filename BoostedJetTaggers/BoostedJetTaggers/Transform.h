// for editors : this file is -*- C++ -*- 
#ifndef BOOSTEDJETSTAGGERS_TRANSFORM_H_
#define BOOSTEDJETSTAGGERS_TRANSFORM_H_

#include <math.h>
#include <map>
#include <vector>


class Transform {
  public:

  	static double pt_min_max_scale(double clust_pt, double jet_pt);
	static double jet_pt_scale(double clust_pt, double jet_pt);
	static double eta_min_max_scale(double clust_eta, double jet_eta);
	static double phi_min_max_scale(double clust_phi, double jet_phi);
	static double eta_shift(double clust_eta, double jet_eta);
	static double phi_shift(double clust_phi, double jet_phi);
	static double eta_shift_and_scale(double clust_eta, double jet_eta);
	static double phi_shift_and_scale(double clust_phi, double jet_phi);
	static std::vector<double> calculate_thetas_for_rotations(std::map<std::string,double> clusters);
	static void rotate_about_primary(std::map<std::string,double> &clusters, std::vector<double> thetas);
	static void flip(std::map<std::string,double> &clusters);
	// static double phi_eta_shift_and_rotate(double & pt,double & eta, double & phi, double & jet_eta, double & jet_phi, double & theta);

};

#endif
