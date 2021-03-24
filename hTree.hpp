#include <utility>

#include <utility>

//
// Created by Summer Li on 2021-03-23.
//
#include <vector>
#include <math.h>

#ifndef KDTREE_HTREE_H
#define KDTREE_HTREE_H



class Node{
    using NodePtr = std::shared_ptr<Node>;

public:
    int axis;
    int value;
    NodePtr left;
    NodePtr right;

    Node(int axis, int value){
        this->axis = axis;
        this->value = value;
        this->left = this->right = nullptr;
    }

    ~Node()= default;

    void setLeft(NodePtr child){
        this->left = std::move(child);
    }

    void setRight(NodePtr child){
        this->right = std::move(child);
    }


    void query(std::vector<int>& pt, std::vector<int>& code, std::vector<std::vector<int>>& bound_box){
        if (pt[axis]>=value){
            code.push_back(1);
            bound_box[axis][0] = value;
            if (this->right != nullptr) this->right->query(pt,code,bound_box);
        }else{
            code.push_back(0);
            bound_box[axis][1] = value;
            if(this->left != nullptr) this->left->query(pt,code,bound_box);
        }
    }


};


using NodePtr = std::shared_ptr<Node>;


class kdTree {
    unsigned int leaf_size;
    unsigned int dim;
    NodePtr root;
    std::vector<int> maxs;

public:
    kdTree(std::vector<std::vector<int>>& data, unsigned int leaf_size=8){
        this->leaf_size = leaf_size;
        this->dim = data[0].size();
        this->root = this->constructTree(data);


        // init maxs
        for(int j=0; j<data[0].size();j++) {
            this->maxs.push_back(0);
        }
        for(auto & pt : data){
            for(int j=0; j<pt.size();j++){
                if (this->maxs[j] < pt[j]){
                    this->maxs[j] = pt[j]+1;
                }
            }
        }
    }

    NodePtr constructTree(std::vector<std::vector<int>>& data,int depth=0){
        //std::cout<<"build tree "<< depth << " " << data.size()<<std::endl;
        int size = data.size();
        if(size <= leaf_size) return nullptr;
        int mid_index = int(size/2);
        int axis = depth%dim;
        // sort along the picked axis
        std::vector<int> picked_axis;
        picked_axis.reserve(data.size());
        for(auto &pt: data){
            picked_axis.push_back(pt[axis]);
        }
        std::sort(picked_axis.begin(),picked_axis.end());
        int value = picked_axis[mid_index];


        std::vector<std::vector<int>> smaller_list;
        std::vector<std::vector<int>> larger_list;
        for(auto &pt: data){
            if (pt[axis]<value){
                smaller_list.push_back(pt);
            }else if(pt[axis]>value){
                larger_list.push_back(pt);
            }else{
                if(rand() % 100 > 50){
                    smaller_list.push_back(pt);
                }else{
                    larger_list.push_back(pt);
                }
            }
        }
        NodePtr res_node = std::make_shared<Node>(axis,value);
        int newdepth = depth+1;

        res_node->setLeft(constructTree(smaller_list,newdepth));
        res_node->setRight(constructTree(larger_list,newdepth));
        return res_node;
    }

    std::vector<std::vector<int>> query(std::vector<int>& pt, std::vector<int>& code){
        std::vector<std::vector<int>> bound_box{};
        for(int & max : maxs){
            bound_box.push_back({0,max});
        }
        this->root->query(pt,code,bound_box);
        return bound_box;
    }
};

void zorder(std::vector<int>& pt, std::vector<int>& code,int depth){
    if(depth == 0)
        return;
    int base = int(pow(2,depth));
    std::vector<int> res;
    for(auto &dim : pt){
        if (dim < base){
            code.push_back(0);
            res.push_back(dim);
        }else{
            code.push_back(1);
            res.push_back(dim-base);
        }
    }
    zorder(res,code,depth-1);
}

unsigned int code_value(std::vector<int> code){

    unsigned int v = 0;
    for(int i=code.size()-1;i>0;i--){
        v += int(code[i]*pow(2,i));
    }
    return v;
}


class hTree{
    std::shared_ptr<kdTree> kdtree;
    unsigned int leaf_size;
    unsigned int z_depth;

public:
    hTree(std::vector<std::vector<int>>& data, unsigned int leaf_size=8, unsigned int z_depth=3){
        this-> kdtree = std::make_shared<kdTree>(data,leaf_size);
        this->leaf_size = leaf_size;
        this->z_depth = z_depth;
    }

    unsigned int encode(std::vector<int> &pt){
        std::vector<int> code;
        auto bound_box = this->kdtree->query(pt,code);

        std::vector<int> new_pt;
        new_pt.reserve(pt.size());
        for(int i=0;i<pt.size();i++){
            new_pt.push_back(pt[i]-bound_box[i][0]);
        }
        int target_length = int(pow(2,this->z_depth-1));
        for(int i=0;i<new_pt.size();i++){
            new_pt[i] = int(new_pt[i]/(bound_box[i][1]- bound_box[i][0])*target_length);

        }
        zorder(new_pt,code,this->z_depth);
        for(auto &dim: code){
            std::cout<<dim<<" ";
        }
        std::cout<<std::endl;
        return code_value(code);
    }



};

#endif //KDTREE_HTREE_H
