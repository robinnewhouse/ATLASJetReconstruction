#include "BoostedJetTaggers/Transform.h"


static const double MIN_PT = 0.0;
static const double MAX_PT = 1679.1593231;
static const double MIN_ETA = 0; // confirm
static const double MAX_ETA = 2.7;
static const double MIN_PHI = 0; // confirm
static const double MAX_PHI = M_PI;

double Transform::pt_min_max_scale(double clust_pt, double jet_pt){
		return (clust_pt - MIN_PT) / (MAX_PT - MIN_PT);
	}

double Transform::jet_pt_scale(double clust_pt, double jet_pt){
        return clust_pt / jet_pt;
    }

// double Transform::pt_indiv_scale(double clust_pt, double scale){
//         return clust_pt / scale;
//     }

double Transform::eta_min_max_scale(double clust_eta, double jet_eta){
        return (clust_eta - MIN_ETA) / (MAX_ETA - MIN_ETA);
    }

double Transform::phi_min_max_scale(double clust_phi, double jet_phi){
        return (clust_phi - MIN_PHI) / (MAX_PHI - MIN_PHI);
    }

double Transform::eta_shift(double clust_eta, double jet_eta){
        return (clust_eta - jet_eta);
    }

double Transform::phi_shift(double clust_phi, double jet_phi){
        // must account for the wrap
        double phi_trans = clust_phi - jet_phi;
        if (phi_trans < -M_PI)
            return phi_trans + 2.0 * M_PI;
        else if (phi_trans >= M_PI)
            return phi_trans - 2.0 * M_PI;
        else
            return phi_trans;
    }

double Transform::eta_shift_and_scale(double clust_eta, double jet_eta){
        return ((clust_eta - jet_eta) + 1.0) / 2.0;
    }

double Transform::phi_shift_and_scale(double clust_phi, double jet_phi){
        // must account for the wrap
        double phi_trans = clust_phi - jet_phi;
        if (phi_trans < -M_PI)
            return phi_trans + 2.0 * M_PI;
        else if (phi_trans >= M_PI)
            return phi_trans - 2.0 * M_PI;
        else
            return (phi_trans + 1.0) / 2.0;
    }



// double Transform::phi_eta_shift_and_rotate(double & pt,double & eta, double & phi, double & jet_eta, double & jet_phi, double & theta){
//         // translate
//         double eta_p = jet_eta - eta
//         phi_trans = jet_phi - phi
//         if phi_trans < -math.pi:
//             phi_p = phi_trans + 2 * math.pi
//         elif phi_trans >= math.pi:
//             phi_p = phi_trans - 2 * math.pi
//         else:
//             phi_p = phi_trans
//         # rotate
//         eta_pp = eta_p * math.cos(theta) + phi_p * math.sin(theta)
//         phi_pp = -eta_p * math.sin(theta) + phi_p * math.cos(theta)
//         return pt, eta_pp, phi_pp



