#include <iostream>
#include <math.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>
#include <tuple>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;


int main(int argc, char* argv[1]) {

    using namespace std::chrono;

    //Start timer
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    if (argc != 2){
        cout<<"usage: "<< argv[0] <<" <filename>\n";
    } else {

        ifstream data(argv[1]);



        //Make sure the file opens correctly
        if (!data.is_open()) std::cout << "Error on: File Open" << '\n';

        std::string str;

        //declare multimap containing argmax and it's corresponding (x,y) pair.
        std::unordered_multimap<int, std::pair<double, double>> arg_mm;


        double x;
        double y;
        double lon;
        double lat;
        double delta_x;
        double delta_y;
        double feature;
        std::vector<double> features;
        int argMax;

        double lines_count = 0;

        while (data.good()) {
//    for (int i = 0; i<10; i++){

            //clears the temporary feature vector at the start of each loop so that each line's features can be added to the map
            //as one vector and the other stuff isn't included from the previous iteration.
            features.clear();
            argMax = 0;

            //grab longitude from file
            std::getline(data, str, ':');
            lon = atof(str.c_str()); // convert string to double

            //get latitude
            std::getline(data, str, ':');
            lat = atof(str.c_str());

            //get delta X
            std::getline(data, str, ':');
            delta_x = atof(str.c_str());

            //get delta Y
            std::getline(data, str, ':');
            delta_y = atof(str.c_str());

            //loop through the feature columns 0-19
            for (int j = 0; j < 20; j++) {
                std::getline(data, str, ':');
                feature = atof(str.c_str());
                features.push_back(feature);
            }

            //handles the '\n' on the last feature column
            std::getline(data, str, '\n');
            feature = atof(str.c_str());
            features.push_back(feature);

            //increment line counter
            lines_count++;

            //gets argmax for each feature vector
            argMax = std::distance(features.begin(), std::max_element(features.begin(), features.end()));

            //calculate X and Y
            x = (delta_x / 221200) * cos(lat) + lon;
            y = (delta_y / 221200) + lat;

            //make x and y into a pair
            std::pair<double, double> xyPair = make_pair(x, y);

            //add argMax and it's corresponding x,y pairs to multimap
            arg_mm.insert(make_pair(argMax, xyPair));
        }
        //Close file
        data.close();

        //Record parse finish time
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        //calculate time to parse
        duration<double> time_span = duration_cast<duration<double>>(t2 - t1);



        std::cout << "Time to Parse: " << time_span.count() << " seconds." << "\n";
        std::cout << "Number of Lines Parsed: " + std::to_string(lines_count) << "\n";
        std::cout << "Number of Buckets in Multimap: " + std::to_string(arg_mm.bucket_count()) << "\n";

        //Start time for getting the count of buckets
        high_resolution_clock::time_point bucket_count_start = high_resolution_clock::now();

        //get the count of the buckets
        for (auto& i: {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20}) {
            if (arg_mm.count(i) < 5001){
                std::cout << i << ": " << arg_mm.count(i) << " entries.\n";
            }
        }


        


        //bucket count end time
        high_resolution_clock::time_point bucket_count_end = high_resolution_clock::now();
        //calculate time to count buckets
        duration<double> count_time = duration_cast<duration<double>>(bucket_count_end - bucket_count_start);
        std::cout << "Bucket Count Time: " << count_time.count() << " seconds." << "\n";


    }

    return 0;
}