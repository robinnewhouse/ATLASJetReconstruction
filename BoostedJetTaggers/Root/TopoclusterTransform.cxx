#include "BoostedJetTaggers/TopoclusterTransform.h"


static const double MIN_PT = 0.0;
static const double MAX_PT = 1679.1593231;
static const double MIN_ETA = 0; // confirm
static const double MAX_ETA = 2.7;
static const double MIN_PHI = 0; // confirm
static const double MAX_PHI = M_PI;

double TopoclusterTransform::pt_min_max_scale(double clust_pt, double jet_pt){
		return (clust_pt - MIN_PT) / (MAX_PT - MIN_PT);
	}

double TopoclusterTransform::jet_pt_scale(double clust_pt, double jet_pt){
        return clust_pt / jet_pt;
    }

// double TopoclusterTransform::pt_indiv_scale(double clust_pt, double scale){
//         return clust_pt / scale;
//     }

double TopoclusterTransform::eta_min_max_scale(double clust_eta, double jet_eta){
        return (clust_eta - MIN_ETA) / (MAX_ETA - MIN_ETA);
    }

double TopoclusterTransform::phi_min_max_scale(double clust_phi, double jet_phi){
        return (clust_phi - MIN_PHI) / (MAX_PHI - MIN_PHI);
    }

double TopoclusterTransform::eta_shift(double clust_eta, double jet_eta){
        return (clust_eta - jet_eta);
    }

double TopoclusterTransform::phi_shift(double clust_phi, double jet_phi){
        // must account for the wrap
        double phi_trans = clust_phi - jet_phi;
        if (phi_trans < -M_PI)
            return phi_trans + 2.0 * M_PI;
        else if (phi_trans >= M_PI)
            return phi_trans - 2.0 * M_PI;
        else
            return phi_trans;
    }

double TopoclusterTransform::eta_shift_and_scale(double clust_eta, double jet_eta){
        return ((clust_eta - jet_eta) + 1.0) / 2.0;
    }

double TopoclusterTransform::phi_shift_and_scale(double clust_phi, double jet_phi){
        // must account for the wrap
        double phi_trans = clust_phi - jet_phi;
        if (phi_trans < -M_PI)
            return phi_trans + 2.0 * M_PI;
        else if (phi_trans >= M_PI)
            return phi_trans - 2.0 * M_PI;
        else
            return (phi_trans + 1.0) / 2.0;
    }


double TopoclusterTransform::calculate_theta_for_rotations(std::map<std::string,double> clusters){

    double phi_axis = 0.0;
    double eta_axis = 0.0;
    double pt_axis = 0.0;
    double y_axis = 0.0;
    double z_axis = 0.0;

    // Calculate principal axis
	for (uint i = 0; i < clusters.size() / 3 ; ++i){
		double pt_i = clusters["clust_"+std::to_string(i)+"_pt"];
		double eta_i = clusters["clust_"+std::to_string(i)+"_eta"];
		double phi_i = clusters["clust_"+std::to_string(i)+"_phi"];
		double e_i = pt_i * cosh(eta_i);
        double rad_i = sqrt(pow(eta_i,2) + pow(phi_i,2));
        if(rad_i != 0){
            pt_axis += pt_i;						// This seems strange to me
            phi_axis += (phi_i * e_i / rad_i);		// why do we add to the axis at every
            eta_axis += (eta_i * e_i / rad_i);		// iteration. Not static? - RN 
        }
        // Calculate cartesian coordinates
	    double px = pt_axis * cos(phi_axis);
	    double py = pt_axis * sin(phi_axis);
	    double pz = pt_axis * sinh(eta_axis);
		y_axis = py;
		z_axis = pz;

	}
    // Calculate the rotation angle for this cluster
    return atan2(y_axis, z_axis) + M_PI / 2;
}


void TopoclusterTransform::rotate_about_primary(std::map<std::string,double> &clusters, double angle){

    for (uint i = 0; i < clusters.size() / 3; ++i) {
        double theta; // may want to remove this. resdundant calclation
        // extract cylindrical
        double pt = clusters["clust_"+std::to_string(i)+"_pt"];
        double eta = clusters["clust_"+std::to_string(i)+"_eta"];
        double phi = clusters["clust_"+std::to_string(i)+"_phi"];
        // convert to cartesian
        double px = pt * cos(phi);
        double py = pt * sin(phi);
        double pz = pt * sinh(eta);
        // rotate
        double pxx = px;
        double pyy = py * cos(angle) - pz * sin(angle);
        double pzz = pz * cos(angle) + py * sin(angle);
        // convert to cylindrical
        double pt_r = sqrt(pow(pxx,2) + pow(pyy,2));
        double phi_r;
        double eta_r;
        if ((pxx == 0) & (pyy == 0) & (pzz == 0))
            theta = 0.0;
        else
            theta = atan2(pt_r, pzz);  // why are we recalculating theta here? and not adding pi/2
        if (pow(cos(theta),2) < 1)
            eta_r = -0.5 * log((1 - cos(theta)) / (1 + cos(theta)));
        else if (pzz == 0.0)
            eta_r = 0;
        else
            eta_r = 10e10;
        if ((pxx == 0) & (pyy == 0))
            phi_r = 0;
        else
            phi_r = atan2(pyy, pxx);
        // save
        clusters["clust_"+std::to_string(i)+"_pt"] = pt_r;
        clusters["clust_"+std::to_string(i)+"_eta"] = eta_r;
        clusters["clust_"+std::to_string(i)+"_phi"] = phi_r;
    }
    return;
}

void TopoclusterTransform::flip(std::map<std::string,double> &clusters){
    double sum_eta = 0.0;
    double sum_phi = 0.0;
    for (uint i = 0; i < clusters.size() / 3; ++i) {
        sum_eta += clusters["clust_"+std::to_string(i)+"_pt"] * clusters["clust_"+std::to_string(i)+"_eta"];
        sum_phi += clusters["clust_"+std::to_string(i)+"_pt"] * clusters["clust_"+std::to_string(i)+"_phi"];
    }

    if (sum_eta < 0){ // if more eta on negative, flip all eta
        for (uint i = 0; i < clusters.size() / 3; ++i) {
            clusters["clust_"+std::to_string(i)+"_eta"] = -clusters["clust_"+std::to_string(i)+"_eta"];
        }
    }
    return;
}
