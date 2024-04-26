#include <linesdiscoverysystem/readfiles.h>

#include <vector>
#include <string>
#include <filesystem>

#include <rendering/window.h>
#include <fmt/format.h>

#include <unordered_map>

void plotData(const std::string& name);

void findPattern(const std::string& name);

/* ************************************* */

int main() try {
    std::cout << "Enter the name of input points file: ";
    std::string s;
    std::cin >> s;  // e.g. points1.txt, points200.txt, largeMystery.txt
    
    findPattern(s);
    plotData(s);
} catch (const std::exception& e) {
    fmt::print("Error: {}", e.what());
    return 1;
}

/* ************************************* */

void plotData(const std::string& name) {
    std::filesystem::path points_name = name;
    const auto points = readPoints(data_dir / points_name);

    std::filesystem::path segments_name = "segments-" + name;
    const auto lines = readLineSegments(data_dir / "output" / segments_name);

    rendering::Window window(850, 850, rendering::Window::UseVSync::Yes);
    while (!window.shouldClose()) {
        window.beginFrame();
        window.clear({0, 0, 0, 1});
        window.drawLines(lines);    // to plot the line segments discovered
        window.drawPoints(points);  // to plot input points
        window.endFrame();
    }
}

struct hash_points {
    // Change to work with points
    // https://www.geeksforgeeks.org/how-to-create-an-unordered_map-of-pairs-in-c/
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const
    {
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
 
        if (hash1 != hash2) {
            return hash1 ^ hash2;              
        }
         
        // If hash1 == hash2, their XOR is zero.
          return hash1;
    }
};

void findPattern(const std::string& name) {
    std::filesystem::path points_name = name;
    const auto points = readPoints(data_dir / points_name);

    std::unordered_map<float, std::unordered_map<rendering::Point, bool>, hash_points> slopes;

    for(int i = 0;i < std::ssize(points);i++)
    {
        for(int j = std::ssize(points) - 1;j >= 0;j--)
        {
            const float key = (points[j].position.y - points[i].position.y)/(points[j].position.x - points[i].position.x);
            if(i != j && key != -std::numeric_limits<double>::infinity())
            {
                if(slopes[key].size() == 1)
                    slopes[key][points[i]] = true;
                slopes[key][(points[j]] = true;
            }
        }
    }

    for(auto& it : slopes)
    {
    }
}
