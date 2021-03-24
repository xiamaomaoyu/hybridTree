#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include "hTree.hpp"


std::vector<std::vector<int>> load_data(const std::string& filename, const std::string& dirname){
    std::vector<std::vector<int>> data;
    std::string myText;

    std::ifstream MyReadFile(dirname+filename);
    getline (MyReadFile, myText);
    // Use a while loop together with the getline() function to read the file line by line
    while (getline (MyReadFile, myText)) {
        // Output the text from the file
        std::stringstream check1(myText);
        std::string text;
        std::vector<int> pt;
        getline(check1, text, ',');
        pt.push_back(int(std::stof(text)));
        getline(check1, text, ',');
        pt.push_back(int(std::stof(text)));
        data.push_back(pt);
    }
    // Close the file
    MyReadFile.close();
    return data;

}

int main() {
    // todo test htree
    std::string filename="poi3515_50.csv";
    std::string dirname="/Users/mengmeng/CLionProjects/kdtree/";
    auto data = load_data(filename,dirname);

    hTree ht = hTree(data);
    auto code = ht.encode(data[12]);
    std::cout<< code <<std::endl;


    return 0;
}