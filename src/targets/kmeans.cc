#include <eclipsedfs/vmr.hh>
#include <eclipsedfs/map_output_collection.hh>
#include <utility>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <list>
#include <limits>
#include <math.h>
#include <time.h>
#include <cstdlib>

#define INPUT_NAME "kmeans.input"
#define OUTPUT_NAME "kmeans.output"
#define CENTROID_NAME "kmeans_centroids.data"
#define LOCAL_CENTROID_PATH "/home/deukyeon/EclipseMR/data/kmeans_centroids.data"
#define ITERATIONS 5
#define NUM_CLUSTERS 25

using namespace std;
using namespace velox;

extern "C" {
  void mymapper(std::string&, velox::MapOutputCollection&);
  void myreducer(std::string&, std::list<std::string>&, MapOutputCollection&);
}

class Point {
  private:
    double x;
    double y;

  public:
    Point() = default;
    Point(double _x, double _y) : x(_x), y(_y) {};
    Point(std::string& str) {
      get_point_from_string(str);
    };
    Point(const Point& other) {
      x = other.x;
      y = other.y;
    };

    Point& operator=(const Point& other) {
      x = other.x;
      y = other.y;

      return *this;
    };

    double getX() {
      return x;
    };
    
    double getY() {
      return y;
    };

    void setX(double _x) {
      x = _x;
    };

    void setY(double _y) {
      y = _y;
    };

    void get_point_from_string(std::string str) {
      std::istringstream stream(str);
      stream >> x >> y;
    };

    double distance_square(Point& p) {
      return (double)(pow((x - p.x), 2.0) + pow((y - p.y), 2.0));
    };

    static double distance_square(Point& p1, Point& p2) {
      return (pow((p1.x - p2.x), 2.0) + pow((p1.y - p2.y), 2.0));
    };

    static double distance(Point& p1, Point& p2) {
      return distance_square(p1, p2);
    };

    std::string to_string() {
      return (std::to_string(x) + " " + std::to_string(y));
    };
};

void mymapper(std::string& input, velox::MapOutputCollection& mapper_results) {
  // load centroids
  // TODO: using distributed cache
  
  ifstream fs;
  fs.open(LOCAL_CENTROID_PATH);

  std::list<Point> centroids;

  std::string centroid_str;
  while(getline(fs, centroid_str)) {
    Point centroid(centroid_str);
    centroids.push_back(std::move(centroid));
  }

  fs.close();

  Point p(input);

  double min = std::numeric_limits<double>::max();
  Point nearest_centroid;
  for(Point centroid : centroids) {
    //double dist = p.distance_square(centroid);
    double dist = Point::distance_square(p, centroid);
    if(dist < min) {
      nearest_centroid = centroid;
      min = dist;
    }
  }

  mapper_results.insert(nearest_centroid.to_string(), p.to_string());
}

void myreducer(std::string& key, std::list<std::string>& values, MapOutputCollection& output) {
  if(values.size() == 0) return;

  double sumX = 0, sumY = 0;
  unsigned int count = 0;
  std::string value_string = "";
  for(std::string& current_value : values) {
    Point p(current_value); 
  
    sumX += p.getX();
    sumY += p.getY();

    count++;

    value_string += p.to_string();
    if(count < values.size()) value_string += ", ";
  }

  Point centroid((sumX / count), (sumY / count));
  output.insert(centroid.to_string(), value_string);
}

int main (int argc, char** argv) {
  vdfs cloud;

  // temp: make a file for centroid in nfs
  std::ofstream os;
  os.open(LOCAL_CENTROID_PATH);
  if(!os.is_open()) return 1;

  // initialize the first centroid points randomly
  srand(time(nullptr));
  for(int i=0; i<NUM_CLUSTERS; i++) {
    double x = ((double)(rand() % 10001) / 100);
    double y = ((double)(rand() % 10001) / 100);
    Point centroid(x, y);
    std::string centroid_content = centroid.to_string() + "\n";
    os.write(centroid_content.c_str(), centroid_content.size());
  }
  os.close();

  file myfile = cloud.open(INPUT_NAME);

  vmr mr(&cloud);

  for(int i=0; i<ITERATIONS; i++) {
    std::cout << "MR iteration " << i << std::endl;

    dataset A = mr.make_dataset({INPUT_NAME});

    A.map("mymapper");
    A.reduce("myreducer", OUTPUT_NAME);

    if(i < ITERATIONS - 1) {
      // parse output and make centroid file updated
      file output_file = cloud.open(OUTPUT_NAME);
      std::istringstream stream(output_file.get());
      std::string output_line;

      os.open(LOCAL_CENTROID_PATH);
      while(getline(stream, output_line)) {
        std::string::size_type pos = output_line.find(':');
        std::string centroid_string = output_line.substr(0, pos) + "\n";
        os.write(centroid_string.c_str(), centroid_string.size());
      }
      os.close();

      // remove output
      cloud.rm(OUTPUT_NAME);
    }
  }

  std::remove(LOCAL_CENTROID_PATH);

  std::cout << "FINISH k-means clusering" << std::endl;

  // summary


  return 0;
}
