#pragma once

#include "SurfaceMeshTypes.h"
#include <nanoflann.hpp>

using namespace std;
using namespace nanoflann;
using namespace SurfaceMeshTypes;

template <typename T>
struct PointCloud{
	std::vector<Vector3>  pts;

	inline size_t kdtree_get_point_count() const { return pts.size(); }

	inline T kdtree_distance(const T *p1, const size_t idx_p2, size_t size) const{
		const T d0=p1[0]-pts[idx_p2].x();
		const T d1=p1[1]-pts[idx_p2].y();
		const T d2=p1[2]-pts[idx_p2].z();
        size = size;
		return sqrt(d0*d0 + d1*d1 + d2*d2);
	}

	inline T kdtree_get_pt(const size_t idx, int dim) const{
		if (dim==0) return pts[idx].x();
		else if (dim==1) return pts[idx].y();
		else return pts[idx].z();
	}

	template <class BBOX>
	bool kdtree_get_bbox(BBOX &) const { return false; }
};

typedef std::pair<size_t, double> KDResultPair;
typedef std::vector< KDResultPair  > KDResults;

class NanoKdTree{
public:

	PointCloud<double> cloud;

	typedef KDTreeSingleIndexAdaptor< L2_Simple_Adaptor<double, PointCloud<double> >, PointCloud<double>, 3 /* dim */ > my_kd_tree;

	my_kd_tree * tree;

    NanoKdTree(){
        tree = NULL;
    }

    ~NanoKdTree(){
        delete tree;
    }

	void addPoint(const Vector3 & p){
		cloud.pts.push_back(p);
	}

	void build()
	{
        if(tree) delete tree;

		// construct a kd-tree index:
		tree = new my_kd_tree(3 /*dim*/, cloud, KDTreeSingleIndexAdaptorParams(10 /* max leaf */) );
		tree->buildIndex();
	}

	size_t k_closest(Vector3 p, int k, KDResults & ret_matches)
	{
        k = k < (int)cloud.pts.size() ? k : cloud.pts.size();

		ret_matches.clear();
		ret_matches.resize(k);

		std::vector<size_t> ret_index(k);
		std::vector<double> out_dist(k);

		tree->knnSearch(&p[0], k, &ret_index[0], &out_dist[0]);

		for(int i = 0; i < k; i++)
			ret_matches[i] = std::make_pair(ret_index[i], out_dist[i]);

		return k;
	}

    size_t ball_search(Vector3 p, double search_radius, KDResults & ret_matches = KDResults())
	{
		ret_matches.clear();

		nanoflann::SearchParams params;
		//params.sorted = false;

		return tree->radiusSearch(&p[0], search_radius, ret_matches, params);
	}

    int closest(Vector3 p)
    {
        KDResults match;
        this->k_closest(p, 1, match);

        if(!match.size())
            return -1;
        else
            return match[0].first;
    }
};
